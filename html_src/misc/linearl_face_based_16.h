/*  GIMP header image file format (RGB): /home/ondra/downloads/linearl_face_based_16.h  */

static unsigned int width = 1;
static unsigned int height = 16;

/*  Call this macro repeatedly.  After each use, the pixel data can be extracted  */

#define HEADER_PIXEL(data,pixel) {\
pixel[0] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4)); \
pixel[1] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)); \
pixel[2] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33))); \
data += 4; \
}
static char *header_data =
	"!!!!+2%J)B&/%2ZJ%4F_%63!%7^T%9:A%:N(2-)2<^%2N.52X^F7_.[Q`_X?````"
	"";
