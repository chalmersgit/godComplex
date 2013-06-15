#pragma once
#include "cinder/Channel.h"
#include "cinder/Vector.h"

#include <vector>


class DirVector{
private:
	ci::Vec2f mLoc;
	ci::Vec2f mDir;
	ci::Vec2f mRotationVector;
	
public:
	DirVector();
	DirVector(ci::Vec2f, ci::Vec2f);
	void update(const ci::Channel32f &channel, const ci::Vec2f &windDirectionr);
	void draw();
	//Getters
	ci::Vec2f getLocation();
	ci::Vec2f getDirection();
	ci::Vec2f getRotation();
	
	
	float mRadius;
	
	
};

//#endif /* defined(__VectorFlowField__Particle__) */
