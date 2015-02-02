



#include "main.h"
#include "font.h"

CFont g_font[FONTS];

HFONT g_hOldFont;

void CreateFont(int id, char* strFontName, int height)
{
	static bool first = true;
	HFONT	hFont;

	g_font[id].height = height;
	g_font[id].listID = glGenLists(MAX_CHARS);
	g_font[id].listID3D = glGenLists(MAX_CHARS);

	hFont = CreateFont(	height,							// Our desired HEIGHT of the font
						0,								// The WIDTH (If we leave this zero it will pick the best width depending on the height)
						0,								// The angle of escapement
						0,								// The angle of orientation
						FW_BOLD,						// The font's weight (We want it bold)
						FALSE,							// Italic - We don't want italic
						FALSE,							// Underline - We don't want it underlined
						FALSE,							// Strikeout - We don't want it strikethrough
						ANSI_CHARSET,					// This is the type of character set
						OUT_TT_PRECIS,					// The Output Precision
						CLIP_DEFAULT_PRECIS,			// The Clipping Precision
						ANTIALIASED_QUALITY,			// The quality of the font - We want anitaliased fonts
						FF_DONTCARE | DEFAULT_PITCH,	// The family and pitch of the font.  We don't care.
						strFontName);					// The font name (Like "Arial", "Courier", etc...)

	if(first)	// Save first object being replaced to prevent memory leaks
		g_hOldFont = (HFONT)SelectObject(g_hDC, hFont);
	else
		SelectObject(g_hDC, hFont);

	first = false;

	wglUseFontBitmaps(g_hDC, 0, MAX_CHARS - 1, g_font[id].listID);
	wglUseFontOutlines(g_hDC, 0, MAX_CHARS - 1, g_font[id].listID3D, 0, FONT_EXTRUDE, WGL_FONT_POLYGONS, g_font[id].glyphInfo);
}

void PositionText(int id, int x, int y)
{
	// If you are to use this font code for your applications,
	// you must be aware that you cannot position the font in 3D,
	// which means you can't rotate and scale it.  That will be covered in
	// the next font tutorial.  BUT, though that might be a drag, this code
	// is useful because when you display the text, it will always be on top
	// of everything else.  This is good if the camera is moving around, and you
	// don't want the text to move.  If the text was positioned in 3D you would have
	// to come up with a tricky way of making it always render in front of the camera.
	// To do this, we need to set the Raster Position.  That is the position that OpenGL
	// starts drawing at.  Since it's in floating point, it's not very intuitive, so what
	// we do is create a new view port, and then always draw the text at (0, 0, 0) in that
	// view port.  The weird part is that the Y is flipped, so (0, 0) is the bottom left corner.
	// Below we do some simple math to flip it back to normal.

	// Before we create a new view port, we need to save the current one we have.
	// This saves our transform (matrix) information and our current viewport information.
	glPushAttrib(GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	y = g_height - g_font[id].height - y;

	glViewport( x - 1, y - 1, 0, 0 );

	glRasterPos4f( 0, 0, 0, 1 );

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();										// This restores our TRANSFORM and VIEWPORT attributes
}

void glDrawText(int id, int x, int y, const char *strString, ...)
{
	char strText[256];
	va_list argumentPtr;

	if (strString == NULL)
		return;

	va_start(argumentPtr, strString);
	vsprintf(strText, strString, argumentPtr);
	va_end(argumentPtr);

	PositionText(id, x, y);
	glPushAttrib(GL_LIST_BIT);
	glListBase(g_font[id].listID);
	glCallLists(strlen(strText), GL_UNSIGNED_BYTE, strText);
	glPopAttrib();
}

void glDraw3DText(int id, const char *strString, ...)
{
	char strText[256];
	va_list argumentPtr;
	float unitLength = 0.0f;

	if (strString == NULL)
		return;

	va_start(argumentPtr, strString);
	vsprintf(strText, strString, argumentPtr);
	va_end(argumentPtr);

	// Below we find out the total length of the characters in 3D units, then center them.
	for (int i=0; i < (int)strlen(strText); i++)
	{
		unitLength += g_font[id].glyphInfo[strText[i]].gmfCellIncX;	
	}

	
	glTranslatef(0.0f - (unitLength / 2), 0.0f, 0.0f);
	
	glPushAttrib(GL_LIST_BIT);
	glListBase(g_font[id].listID3D);
	glCallLists((int)strlen(strText), GL_UNSIGNED_BYTE, strText);
	glPopAttrib();
}

void DestroyFont(int id)										
{
	glDeleteLists(g_font[id].listID, MAX_CHARS);
	glDeleteLists(g_font[id].listID3D, MAX_CHARS);
}

void DestroyFonts()
{
	for(int i=0; i<FONTS; i++)
		DestroyFont(i);
	SelectObject(g_hDC, g_hOldFont);
}

void CreateFonts()
{
	CreateFont(ARIAL16, "Arial", 16);
	CreateFont(ARIAL32, "Arial", 32);
	CreateFont(ARIAL64, "Arial", 64);
}