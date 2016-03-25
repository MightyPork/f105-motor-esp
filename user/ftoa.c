#include <esp8266.h>

/* reverse:  reverse string s in place */
static void str_reverse(char *s)
{
	for (int i = 0, j = (int)(strlen(s) - 1); i < j; i++, j--) {
		char c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

/* itoa:  convert n to characters in s. returns length */
int my_itoa(int n, char *s)
{
	int i, sign;

	if ((sign = n) < 0)  /* record sign */
		n = -n;          /* make n positive */
	i = 0;
	do {       /* generate digits in reverse order */
		s[i++] = n % 10 + '0';   /* get next digit */
	} while ((n /= 10) > 0);     /* delete it */
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';

	str_reverse(s);

	return i;
}

int my_ftoa(char *buffer, float f, int precision)
{
	// add 0.5 * 10^-precision
	float rounder = 0.5;
	for (int i = 0; i < precision; i++) rounder *= 0.1;
	f += rounder;

	int wholePart = (int) f;

	// Deposit the whole part of the number.

	int length = my_itoa(wholePart, buffer);

	// Now work on the faction if we need one.
	if (precision > 0) {
		// We do, so locate the end of the string and insert
		// a decimal point.
		char *endOfString = buffer + length;
		//while (*endOfString != '\0') endOfString++;
		*endOfString++ = '.';

		length++;

		// Now work on the fraction, be sure to turn any negative
		// values positive.
		if (f < 0) {
			f *= -1;
			wholePart *= -1;
		}

		float fraction = f - wholePart;
		while (precision > 0) {
			// Multiply by ten and pull out the digit.
			fraction *= 10;
			int dig = (int) fraction;
			*endOfString++ = (char)('0' + dig);

			// Update the fraction and move on to the
			// next digit.
			fraction -= dig;
			precision--;

			length++;
		}

		// Terminate the string.
		*endOfString = 0;
	}

	return length;
}
