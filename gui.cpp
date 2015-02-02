

#include "gui.h"
#include "main.h"
#include "image.h"
#include "font.h"
#include "shader.h"
#include "menu.h"

CGUI g_GUI;

void OrthoMode(int left, int top, int right, int bottom)
{
	glMatrixMode(GL_PROJECTION);						
	glPushMatrix();									
	glLoadIdentity();								
	glOrtho(left, right, bottom, top, 0, 1);	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();		
	glDisable(GL_DEPTH_TEST);
}

void PerspectiveMode()
{
	glEnable(GL_DEPTH_TEST);
	glMatrixMode( GL_PROJECTION );							
	glPopMatrix();											
	glMatrixMode( GL_MODELVIEW );						
}

inline void DrawImage(unsigned int tex, float left, float top, float right, float bottom)
{
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);		glVertex2f(left, top);
	glTexCoord2f(0, 1);		glVertex2f(left, bottom);
	glTexCoord2f(1, 1);		glVertex2f(right, bottom);
	glTexCoord2f(1, 0);		glVertex2f(right, top);
	glEnd();
}

inline void DrawSquare(float r, float g, float b, float a, float left, float top, float right, float bottom)
{
	glDisable(GL_TEXTURE_2D);
	glColor4f(r, g, b, a);

	glBegin(GL_QUADS);
	glVertex2f(left, top);
	glVertex2f(left, bottom);
	glVertex2f(right, bottom);
	glVertex2f(right, top);
	glEnd();

	glColor4f(1, 1, 1, 1);
	glEnable(GL_TEXTURE_2D);
}

void CWidget::Image(char* filepath, float left, float top, float right, float bottom)
{
	type = IMAGE;
	CreateTexture(tex, filepath);
	pos[0] = left;
	pos[1] = top;
	pos[2] = right;
	pos[3] = bottom;
	ldown = false;
}

void CWidget::Image(unsigned int t, float left, float top, float right, float bottom)
{
	type = IMAGE;
	tex = t;
	pos[0] = left;
	pos[1] = top;
	pos[2] = right;
	pos[3] = bottom;
	ldown = false;
}

void CWidget::Button(char* filepath, float left, float top, float right, float bottom, void (*click)(), void (*overf)(), void (*out)())
{
	type = BUTTON;
	over = false;
	ldown = false;
	CreateTexture(tex, filepath);
	CreateTexture(bgtex, "gui\\bg.jpg");
	CreateTexture(bgovertex, "gui\\bgover.jpg");
	pos[0] = left;
	pos[1] = top;
	pos[2] = right;
	pos[3] = bottom;
	clickfunc = click;
	overfunc = overf;
	outfunc = out;
}

void CWidget::Button(char* filepath, float left, float top, float right, float bottom, void (*click2)(int p), int parm)
{
	type = BUTTON;
	over = false;
	ldown = false;
	CreateTexture(tex, filepath);
	CreateTexture(bgtex, "gui\\bg.jpg");
	CreateTexture(bgovertex, "gui\\bgover.jpg");
	pos[0] = left;
	pos[1] = top;
	pos[2] = right;
	pos[3] = bottom;
	clickfunc2 = click2;
	param = parm;
}

void CWidget::DropDown(char* n, int f, float left, float top, float width, void (*change)())
{
	type = DROPDOWN;
	name = n;
	font = f;
	opened = false;
	selected = 0;
	scroll = 0;
	mousescroll = false;
	ldown = false;
	changefunc = change;
	pos[0] = left;
	pos[1] = top;
	pos[2] = left+width;
	pos[3] = top+g_font[f].height;
	CreateTexture(frametex, "gui\\frame.jpg");
	CreateTexture(filledtex, "gui\\filled.jpg");
	CreateTexture(uptex, "gui\\up.jpg");
	CreateTexture(downtex, "gui\\down.jpg");
}

void CWidget::Text_draw()
{
	glDisable(GL_TEXTURE_2D);

	glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
	glDrawText(font, pos[0], pos[1], text.c_str());
	glColor4f(1, 1, 1, 1);

	glEnable(GL_TEXTURE_2D);
}

void CWidget::Link_draw()
{
	glDisable(GL_TEXTURE_2D);

	if(over)
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	else
		glColor4f(0.8f, 0.8f, 0.8f, 1.0f);

	glDrawText(font, pos[0], pos[1], text.c_str());

	glColor4f(1, 1, 1, 1);

	glEnable(GL_TEXTURE_2D);
}

