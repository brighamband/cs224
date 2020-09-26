#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE  1
#define FALSE 0

#define BAD_NUMBER_ARGS 1
#define FSEEK_ERROR 2
#define FREAD_ERROR 3
#define MALLOC_ERROR 4
#define FWRITE_ERROR 5

/**
 * Parses the command line.
 *
 * argc:      the number of items on the command line (and length of the
 *            argv array) including the executable
 * argv:      the array of arguments as strings (char* array)
 * grayscale: the integer value is set to TRUE if grayscale output indicated
 *            outherwise FALSE for threshold output
 *
 * returns the input file pointer (FILE*)
 **/
FILE *parseCommandLine(int argc, char **argv, int *isGrayscale) {
  if (argc > 2) {
    printf("Usage: %s [-g]\n", argv[0]);
    exit(BAD_NUMBER_ARGS);
  }
  
  if (argc == 2 && strcmp(argv[1], "-g") == 0) {
    *isGrayscale = TRUE;
  } else {
    *isGrayscale = FALSE;
  }

  return stdin;
}

unsigned getFileSizeInBytes(FILE* stream) {
  unsigned fileSizeInBytes = 0;
  
  rewind(stream);
  if (fseek(stream, 0L, SEEK_END) != 0) {
    exit(FSEEK_ERROR);
  }
  fileSizeInBytes = ftell(stream);

  return fileSizeInBytes;
}

void getBmpFileAsBytes(unsigned char* ptr, unsigned fileSizeInBytes, FILE* stream) {
  rewind(stream);
  if (fread(ptr, fileSizeInBytes, 1, stream) != 1) {
#ifdef DEBUG
    printf("feof() = %x\n", feof(stream));
    printf("ferror() = %x\n", ferror(stream));
#endif
    exit(FREAD_ERROR);
  }
}

int getBytesInRow(int width) {
  size_t count = 0;
  while ((width * 3 + count) % 4 != 0) {
    count++;
  }
  return width * 3 + count;
}

unsigned char getAverageIntensity(unsigned char blue, unsigned char green, unsigned char red) {
  return (blue + green + red) / 3;
}

void applyGrayscaleToPixel(unsigned char* pixel) {
  unsigned char* blue = (pixel);
  unsigned char* green = (pixel + 1);
  unsigned char* red = (pixel + 2);
  unsigned char avgIntensity = getAverageIntensity(*blue, *green, *red);

  *blue = avgIntensity;
  *green = avgIntensity;
  *red = avgIntensity;
}

void applyThresholdToPixel(unsigned char* pixel) {
  unsigned char* blue = (pixel);
  unsigned char* green = (pixel + 1);
  unsigned char* red = (pixel + 2);
  unsigned char avgIntensity = getAverageIntensity(*blue, *green, *red);
  
  if (avgIntensity >= 128) {
    *blue = 0xff;
    *green = 0xff;
    *red = 0xff;
  }
  else {
    *blue = 0x00;
    *green = 0x00;
    *red = 0x00;
  }
}

void applyFilterToPixel(unsigned char* pixel, int isGrayscale) {
  if (isGrayscale) {
    applyGrayscaleToPixel(pixel);
  }
  else {
    applyThresholdToPixel(pixel);
  }
}

void applyFilterToRow(unsigned char* row, int width, int isGrayscale) {
  size_t bytesBeforePadding = width * 3;
  for (size_t i = 0; i < bytesBeforePadding; i += 3) {
    applyFilterToPixel(&row[i], isGrayscale);
  }
}

void applyFilterToPixelArray(unsigned char* pixelArray, int width, int height, int isGrayscale) {
  unsigned char* row = NULL;
  int rowBytes = getBytesInRow(width);

  for (size_t i = 0; i < height; i++) {
    row = pixelArray + (i * rowBytes);
    applyFilterToRow(row, width, isGrayscale);
  }
}

void parseHeaderAndApplyFilter(unsigned char* bmpFileAsBytes, int isGrayscale) {
  int offsetFirstBytePixelArray = 0;
  int width = 0;
  int height = 0;
  unsigned char* pixelArray = NULL;

  offsetFirstBytePixelArray = *((int *)(bmpFileAsBytes + 10));
  width = *((int *)(bmpFileAsBytes + 18));
  height = *((int *)(bmpFileAsBytes + 22));
  pixelArray = bmpFileAsBytes + offsetFirstBytePixelArray;

#ifdef DEBUG
  printf("offsetFirstBytePixelArray = %u\n", offsetFirstBytePixelArray);
  printf("width = %u\n", width);
  printf("height = %u\n", height);
  printf("pixelArray = %p\n", pixelArray);
#endif

  applyFilterToPixelArray(pixelArray, width, height, isGrayscale);
}

int main(int argc, char **argv) {
  int grayscale = FALSE;
  unsigned fileSizeInBytes = 0;
  unsigned char* bmpFileAsBytes = NULL;
  FILE *stream = NULL;

  stream = parseCommandLine(argc, argv, &grayscale);
  fileSizeInBytes = getFileSizeInBytes(stream);

#ifdef DEBUG
  printf("fileSizeInBytes = %u\n", fileSizeInBytes);
#endif

  bmpFileAsBytes = (unsigned char *)malloc(fileSizeInBytes);
  if (bmpFileAsBytes == NULL) {
    exit(MALLOC_ERROR);
  }
  getBmpFileAsBytes(bmpFileAsBytes, fileSizeInBytes, stream);

  parseHeaderAndApplyFilter(bmpFileAsBytes, grayscale);

#ifndef DEBUG
  if (fwrite(bmpFileAsBytes, fileSizeInBytes, 1, stdout) != 1) {
    exit(FWRITE_ERROR);
  }
#endif

  free(bmpFileAsBytes);
  return 0;
}
