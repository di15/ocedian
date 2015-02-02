

#pragma warning( disable : 4018 )
#pragma warning( disable : 4996 )

#include <Windows.h>
#include <vector>

using namespace std;

inline void DrawImage(unsigned int tex, float left, float top, float right, float bottom);

#define MAX_OPTIONS_SHOWN	5

enum WIDGET{IMAGE, BUTTON, TEXT, LINK, DROPDOWN};

class CWidget
{
public:
	int type;
	float pos[4];
	unsigned int tex;
	unsigned int bgtex;
	unsigned int bgovertex;
	bool over;
	bool ldown;
	string name;
	string text;
	int font;
	unsigned int frametex, filledtex, uptex, downtex;
	bool opened;
	vector<string> options;
	int selected;
	float scroll;
	bool mousescroll;
	int mousedowny;
	float healthbar;
	int param;
	
	void (*clickfunc)();
	void (*clickfunc2)(int p);
	void (*overfunc)();
	void (*outfunc)();
	void (*changefunc)();

	CWidget() 
	{
		clickfunc = NULL;
		clickfunc2 = NULL;
		overfunc = NULL;
		outfunc = NULL;
		changefunc = NULL;
	}

	// Initializers
	
	void Image(char* filepath, float left, float top, float right, float bottom);
	void Image(unsigned int t, float left, float top, float right, float bottom);
	
	void Button(char* filepath, float left, float top, float right, float bottom, void (*click)(), void (*overf)(), void (*out)());
	void Button(char* filepath, float left, float top, float right, float bottom, void (*click2)(int p), int parm);
	
	void Text(char* t, int f, float left, float top)
	{
		type = TEXT;
		name = "";
		text = t;
		font = f;
		pos[0] = left;
		pos[1] = top;
		ldown = false;
	}

	void Text(char* n, char* t, int f, float left, float top)
	{
		type = TEXT;
		name = n;
		text = t;
		font = f;
		pos[0] = left;
		pos[1] = top;
		ldown = false;
	}

	void Link(char* t, int f, float left, float top, void (*click)())
	{
		type = LINK;
		over = false;
		ldown = false;
		text = t;
		font = f;
		pos[0] = left;
		pos[1] = top;
		clickfunc = click;
	}

	void DropDown(char* n, int f, float left, float top, float width, void (*change)());

	// Drawers

	void Image_draw()
	{
		DrawImage(tex, pos[0], pos[1], pos[2], pos[3]);
	}
	
	void Button_draw()
	{
		if(over)
			DrawImage(bgovertex, pos[0], pos[1], pos[2], pos[3]);
		else
			DrawImage(bgtex, pos[0], pos[1], pos[2], pos[3]);

		DrawImage(tex, pos[0], pos[1], pos[2], pos[3]);
	}

	void Text_draw();

	void Link_draw();
	
	void DropDown_draw();
	void DropDown_draw2();

	// L button up

	bool Button_lbuttonup()
	{
		if(over && ldown)
		{
			if(clickfunc != NULL)
				clickfunc();

			if(clickfunc2 != NULL)
				clickfunc2(param);

			ldown = false;

			return true;	// intercept mouse event
		}

		return false;
	}

	bool Link_lbuttonup()
	{
		if(over && ldown)
		{
			if(clickfunc != NULL)
				clickfunc();

			ldown = false;

			return true;	// intercept mouse event
		}

		return false;
	}

	bool DropDown_lbuttonup();

	// L button down

	bool Button_lbuttondown()
	{
		if(over)
		{
			ldown = true;
			return true;	// intercept mouse event
		}

		return false;
	}
	
	bool Link_lbuttondown()
	{
		if(over)
		{
			ldown = true;
			return true;	// intercept mouse event
		}

		return false;
	}

	bool DropDown_lbuttondown();

	// Mouse move

	void Button_mousemove();
	void Link_mousemove();
	void DropDown_mousemove();

	// Mousewheel

	bool DropDown_mousewheel(int delta)
	{
		if(opened)
		{


			return true;	// intercept mouse event
		}

		return false;
	}

	// Misc.

	int rowsshown()
	{
		int rows = MAX_OPTIONS_SHOWN;
		if(rows > options.size())
			rows = options.size();

		return rows;
	}

	int square();

	float topratio()
	{
		return scroll / (float)(options.size()-1);
	}

