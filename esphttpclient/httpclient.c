/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Martin d'Allens <martin.dallens@gmail.com> wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

// FIXME: sprintf->snprintf everywhere.

#include <esp8266.h>
#include <httpd.h>

#include "httpclient.h"
#include <limits.h>

// Internal state.
typedef struct {
	char *path;
	int port;
	char *post_data;
	char *headers;
	char *hostname;
	char *buffer;
	int buffer_size;
	bool secure;
	httpclient_cb user_callback;
	int timeout;
	os_timer_t timeout_timer;
	http_method method;
} request_args;

static char *FLASH_FN esp_strdup(const char *str)
{
	if (str == NULL) {
		return NULL;
	}
	char *new_str = (char *)malloc(strlen(str) + 1); // 1 for null character
	if (new_str == NULL) {
		error("esp_strdup: malloc error");
		return NULL;
	}
	strcpy(new_str, str);
	return new_str;
}

static int FLASH_FN esp_isupper(char c)
{
	return (c >= 'A' && c <= 'Z');
}

static int FLASH_FN esp_isalpha(char c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}


static int FLASH_FN esp_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

static int FLASH_FN esp_isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

/*
 * Convert a string to a long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
static long FLASH_FN esp_strtol(const char *nptr, char **endptr, int base)
{
	const char *s = nptr;
	unsigned long acc;
	int c;
	unsigned long cutoff;
	int neg = 0, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	do {
		c = *s++;
	} while (esp_isspace((char)c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
			c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	} else if ((base == 0 || base == 2) &&
			   c == '0' && (*s == 'b' || *s == 'B')) {
		c = s[1];
		s += 2;
		base = 2;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long)base;
	cutoff /= (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (esp_isdigit((char)c))
			c -= '0';
		else if (esp_isalpha((char)c))
			c -= esp_isupper((char)c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = neg ? LONG_MIN : LONG_MAX;
//      errno = ERANGE;
	} else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char *)(any ? s - 1 : nptr);
	return (acc);
}

static int FLASH_FN chunked_decode(char *chunked, int size)
{
	char *src = chunked;
	char *end = chunked + size;
	int i, dst = 0;

	do {
		char *endstr = NULL;
		//[chunk-size]
		i = esp_strtol(src, &endstr, 16);
		dbg("Chunk Size:%d\r\n", i);
		if (i <= 0)
			break;
		//[chunk-size-end-ptr]
		src = (char *)strstr(src, "\r\n") + 2;
		//[chunk-data]
		memmove(&chunked[dst], src, (size_t)i);
		src += i + 2; /* CRLF */
		dst += i;
	} while (src < end);

	//
	//footer CRLF
	//

	/* decoded size */
	return dst;
}

static void FLASH_FN receive_callback(void * arg, char *buf, unsigned short len)
{
	struct espconn * conn = (struct espconn *)arg;
	request_args * req = (request_args *)conn->reserve;

	if (req->buffer == NULL) {
		return;
	}

	// Let's do the equivalent of a realloc().
	const int new_size = req->buffer_size + len;
	char *new_buffer;
	if (new_size > BUFFER_SIZE_MAX || NULL == (new_buffer = (char *)malloc(new_size))) {
		error("Response too long (%d)", new_size);
		req->buffer[0] = '\0'; // Discard the buffer to avoid using an incomplete response.
		if (req->secure) {
#ifdef USE_SECURE
			espconn_secure_disconnect(conn);
#endif
		} else {
			espconn_disconnect(conn);
		}
		return; // The disconnect callback will be called.
	}

	memcpy(new_buffer, req->buffer, req->buffer_size);
	memcpy(new_buffer + req->buffer_size - 1 /*overwrite the null character*/, buf, len); // Append new data.
	new_buffer[new_size - 1] = '\0'; // Make sure there is an end of string.

	free(req->buffer);
	req->buffer = new_buffer;
	req->buffer_size = new_size;
}


static void FLASH_FN sent_callback(void * arg)
{
	struct espconn * conn = (struct espconn *)arg;
	request_args * req = (request_args *)conn->reserve;

	if (req->post_data == NULL) {
		dbg("All sent");
	} else {
		// The headers were sent, now send the contents.
		dbg("Sending request body");
		if (req->secure) {
#ifdef USE_SECURE
			espconn_secure_sent(conn, (uint8_t *)req->post_data, strlen(req->post_data));
#endif
		} else {
			espconn_sent(conn, (uint8_t *)req->post_data, strlen(req->post_data));
		}
		free(req->post_data);
		req->post_data = NULL;
	}
}

