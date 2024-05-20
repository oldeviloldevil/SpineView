#pragma once
#include "mainwindow.h"

#define WM_LOADNEXT WM_USER + 1001

class ListPlugWnd : public utils::Win32Window
{
public:
	ListPlugWnd(char* pstrSpineFile);
	~ListPlugWnd();
protected:
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& result);
	virtual void OnNcDestroy() {
		delete this;
	}
private:
	void BuildMenu();
	bool HandleMenu(int mouseX, int mouseY);
	void DrawMenu();
private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	uint64_t lastFrameTime;
	MySpineSdl* mySpineSdl;
	string spinefile;
	Vector<MySdlText*> vecAniTexts;
	MySdlText* pmaText = NULL;

	bool isDown;
	float lastX;
	float lastY;

	bool paused = false;
};