void CWidget::Button_mousemove()
{
	if(g_mouse.x >= pos[0] && g_mouse.x <= pos[2] && g_mouse.y >= pos[1] && g_mouse.y <= pos[3])
	{
		if(overfunc != NULL)
			overfunc();
			
		over = true;
	}
	else
	{
		if(over && outfunc != NULL)
			outfunc();

		over = false;
	}
}

void CWidget::Link_mousemove()
{
	if(g_mouse.x >= pos[0] && g_mouse.y >= pos[1] && 
		g_mouse.x <= pos[0]+strlen(text.c_str())*g_font[font].height/2 && 
		g_mouse.y <= pos[1]+g_font[font].height)
		over = true;
	else
		over = false;
}

int CWidget::square()
{
	return g_font[font].height;
}

float CWidget::scrollspace()
{
	return g_font[font].height*(rowsshown()-1);
}

void CWidget::DropDown_draw()
{
	glColor4f(1, 1, 1, 1);

	DrawImage(frametex, pos[0], pos[1]+5, pos[2], pos[3]+5);

	if(!opened)
		DrawImage(downtex, pos[2]-square(), pos[1]+5, pos[2], pos[1]+5+square());

	glDrawText(font, pos[0]+30, pos[1], options[selected].c_str());
}

void CWidget::DropDown_draw2()
{
	if(!opened)
		return;

	glColor4f(1, 1, 1, 1);

	DrawImage(frametex, pos[0], pos[1]+5+g_font[font].height, pos[2], pos[3]+5+g_font[font].height*rowsshown());
	DrawImage(frametex, pos[2]-square(), pos[1]+5, pos[2], pos[3]+5+g_font[font].height*rowsshown());
	DrawImage(uptex, pos[2]-square(), pos[1]+5, pos[2], pos[1]+5+square());
	DrawImage(downtex, pos[2]-square(), pos[3]+5+g_font[font].height*rowsshown()-square(), pos[2], pos[3]+5+g_font[font].height*rowsshown());
	DrawImage(filledtex, pos[2]-square(), pos[3]+5+scrollspace()*topratio(), pos[2], pos[3]+5+scrollspace()*bottomratio());

	for(int i=(int)scroll; i<(int)scroll+rowsshown(); i++)
		glDrawText(font, pos[0]+30, pos[3]+g_font[font].height*(i-(int)scroll), options[i].c_str());
}


void CWidget::DropDown_mousemove()
{
	if(!mousescroll)
		return;

	int dy = g_mouse.y - mousedowny;
	int topspace = topratio() * scrollspace();
	int bottomspace = scrollspace() - bottomratio() * scrollspace();

	if(dy < 0 && abs(dy) > topspace)
	{
		scroll = 0;
		return;
	}
	else if(dy > 0 && dy > bottomspace)
	{
		scroll = options.size()-rowsshown();
		if(scroll < 0)
			scroll = 0;

		//char msg[128];
		//sprintf(msg, "bottomspace=%d", bottomspace);
		//MessageBox(NULL, msg, " ", NULL);

		return;
	}

	float topy = pos[3]+5+scrollspace()*topratio();
	float newtopy = topy + dy;

	//topratio = (float)scroll / (float)(options.size()-1);
	//topy = pos[3]+5+scrollspace*topratio
	//topy = pos[3]+5+scrollspace*((float)scroll / (float)(options.size()-1))
	//topy - pos[3] - 5 = scrollspace*(float)scroll / (float)(options.size()-1)
	//(topy - pos[3] - 5)*(float)(options.size()-1)/scrollspace = scroll

	scroll = (newtopy - pos[3] - 5)*(float)(options.size()-1)/scrollspace();
	mousedowny = g_mouse.y;
}

