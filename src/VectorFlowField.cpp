#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "Resources.h"
#include "cinder/Timeline.h"
#include "cinder/CinderMath.h"
#include "cinder/Rand.h"
#include <vector>

#include "VectorFlowField.h"
#include "VectorSet.h"             //TODO: rename local variable for VectorSet from mParticleController
#include "CloudController.h"
#include "CloudParticle.h"

#define MAXCONTROLLERS 16
#define RESOLUTION 50
using namespace ci;
using namespace ci::app;
using namespace std;

void VectorFlowField::setup()
{
	mPrevTime = 0.0f;
	mPrevTimeController = 0.0f;
	mTheta = 0.1f * (M_PI/180);
	mWindDirection = Vec2f(1.0f, 0.0f);
	mChannel = Channel32f(loadImage(loadResource(RES_WELLINGTON_IMG)));
	mTexture = mChannel;
	
	mParticleController = new VectorSet(RESOLUTION);
	
	//mCloudController = new CloudController(Vec2f(0, 0), 0.0f, 0.0f, mParticleController);
	
	mCloudControllers = new vector<CloudController*>();
	
	float indexIncrement = 0.2f;
	float currentMinIndex = 0.0f;
	float currentMaxIndex = indexIncrement;
	
	for(int i = 0; i < 5; ++i){
		glPushMatrix();
		Vec2f loc = Vec2f(Rand::randFloat(0.0f, app::getWindowWidth()), Rand::randFloat(0.0f, app::getWindowHeight()));
		CloudController* cloudController = new CloudController(loc, Rand::randFloat(0.1, 0.3), Rand::randFloat(0.01f, 0.01f), mParticleController);
		
		if(i == 1){
			cloudController->setColor(Vec3f(1, 0, 0));
		}
		else if(i == 2){
			cloudController->setColor(Vec3f(0, 1, 0));
		}
		else if(i == 3){
			cloudController->setColor(Vec3f(0, 0, 1));
		}
		cloudController->setIndex(currentMinIndex, currentMaxIndex);
		
		mCloudControllers->push_back(cloudController);
		glPopMatrix();
		
		currentMinIndex += indexIncrement;
		currentMaxIndex += indexIncrement;
	}
	numAliveControllers = 16;
	mCloudParticle = new CloudParticle(mCloudControllers);
	
	
	mDrawParticles = true;
	mDrawImage = false;
	
}


void VectorFlowField::update()
{
	if(! mChannel) return;
	
	if((timeline().getCurrentTime() - mPrevTime) > 0.01){
		
		float newX = mWindDirection.x*cos(mTheta) - mWindDirection.y*sin(mTheta);
		float newY = mWindDirection.x*sin(mTheta) + mWindDirection.y*cos(mTheta);
		
		mWindDirection.x = newX;
		mWindDirection.y = newY;
		
		mWindDirection.normalize();
		
		
		mPrevTime = timeline().getCurrentTime();
	}
	
	mParticleController->update(mChannel, mWindDirection);
	
	
	//Cloud stuff
	checkRespawn();
	
	//Cloud controllers
	if(!mCloudControllers->empty()){
		for(int i = 0; i < mCloudControllers->size(); i++){
			glPushMatrix();
			//Steer the cloud controller
			Vec2f desiredDir = mParticleController->flowLookUp((*mCloudControllers)[i]->mLoc);
			desiredDir = desiredDir * (*mCloudControllers)[i]->mMaxSpeed;
			Vec2f steer = desiredDir - (*mCloudControllers)[i]->mVel;
			steer.limit((*mCloudControllers)[i]->mMaxForce);
			(*mCloudControllers)[i]->applyForce(steer);
			
			//update its position
			(*mCloudControllers)[i]->update();
			glPopMatrix();
		}
	}
	
	//Cloud particles
	if(mCloudParticle !=NULL){
		glPushMatrix();
		mCloudParticle->updateGPUcloudControllers();
		mCloudParticle->update();
		glPopMatrix();
	}
}

void VectorFlowField::draw()
{
	
	gl::setViewport(getWindowBounds());
	gl::setMatricesWindow(getWindowSize());  //NOTE: remeber this big dog error thrower
	//    gl::setMatricesWindowPersp(getWindowSize()); //NOTE: doesnt seem to work as well
	gl::color(1, 1, 1);
	
	
	if(mDrawImage){
		mTexture.enableAndBind();
		gl::draw(mTexture, getWindowBounds()); //NOTE: this is the image, dont really need, keeping for debugging
	}
	
	
	
	
	
	if(mDrawParticles && mDrawFlowField){                 //NOTE: this is the flow field
		glDisable(GL_TEXTURE_2D);
		mParticleController->draw();
	}
	
	
	if(!mCloudControllers->empty()){
		for(int i = 0; i < mCloudControllers->size(); i++){
			glPushMatrix();
			(*mCloudControllers)[i]->draw();
			glPopMatrix();
		}
	}
	///*
	if(mCloudParticle !=NULL){
		glPushMatrix();
		mCloudParticle->draw();
		glPopMatrix();
	}
	//*/
}

void VectorFlowField::setDrawFlow(bool b){
	mDrawFlowField = b;
}

void VectorFlowField::invokePrintFlowField(){
	mParticleController->printFlowLookUpTable();
	
	if(!mCloudControllers->empty()){
		for(int i = 0; i < mCloudControllers->size(); i++){
			glPushMatrix();
			mParticleController->flowLookUp((*mCloudControllers)[i]->mLoc);
			glPopMatrix();
		}
	}
}

void VectorFlowField::cloudControllerFollow(){
	
}


void VectorFlowField::mouseDown(MouseEvent event)
{
	mCloudParticle->mouseDown(event);
}


void VectorFlowField::mouseDrag(MouseEvent event)
{
	mCloudParticle->mouseDrag(event);
}

void VectorFlowField::mouseUp( MouseEvent event ){
	mCloudParticle->mouseUp(event);
}


void VectorFlowField::checkRespawn(){
	if((timeline().getCurrentTime() - mPrevTimeController) > 30.0f){ //change timer to somewhat random/longer
		//Respawn the controller
		//mLoc = Vec2f(randFloat(0.0f, getWindowWidth()), randFloat(0.0f, getWindowHeight()));
		
		if(!mCloudControllers->empty()){
			for(int i = 0; i < mCloudControllers->size(); i++){
				//console() << "before respawn: " << (*mCloudControllers)[i]->mLoc << endl;
				(*mCloudControllers)[i]->doRespawn();
				//console() << "after respawn: " << (*mCloudControllers)[i]->mLoc << endl;
			}
		}
		
		//Fade the following particles
		mPrevTimeController = timeline().getCurrentTime();
	}
}


//CINDER_APP_BASIC( VectorFlowFieldApp, RendererGl )
