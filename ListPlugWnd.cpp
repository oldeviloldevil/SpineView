//#include "StdAfx.h"
#include <spine-sdl-cpp.h>
#include <SDL.h>
#include <SDL_ttf.h>
#ifdef _DEBUG
#include <SDL_test.h>
#endif
#include <windows.h>

using namespace std;
using namespace spine;

#include "MySpineSdl.h"
#include "MySdlText.h"
#include "ListPlugWnd.h"

extern void TraceLog(char* szFormat, ...);

BOOL IsLoopPaused(HWND hWnd) {
	return GetWindowWord(hWnd, sizeof(LONG_PTR));
}
void PauseLoop(HWND hWnd, BOOL bPause) {
	SetWindowWord(hWnd, sizeof(LONG_PTR), bPause);
}
DWORD WINAPI ThreadFunc(LPVOID param) {
	HWND hWnd = (HWND)param;
	while (::IsWindow(hWnd)) {
		BOOL bPause = IsLoopPaused(hWnd);
		if (!bPause) {
			InvalidateRect(hWnd, NULL, false);
		}
		Sleep(15);
	}
	TraceLog("thread %d quit\n", hWnd);
	return 0;
}

ListPlugWnd::ListPlugWnd(char* pstrSpineFile)
{
	window = NULL;
	renderer = NULL;
	lastFrameTime = 0;
	mySpineSdl = new MySpineSdl();
	spinefile = pstrSpineFile;
	isDown = false;
}


ListPlugWnd::~ListPlugWnd()
{
	if(mySpineSdl)
		delete mySpineSdl;
}

bool ListPlugWnd::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& result)
{
	result = 0;
	if (uMsg == WM_CREATE) {

		//SDL_Window *window = SDL_CreateWindow("Spine SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
		window = SDL_CreateWindowFrom(handle()); //嵌入式
		if (!window) {
			TraceLog("Error: %s", SDL_GetError());
			result = -1;
		}
		::CreateThread(NULL, 0, ThreadFunc, handle(), 0, NULL);
		return true;
	}
	else if (uMsg == WM_SIZE) {
		int w = LOWORD(lParam);
		int h = HIWORD(lParam);
		if (w > 0 && h > 0 && !renderer) {
			//render 必须在这里创建,否则size为0时会崩溃
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (!renderer) {
				TraceLog("Error: %s", SDL_GetError());
				result = -1;
				return true;
			}

			//////有了render才可以加载
			if (!mySpineSdl->Load(spinefile, renderer)) {
				result = -1;
				return true;
			}

			BuildMenu();

			lastFrameTime = SDL_GetPerformanceCounter();
		}
		if (w > 0 && h > 0 && renderer && mySpineSdl) {
			RECT r;
			r.left = 0; r.top = 0; r.right = w; r.bottom = h;
			mySpineSdl->CenterToRect(r);
		}
		return true;

	}
	else if (uMsg == WM_PAINT) {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(handle(), &ps);

		// 在paint里做绘制
		SDL_SetRenderDrawColor(renderer, 94, 93, 96, 255);
		SDL_RenderClear(renderer);

		uint64_t now = SDL_GetPerformanceCounter();
		double deltaTime = (now - lastFrameTime) / (double)SDL_GetPerformanceFrequency();
		lastFrameTime = now;

		mySpineSdl->Draw(renderer, deltaTime);

#ifdef _DEBUG
		SDL_Rect r = { mySpineSdl->GetPosX(),mySpineSdl->GetPosY(),20,20 };
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_RenderFillRect(renderer, &r);
		RECT rr;
		GetClientRect(this->handle(), &rr);
		r = { (rr.left+rr.right)/2,(rr.top+rr.bottom)/2,10,10 };
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_RenderFillRect(renderer, &r);

		float x, y, w, h;
		mySpineSdl->GetBounds(x, y, w, h);
		SDL_FRect fr = { x,y,w,h };
		SDL_RenderDrawRectF(renderer, &fr);

		char buf[256];
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		sprintf(buf, "scale: %f", mySpineSdl->GetScale());
		SDLTest_DrawString(renderer, 500, 20, buf);
		sprintf(buf, "b:%.4f,%.4f - %.4f,%.4f", x,y,x+w,y+h);
		SDLTest_DrawString(renderer, 800, 20 , buf);
		sprintf(buf, "bc: %f %f", x + w / 2, y + h / 2);
		SDLTest_DrawString(renderer, 800, 40, buf);
		sprintf(buf, "wc: %d %d", r.x + r.w / 2, r.y + r.h / 2);
		SDLTest_DrawString(renderer, 800, 60, buf);
#endif

		DrawMenu();

		SDL_RenderPresent(renderer);

	//	InvalidateRect(handle(), NULL, false);
		EndPaint(handle(), &ps);
		return true;
	}
	else if (uMsg == WM_LOADNEXT) {
		if (!mySpineSdl->Load((char*)wParam, renderer))
		{
			result = -1;
		}
		BuildMenu();
		RECT r;
		GetClientRect(handle(), &r);
		mySpineSdl->CenterToRect(r);
		return true;
	}
	else if (uMsg == WM_RBUTTONDOWN) {
		paused = !paused;
		if (paused)
			mySpineSdl->PauseAnimation();
		else
			mySpineSdl->ResumeAnimation();
		//BOOL bPause = IsLoopPaused(handle());
		//bPause = !bPause;
		//PauseLoop(handle(), bPause);
		return true;
	}
	else if (uMsg == WM_LBUTTONDOWN) {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		bool handled = HandleMenu(x, y);
		if (!handled) {
			isDown = true;
			lastX = x;
			lastY = y;
		}

		return true;
	}
	else if (uMsg == WM_MOUSEMOVE) {
		if (isDown) {
			float deltaX = LOWORD(lParam) - lastX;
			float deltaY = HIWORD(lParam) - lastY;
			lastX = LOWORD(lParam);
			lastY = HIWORD(lParam);
			float posX = mySpineSdl->GetPosX();
			float posY = mySpineSdl->GetPosY();
			posX += deltaX;
			posY += deltaY;
			mySpineSdl->SetPosition(posX, posY);
		}
		return true;
	}
	else if (uMsg == WM_LBUTTONUP) {
		isDown = false;
		return true;
	}
	else if (uMsg == WM_MOUSEHWHEEL || uMsg == 0x20A || uMsg == 0x20E) {
		short delta = HIWORD(wParam);
		float scale = mySpineSdl->GetScale();
		scale *= delta > 0 ? 1.1 : 0.9;
		scale = max(0.001, min(scale, 1000));
		RECT r;
		GetClientRect(handle(), &r);
		mySpineSdl->ScaleToCenter(scale, r);
		return true;
	}
	else if (uMsg == WM_LBUTTONDBLCLK) {
		RECT r;
		GetClientRect(handle(), &r);
		mySpineSdl->CenterToRect(r);
	}
	else if (uMsg == WM_DESTROY) {
		SDL_DestroyWindow(window);
		return true;
	}
	return false;
}

