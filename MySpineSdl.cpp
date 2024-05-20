
#include <spine-sdl-cpp.h>
#include <SDL.h>
#include <windows.h>
#include <io.h>
using namespace std;
using namespace spine;

#include "MySpineSdl.h"

extern void TraceLog(char* szFormat, ...);


MySpineSdl::MySpineSdl()
{
	isFapCeo = false;
	scale = 1;
	drawable = NULL;
	drawableHand = NULL;
}

MySpineSdl::~MySpineSdl()
{
	release();
}

bool MySpineSdl::Load(string fileToLoad, SDL_Renderer* renderer)
{
	reset();

	spineJsonFile = fileToLoad;
	string jsonPath = spineJsonFile.substr(0, spineJsonFile.find_last_of('\\') + 1);
	string ext = spineJsonFile.substr(spineJsonFile.find_last_of('.'));
	string jsonName = spineJsonFile.substr(0, spineJsonFile.find_last_of('.'));

	if (ext == ".json") {
		ext = ".txt";
	}
	if (ext.compare(".prefab") == 0) {
		isFapCeo = true;
	}
	else {
		isFapCeo = false;
	}

	string atlasFile = spineJsonFile.substr(0, spineJsonFile.find_last_of('.')) + ".atlas" + ext;

	spine::SDLTextureLoader textureLoader(renderer);
	Atlas* atlas = new spine::Atlas(atlasFile.c_str(), &textureLoader);
	spine::AtlasAttachmentLoader attachmentLoader(atlas);
	spine::SkeletonJson json(&attachmentLoader);
	json.setScale(1.0f);
	spine::SkeletonData *skeletonData = json.readSkeletonDataFile(spineJsonFile.c_str());
	drawable = new spine::SkeletonDrawable(skeletonData);
	drawable->skeleton->setPosition(400, 500);
	drawable->skeleton->setToSetupPose();
	drawable->update(0);
	drawable->setUsePremultipliedAlpha(true);

	if (isFapCeo) {
		string atlasHand = jsonPath + "hand.atlas" + ext;
		string jsonHand = jsonPath + "hand" + ext;;
		if (_access(jsonHand.c_str(),0) == 0) {
			Atlas* atlas = new Atlas(atlasHand.c_str(), &textureLoader);

			spine::AtlasAttachmentLoader attachmentLoader(atlas);
			spine::SkeletonJson json(&attachmentLoader);
			json.setScale(1);
			spine::SkeletonData *skeletonData = json.readSkeletonDataFile(jsonHand.c_str());
			drawableHand = new spine::SkeletonDrawable(skeletonData);
			drawableHand->skeleton->setToSetupPose();
			drawableHand->update(0);
			drawableHand->setUsePremultipliedAlpha(true);

		}
		else {
			isFapCeo = false;
		}
	}

	Vector<Animation*> aniVec = skeletonData->getAnimations();

	for (int i = 0; i < aniVec.size(); i++) {
		vecAnimationNames.add(aniVec[i]->getName().buffer());
	}

	drawable->animationState->setAnimation(0, aniVec[0]->getName(), true);
	if (isFapCeo) {
		drawableHand->animationState->setAnimation(0, aniVec[0]->getName(), true);
	}

	return true;
}

void MySpineSdl::SetPosition(float posX, float posY)
{
	drawable->skeleton->setPosition(posX, posY);
	if (isFapCeo) {
		drawableHand->skeleton->setPosition(posX, posY);
	}
	this->posX = posX;
	this->posY = posY;
}

void MySpineSdl::CenterToRect(RECT& rWindow)
{
	float x, y, w, h;
	Vector<float> v;
	drawable->skeleton->getBounds(x, y, w, h, v);
	float cx = x + w / 2;
	float cy = y + h / 2;
	int wcx = (rWindow.left + rWindow.right) / 2;
	int wcy = (rWindow.top + rWindow.bottom) / 2;
	float px = drawable->skeleton->getX();
	float py = drawable->skeleton->getY();
	float npx = px + wcx - cx;
	float npy = py + wcy - cy;
	SetPosition(npx, npy);
	drawable->skeleton->updateWorldTransform();	//ÁîboundsÉúÐ§
	if (isFapCeo) {
		drawableHand->skeleton->updateWorldTransform();
	}
}