static void FLASH_FN connect_callback(void * arg)
{
	info("Connected!");
	struct espconn *conn = (struct espconn *)arg;
	request_args *req = (request_args *)conn->reserve;

	espconn_regist_recvcb(conn, receive_callback);
	espconn_regist_sentcb(conn, sent_callback);

	char post_headers[32] = "";

	if (req->post_data != NULL) { // If there is data this is a POST request.
		sprintf(post_headers, "Content-Length: %d\r\n", strlen(req->post_data));

		if (req->method == HTTP_GET) {
			req->method = HTTP_POST;
		}
	}

	const char* method = http_method_str(req->method);

	if (req->headers == NULL) { /* Avoid NULL pointer, it may cause exception */
		req->headers = (char *)malloc(sizeof(char));
		req->headers[0] = '\0';
	}

	char buf[69 + strlen(method) + strlen(req->path) + strlen(req->hostname) + strlen(req->headers) + strlen(post_headers)];

	int len = sprintf(buf,
					  "%s %s HTTP/1.1\r\n"
					  "Host: %s:%d\r\n"
					  "Connection: close\r\n"
					  "User-Agent: ESP8266\r\n"
					  "%s"
					  "%s"
					  "\r\n",
					  method, req->path, req->hostname, req->port, req->headers, post_headers);

	dbg("Sending request");
	if (req->secure) {
#ifdef USE_SECURE
		espconn_secure_sent(conn, (uint8_t *)buf, len);
#endif
	} else {
		espconn_sent(conn, (uint8_t *)buf, len);
	}

	if (req->headers != NULL) {
		free(req->headers);
		req->headers = NULL;
	}
}

/**
 * @brief Free all that could be allocated in a request, including the struct itself.
 * @param req : req to free
 */
static void free_req(request_args *req)
{
	if (!req) return;

	if (req->buffer) free(req->buffer);
	if (req->hostname) free(req->hostname);
	if (req->path) free(req->path);
	if (req->post_data) free(req->post_data);
	if (req->headers) free(req->headers);
	free(req);
}

static void FLASH_FN disconnect_callback(void * arg)
{
	dbg("Disconnected");
	struct espconn *conn = (struct espconn *)arg;

	if (conn == NULL) {
		return;
	}

	if (conn->reserve != NULL) {
		request_args * req = (request_args *)conn->reserve;
		int http_status = HTTP_STATUS_GENERIC_ERROR;
		int body_size = 0;
		char *body = "";

		/* Turn off timeout timer */
		os_timer_disarm(&(req->timeout_timer));

		if (req->buffer == NULL) {
			error("Buffer shouldn't be NULL");
		} else if (req->buffer[0] != '\0') {
			// FIXME: make sure this is not a partial response, using the Content-Length header.

			const char *version10 = "HTTP/1.0 ";
			const char *version11 = "HTTP/1.1 ";
			if (!strstarts(req->buffer, version10) && !strstarts(req->buffer, version11)) {
				error("Invalid version in %s", req->buffer);
			} else {
				http_status = atoi(req->buffer + strlen(version10));
				/* find body and zero terminate headers */
				body = (char *)strstr(req->buffer, "\r\n\r\n") + 2;
				*body++ = '\0';
				*body++ = '\0';

				body_size = req->buffer_size - (body - req->buffer);

				if (strstr(req->buffer, "Transfer-Encoding: chunked")) {
					body_size = chunked_decode(body, body_size);
				}
			}
		}

		info("Request completed.");
		if (req->user_callback != NULL) { // Callback is optional.
			req->user_callback(body, http_status, req->buffer, body_size);
		}

		free_req(req);
	}
	espconn_delete(conn);
	if (conn->proto.tcp != NULL) {
		free(conn->proto.tcp);
	}
	free(conn);
}

static void FLASH_FN error_callback(void *arg, sint8 errType)
{
	error("Disconnected with error, type %d", errType);
	disconnect_callback(arg);
}

static void FLASH_FN http_timeout_callback(void *arg)
{
	error("Connection timeout\n");
	struct espconn * conn = (struct espconn *) arg;

	if (conn == NULL) {
		return;
	}

	request_args *req = (request_args*) conn->reserve;
	if (req) {
		/* Call disconnect */
		if (req->secure) {
#ifdef USE_SECURE
			espconn_secure_disconnect(conn);
#endif
		} else {
			espconn_disconnect(conn);
		}

		free_req(req);
	}

	// experimental - better cleanup
	if (conn->proto.tcp != NULL) {
		free(conn->proto.tcp);
	}
	free(conn);
}

static void FLASH_FN dns_callback(const char *hostname, ip_addr_t *addr, void *arg)
{
	request_args * req = (request_args *)arg;

	if (addr == NULL) {
		error("DNS failed for %s", hostname);
		if (req->user_callback != NULL) {
			req->user_callback("", -1, "", 0);
		}
		free(req);
	} else {
		info("DNS found %s " IPSTR, hostname, IP2STR(addr));

		struct espconn * conn = (struct espconn *)malloc(sizeof(struct espconn));
		conn->type = ESPCONN_TCP;
		conn->state = ESPCONN_NONE;
		conn->proto.tcp = (esp_tcp *)malloc(sizeof(esp_tcp));
		conn->proto.tcp->local_port = espconn_port();
		conn->proto.tcp->remote_port = req->port;
		conn->reserve = req;

		memcpy(conn->proto.tcp->remote_ip, addr, 4);

		espconn_regist_connectcb(conn, connect_callback);
		espconn_regist_disconcb(conn, disconnect_callback);
		espconn_regist_reconcb(conn, error_callback);

		/* Set connection timeout timer */
		os_timer_disarm(&(req->timeout_timer));
		os_timer_setfn(&(req->timeout_timer), (os_timer_func_t *) http_timeout_callback, conn);
		os_timer_arm(&(req->timeout_timer), req->timeout, false);

		if (req->secure) {
#ifdef USE_SECURE
			espconn_secure_set_size(ESPCONN_CLIENT, 5120); // set SSL buffer size
			espconn_secure_connect(conn);
#endif
		} else {
			espconn_connect(conn);
		}
	}
}