bool CWidget::DropDown_lbuttondown()
{
	if(opened)
	{
		for(int i=(int)scroll; i<(int)scroll+rowsshown(); i++)
		{
			// list item?
			if(g_mouse.x >= pos[0] && g_mouse.x <= pos[2]-square() && g_mouse.y >= pos[3]+g_font[font].height*(i-(int)scroll)
				&& g_mouse.y <= pos[3]+g_font[font].height*(i-(int)scroll+1))
			{
				selected = i;
				opened = false;
				if(changefunc != NULL)
					changefunc();
				return true;	// intercept mouse event
			}
		}

		// scroll bar?
		if(g_mouse.x >= pos[2]-square() && g_mouse.y >= pos[3]+5+scrollspace()*topratio() && g_mouse.x <= pos[2] && 
				g_mouse.y <= pos[3]+5+scrollspace()*bottomratio())
		{
			mousescroll = true;
			mousedowny = g_mouse.y;
			return true;	// intercept mouse event
		}

		// up button?
		if(g_mouse.x >= pos[2]-square() && g_mouse.y >= pos[1]+5 && g_mouse.x <= pos[2] && g_mouse.y <= pos[3]+5)
		{
			scroll--;
			if(scroll < 0)
				scroll = 0;

			return true;
		}

		// down button?
		if(g_mouse.x >= pos[2]-square() && g_mouse.y >= pos[3]+5+scrollspace() && g_mouse.x <= pos[2] && g_mouse.y <= pos[3]+5+scrollspace()+g_font[font].height)
		{
			scroll++;
			if(scroll+rowsshown() > options.size())
				scroll = options.size() - rowsshown();

			return true;
		}

		opened = false;

		return true;	// intercept mouse event
	}

	if(g_mouse.x >= pos[2]-square() && g_mouse.y >= pos[1]+5 && g_mouse.x <= pos[2] && g_mouse.y <= pos[1]+5+square())
	{
		opened = true;
		return true;
	}

	return false;
}

bool CWidget::DropDown_lbuttonup()
{
	if(mousescroll)
	{
		mousescroll = false;
		return true;	// intercept mouse event
	}

	return false;
}

void CView::draw()
{
	for(int i=0; i<widget.size(); i++)
		widget[i].draw();
		
	for(int i=widget.size()-1; i>=0; i--)
		widget[i].draw2();
}

void CGUI::draw()
{
	OrthoMode(0, 0, g_width, g_height);

	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, NULL);
    
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, NULL);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);

	glColor4f(1,1,1,1);

	for(int i=0; i<view.size(); i++)
		if(view[i].opened)
			view[i].draw();

	PerspectiveMode();
}

void AssignKey(int i, void (*down)(), void (*up)())
{
	g_GUI.assignKey(i, down, up);
}

void AssignMouseWheel(void (*wheel)(int delta))
{
	g_GUI.assignMouseWheel(wheel);
}

void AssignMouseMove(void (*mouse)())
{
	g_GUI.assignMouseMove(mouse);
}

void AssignLButton(void (*down)(), void (*up)())
{
	g_GUI.assignLButton(down, up);
}

void AssignRButton(void (*down)(), void (*up)())
{
	g_GUI.assignRButton(down, up);
}

void AssignMButton(void (*down)(), void (*up)())
{
	g_GUI.assignMButton(down, up);
}

void AddView(char* name, int page)
{
	CView view(name, page);
	g_GUI.view.push_back(view);
}

void AddImage(char* filepath, float left, float top, float right, float bottom)
{
	int last = g_GUI.view.size()-1;
	CWidget image;
	image.Image(filepath, left, top, right, bottom);
	g_GUI.view[last].widget.push_back(image);
}

void AddImage(CView* v, unsigned int tex, float left, float top, float right, float bottom)
{
	CWidget image;
	image.Image(tex, left, top, right, bottom);
	v->widget.push_back(image);
}

void AddLink(char* t, int f, float left, float top, void (*click)())
{
	int last = g_GUI.view.size()-1;
	CWidget link;
	link.Link(t, f, left, top, click);
	g_GUI.view[last].widget.push_back(link);
}

void AddButton(char* filepath, float left, float top, float right, float bottom, void (*click)(), void (*overf)(), void (*out)())
{
	int last = g_GUI.view.size()-1;
	CWidget button;
	button.Button(filepath, left, top, right, bottom, click, overf, out);
	g_GUI.view[last].widget.push_back(button);
}

void AddButton(char* filepath, float left, float top, float right, float bottom, void (*click2)(int p), int parm)
{
	int last = g_GUI.view.size()-1;
	CWidget button;
	button.Button(filepath, left, top, right, bottom, click2, parm);
	g_GUI.view[last].widget.push_back(button);
}

void AddButton(CView* v, char* filepath, float left, float top, float right, float bottom, void (*click)(), void (*overf)(), void (*out)())
{
	CWidget button;
	button.Button(filepath, left, top, right, bottom, click, overf, out);
	v->widget.push_back(button);
}

void AddText(char* n, char* t, int f, float left, float top)
{
	int last = g_GUI.view.size()-1;
	CWidget text;
	text.Text(n, t, f, left, top);
	g_GUI.view[last].widget.push_back(text);
}