void MySpineSdl::ScaleToCenter(float scale, RECT& rWindow)
{
	// scale to bounds center
	//float oldCenterX, oldCenterY;
	//GetBoundsCenter(oldCenterX, oldCenterY);

	//SetScale(scale);
	//drawable->skeleton->updateWorldTransform();
	//if (isFapCeo)
	//	drawableHand->skeleton->updateWorldTransform();

	//float newCenterX, newCenterY;
	//GetBoundsCenter(newCenterX, newCenterY);
	//float nx = GetPosX() - newCenterX + oldCenterX;
	//float ny = GetPosY() - newCenterY + oldCenterY;
//////////////////scale to window center/////////////////////////////////
	//float sx = drawable->skeleton->getX();
	//float sy = drawable->skeleton->getY();
	//float bx, by, w, h;
	//Vector<float> v;
	//drawable->skeleton->getBounds(bx, by, w, h, v);
	//int wcx = (rWindow.left + rWindow.right) / 2;
	//int wcy = (rWindow.top + rWindow.bottom) / 2;
	//float fx = (wcx - bx) / w;
	//float fy = (wcy - by) / h;
	//
	//SetScale(scale);
	//drawable->skeleton->updateWorldTransform();

	//drawable->skeleton->getBounds(bx, by, w, h, v);
	//float wcx1 = bx + fx * w;
	//float wcy1 = by + fy * h;

	//float nx = sx- wcx1 + wcx;
	//float ny = sy- wcy1 + wcy;
	///////////////////////////////////
	float sx = drawable->skeleton->getX();
	float sy = drawable->skeleton->getY();
	float bx, by, w, h, w1, h1;
	Vector<float> v;
	drawable->skeleton->getBounds(bx, by, w, h, v);
	int wcx = (rWindow.left + rWindow.right) / 2;
	int wcy = (rWindow.top + rWindow.bottom) / 2;
	float dx = (wcx - bx);
	float dy = (wcy - by);

	SetScale(scale);
	drawable->skeleton->updateWorldTransform();

	drawable->skeleton->getBounds(bx, by, w1, h1, v);
	float wcx1 = bx + dx * w1 / w;
	float wcy1 = by + dy * h1 / h;

	float nx = sx - wcx1 + wcx;
	float ny = sy - wcy1 + wcy;
	SetPosition(nx, ny);
	drawable->skeleton->updateWorldTransform();
}

void MySpineSdl::SetPremultiplied(bool b)
{
	premultipliedAlpha = b;
	drawable->setUsePremultipliedAlpha(b);
	if (isFapCeo) {
		drawableHand->setUsePremultipliedAlpha(b);
	}
}

SkeletonData* MySpineSdl::readSkeletonJsonData(const String& filename, Atlas* atlas, float scale)
{
	//SkeletonJson json(atlas);
	//json.setScale(scale);
	//auto skeletonData = json.readSkeletonDataFile(filename);
	//if (!skeletonData) {
	//	TraceLog("%s\n", json.getError().buffer());
	//}
	//return skeletonData;
	return NULL;
}

void MySpineSdl::release()
{
	if (drawable) {
		delete drawable;
		drawable = NULL;
	}
	if (drawableHand) {
		delete drawableHand;
		drawableHand = NULL;
	}
}

void MySpineSdl::Draw(SDL_Renderer *renderer, double delta)
{
	drawable->update(delta);
	drawable->draw(renderer);

	if (isFapCeo) {
		drawableHand->update(delta);
		drawableHand->draw(renderer);
	}
}

Vector<string>& MySpineSdl::GetAnimationNames()
{
	return vecAnimationNames;
}

void MySpineSdl::SetAnimation(string name)
{
	drawable->animationState->setAnimation(0, name.c_str(), true);
	if (isFapCeo) {
		drawableHand->animationState->setAnimation(0, name.c_str(), true);
	}
}

void MySpineSdl::SetAnimation(int index)
{
	if (index < vecAnimationNames.size()) {
		animationNum = index;
		string name = vecAnimationNames[index];
		SetAnimation(name);
	}
}

void MySpineSdl::PauseAnimation()
{
	drawable->animationState->setTimeScale(0);
	if (isFapCeo) {
		drawableHand->animationState->setTimeScale(0);
	}
}

void MySpineSdl::ResumeAnimation()
{
	drawable->animationState->setTimeScale(1);
	if (isFapCeo) {
		drawableHand->animationState->setTimeScale(1);
	}
}

void MySpineSdl::SetScale(float scale)
{
	this->scale = scale;
	drawable->skeleton->setScaleX(scale);
	drawable->skeleton->setScaleY(scale);
	if (isFapCeo) {
		drawableHand->skeleton->setScaleX(scale);
		drawableHand->skeleton->setScaleY(scale);
	}
}

float MySpineSdl::GetScale()
{
	return scale;
}

string& MySpineSdl::GetSpineJsonFile()
{
	return spineJsonFile;
}

float MySpineSdl::GetPosX()
{
	return posX;
}

float MySpineSdl::GetPosY()
{
	return posY;
}

bool MySpineSdl::GetPremultipliedAlpha()
{
	return premultipliedAlpha;
}

int MySpineSdl::GetAnimation()
{
	return animationNum;
}

void MySpineSdl::GetBounds(float& outx, float& outy, float& outw, float& outh)
{
	Vector<float> v;
	drawable->skeleton->getBounds(outx, outy, outw, outh, v);
}

void MySpineSdl::reset()
{
	spineJsonFile.clear();
	vecAnimationNames.clear();
	scale = 1;
	posX = 0;
	posY = 0;
	isFapCeo = false;
	premultipliedAlpha = true;
	animationNum = 0;
	release();
}
