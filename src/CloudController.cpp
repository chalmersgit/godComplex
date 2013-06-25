#include "cinder/app/AppNative.h"

#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"
#include "cinder/Rand.h"

#include "CloudController.h"

using namespace ci;
using namespace ci::app;
using namespace std;

CloudController::CloudController(){
	
}


CloudController::CloudController(Vec2f loc, float ms, float mf, shared_ptr<VectorSet> pc){
	prevmLoc = loc;
	mLoc = loc;
	mMaxForce = mf;
	mMaxSpeed = ms;
	mParticleController = pc;
	
	mAcc = Vec2f(1.0f, 0.0f);
	mVel = Vec2f(0.0f, 0.0f);
	mRadius = 10.0f;
	
	mPrevTime = 0.0f;
	
	mCol = Vec3f(1, 1, 1);
	
	state = true;
	indexMin = -1.0f;
	indexMax = -1.0f;

	lifeLength = randFloat(40.0f, 60.0f); //Change this for life of cc. In seconds. 
	prevTime = 0.0f;
	controllerAlpha = 1.0f;
}

void CloudController::setPrevTime(){
	prevTime = timeline().getCurrentTime();
}

void CloudController::setColor(Vec3f col)
{
	mCol = col;
	
}

void CloudController::update(float ws, float wg){
	mVel += mAcc;
	mVel.limit(mMaxSpeed+ws);
	prevmLoc = mLoc;
	mLoc += mVel;
	
	//?
	mAcc *= mMaxForce;

	if(randFloat() < 0.05){
		mAcc *= wg;
	}
}


void CloudController::draw(){
	glPushMatrix();
	Rectf rect(mLoc.x, mLoc.y, mLoc.x + mRadius, mLoc.y + mRadius);
	gl::color(Color(mCol.x, mCol.y, mCol.z));
	gl::drawSolidRect(rect);
	glPopMatrix();
	
}


void CloudController::applyForce(Vec2f force){
	mAcc = mAcc + force;
}

void CloudController::doRespawn(){
	mLoc = Vec2f(randFloat(0.0f, getWindowWidth()), randFloat(0.0f, getWindowHeight()));
}

void CloudController::setIndex(float iMin, float iMax){
	indexMin = iMin;
	indexMax = iMax;
}

void CloudController::setMaxSpeed(float ms){
	mMaxSpeed = ms;
}



