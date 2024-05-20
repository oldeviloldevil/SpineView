#include <SDL.h>
#include <SDL_ttf.h>
using namespace std;
#include <string>

#include "MySdlText.h"

extern void TraceLog(char* szFormat, ...);
extern std::string GetDllPath();


MySdlText::MySdlText(char* s)
{
	text = s;
}

MySdlText::MySdlText()
{

}

MySdlText::~MySdlText()
{
	TTF_CloseFont(font);
	SDL_DestroyTexture(_texture);//ÊÍ·ÅÎÆÀí
}


void MySdlText::TextOut(SDL_Renderer* renderer, char* text, int x, int y)
{

}

MySdlText& MySdlText::setString(char* s)
{
	text = s; return *this;
}

MySdlText& MySdlText::setFont(TTF_Font* f)
{
	if (font)
		TTF_CloseFont(font);
	font = f;
	return *this;
}

MySdlText& MySdlText::setFontSize(int size)
{
	fontsize = size; 
	return *this;
}

MySdlText& MySdlText::setStyle(int style)
{
	fontstyle = style; 
	return *this;
}

MySdlText& MySdlText::setColor(SDL_Color color)
{
	fontcolor = color; 
	return *this;
}

MySdlText& MySdlText::setPosition(int x, int y)
{
	textRect.x = x;
	textRect.y = y;
	return *this;
}

SDL_Rect& MySdlText::GetBounds()
{
	return textRect;
}

void MySdlText::Draw(SDL_Renderer* renderer)
{
	if (!_texture) {
		build(renderer);
	}
	SDL_RenderCopy(renderer, _texture, NULL, &textRect);
}

void MySdlText::Draw(SDL_Renderer* renderer, int x, int y)
{
	if (!_texture) {
		build(renderer);
	}
	textRect.x = x;
	textRect.y = y;
	SDL_RenderCopy(renderer, _texture, NULL, &textRect);
}

void MySdlText::Draw(SDL_Renderer* renderer, int x, int y, int w, int h)
{
	if (!_texture) {
		build(renderer);
	}
	textRect.x = x;
	textRect.y = y;
	textRect.w = w;
	textRect.h = h;
	SDL_RenderCopy(renderer, _texture, NULL, &textRect);
}

void MySdlText::build(SDL_Renderer* renderer)
{
	if (_texture) {
		SDL_DestroyTexture(_texture);
	}
	if (!font) {
		string fontFile = GetDllPath() + "arial.ttf";
		font = TTF_OpenFont(fontFile.c_str(), fontsize);
		if (!font) {
			printf("globalFont Error:");
			return;
		}
		TTF_SetFontSize(font, fontsize);
		TTF_SetFontStyle(font, fontstyle);
	}

	SDL_Surface *text_surface = TTF_RenderUTF8_Blended(font, text.c_str(), fontcolor);
	_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	textRect.w = text_surface->w;
	textRect.h = text_surface->h;
	SDL_FreeSurface(text_surface);
	TTF_CloseFont(font);
	font = NULL;
}

