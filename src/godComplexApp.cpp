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

using namespace ci;
using namespace ci::app;
using namespace std;

class godComplexApp : public AppNative {
public:
	void prepareSettings( Settings *settings );
	void setup();
	//	void resize(ResizeEvent event);
	void mouseDown(MouseEvent event);
	void mouseDrag( MouseEvent event );
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
	
	WaterModule *mWaterModule;
	VectorFlowField *mFlowField;
	
	//Mouse
	ci::Vec2i mMouse;
	bool mMouseDown;
	
	//show stuff
	bool drawWater;
	bool drawMesh;
	bool mDrawFlowField;
	
	//shader
	gl::GlslProg wellingtonShader;
};

void godComplexApp::prepareSettings(Settings *settings )
{
//    settings->setWindowSize(1280, 720); //NOTE: DEBUG SIZE
	settings->setWindowSize(1920, 1080); //NOTE: OUTPUT SIZE TODO: FIX
	settings->setFrameRate(60.0);
}

/*
 void wellingtonModelApp::resize(ResizeEvent event)
 {
 App::resize( event );
 mArcball.setWindowSize( getWindowSize() );
 mArcball.setCenter( Vec2f( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f ) );
 mArcball.setRadius( 150 );
 }
 */

void godComplexApp::setup()
{
	waterPrevTime = 0.0f;
	drawWater = true;
	drawMesh = true;
	mDrawFlowField = true;
	
	userIncr1 = 0.0f;
	userIncr2 = 0.0f;
	userIncr3 = 0.0f;
	
	//setFullScreen(true);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	
	
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
	//    initialCam.lookAt(Vec3f(0, 43, 0), Vec3f(0, 0, 0), Vec3f(0, -1, 0)); //NOTE: orginal camera
	
	mMayaCam.setCurrentCam( initialCam );
	
	
	
	
	mWaterModule = new WaterModule();
	mWaterModule->setup();
	
	mFlowField = new VectorFlowField();
	mFlowField->setup();
	
}
void godComplexApp::mouseDown( MouseEvent event )
{
	/*
	 if( event.isAltDown() )
	 mMayaCam.mouseDown( event.getPos() );
	 else
	 mArcball.mouseDown( event.getPos() );
	 */
	
	
	mFlowField->mouseDown( event );
	mWaterModule->mouseDown( event );
}

void godComplexApp::mouseUp( MouseEvent event ){
	mFlowField->mouseUp( event );
}
void godComplexApp::mouseDrag(MouseEvent event)
{
	/*
	 mouseMove(event);
	 
	 if( event.isAltDown() )
	 mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
	 else
	 mArcball.mouseDrag( event.getPos() );
	 */
	
	mFlowField->mouseDrag( event );
	mWaterModule->mouseDrag(event);
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
		if(mWaterModule != NULL){
			delete mWaterModule;
		}
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
	if(event.getChar() == 'z'){
		mFlowField->invokePrintFlowField();
	}
	
	if(event.getChar() == 'f'){
		setFullScreen(!isFullScreen());
	}
}

void godComplexApp::update()
{
	if(timeline().getCurrentTime() > waterPrevTime + 10.0f  ){
		console() << "water ripples" << endl;
		mWaterModule->setMakeRipples();
		waterPrevTime = timeline().getCurrentTime();
	}
	mFlowField->update();
}

void godComplexApp::draw()
{
	
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
		glLineWidth(0.2f);
		gl::enableWireframe();
		gl::translate(Vec3f(280.0, 0.0, -180.0));
		gl::rotate(Vec3f(-10.0, -10.0, 0.0));
		gl::draw(mVbo);
		gl::disableWireframe();
		myImage.unbind();
		gl::popMatrices();
	}
	
	
	gl::pushMatrices();
	mFlowField->draw();
	gl::popMatrices();
	
	
	
	/*
	 glPushMatrix();
	 glColor4f(1.0, 0.0, 0.0, 1.0);
	 gl::drawSphere(Vec3f(userIncr1, userIncr2, userIncr3), 30.0, 12.0);
	 glPopMatrix();
	 */
}

CINDER_APP_NATIVE( godComplexApp, RendererGl )
