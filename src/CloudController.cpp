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


CloudController::CloudController(Vec2f loc, float ms, float mf, VectorSet* pc){
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
	
	/*gl::pushMatrices();
	 mCloudParticle = new CloudParticle();
	 gl::popMatrices();*/
}

void CloudController::setColor(Vec3f col)
{
	mCol = col;
	
}

void CloudController::update(){
	//console() << "1 " << mLoc << ", " << mVel << ", " << mAcc << endl;
	mVel += mAcc;
	
	//console() << "2 " << mLoc << ", " << mVel << ", " << mAcc << endl;
	
	mVel.limit(mMaxSpeed);
	
	//console() << "3 " << mLoc << ", " << mVel << ", " << mAcc << endl;
	
	prevmLoc = mLoc;
	
	mLoc += mVel;
	
	//?
	mAcc *= mMaxForce;
	
	//console() << "4 " << mLoc << ", " << mVel << ", " << mAcc << endl << endl;
	
	/*if(mCloudParticle !=NULL){
	 mCloudParticle->setPos(mLoc);
	 mCloudParticle->update();
	 }*/
	
}

void CloudController::draw(){
/*
	//console() << mCol << endl;
	glPushMatrix();
	
	Rectf rect(mLoc.x, mLoc.y, mLoc.x + mRadius, mLoc.y + mRadius);
	gl::color(Color(mCol.x, mCol.y, mCol.z));
	
	if(mCol.x >= 1){
		//console() << "blue: " << mLoc << endl;
		gl::drawSolidRect(rect);
	}
	else{
		gl::drawSolidRect(rect);
	}
	
	glPopMatrix();
	
	
	
	if(mCloudParticle !=NULL){
	 //console() << mCol << endl;
	 glPushMatrix();
	 mCloudParticle->draw();
	 glPopMatrix();
	 }*/
	
}


void CloudController::applyForce(Vec2f force){
	//console() << "5 " << mAcc << endl;
	mAcc = mAcc + force;
	//console() << "6 " << mAcc << endl << endl;
}

/*
 void CloudController::checkRespawn(){
 if((timeline().getCurrentTime() - mPrevTime) > 5.0f){ //change timer to somewhat random/longer
 //Respawn the controller
 mLoc = Vec2f(randFloat(0.0f, getWindowWidth()), randFloat(0.0f, getWindowHeight()));
 
 //Fade the following particles
 
 mPrevTime = timeline().getCurrentTime();
 }
 }
 */

void CloudController::doRespawn(){
	mLoc = Vec2f(randFloat(0.0f, getWindowWidth()), randFloat(0.0f, getWindowHeight()));
	//console() << "reset: " << mLoc << endl;
}

void CloudController::setIndex(float iMin, float iMax){
	indexMin = iMin;
	indexMax = iMax;
}