void AddText(CView* v, char* n, char* t, int f, float left, float top)
{
	CWidget text;
	text.Text(n, t, f, left, top);
	v->widget.push_back(text);
}

void AddText(char* t, int f, float left, float top)
{
	int last = g_GUI.view.size()-1;
	CWidget text;
	text.Text(t, f, left, top);
	g_GUI.view[last].widget.push_back(text);
}

void AddDropDown(char* n, int f, float left, float top, float width, void (*change)())
{
	int last = g_GUI.view.size()-1;
	CWidget dropdown;
	dropdown.DropDown(n, f, left, top, width, change);
	g_GUI.view[last].widget.push_back(dropdown);
}

void AddOption(char* option)
{
	int last = g_GUI.view.size()-1;
	int lastdropdown = g_GUI.view[last].widget.size()-1;
	g_GUI.view[last].widget[lastdropdown].options.push_back(option);
}

void SelectOption()
{
	int last = g_GUI.view.size()-1;
	int lastdropdown = g_GUI.view[last].widget.size()-1;
	int lastoption = g_GUI.view[last].widget[lastdropdown].options.size()-1;
	CWidget* dropdown = &g_GUI.view[last].widget[lastdropdown];
	dropdown->selected = lastoption;
	dropdown->scroll = lastoption+1;
	if(dropdown->scroll+dropdown->rowsshown() > dropdown->options.size())
		dropdown->scroll = dropdown->options.size() - dropdown->rowsshown();
	if(dropdown->scroll < 0)
		dropdown->scroll = 0;
}

void CloseView(char* name)
{
	for(int i=0; i<g_GUI.view.size(); i++)
		if(stricmp(g_GUI.view[i].name.c_str(), name) == 0)
			g_GUI.view[i].opened = false;
}

void OpenSoleView(char* name, int page)
{
	for(int i=0; i<g_GUI.view.size(); i++)
	{
		if(stricmp(g_GUI.view[i].name.c_str(), name) == 0 && g_GUI.view[i].page == page)
			g_GUI.view[i].opened = true;
		else
			g_GUI.view[i].opened = false;
	}
}

bool OpenAnotherView(char* name, int page)
{
	for(int i=0; i<g_GUI.view.size(); i++)
	{
		if(stricmp(g_GUI.view[i].name.c_str(), name) == 0 && g_GUI.view[i].page == page)
		{
			g_GUI.view[i].opened = true;
			return true;
		}
	}

	return false;
}

void NextPage(char* name)
{
	int page = 0;

	for(int i=0; i<g_GUI.view.size(); i++)
	{
		if(stricmp(g_GUI.view[i].name.c_str(), name) == 0 && g_GUI.view[i].opened)
		{
			page = g_GUI.view[i].page;
			g_GUI.view[i].opened = false;
			break;
		}
	}

	if(!OpenAnotherView(name, page+1))
		OpenAnotherView(name, 0);
}