	float bottomratio()
	{
		return (scroll+rowsshown()-1) / (float)(options.size()-1);
	}

	float scrollspace();

	// Common

	bool lbuttonup()
	{
		switch(type)
		{
		case BUTTON: return Button_lbuttonup();
		case LINK: return Link_lbuttonup();
		case DROPDOWN: return DropDown_lbuttonup();
		default: return false;
		}
	}
	
	bool lbuttondown()
	{
		switch(type)
		{
		case BUTTON: return Button_lbuttondown();
		case LINK: return Link_lbuttondown();
		case DROPDOWN: return DropDown_lbuttondown();
		default: return false;
		}
	}

	void mousemove()
	{
		switch(type)
		{
		case BUTTON: Button_mousemove(); break;
		case LINK: Link_mousemove(); break;
		case DROPDOWN: DropDown_mousemove(); break;
		default: break;
		}
	}

	bool mousewheel(int delta)
	{
		switch(type)
		{
		case DROPDOWN: return DropDown_mousewheel(delta);
		default: return false;
		}
	}

	void draw()
	{
		switch(type)
		{
		case IMAGE: Image_draw(); break;
		case BUTTON: Button_draw(); break;
		case TEXT: Text_draw(); break;
		case LINK: Link_draw(); break;
		case DROPDOWN: DropDown_draw(); break;
		default: break;
		}
	}

	void draw2()
	{
		switch(type)
		{
		case DROPDOWN: DropDown_draw2(); break;
		default: break;
		}
	}
};

class CView
{
public:
	string name;
	int page;
	bool opened;
	vector<CWidget> widget;

	CView(char* n, int p=0)
	{
		name = n;
		opened = false;
		page = p;
	}

	void mousemove()
	{
		for(int i=0; i<widget.size(); i++)
			widget[i].mousemove();
	}
	
	bool lbuttonup()
	{
		for(int i=widget.size()-1; i>=0; i--)
			if(widget[i].type == DROPDOWN && widget[i].opened && widget[i].lbuttonup())
				return true;	// intercept mouse event

		for(int i=widget.size()-1; i>=0; i--)
			if(widget[i].lbuttonup())
				return true;	// intercept mouse event

		return false;
	}

	bool lbuttondown()
	{
		for(int i=widget.size()-1; i>=0; i--)
			if(widget[i].type == DROPDOWN && widget[i].opened && widget[i].lbuttondown())
				return true;	// intercept mouse event

		for(int i=widget.size()-1; i>=0; i--)
			if(widget[i].lbuttondown())
				return true;	// intercept mouse event

		return false;
	}
	
	bool mousewheel(int delta)
	{
		for(int i=widget.size()-1; i>=0; i--)
			if(widget[i].mousewheel(delta))
				return true;	// intercept mouse event

		return false;
	}

	CWidget* gettext(char* name)
	{
		for(int i=0; i<widget.size(); i++)
			if(widget[i].type == TEXT && stricmp(widget[i].name.c_str(), name) == 0)
				return &widget[i];

		return NULL;
	}
	
	CWidget* getdropdown(char* name)
	{
		for(int i=0; i<widget.size(); i++)
			if(widget[i].type == DROPDOWN && stricmp(widget[i].name.c_str(), name) == 0)
				return &widget[i];

		return NULL;
	}

	void draw();
};

class CButtonToAdd
{
public:
	char sprite[MAX_PATH];
	char view[32];
	void (*clickfunc)();
	void (*overfunc)();
	void (*outfunc)();

	CButtonToAdd(char* spr, char* m, void (*clickf)(), void (*overf)(), void (*outf)())
	{
		strcpy(sprite, spr);
		strcpy(view, m);
		clickfunc = clickf;
		overfunc = overf;
		outfunc = outf;
	}
};

class CGUI
{
public:
	vector<CView> view;
	vector<CButtonToAdd> button;
	void (*keyupfunc[256])();
	void (*keydownfunc[256])();
	void (*mousemovefunc)();
	void (*lbuttondownfunc)();
	void (*lbuttonupfunc)();
	void (*rbuttondownfunc)();
	void (*rbuttonupfunc)();
	void (*mbuttondownfunc)();
	void (*mbuttonupfunc)();
	void (*mousewheelfunc)(int delta);

