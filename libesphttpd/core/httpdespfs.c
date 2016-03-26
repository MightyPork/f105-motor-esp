/*
Connector to let httpd use the espfs filesystem to serve the files in it.
*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <esp8266.h>
#include "httpdespfs.h"
#include "espfs.h"
#include "espfsformat.h"

// The static files marked with FLAG_GZIP are compressed and will be served with GZIP compression.
// If the client does not advertise that he accepts GZIP send following warning message (telnet users for e.g.)
static const char *gzipNonSupportedMessage = "HTTP/1.0 501 Not implemented\r\n"
											 "Server: esp8266-httpd/"HTTPDVER"\r\n"
											 "Connection: close\r\n"
											 "Content-Type: text/plain\r\n"
											 "Content-Length: 52\r\n"
											 "\r\n"
											 "Your browser does not accept gzip-compressed data.\r\n";



static EspFsFile *tryOpenIndex_do(const char *path, const char *indexname)
{
	// Try appending index.tpl
	char fname[100];

	size_t url_len = strlen(path);
	strcpy(fname, path); // add current path

	// add slash if not already ending with slash
	if (path[url_len - 1] != '/') {
		fname[url_len++] = '/';
	}

	// add index
	strcpy(fname + url_len, indexname);

	return espFsOpen(fname);
}


EspFsFile *tryOpenIndex(const char *path)
{
	EspFsFile * file;

	// if there is a dot in the file, assume it's an extension
	// no point in trying to find index in this case, abort.
	if (strchr(path, '.') != NULL) return NULL;


	// try index.html
	file = tryOpenIndex_do(path, "index.html");
	if (file != NULL) return file;

	// try index.htm
	file = tryOpenIndex_do(path, "index.htm");
	if (file != NULL) return file;

	// try index.tpl
	file = tryOpenIndex_do(path, "index.tpl");
	if (file != NULL) return file;

	return NULL; // failed to guess the right name
}


int ICACHE_FLASH_ATTR serveStaticFile(HttpdConnData *connData, const char* filepath)
{
	EspFsFile *file = connData->cgiData;
	int len;
	char buff[1024];
	char acceptEncodingBuffer[64];
	int isGzip;

	if (connData->conn == NULL) {
		//Connection aborted. Clean up.
		espFsClose(file);
		return HTTPD_CGI_DONE;
	}

	// invalid call.
	if (filepath == NULL) {
		error("serveStaticFile called with NULL path!");
		return HTTPD_CGI_NOTFOUND;
	}

	if (file == NULL) {
		//First call to this cgi. Open the file so we can read it.
		file = espFsOpen(filepath);
		if (file == NULL) {
			// file not found

			// If this is a folder, look for index file
			file = tryOpenIndex(filepath);
			if (file == NULL) return HTTPD_CGI_NOTFOUND;
		}

		// The gzip checking code is intentionally without #ifdefs because checking
		// for FLAG_GZIP (which indicates gzip compressed file) is very easy, doesn't
		// mean additional overhead and is actually safer to be on at all times.
		// If there are no gzipped files in the image, the code bellow will not cause any harm.

		// Check if requested file was GZIP compressed
		isGzip = espFsFlags(file) & FLAG_GZIP;
		if (isGzip) {
			// Check the browser's "Accept-Encoding" header. If the client does not
			// advertise that he accepts GZIP send a warning message (telnet users for e.g.)
			httpdGetHeader(connData, "Accept-Encoding", acceptEncodingBuffer, 64);
			if (strstr(acceptEncodingBuffer, "gzip") == NULL) {
				//No Accept-Encoding: gzip header present
				httpdSend(connData, gzipNonSupportedMessage, -1);
				espFsClose(file);
				return HTTPD_CGI_DONE;
			}
		}

		connData->cgiData = file;
		httpdStartResponse(connData, 200);
		httpdHeader(connData, "Content-Type", httpdGetMimetype(filepath));
		if (isGzip) {
			httpdHeader(connData, "Content-Encoding", "gzip");
		}
		httpdHeader(connData, "Cache-Control", "max-age=3600, must-revalidate");
		httpdEndHeaders(connData);
		return HTTPD_CGI_MORE;
	}

	len = espFsRead(file, buff, 1024);
	if (len > 0) httpdSend(connData, buff, len);
	if (len != 1024) {
		//We're done.
		espFsClose(file);
		info("Static file done");

		return HTTPD_CGI_DONE;
	} else {
		//Ok, till next time.
		return HTTPD_CGI_MORE;
	}
}



//This is a catch-all cgi function. It takes the url passed to it, looks up the corresponding
//path in the filesystem and if it exists, passes the file through. This simulates what a normal
//webserver would do with static files.
int ICACHE_FLASH_ATTR cgiEspFsHook(HttpdConnData *connData)
{
	return serveStaticFile(connData, connData->url);
}


int ICACHE_FLASH_ATTR cgiEspFsFile(HttpdConnData *connData)
{
	return serveStaticFile(connData, connData->cgiArg);
}

//cgiEspFsTemplate can be used as a template.

#define TEMPLATE_CHUNK 1024

typedef struct {
	EspFsFile *file;
	void *tplArg;
	char token[64];
	int tokenPos;

	char buff[TEMPLATE_CHUNK + 1];

	bool chunk_resume;
	int buff_len;
	int buff_x;
	int buff_sp;
	char *buff_e;
} TplData;

typedef int (* TplCallback)(HttpdConnData *connData, char *token, void **arg);

int ICACHE_FLASH_ATTR cgiEspFsTemplate(HttpdConnData *connData)
{
	TplData *tpd = connData->cgiData;
	int len;
	int x, sp = 0;
	char *e = NULL;

	if (connData->conn == NULL) {
		//Connection aborted. Clean up.
		((TplCallback)(connData->cgiArg))(connData, NULL, &tpd->tplArg); // call the template func with NULL token
		espFsClose(tpd->file);
		free(tpd);
		return HTTPD_CGI_DONE;
	}

	if (tpd == NULL) {
		//First call to this cgi. Open the file so we can read it.
		tpd = (TplData *)malloc(sizeof(TplData));

		tpd->chunk_resume = false;


		const char *filepath = connData->url;

		// check for custom template URL
		if (connData->cgiArg2 != NULL) {
			filepath = connData->cgiArg2;
			dbg("Using filepath %s", filepath);
		}

		tpd->file = espFsOpen(filepath);
		if (tpd->file == NULL) {
			// If this is a folder, look for index file
			tpd->file = tryOpenIndex(filepath);
			if (tpd->file == NULL) {
				free(tpd);
				return HTTPD_CGI_NOTFOUND;
			}
		}

		tpd->tplArg = NULL;
		tpd->tokenPos = -1;
		if (espFsFlags(tpd->file) & FLAG_GZIP) {
			error("cgiEspFsTemplate: Trying to use gzip-compressed file %s as template!", connData->url);
			espFsClose(tpd->file);
			free(tpd);
			return HTTPD_CGI_NOTFOUND;
		}
		connData->cgiData = tpd;
		httpdStartResponse(connData, 200);
		httpdHeader(connData, "Content-Type", httpdGetMimetype(filepath));
		httpdEndHeaders(connData);
		return HTTPD_CGI_MORE;
	}

	char *buff = tpd->buff;


	// resume the parser state from the last token,
	// if subst. func wants more data to be sent.
	if (tpd->chunk_resume) {
//		dbg("Resuming tpl parser for multi-part subst");
		// resume
		len = tpd->buff_len;
		e = tpd->buff_e;
		sp = tpd->buff_sp;
		x = tpd->buff_x;
	} else {
		len = espFsRead(tpd->file, buff, TEMPLATE_CHUNK);
		tpd->buff_len = len;

		e = buff;
		sp = 0;
		x =  0;
	}

	if (len > 0) {
		for (; x < len; x++) {
			if (tpd->tokenPos == -1) {
				//Inside ordinary text.
				if (buff[x] == '%') {
					//Send raw data up to now
					if (sp != 0) httpdSend(connData, e, sp);
					sp = 0;
					//Go collect token chars.
					tpd->tokenPos = 0;
				} else {
					sp++;
				}
			} else {
				if (buff[x] == '%') {
					if (tpd->tokenPos == 0) {
						//This is the second % of a %% escape string.
						//Send a single % and resume with the normal program flow.
						httpdSend(connData, "%", 1);
					} else {

						if (!tpd->chunk_resume) {
							//This is an actual token.
							tpd->token[tpd->tokenPos++] = 0; //zero-terminate token
						}

						tpd->chunk_resume = false;

						int status = ((TplCallback)(connData->cgiArg))(connData, tpd->token, &tpd->tplArg);

						if (status == HTTPD_CGI_MORE) {
//							dbg("Multi-part tpl subst, saving parser state");
							// wants to send more in this token's place.....
							tpd->chunk_resume = true;

							tpd->buff_len = len;
							tpd->buff_e = e;
							tpd->buff_sp = sp;
							tpd->buff_x = x;

							break;
						}
					}
					//Go collect normal chars again.
					e = &buff[x + 1];
					tpd->tokenPos = -1;
				} else {
					if (tpd->tokenPos < (sizeof(tpd->token) - 1)) tpd->token[tpd->tokenPos++] = buff[x];
				}
			}
		}
	}

	if (tpd->chunk_resume) {
		return HTTPD_CGI_MORE;
	}

	//Send remaining bit.
	if (sp != 0) httpdSend(connData, e, sp);
	if (len != TEMPLATE_CHUNK) {

		//We're done.

		// let the cgi func clean it's stuff
		((TplCallback)(connData->cgiArg))(connData, NULL, &tpd->tplArg);

		info("Template sent.");

		espFsClose(tpd->file);
		free(tpd);
		return HTTPD_CGI_DONE;
	} else {
		//Ok, till next time.
		return HTTPD_CGI_MORE;
	}
}

