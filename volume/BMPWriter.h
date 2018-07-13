#ifndef BMP_INCLUDED
#define BMP_INCLUDED

namespace Util {

/* Writes an image to a BMP file. */
void BMPWriteImage(const char* filename, unsigned char* pData, int width, int height, bool color = false);

}

#endif /* BMP_INCLUDED */
