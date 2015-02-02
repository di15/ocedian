


#ifndef _FONT_H
#define _FONT_H

#include <Windows.h>

#define MAX_CHARS	256	

class CFont
{
public:
	unsigned int listID;
	unsigned int listID3D;
	unsigned int height;
	GLYPHMETRICSFLOAT glyphInfo[MAX_CHARS];	
	
	CFont()
	{
		listID = 0;
		listID3D = 0;
	}
};

enum FONT{ARIAL16, ARIAL32, ARIAL64, FONTS};
extern CFont g_font[FONTS];

#define FONT_EXTRUDE	0.01f

void CreateFont(int id, char* strFontName, int height);
void PositionText(int id, int x, int y);
void glDrawText(int id, int x, int y, const char *strString, ...);
void glDraw3DText(int id, const char *strString, ...);
void DestroyFont(int id);
void DestroyFonts();
void CreateFonts();

#endif