/*  GIMP header image file format (RGB): /home/ondra/downloads/linearl_face_based_20.h  */

static unsigned int width = 1;
static unsigned int height = 20;

/*  Call this macro repeatedly.  After each use, the pixel data can be extracted  */

#define HEADER_PIXEL(data,pixel) {\
pixel[0] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4)); \
pixel[1] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)); \
pixel[2] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33))); \
data += 4; \
}
static char *header_data =
	"%AA1*R!D*B&'(B&7$S>O%4W#%5S$%7FW%9&F%:6-%;&#%,9A5=929=Y2J>12W.AQ"
	"ZNJ]`>_^``,H``TY";
