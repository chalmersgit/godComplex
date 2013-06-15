#include "cinder/app/AppNative.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Perlin.h"
#include "cinder/CinderMath.h"
#include <vector>
#include <stdlib.h>

#include "VectorSet.h"

using namespace ci;
using namespace ci::app;
using std::list;

VectorSet::VectorSet()
{
	
}

VectorSet::~VectorSet()
{
	/*
	 if(mFlowLookUpTable != NULL){
	 for (int i = 0; i < mXRes; i++){
	 //console() << i << endl;
	 free(mFlowLookUpTable[i]);
	 }
	 free(mFlowLookUpTable);
	 }*/
}

VectorSet::VectorSet(int res)
{
	mXRes = app::getWindowWidth()/res;
	mYRes = app::getWindowHeight()/res;
	
	mFlowLookUpTable = createFlowField(mXRes, mYRes);
	
	Perlin perlin = Perlin(32, clock() * 0.1f);
	Vec2f v = Vec2f(0.05f, 0.05f);
	float noise = perlin.fBm(v);
	float angle = noise * 15.0f;
	float xoff = 0.1f;
	for(int y=0; y<mYRes; y++){
		float yoff = 0.1f;
		for(int x=0; x<mXRes; x++){
			float thetaX = cos(angle) * Rand::randFloat(0.0001, xoff);
			float thetaY = sin(angle) * Rand::randFloat(0.0001, yoff);
			
			Vec2f rotationVector(thetaX, thetaY);
			
			addParticle(x, y, res, rotationVector);
			yoff += 0.1f;
		}
		xoff += 0.1f;
	}
}

void VectorSet::update( const Channel32f &channel, const Vec2f &windDirection)
{
	for(list<DirVector*>::iterator p = mParticles.begin(); p != mParticles.end(); ++p){
		(*p)->update(channel, windDirection);
	}
}

void VectorSet::draw()
{
	for (list<DirVector*>::iterator p = mParticles.begin(); p != mParticles.end(); ++p){
		(*p)->draw();
	}
}

void VectorSet::addParticle(int xi, int yi, int res, Vec2f rotationVector)
{
	float x = (xi + 0.5f) * (float)res;
	float y = (yi + 0.5f) * (float)res;
	rotationVector.normalize();
	//console() << "Rotation: " << rotationVector << endl;
	DirVector* p = new DirVector(Vec2f(x, y), rotationVector);
	mParticles.push_back(p);
	
	mFlowLookUpTable[xi][yi] = p;
	
}

DirVector*** VectorSet::createFlowField(int arraySizeX, int arraySizeY) {
	DirVector*** theArray;
	theArray = (DirVector***) malloc(arraySizeX*sizeof(DirVector*));
	for (int i = 0; i < arraySizeX; i++){
		theArray[i] = (DirVector**) malloc(arraySizeY*sizeof(DirVector));
	}
	return theArray;
}

void VectorSet::addParticles(int amt)
{
	for(int i=0; i<amt; i++){
		for(int i=0; i<amt; i++){
			float x = Rand::randFloat(app::getWindowWidth());
			float y = Rand::randFloat(app::getWindowHeight());
			mParticles.push_back(new DirVector(Vec2f(x, y), Vec2f(1.0f, 0.0f)));
		}
	}
}

void VectorSet::removeParticles(int amt)
{
	for(int i=0; i<amt; i++){
		mParticles.pop_back();
	}
	
}

void VectorSet::printFlowLookUpTable(){
	if(mFlowLookUpTable != NULL){
		for (int x = 0; x < mXRes; ++x){
			for(int y = 0; y < mYRes; ++y){
				console() << "[" << x << "," << y << "] " << mFlowLookUpTable[x][y]->getLocation() << ", " << mFlowLookUpTable[x][y]->getDirection() << endl;
			}
		}
	}
}

Vec2f VectorSet::flowLookUp(Vec2f lookup){
	int row = constrain((lookup.y/(float)app::getWindowHeight()) * mYRes, 0.0f, (float)mYRes-1);
	int column = constrain((lookup.x/(float)app::getWindowWidth()) * mXRes, 0.0f, (float)mXRes-1);
	//console() << lookup << endl;
	if(mFlowLookUpTable != NULL){
		/*
		 Vec2f v = mFlowLookUpTable[row][column].getDirection();
		 console() << "Dir: " << v << endl;
		 v.normalize();
		 console() << "nDir: " <<v << endl << endl;
		 
		 Vec2f v2 = mFlowLookUpTable[row][column].getRotation();
		 console() << "Rot: " << v2 << endl;
		 v2.normalize();
		 console() << "nRot: " << v2 << endl << endl;
		 */
		//console() << "Row,Col: " <<  row << ", " << column << endl;
		Vec2f flowFieldDirection = mFlowLookUpTable[column][row]->getDirection(); //TODO: May need to change this to getDirection
		//console() << column << ", " << row << endl;
		//check col row
		
		//flowFieldDirection.x *= -1;
		//flowFieldDirection.y *= -1;
		return flowFieldDirection;
	}
	else{
		console() << "Error: No flow look up table found, returning zero vector" << endl;
		return Vec2f(0, 0);
	}
	//return field.at(column).at(row); //NOTE: working on
}




