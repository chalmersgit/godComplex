#pragma once
#include "DirVector.h"
#include <list>

#include <vector>

using namespace std;

class VectorSet{
public:
	VectorSet();
	~VectorSet();
	VectorSet(int res);
	void update(const ci::Channel32f &channel, const ci::Vec2f &mouseLoc);
	void draw();
	void addParticle(int xi, int yi, int res, ci::Vec2f);
	void addParticles(int amt);
	void removeParticles(int amt);
	void printFlowLookUpTable();
	DirVector*** createFlowField(int arraySizeX, int arraySizeY);
	
	ci::Vec2f flowLookUp(ci::Vec2f);
	
	list<DirVector*> mParticles;
	
	//    Vec2f mParticleLookUp[][];
	
	int mXRes, mYRes;
	
	DirVector*** mFlowLookUpTable;
	
};

