#include "BMPWriter.h"
#include <cassert>
#include <cstdlib>
#include <cstdio>

namespace Util {

#define DEBUG_HEADERS 0

typedef char BYTE;					/* 8 bits */
typedef unsigned short int WORD;	/* 16-bit unsigned integer. */
typedef unsigned int DWORD;			/* 32-bit unsigned integer */
typedef int LONG;					/* 32-bit signed integer */

struct BITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
};

struct BITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
};

/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

struct RGBTRIPLE {
    BYTE rgbtBlue;
    BYTE rgbtGreen;
    BYTE rgbtRed;
};

struct RGBQUAD {
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
};


/* Some magic numbers */

#define BMP_BF_TYPE		0x4D42
/* word BM */

#define BMP_BF_OFF_BITS 54
/* 14 for file header + 40 for info header (not sizeof(), but packed size) */

#define BMP_BI_SIZE		40
/* packed size of info header */

/* Reads a WORD from a file in little endian format */
static WORD WordReadLE(FILE *fp)
{
    WORD lsb, msb;

    lsb = getc(fp);
    msb = getc(fp);
    return (msb << 8) | lsb;
}

/* Writes a WORD to a file in little endian format */
static void WordWriteLE(WORD x, FILE *fp)
{
    BYTE lsb, msb;

    lsb = (BYTE) (x & 0x00FF);
    msb = (BYTE) (x >> 8);
    putc(lsb, fp);
    putc(msb, fp);
}

/* Reads a DWORD word from a file in little endian format */
static DWORD DWordReadLE(FILE *fp)
{
    DWORD b1, b2, b3, b4;

    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);
    b4 = getc(fp);
    return (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
}

/* Writes a DWORD to a file in little endian format */
static void DWordWriteLE(DWORD x, FILE *fp)
{
    unsigned char b1, b2, b3, b4;

    b1 = (x & 0x000000FF);
    b2 = ((x >> 8) & 0x000000FF);
    b3 = ((x >> 16) & 0x000000FF);
    b4 = ((x >> 24) & 0x000000FF);
    putc(b1, fp);
    putc(b2, fp);
    putc(b3, fp);
    putc(b4, fp);
}

/* Reads a LONG word from a file in little endian format */
static LONG LongReadLE(FILE *fp)
{
    LONG b1, b2, b3, b4;

    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);
    b4 = getc(fp);
    return (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
}

/* Writes a LONG to a file in little endian format */
static void LongWriteLE(LONG x, FILE *fp)
{
    char b1, b2, b3, b4;

    b1 = (x & 0x000000FF);
    b2 = ((x >> 8) & 0x000000FF);
    b3 = ((x >> 16) & 0x000000FF);
    b4 = ((x >> 24) & 0x000000FF);
    putc(b1, fp);
    putc(b2, fp);
    putc(b3, fp);
    putc(b4, fp);
}

void BMPWriteImage(const char* filename, unsigned char* pData, int width, int height, bool color)
{
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    int x, y;
    int lineLength;

	FILE* fp = fopen(filename, "wb");

    lineLength = width * 3;	/* RGB */
    assert((lineLength % 4) == 0);

    /* Write file header */
    bmfh.bfType = BMP_BF_TYPE;
    bmfh.bfSize = BMP_BF_OFF_BITS + lineLength * height;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = BMP_BF_OFF_BITS;

    /* Debug file header */
#if DEBUG_HEADERS
    fprintf(stderr, "file header:\n");
    fprintf(stderr, "\tbfType: %x\n", bmfh.bfType);
    fprintf(stderr, "\tbfSize: %d\n", (int) bmfh.bfSize);
    fprintf(stderr, "\tbfReserved1: %d\n", (int) bmfh.bfReserved1);
    fprintf(stderr, "\tbfReserved2: %d\n", (int) bmfh.bfReserved2);
    fprintf(stderr, "\tbfOffBits: %d\n", (int) bmfh.bfOffBits);
#endif

    WordWriteLE(bmfh.bfType, fp);
    DWordWriteLE(bmfh.bfSize, fp);
    WordWriteLE(bmfh.bfReserved1, fp);
    WordWriteLE(bmfh.bfReserved2, fp);
    DWordWriteLE(bmfh.bfOffBits, fp);

    /* Write info header */
    bmih.biSize = BMP_BI_SIZE;
    bmih.biWidth = width;
    bmih.biHeight = height;
    bmih.biPlanes = 1;
    bmih.biBitCount = 24;		/* RGB */
    bmih.biCompression = BI_RGB;	/* RGB */
    bmih.biSizeImage = lineLength * (DWORD) bmih.biHeight;	/* RGB */
    bmih.biXPelsPerMeter = 2925;
    bmih.biYPelsPerMeter = 2925;
    bmih.biClrUsed = 0;
    bmih.biClrImportant = 0;

    /* Debug info header */
#if DEBUG_HEADERS
    fprintf(stderr, "info header:\n");
    fprintf(stderr, "\tbiSize: %d\n", (int) bmih.biSize);
    fprintf(stderr, "\tbiWidth: %d\n", (int) bmih.biWidth);
    fprintf(stderr, "\tbiHeight: %d\n", (int) bmih.biHeight);
    fprintf(stderr, "\tbiPlanes: %d\n", (int) bmih.biPlanes);
    fprintf(stderr, "\tbiBitCount: %d\n", (int) bmih.biBitCount);
    fprintf(stderr, "\tbiCompression: %d\n", (int) bmih.biCompression);
    fprintf(stderr, "\tbiSizeImage: %d\n", (int) bmih.biSizeImage);
    fprintf(stderr, "\tbiXPelsPerMeter: %d\n", (int) bmih.biXPelsPerMeter);
    fprintf(stderr, "\tbiYPelsPerMeter: %d\n", (int) bmih.biYPelsPerMeter);
    fprintf(stderr, "\tbiClrUsed: %d\n", (int) bmih.biClrUsed);
    fprintf(stderr, "\tbiClrImportant: %d\n", (int) bmih.biClrImportant);
#endif

    DWordWriteLE(bmih.biSize, fp);
    LongWriteLE(bmih.biWidth, fp);
    LongWriteLE(bmih.biHeight, fp);
    WordWriteLE(bmih.biPlanes, fp);
    WordWriteLE(bmih.biBitCount, fp);
    DWordWriteLE(bmih.biCompression, fp);
    DWordWriteLE(bmih.biSizeImage, fp);
    LongWriteLE(bmih.biXPelsPerMeter, fp);
    LongWriteLE(bmih.biYPelsPerMeter, fp);
    DWordWriteLE(bmih.biClrUsed, fp);
    DWordWriteLE(bmih.biClrImportant, fp);

    /* Write pixels */
	unsigned char r, g, b;
    for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			int index = y * width + x;//(height - y - 1)
			if(color) {
				r = pData[index * 3];
				g = pData[index * 3 + 1];
				b = pData[index * 3 + 2];
			} else {
				r = g = b = pData[index];
			}
			putc(b, fp);
			putc(g, fp);
			putc(r, fp);
		}
    }
	fclose(fp);
}

} // namespace VolumeRender
