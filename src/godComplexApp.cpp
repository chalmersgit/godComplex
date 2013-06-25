#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ObjLoader.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Vbo.h"
#include "cinder/Arcball.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Sphere.h"
#include "cinder/Sphere.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Timeline.h"

#include "Resources.h"
#include "VectorFlowField.h"
#include "WaterModule.h"

#include <boost\shared_ptr.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

class godComplexApp : public AppNative {
public:
	void prepareSettings( Settings *settings );
	void setup();
	void mouseDown(MouseEvent event);
	void mouseUp( MouseEvent event );
	void keyDown( KeyEvent event );
	void frameCurrentObject();
	void update();
	void draw();
	
	float userIncr1;
	float userIncr2;
	float userIncr3;
	float waterPrevTime;
	
	gl::Texture myImage;
	gl::VboMesh mVbo;
	
	Channel32f mChannel;
	TriMesh mMesh;
	Arcball mArcball;
	MayaCamUI mMayaCam;
	
	shared_ptr<WaterModule> mWaterModule;
	shared_ptr<VectorFlowField> mFlowField;
	
	//Mouse
	ci::Vec2i mMouse;
	bool mMouseDown;
	
	//show stuff
	bool drawWater;
	bool drawMesh;
	bool mDrawFlowField;
};

void godComplexApp::prepareSettings(Settings *settings )
{
	settings->setWindowSize(1920, 1080);
	settings->setFrameRate(60.0);
}


void godComplexApp::setup()
{
	waterPrevTime = 0.0f;
	drawWater = true;
	drawMesh = true;
	mDrawFlowField = false;
	
	userIncr1 = 0.0f;
	userIncr2 = 0.0f;
	userIncr3 = 0.0f;
	
	myImage = gl::Texture(loadImage(loadResource( RES_WELLINGTON_IMG_ALPHA )));
	
	ObjLoader loader(loadResource(RES_WELLINGTON_OBJ));
	loader.load(&mMesh);
	mVbo = gl::VboMesh(mMesh);
	
	CameraPersp initialCam;
	initialCam.setPerspective( 77.5f, getWindowAspectRatio(), 5.0f, 3000.0f ); //TODO: get correct camera persp from C4D
	
	Vec3f mEye = Vec3f(0.0f, 30.0f, 0.0f);
	Vec3f mCenter = Vec3f::zero();
	Vec3f mUp = Vec3f::yAxis();
	
	initialCam.lookAt( mEye, mCenter, mUp ); //NOTE: new camera
	mMayaCam.setCurrentCam( initialCam );
	
	mWaterModule = make_shared<WaterModule>();
	mWaterModule->setup();
	
	mFlowField = make_shared<VectorFlowField>();
	mFlowField->setup();
	mFlowField->setDrawFlow(mDrawFlowField);
	
}
void godComplexApp::mouseDown( MouseEvent event )
{
	mFlowField->mouseDown( event );
	mWaterModule->mouseDown( event );
}

void godComplexApp::mouseUp( MouseEvent event ){
	mFlowField->mouseUp( event );
}


void godComplexApp::frameCurrentObject()
{
	Sphere boundingSphere = Sphere::calculateBoundingSphere( mMesh.getVertices() );
	
	mMayaCam.setCurrentCam( mMayaCam.getCamera().getFrameSphere( boundingSphere, 100 ) );
	
}

void godComplexApp::keyDown(KeyEvent event)
{
	if(event.getChar() == 'p'){
		userIncr1+= 1.0f;
		console() << "p " << userIncr1 << endl;
	}
	if(event.getChar() == 'o'){
		userIncr1 -= 1.0f;
		console() << "o " << userIncr1 << endl;
	}
	if(event.getChar() == 'l'){
		userIncr2+= 1.0f;
		console() << "l " << userIncr2 << endl;
	}
	if(event.getChar() == 'k'){
		userIncr2 -= 1.0f;
		console() << "k " << userIncr2 << endl;
	}
	if(event.getChar() == 'm'){
		userIncr3+= 1.0f;
		console() << "m " << userIncr3 << endl;
	}
	if(event.getChar() == 'n'){
		userIncr3 -= 1.0f;
		console() << "n " << userIncr3 << endl;
	}
	
	
	if(event.getCode() == 27){
		/*if(mWaterModule != NULL){
			delete mWaterModule;
		}*/
		exit(0);
	}
	
	if(event.getChar() == 'w'){
		drawWater = !drawWater;
	}
	if(event.getChar() == 'm'){
		drawMesh = !drawMesh;
	}
	
	if(event.getChar() == 'd'){
		mDrawFlowField = !mDrawFlowField;
		mFlowField->setDrawFlow(mDrawFlowField);
	}

	if(event.getChar() == '9'){
		mFlowField->setDisplayUI();
	}

	if(event.getChar() == 'z'){
		mFlowField->invokePrintFlowField();
	}
	
	if(event.getChar() == 'f'){
		setFullScreen(!isFullScreen());
	}
}

void godComplexApp::update()
{
	if(timeline().getCurrentTime() > waterPrevTime + 30.0f  ){
		mWaterModule->setMakeRipples();
		waterPrevTime = timeline().getCurrentTime();
	}
	mFlowField->update();
}

void godComplexApp::draw()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.1f);

	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enableAlphaBlending();
	
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices( mMayaCam.getCamera());

	if(drawWater == true){
		if(mWaterModule != NULL){
			gl::pushMatrices();
			mWaterModule->draw(0);
			gl::popMatrices();
		}
	}
	
	
	
	if(drawMesh == true){
		gl::pushMatrices();
		myImage.enableAndBind();
		//      gl::rotate( mArcball.getQuat() ); //NOTE: for debugging
		gl::scale(Vec3f(0.035,0.035,0.035));
		glLineWidth(0.05f);
		gl::enableWireframe();
		gl::translate(Vec3f(280.0, 0.0, -180.0));
		gl::rotate(Vec3f(-10.0, -10.0, 0.0));
		gl::draw(mVbo);
		gl::disableWireframe();
		myImage.unbind();
		gl::popMatrices();
	}
	
	glDisable( GL_LIGHTING );

	gl::pushMatrices();
	mFlowField->draw();
	gl::popMatrices();

}

CINDER_APP_NATIVE( godComplexApp, RendererGl )
