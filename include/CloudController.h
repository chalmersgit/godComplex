
#pragma once

#include "cinder/Vector.h"
#include "VectorSet.h"
#include <vector>

class CloudController {
public:
	CloudController();
	CloudController(ci::Vec2f, float, float, VectorSet*);
	
	void setColor(ci::Vec3f);
	
	void update();
	void draw();
	
	void applyForce(ci::Vec2f);

	void setPrevTime();
	void doRespawn();
	void setIndex(float iMin, float iMax);
	
	VectorSet* mParticleController;
	
	//CloudParticle* mCloudParticle;
	
	bool state;
	float indexMin;
	float indexMax;
	float lifeLength;
	float prevTime;
	float controllerAlpha;

	
	ci::Vec2f prevmLoc;
	ci::Vec2f mLoc;
	ci::Vec2f mVel;
	ci::Vec2f mAcc;
	ci::Vec2f mDesired;
	
	ci::Vec3f mCol;
	
	float mMaxForce;
	float mMaxSpeed;
	float mRadius;
	float mPrevTime;
};


