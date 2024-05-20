#pragma once

class MySpineSdl
{
public:
	MySpineSdl();
	~MySpineSdl();

	bool Load(string fileToLoad, SDL_Renderer* renderer);

	void SetPosition(float posX, float posY);

	void CenterToRect(RECT& rWindow);
	void ScaleToCenter(float scale, RECT& rWindow);
	void SetPremultiplied(bool b);

	void Draw(SDL_Renderer *renderer, double delta);

	Vector<string>& GetAnimationNames();

	void SetAnimation(int index);
	void PauseAnimation();
	void ResumeAnimation();
	void SetScale(float scale);
	float GetScale();

	string& GetSpineJsonFile();

	float GetPosX();
	float GetPosY();
	bool GetPremultipliedAlpha();
	int GetAnimation();
	void GetBounds(float& outx, float& outy, float& outw, float& outh);
private:
	SkeletonData* readSkeletonJsonData(const String& filename, Atlas* atlas, float scale);

	void release();
	void SetAnimation(string name);

	void reset();
private:
	string spineJsonFile;

	SkeletonDrawable* drawable;
	Vector<string> vecAnimationNames;

	float scale;
	float posX;
	float posY;
	bool premultipliedAlpha;
	int animationNum;

	bool isFapCeo;
	SkeletonDrawable* drawableHand;
};

