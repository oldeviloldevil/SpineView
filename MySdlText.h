#pragma once
class MySdlText
{
public:
	MySdlText();
	MySdlText(char* s);
	~MySdlText();

	static void TextOut(SDL_Renderer* renderer, char* text, int x, int y);

	MySdlText& setString(char* s);
	MySdlText& setFont(TTF_Font* f);
	MySdlText& setFontSize(int size);
	MySdlText& setStyle(int style);
	MySdlText& setColor(SDL_Color color);
	MySdlText& setPosition(int x, int y);
	SDL_Rect& GetBounds();

	void Draw(SDL_Renderer* renderer, int x, int y, int w, int h);
	void Draw(SDL_Renderer* renderer, int x, int y);
	void Draw(SDL_Renderer* renderer);

	void build(SDL_Renderer* renderer);
private:
	TTF_Font* font = NULL;
	std::string text;
	int fontsize = 24;
	int fontstyle = TTF_STYLE_NORMAL;
	SDL_Color fontcolor = { 0,0,0,255 };
	SDL_Rect textRect = { 0,0,0,0 };
	SDL_Texture* _texture = NULL;
};

