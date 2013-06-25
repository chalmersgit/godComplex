#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "Resources.h"
#include "cinder/Timeline.h"
#include "cinder/CinderMath.h"
#include "cinder/Rand.h"
#include <vector>
#include <boost/lexical_cast.hpp>

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
	mDisplayUI = false;
	gustPrevTime = 0.0f;
	mPrevTime = 0.0f;
	mPrevTimeController = 0.0f;
	mTheta = 0.05f * (M_PI/180);
	mWindDirection = Vec2f(1.0f, 0.0f);
	mChannel = Channel32f(loadImage(loadResource(RES_WELLINGTON_IMG)));
	mTexture = mChannel;
	windDirectionIcon = gl::Texture(loadImage(loadResource(WINDDIR_IMG)));
	
	mParticleController = make_shared<VectorSet>(RESOLUTION);
	mCloudControllers = make_shared<vector<shared_ptr<CloudController>>>();
	
	windSpeed = 0.1;
	windGust = 0.1;

	float indexIncrement = 0.2f;
	float currentMinIndex = 0.0f;
	float currentMaxIndex = indexIncrement;
	int numControllers = 5;
	for(int i = 0; i < numControllers; ++i){
		glPushMatrix();
		Vec2f loc = Vec2f(Rand::randFloat(0.0f, app::getWindowWidth()), Rand::randFloat(0.0f, app::getWindowHeight()));
		shared_ptr<CloudController> cloudController(new CloudController(loc, Rand::randFloat(0.1, 0.3), Rand::randFloat(0.01f, 0.01f), mParticleController));
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
	mCloudParticle = make_shared<CloudParticle>(mCloudControllers, numControllers);

	mDrawParticles = true;
	mDrawImage = false;
}


void VectorFlowField::update()
{
	if(! mChannel) return;
	
	if((timeline().getCurrentTime() - mPrevTime) > 0.00001){
		if(randFloat(0.0, 9.0) < 0.02){
			mTheta *= -1.0;
		}

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
			(*mCloudControllers)[i]->update(windSpeed, windGust);
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



	//Wind Speed / Gust
	windSpeed = (exp((sin(timeline().getCurrentTime() * 0.001) + 1.0) * 0.5) - 1.0) * 5.0;
	if(timeline().getCurrentTime() - gustPrevTime > 45.0){
		windGust = randFloat( 0.0, 9.0 );
		gustPrevTime = timeline().getCurrentTime();
	}
}

void VectorFlowField::draw()
{
	gl::setViewport(getWindowBounds());
	gl::setMatricesWindow(getWindowSize());  //NOTE: remeber this big dog error thrower
	//    gl::setMatricesWindowPersp(getWindowSize()); //NOTE: doesnt seem to work as well
	gl::color(1, 1, 1);
	
	/*
	if(mDrawImage){
		mTexture.enableAndBind();
		gl::draw(mTexture, getWindowBounds()); //NOTE: this is the image, dont really need, keeping for debugging
	}
	*/
	
	if(mDrawParticles && mDrawFlowField){                 //NOTE: this is the flow field
		glDisable(GL_TEXTURE_2D);
		mParticleController->draw();
	}
	
	/*
	if(!mCloudControllers->empty()){
		for(int i = 0; i < mCloudControllers->size(); i++){
			glPushMatrix();
			(*mCloudControllers)[i]->draw();
			glPopMatrix();
		}
	}
	*/

	gl::enableAlphaBlending();


	if(mCloudParticle !=NULL){
		glPushMatrix();
		mCloudParticle->draw();
		glPopMatrix();
	}

	gl::pushMatrices();
	float thetaVal = mWindDirection.dot(Vec2f(0.0f, 1.0f));
	thetaVal = acos(thetaVal)*(180/M_PI);
	if(mWindDirection.x < 0){
		thetaVal = 360 - thetaVal;
	}

	if(mDisplayUI){
		gl::translate(getWindowWidth()*0.5 + 150.0, getWindowHeight() - 110.0);
		gl::rotate(thetaVal);
		gl::scale(0.3, 0.3);
		gl::translate(-150.0, -150.0);
		gl::draw(windDirectionIcon, windDirectionIcon.getBounds());
		gl::popMatrices();
	}
	gl::pushMatrices();
	gl::Texture fontTex;
	TextLayout layout;
	layout.clear(ColorA(0.0, 0.0, 0.0, 0.0));
	layout.setFont(Font("Consolas", 18));
	layout.setColor(Color(1.0, 1.0, 1.0));

	if(mDisplayUI){
		int windSpeedKm = (windSpeed * 10) + 2;
		string ws = boost::lexical_cast<string> (windSpeedKm);
		string speed = "wind speed : ";
		speed.append(ws);
		layout.addLine(speed);
	
		int windGustKm = windGust + windSpeedKm + 1;
		string wg = boost::lexical_cast<string> (windGustKm);
		string gust = "wind gust : ";
		gust.append(wg);
		layout.addLine(gust);
	}
	fontTex = gl::Texture(layout.render(true, false));
	gl::draw(fontTex, Vec2f(getWindowWidth() * 0.5 - 250.0, getWindowHeight() - 135.0));
	gl::popMatrices();
	
	gl::disableAlphaBlending();
}

void VectorFlowField::setDrawFlow(bool b){
	mDrawFlowField = b;
}

void VectorFlowField::setDisplayUI(){
	mDisplayUI = !mDisplayUI;
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
	if(!mCloudControllers->empty()){
		for(int i = 0; i < mCloudControllers->size(); i++){
			//position

			//life
			(*mCloudControllers)[i]->controllerAlpha = timeline().getCurrentTime() - (*mCloudControllers)[i]->prevTime;
			if((timeline().getCurrentTime() - (*mCloudControllers)[i]->prevTime) > (*mCloudControllers)[i]->lifeLength){
				(*mCloudControllers)[i]->doRespawn();
				(*mCloudControllers)[i]->setPrevTime();
				(*mCloudControllers)[i]->controllerAlpha = (*mCloudControllers)[i]->lifeLength;
			}
		}
	}

	/*
	if((timeline().getCurrentTime() - mPrevTimeController) > 30.0f){
		if(!mCloudControllers->empty()){
			for(int i = 0; i < mCloudControllers->size(); i++){
				(*mCloudControllers)[i]->doRespawn();
			}
		}
		mPrevTimeController = timeline().getCurrentTime();
	}
	*/
}


//CINDER_APP_BASIC( VectorFlowFieldApp, RendererGl )