void RedoGUI()
{
	g_GUI.view.clear();

	AddView("loading");
	AddImage("gui\\logo.jpg", g_width/2 - g_height/2, 0, g_width/2 + g_height/2, g_height);
	AddText("status", "", ARIAL16, g_width/2 - g_height/2 + g_height*0.712f, g_height*0.349f);

	AddView("main");
	AddImage("gui\\logo.jpg", g_width/2 - g_height/2, 0, g_width/2 + g_height/2, g_height);
	AddLink("Join", ARIAL32, g_width/2 - 50, g_height/2 - g_font[ARIAL32].height*3, &Click_OpenJoin);
	AddLink("Host", ARIAL32, g_width/2 - 50, g_height/2 - g_font[ARIAL32].height*2, &Click_OpenHost);
	AddLink("Settings", ARIAL32, g_width/2 - 50, g_height/2 - g_font[ARIAL32].height*1, &Click_Settings);
	AddLink("Credits", ARIAL32, g_width/2 - 50, g_height/2 + g_font[ARIAL32].height*0, &Click_Credits);
	AddLink("Quit", ARIAL32, g_width/2 - 50, g_height/2 + g_font[ARIAL32].height*1, &Click_Quit);

	AddView("settings");
	AddImage("gui\\logo.jpg", g_width/2 - g_height/2, 0, g_width/2 + g_height/2, g_height);
	AddText("Fullscreen:", ARIAL32, g_width/2 - 225, g_height/2 - g_font[ARIAL32].height*2);
	AddDropDown("fullscreen", ARIAL32, g_width/2 - 25, g_height/2 - g_font[ARIAL32].height*2, 250, &Change_Fullscreen);
	AddOption("No");
	if(!g_fullscreen)
		SelectOption();
	AddOption("Yes");
	if(g_fullscreen)
		SelectOption();
	AddText("Resolution:", ARIAL32, g_width/2 - 225, g_height/2 - g_font[ARIAL32].height*1);
	AddDropDown("resolution", ARIAL32, g_width/2 - 25, g_height/2 - g_font[ARIAL32].height*1, 250, &Change_Resolution);
	for(int i=0; i<g_resolution.size(); i++)
	{
		char res[64];
		sprintf(res, "%d x %d", g_resolution[i].width, g_resolution[i].height);
		AddOption(res);
		if(g_resolution[i].width == g_selectedRes.width && g_resolution[i].height == g_selectedRes.height)
			SelectOption();
	}
	AddText("Bit depth:", ARIAL32, g_width/2 - 225, g_height/2 - g_font[ARIAL32].height*0);
	AddDropDown("bpp", ARIAL32, g_width/2 - 25, g_height/2 - g_font[ARIAL32].height*0, 250, &Change_BPP);
	for(int i=0; i<g_bpps.size(); i++)
	{
		char bpp[64];
		sprintf(bpp, "%d", g_bpps[i]);
		AddOption(bpp);
		if(g_bpps[i] == g_bpp)
			SelectOption();
	}
	AddLink("Back", ARIAL32, g_width/2 - 50, g_height/2 + g_font[ARIAL32].height*2, &Click_BackToMain);
	
	AddView("credits");
	AddImage("gui\\logo.jpg", g_width/2 - g_height/2, 0, g_width/2 + g_height/2, g_height);
	AddText("PROGRAMMING", ARIAL16, g_width/2 - 50, g_height/2 - g_font[ARIAL16].height*4);
	AddText("polyfrag", ARIAL16, g_width/2 - 50, g_height/2 - g_font[ARIAL16].height*3);
	//AddText("Volodar", ARIAL16, g_width/2 - 50, g_height/2 - g_font[ARIAL16].height*2);
	AddText("MODELS", ARIAL16, g_width/2 - 50, g_height/2 + g_font[ARIAL16].height*0);
	AddText("polyfrag", ARIAL16, g_width/2 - 50, g_height/2 + g_font[ARIAL16].height*1);
	//AddText("Sudden", ARIAL16, g_width/2 - 50, g_height/2 + g_font[ARIAL16].height*2);
	AddLink("Back", ARIAL32, g_width/2 - 50, g_height/2 + g_font[ARIAL32].height*4, &Click_BackToMain);
	
	AddView("play");
	AddText("chat0", "", ARIAL16, 5, 5+0*g_font[ARIAL16].height);
	AddText("chat1", "", ARIAL16, 5, 5+1*g_font[ARIAL16].height);
	AddText("chat2", "", ARIAL16, 5, 5+2*g_font[ARIAL16].height);
	AddText("chat3", "", ARIAL16, 5, 5+3*g_font[ARIAL16].height);
	AddText("chat4", "", ARIAL16, 5, 5+4*g_font[ARIAL16].height);
	AddText("chat5", "", ARIAL16, 5, 5+5*g_font[ARIAL16].height);
	AddText("fps", "", ARIAL16, 5, 5+7*g_font[ARIAL16].height);

	//AddView("quit");	//Ask the player if they want to quit the multiplayer game
	// TO DO

	if(g_mode == LOADING)
		OpenSoleView("loading");
	else if(g_mode == MENU)
		OpenSoleView("main");
	else if(g_mode == PLAY)
		OpenSoleView("play");
}

void Status(char* status)
{
	g_GUI.getview("loading")->gettext("status")->text = status;
}

void Chat(char* chat)
{
	g_GUI.getview("play")->gettext("chat0")->text = g_GUI.getview("play")->gettext("chat1")->text;
	g_GUI.getview("play")->gettext("chat1")->text = g_GUI.getview("play")->gettext("chat2")->text;
	g_GUI.getview("play")->gettext("chat2")->text = g_GUI.getview("play")->gettext("chat3")->text;
	g_GUI.getview("play")->gettext("chat3")->text = g_GUI.getview("play")->gettext("chat4")->text;
	g_GUI.getview("play")->gettext("chat4")->text = g_GUI.getview("play")->gettext("chat5")->text;
	g_GUI.getview("play")->gettext("chat5")->text = chat;
}