	CGUI()
	{
		for(int i=0; i<256; i++)
		{
			keyupfunc[i] = NULL;
			keydownfunc[i] = NULL;
		}

		mousemovefunc = NULL;
	}

	CView* getview(char* name)
	{
		for(int i=0; i<view.size(); i++)
			if(stricmp(view[i].name.c_str(), name) == 0)
				return &view[i];

		return NULL;
	}

	void assignMouseWheel(void (*wheel)(int delta))
	{
		mousewheelfunc = wheel;
	}

	void assignLButton(void (*down)(), void (*up)())
	{
		lbuttondownfunc = down;
		lbuttonupfunc = up;
	}

	void assignRButton(void (*down)(), void (*up)())
	{
		rbuttondownfunc = down;
		rbuttonupfunc = up;
	}

	void assignMButton(void (*down)(), void (*up)())
	{
		mbuttondownfunc = down;
		mbuttonupfunc = up;
	}

	void assignMouseMove(void (*mouse)())
	{
		mousemovefunc = mouse;
	}

	void assignKey(int i, void (*down)(), void (*up)())
	{
		keydownfunc[i] = down;
		keyupfunc[i] = up;
	}

	void mousewheel(int delta)
	{
		for(int i=view.size()-1; i>=0; i--)
			if(view[i].opened && view[i].mousewheel(delta))
				return;	// intercept mouse event

		if(mousewheelfunc != NULL)
			mousewheelfunc(delta);
	}

	void lbuttondown()
	{
		for(int i=view.size()-1; i>=0; i--)
			if(view[i].opened && view[i].lbuttondown())
				return;	// intercept mouse event

		if(lbuttondownfunc != NULL)
			lbuttondownfunc();
	}
	void lbuttonup()
	{
		for(int i=view.size()-1; i>=0; i--)
			if(view[i].opened && view[i].lbuttonup())
				return;	// intercept mouse event

		if(lbuttonupfunc != NULL)
			lbuttonupfunc();
	}

	void rbuttondown()
	{
		if(rbuttondownfunc != NULL)
			rbuttondownfunc();
	}
	void rbuttonup()
	{
		if(rbuttonupfunc != NULL)
			rbuttonupfunc();
	}

	void mbuttondown()
	{
		if(mbuttondownfunc != NULL)
			mbuttondownfunc();
	}
	void mbuttonup()
	{
		if(mbuttonupfunc != NULL)
			mbuttonupfunc();
	}

	void mousemove()
	{
		for(int i=0; i<view.size(); i++)
			if(view[i].opened)
				view[i].mousemove();

		if(mousemovefunc != NULL)
			mousemovefunc();
	}

	void keyup(int i)
	{
		if(keyupfunc[i] != NULL)
			keyupfunc[i]();
	}

	void keydown(int i)
	{
		if(keydownfunc[i] != NULL)
			keydownfunc[i]();
	}

	void draw();
};

extern CGUI g_GUI;

void AssignKey(int i, void (*down)(), void (*up)());
void AssignMouseWheel(void (*wheel)(int delta));
void AssignMouseMove(void (*mouse)());
void AssignLButton(void (*down)(), void (*up)());
void AssignRButton(void (*down)(), void (*up)());
void AssignMButton(void (*down)(), void (*up)());
void AddView(char* name, int page=0);
void AddImage(char* filepath, float left, float top, float right, float bottom);
void AddImage(CView* v, unsigned int tex, float left, float top, float right, float bottom);
void AddLink(char* text, int font, float left, float top, void (*clickfunc)());
void AddButton(char* filepath, float left, float top, float right, float bottom, void (*click)(), void (*overf)(), void (*out)());
void AddButton(char* filepath, float left, float top, float right, float bottom, void (*click2)(int p), int parm);
void AddButton(CView* v, char* filepath, float left, float top, float right, float bottom, void (*click)(), void (*overf)(), void (*out)());
void AddText(char* n, char* t, int f, float left, float top);
void AddText(CView* v, char* n, char* t, int f, float left, float top);
void AddText(char* t, int f, float left, float top);
void CloseView(char* name);
void OpenSoleView(char* name, int page=0);
bool OpenAnotherView(char* name, int page=0);
void NextPage(char* name);
void Status(char* status);
void Chat(char* chat);

void OrthoMode(int left, int top, int right, int bottom);
void PerspectiveMode();
void RedoGUI();