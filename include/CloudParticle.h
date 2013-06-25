
#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Rand.h"
#include "cinder/Surface.h"
#include "cinder/Text.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Perlin.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/GlslProg.h"

#include "cinder/ObjLoader.h"
#include <vector>

#include "CloudController.h"
#include "Resources.h"

#include "LeapController.h"

#include "cinder/params/Params.h"

#include <boost\shared_ptr.hpp>


using namespace ci;
using namespace std;

#define WIDTH 1920 //NOTE: WIDTH HEIGHT FOR LEAP WINDOW MAPPING
#define HEIGHT 1080 // Leap


#define PARTICLES_X 1600 //This should be sqrt(WIDTH * HEIGHT)
#define PARTICLES_Y 1600

class CloudParticle {
public:
	CloudParticle();
	CloudParticle(shared_ptr<vector<shared_ptr<CloudController>>>, int nc);
	void setup();
	//void resize( ResizeEvent event );
	void update();
	void draw();
	void initFBO();
	void setPos(ci::Vec2f);
	void updateGPUcloudControllers();
	void mouseDown(ci::app::MouseEvent event );
	void mouseUp(ci::app::MouseEvent event );
	void mouseDrag(ci::app::MouseEvent event );
	
	bool mIsFullScreen;
	
	//Input
	bool	mMouseDown;
	Vec2f	mMousePos;
	Vec2f	mMousePosNormalised;
	int		mMouseDownInt;
	
	Vec2f particleTexRes;
	int activeControllersCount;
	Vec2f prevControllers[16];
	Vec2f controllers[16];
	float minIndices[16];
	float maxIndices[16];
	float controllerAlpha[16];

	float cloudWidth;
	float cloudHeight;

	Vec2i currentWindowSize;
	Perlin mPerlin;
	Vec3f mVertPos;
	
	gl::VboMesh mVbo;
	gl::Fbo mFbo[2];
	
	gl::GlslProg mPosShader;
	gl::GlslProg mVelShader;
	
	gl::Texture mPosTex;
	gl::Texture mVelTex;
	gl::Texture mVelTex2;
	gl::Texture mInfoTex;
	gl::Texture mNoiseTex;
	gl::Texture mSpriteTex;

	ci::Vec2f		mLoc;
	
	int				mPos;
	int				mVel;
	int				mInfo;
	int				mSprite;
	int				mNoise;
	
	int				mBufferIn;
	int				mBufferOut;
	int				mFrameCounter;
	
	//Leap
	int minX;
	int maxX;
	int minY;
	int maxY;
	
	
	Controller		mController;
	shared_ptr<LeapController>	mLeapController;
	ci::Vec2f			finger1;
	ci::Vec2f			finger2;
	ci::Vec2f			finger3;
	ci::Vec2f			finger4;
	ci::Vec2f			finger5;
	bool				hasFingers;

	Vec2f leapFingersPos[40];
	Vec2f leapFingersVel[40];
	int maxFingers;
	float fingerTrackerTimer;
	
	shared_ptr<vector<shared_ptr<CloudController>>> mCloudControllers;
	
	TriMesh			mMesh;

	
	bool			firstTime;
	//GUI, and associated fields
	//params::InterfaceGlRef	mParams;
	params::InterfaceGl	mParams;
	float				cloudCover;
	float				cloudSharpness;
	float				pointSize;
	float				cloudSize;
	float				testAlpha;
	float				velSpeed;
	float				accTimer;
	float				fingerRadius;
	float				cloudColor;
	int					noiseLevel;
	int					noiseMultiplier;
	int					posDivide;

	//tracker img
	gl::Texture			trackImg;
	gl::Texture			vinImg;
	

};

