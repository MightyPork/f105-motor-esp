#ifndef FTOA_H
#define FTOA_H

#include <esp8266.h>

/* itoa:  convert n to characters in s. returns length */
int my_itoa(int n, char *s);
int my_ftoa(char *buffer, float f, int precision);

#endif // FTOA_H
