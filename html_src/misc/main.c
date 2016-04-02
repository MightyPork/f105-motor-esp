#include "linearl_face_based_16.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void main() {
  int pixel[3];
  char *data = header_data;
  for (int i = 0; i < height; i++) {
    HEADER_PIXEL(data, pixel);
    printf("[%.2f, %d, %d, %d],\n",log10(1+(i/(float)height)*100.0f),pixel[0],pixel[1],pixel[2]);
  }
}