bool FLASH_FN http_request(
		const char *url,
		http_method method,
		const char *body,
		const char *headers,
		httpclient_cb user_callback)
{
	// --- prepare port, secure... ---

	// FIXME: handle HTTP auth with http://user:pass@host/

	char hostname[128] = "";
	int port = 80;
	bool secure = false;

	if (strstarts(url, "http://"))
		url += strlen("http://"); // Get rid of the protocol.
	else if (strstarts(url, "https://")) {
		port = 443;
		secure = true;
		url += strlen("https://"); // Get rid of the protocol.
	} else {
		error("Invalid URL protocol: %s", url);
		return false;
	}

	char *path = strchr(url, '/');
	if (path == NULL) {
		path = strchr(url, '\0'); // Pointer to end of string.
	}

	char *colon = strchr(url, ':');
	if (colon > path) {
		colon = NULL; // Limit the search to characters before the path.
	}

	if (colon == NULL) { // The port is not present.
		memcpy(hostname, url, (size_t)(path - url));
		hostname[path - url] = '\0';
	} else {
		port = atoi(colon + 1);
		if (port == 0) {
			error("Port error %s\n", url);
			return false;
		}

		memcpy(hostname, url, (size_t)(colon - url));
		hostname[colon - url] = '\0';
	}

	if (path[0] == '\0') { // Empty path is not allowed.
		path = "/";
	}

	// ---

	info("HTTP request: %s:%d%s", hostname, port, path);

	request_args * req = (request_args *)malloc(sizeof(request_args));
	req->hostname = esp_strdup(hostname);
	req->path = esp_strdup(path);

	// remove #anchor
	char *hash = strchr(req->path, '#');
	if (hash != NULL) *hash = '\0'; // remove the hash part

	req->port = port;
	req->secure = secure;
	req->headers = esp_strdup(headers);
	req->post_data = esp_strdup(body);
	req->buffer_size = 1;
	req->buffer = (char *)malloc(1);
	req->buffer[0] = '\0'; // Empty string.
	req->user_callback = user_callback;
	req->timeout = HTTP_REQUEST_TIMEOUT_MS;
	req->method = method;

	ip_addr_t addr;
	err_t error = espconn_gethostbyname((struct espconn *)req, // It seems we don't need a real espconn pointer here.
										hostname, &addr, dns_callback);

	if (error == ESPCONN_INPROGRESS) {
		dbg("DNS pending");
	} else if (error == ESPCONN_OK) {
		// Already in the local names table (or hostname was an IP address), execute the callback ourselves.
		dns_callback(hostname, &addr, req);
	} else {
		if (error == ESPCONN_ARG) {
			error("DNS arg error %s", hostname);
		} else {
			error("DNS error code %d", error);
		}
		dns_callback(hostname, NULL, req); // Handle all DNS errors the same way.
	}

	return true;
}


bool FLASH_FN http_post(const char *url, const char *body, const char *headers, httpclient_cb user_callback)
{
	return http_request(url, HTTP_POST, body, headers, user_callback);
}


bool http_get(const char *url, const char *headers, httpclient_cb user_callback)
{
	return http_request(url, HTTP_GET, NULL, headers, user_callback);
}


bool http_put(const char *url, const char *body, const char *headers, httpclient_cb user_callback)
{
	return http_request(url, HTTP_PUT, body, headers, user_callback);
}


void FLASH_FN http_callback_example(char *response_body, int http_status, char *response_headers, int body_size)
{
	dbg("Response: code %d", http_status);
	if (http_status != HTTP_STATUS_GENERIC_ERROR) {
		dbg("len(headers) = %d, len(body) = %d", (int)strlen(response_headers), body_size);
		dbg("body: %s<EOF>", response_body); // FIXME: this does not handle binary data.
	}
}



void FLASH_FN http_callback_showstatus(char *response_body, int code, char *response_headers, int body_size)
{
	(void)response_body;
	(void)response_headers;
	(void)body_size;

	if (code == 200) {
		info("Response OK (200)");
	} else if (code >= 400) {
		error("Response ERROR (%d)", code);
		dbg("Body: %s<EOF>",response_body);
	} else {
		// ???
		warn("Response %d", code);
		dbg("Body: %s<EOF>",response_body);
	}
}
