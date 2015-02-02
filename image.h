


#include <windows.h>
#include <stdio.h>
#include "jpeglib.h"

class CTexture
{
public:
	unsigned int tex;
	char filepath[MAX_PATH];
	bool loaded;

	CTexture()
	{
		loaded = false;
	}
};

#define TEXTURES	2048
extern CTexture g_texture[TEXTURES];

#define TGA_RGB		 2		// This tells us it's a normal RGB (really BGR) file
#define TGA_A		 3		// This tells us it's a ALPHA file
#define TGA_RLE		10		// This tells us that the targa is Run-Length Encoded (RLE)

struct tImage
{
	int channels;			// The channels in the image (3 = RGB : 4 = RGBA)
	int sizeX;				// The width of the image in pixels
	int sizeY;				// The height of the image in pixels
	unsigned char *data;	// The image pixel data
};

tImage *LoadBMP(const char *strFileName);
tImage *LoadTGA(const char *strFileName);
void DecodeJPG(jpeg_decompress_struct* cinfo, tImage *pImageData);
tImage *LoadJPG(const char *strFileName);
bool FindTexture(unsigned int &texture, LPSTR filepath);
int NewTexture();
void TextureLoaded(unsigned int texture, char* filepath, int id=-1);
void FreeTextures();
bool CreateTexture(unsigned int &texture, LPSTR strFileName, int id=-1);
void ReloadTextures();