void ListPlugWnd::BuildMenu()
{
	for (int i = 0; i < vecAniTexts.size(); i++) {
		delete vecAniTexts[i];
	}
	vecAniTexts.clear();
	Vector<string> vecAniNames = mySpineSdl->GetAnimationNames();
	for (int i = 0; i < vecAniNames.size(); i++) {
		MySdlText* txt = new MySdlText();
		txt->setColor({ 255,0,0,255 })
			.setString((char*)vecAniNames[i].c_str())
			.setPosition(20, 10 + i * 24)
			.build(renderer);
		vecAniTexts.add(txt);
	}

	if (pmaText) delete pmaText;
	pmaText = new MySdlText();
	pmaText->setColor({ 255,0,0,255 })
		.setString("Premultiplied Alpha")
		.setPosition(20, 10 + vecAniNames.size() * 24 + 20)
		.build(renderer);
}

bool ListPlugWnd::HandleMenu(int mouseX, int mouseY)
{
	POINT localPosition = { mouseX, mouseY };
	for (int i = 0; i < vecAniTexts.size(); i++) {
		SDL_Rect boundingBox = vecAniTexts[i]->GetBounds();
		RECT r = { boundingBox.x, boundingBox.y, boundingBox.x + boundingBox.w, boundingBox.y + boundingBox.h };
		if (PtInRect(&r,localPosition))
		{
			mySpineSdl->SetAnimation(i);
			return true;
		}
	}
	SDL_Rect boundingBox = pmaText->GetBounds();
	RECT r = { boundingBox.x, boundingBox.y, boundingBox.x + boundingBox.w, boundingBox.y + boundingBox.h };
	if (PtInRect(&r, localPosition))
	{
		bool b = mySpineSdl->GetPremultipliedAlpha();
		b = !b;
		mySpineSdl->SetPremultiplied(b);
		return true;
	}
	return false;
}

void ListPlugWnd::DrawMenu()
{
	for (int i = 0; i < vecAniTexts.size(); i++) {
		vecAniTexts[i]->Draw(renderer);
	}

	pmaText->Draw(renderer);

	int j = mySpineSdl->GetAnimation();
	SDL_Point vertices[3] = {
		{10,10 + 20 + 24 * j - 4},
		{10,15 + 20 + 24 * j - 4 },
		{20,5 + 20 + 24 * j - 4}
	};

	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderDrawLines(renderer, vertices, 3);

	int n = vecAniTexts.size();
	if (mySpineSdl->GetPremultipliedAlpha()) {
		SDL_Point vertices[3] = {
			{10,10 + 40 + 24 * n - 4},
			{10,15 + 40 + 24 * n - 4 },
			{20,5 + 40 + 24 * n - 4}
		};
		SDL_RenderDrawLines(renderer, vertices, 3);
	}

}
