/*  GIMP header image file format (RGB): /home/ondra/downloads/linearl_face_based.h  */

static unsigned int width = 1;
static unsigned int height = 100;

/*  Call this macro repeatedly.  After each use, the pixel data can be extracted  */

#define HEADER_PIXEL(data,pixel) {\
pixel[0] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4)); \
pixel[1] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)); \
pixel[2] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33))); \
data += 4; \
}
static char *header_data =
	"\"A-&&AE4'!I7(!M9)!U=)AY?*!]A*R!D+2%E+B%F+B%Q+2%X*R%_*2&))R&-)2&1"
	"(2&8'B&<%R&B$B*E$2FH$C6N$CFQ$SRS$T*X%$2Z%$>]%4O!%4[#%5#&%5G$%5[\""
	"%6+!%6J^%6V\\%72Y%7>X%7JV%7^S%8*Q%8:N%8RJ%8^G%9*D%9>?%9J<%9R9%:&2"
	"%:..%:6+%:R&%:^$%+5^$[=[$[IX$K]O$L%K$<1E&<E4+,M2.,U21]%23=-24M52"
	"6]E27]M28]U2:N!2;N)2>^)2DN-2G.-2K.12M.12N^52R>52S^92U>92WN>!X>B9"
	"Y>BJ[.K#[NK-\\NO7]^SF^^WM_NWU`^\\!`_('`_<2`_H7`_T=``$E``0J``<O``LW"
	"``X[```_````````";
