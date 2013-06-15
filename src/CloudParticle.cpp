#include "cinder/app/AppNative.h"
#include "cinder/Rand.h"
#include "CloudParticle.h"
#include "cinder/gl/gl.h"
#include <boost/lexical_cast.hpp>

#include "Resources.h"

//#include "LeapController.h"

using namespace ci;
using namespace ci::app;
using namespace std;

CloudParticle::CloudParticle()
{
	setup();
}

CloudParticle::CloudParticle(vector<CloudController*>* cc){
	mCloudControllers = cc;
	setup();
	particleTexRes = Vec2f(PARTICLES_X, PARTICLES_Y);
	
	//need a timer to kill things
	int activeControllersCount;
	Vec2f prevControllers[16];
	Vec2f controllers[16];
	float minIndices[16];
	float maxIndices[16];
}

void CloudParticle::initFBO()
{
	currentWindowSize = Vec2i(WIDTH, HEIGHT);
	mPos = 0;
	mVel = 1;
	mInfo = 2;
	mSprite = 3;
	//original pos and vel = 3, 4
	mNoise = 5;
	
	mBufferIn = 0;
	mBufferOut = 1;
	
	mFbo[0].bindFramebuffer();
	mFbo[1].bindFramebuffer();
	
	//Positionen
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	
	gl::setMatricesWindow( mFbo[0].getSize(), false );   //TODO: point matrices down from wellingtonModelApp camera
	gl::setViewport( mFbo[0].getBounds() );
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mPosTex.enableAndBind();
	gl::draw(mPosTex,mFbo[0].getBounds());
	mPosTex.unbind();
	
	//velocity buffer
	glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mVelTex.enableAndBind();
	gl::draw(mVelTex,mFbo[0].getBounds());
	mVelTex.unbind();
	
	//particle information buffer
	glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mInfoTex.enableAndBind();
	gl::draw(mInfoTex,mFbo[0].getBounds());
	mInfoTex.unbind();
	
	mFbo[1].unbindFramebuffer();
	mFbo[0].unbindFramebuffer();
	
	mPosTex.disable();
	mVelTex.disable();
	mInfoTex.disable();
}

void CloudParticle::setup()
{
	activeControllersCount = 5;
	gl::clear();
	
	try {
		console() << "1" << endl;
		mPosShader = gl::GlslProg(loadResource(POS_VS),loadResource(POS_FS));
		console() << "2" << endl;
		mVelShader = gl::GlslProg(loadResource(VEL_VS),loadResource(VEL_FS));
		console() << "3" << endl;
		
		//Leap
		minX = -200;
		maxX = 200;
		minY = 100;
		maxY = 400;
		console() << "4" << endl;
		
		mLeapController = new LeapController();
		mController.addListener(*mLeapController);
		console() << "5" << endl;
	}
	catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
	}
	catch( ... ) {
		std::cout << "Unable to load shader" << std::endl;
	}
	//	/*
	//controls
	mIsFullScreen = false;
	
	mFrameCounter = 0;
	
	mPerlin = Perlin(32,clock() * .1f);
	
	//initialize buffer
	Surface32f mPosSurface = Surface32f(PARTICLES_X,PARTICLES_Y,true);
	Surface32f mVelSurface = Surface32f(PARTICLES_X,PARTICLES_Y,true);
	Surface32f mInfoSurface = Surface32f(PARTICLES_X,PARTICLES_Y,true);
	Surface32f mNoiseSurface = Surface32f(PARTICLES_X,PARTICLES_Y,true);
	
	Surface32f::Iter iterator = mPosSurface.getIter();
	
	
	while(iterator.line())
	{
		while(iterator.pixel())
		{
			mVertPos = Vec3f(Rand::randFloat(getWindowWidth()) / (float)getWindowWidth(),
							 Rand::randFloat(getWindowHeight()) / (float)getWindowHeight(),0.0f);
			
			//velocity
			//Vec2f vel = Vec2f(Rand::randFloat(-.005f,.005f),Rand::randFloat(-.005f,.005f));
			//console() << vel << endl;
			Vec2f vel = Vec2f(0.01, 0);
			//vel.normalize();
			
			float nX = iterator.x() * 0.005f;
			float nY = iterator.y() * 0.005f;
			float nZ = app::getElapsedSeconds() * 0.1f;
			Vec3f v( nX, nY, nZ );
			float noise = mPerlin.fBm( v );
			
			float angle = noise * 15.0f ;
			
			//noise
			Color noiseColour(cos( angle ) * Rand::randFloat(.00005f,.0002f), sin( angle ) * Rand::randFloat(.00005f,.0002f), 0.0f);
			if(noiseColour.r  < 0.0f){
				noiseColour.r *= -1;
			}
			mNoiseSurface.setPixel(iterator.getPos(), noiseColour);
			
			//position + mass
			ColorA positionColourMASS(mVertPos.x,mVertPos.y,mVertPos.z, Rand::randFloat(.00005f,.0002f));
			
			//ColorA positionColourMASS(mVertPos.x,mVertPos.y,mVertPos.z, 0);
			mPosSurface.setPixel(iterator.getPos(), positionColourMASS);
			
			//forces + decay
			Color forcesColourDECAY(vel.x,vel.y, Rand::randFloat(.01f,1.00f));
			mVelSurface.setPixel(iterator.getPos(), forcesColourDECAY);
			
			//particle age
			ColorA ageColour(Rand::randFloat(.007f,1.0f), 1.0f,0.00f,1.00f);
			mInfoSurface.setPixel(iterator.getPos(), ageColour);
		}
	}
	
	
	//gl texture settings
	gl::Texture::Format tFormat;
	tFormat.setInternalFormat(GL_RGBA16F_ARB);
	
	gl::Texture::Format tFormatSmall;
	tFormat.setInternalFormat(GL_RGBA8);
	
	mSpriteTex = gl::Texture( loadImage( loadResource(RES_STARTER_IMAGE) ), tFormatSmall);
	
	mNoiseTex = gl::Texture(mNoiseSurface, tFormatSmall);
	mNoiseTex.setWrap( GL_REPEAT, GL_REPEAT );
	mNoiseTex.setMinFilter( GL_NEAREST );
	mNoiseTex.setMagFilter( GL_NEAREST );
	
	mPosTex = gl::Texture(mPosSurface, tFormat);
	mPosTex.setWrap( GL_REPEAT, GL_REPEAT );
	mPosTex.setMinFilter( GL_NEAREST );
	mPosTex.setMagFilter( GL_NEAREST );
	
	mVelTex = gl::Texture(mVelSurface, tFormat);
	mVelTex.setWrap( GL_REPEAT, GL_REPEAT );
	mVelTex.setMinFilter( GL_NEAREST );
	mVelTex.setMagFilter( GL_NEAREST );
	
	mInfoTex = gl::Texture(mInfoSurface, tFormatSmall);
	mInfoTex.setWrap( GL_REPEAT, GL_REPEAT );
	mInfoTex.setMinFilter( GL_NEAREST );
	mInfoTex.setMagFilter( GL_NEAREST );
	
	//initialize fbo
	gl::Fbo::Format format;
	format.enableDepthBuffer(false);
	format.enableColorBuffer(true, 3);
	format.setMinFilter( GL_NEAREST );
	format.setMagFilter( GL_NEAREST );
	format.setWrap(GL_CLAMP,GL_CLAMP);
	format.setColorInternalFormat( GL_RGBA16F_ARB );
	
	mFbo[0] = gl::Fbo(PARTICLES_X,PARTICLES_Y, format);
	mFbo[1] = gl::Fbo(PARTICLES_X,PARTICLES_Y, format);
	
	initFBO();
	
	//fill dummy fbo
	vector<Vec2f> texCoords;
	vector<Vec3f> vertCoords, normCoords;
	vector<uint32_t> indices;
	
	gl::VboMesh::Layout layout;
	layout.setStaticIndices();
	layout.setStaticPositions();
	layout.setStaticTexCoords2d();
	layout.setStaticNormals();
	
	//ObjLoader loader( (DataSourceRef)loadResource( SPHERES_OBJ ) );
	//loader.load( &mMesh );
	
	//mVbo = gl::VboMesh( mMesh );
	//mVbo = gl::VboMesh( mMesh, layout );
	
	mVbo = gl::VboMesh(PARTICLES_X*PARTICLES_X,PARTICLES_Y*PARTICLES_Y,layout,GL_POINTS);
	
	vector<Vec3f> points = mMesh.getVertices();
	
	for (int x = 0; x < PARTICLES_X; ++x) {
		for (int y = 0; y < PARTICLES_Y; ++y) {
			indices.push_back( x * PARTICLES_X + y);
			//texCoords.push_back( Vec2f(points[y].x, points[y].y) );
			texCoords.push_back( Vec2f( x/(float)PARTICLES_X, y/(float)PARTICLES_Y));
		}
	}
	
	mVbo.bufferIndices(indices);
	mVbo.bufferTexCoords2d(0, texCoords);
	//    */
	
	
	//Input intialisation
	mMousePos = Vec2f(0.0, 0.0); //Vec2f(((float)WIDTH) / 2, ((float)HEIGHT) / 2);
	mMousePosNormalised = Vec2f(0.0, 0.0); //Vec2f(0.5, 0.5);
	mMouseDown = false;
	mMouseDownInt = 0;
	mLoc = Vec2f(0.0, 0.0);
}

void CloudParticle::setPos(Vec2f loc)
{
	mLoc = loc;
}


void CloudParticle::updateGPUcloudControllers(){
	int i = 0;
	while(i < activeControllersCount){
		prevControllers[i] = (*mCloudControllers)[i]->prevmLoc / particleTexRes;
		controllers[i] = (*mCloudControllers)[i]->mLoc / particleTexRes;
		minIndices[i] = (*mCloudControllers)[i]->indexMin;
		maxIndices[i] = (*mCloudControllers)[i]->indexMax;
		//console() << "Val " << i << ": " << (*mCloudControllers)[i]->mLoc << " - " << (*mCloudControllers)[i]->indexMin << ", " << (*mCloudControllers)[i]->indexMax << endl;
		i++;
	}
}

/*
 all calculations are done in update
 */
void CloudParticle::update()
{
	
	
	//##############
	mFbo[mBufferIn].bindFramebuffer();
	
	//set viewport to fbo size
	gl::setMatricesWindow( mFbo[0].getSize(), false ); // false to prevent vertical flipping
	gl::setViewport( mFbo[0].getBounds() );
	
	GLenum buffer[3] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT };
	glDrawBuffers(3,buffer);
	
	mFbo[mBufferOut].bindTexture(0,0);
	mFbo[mBufferOut].bindTexture(1,1);
	mFbo[mBufferOut].bindTexture(2,2);
	
	mVelTex.bind(3);
	mPosTex.bind(4);
	mNoiseTex.bind(5);
	
	mVelShader.bind();
	mVelShader.uniform("positions", mPos);
	mVelShader.uniform("velocities", mVel);
	mVelShader.uniform("information", mInfo);
	mVelShader.uniform("oVelocities",3);
	mVelShader.uniform("oPositions",4);
	mVelShader.uniform("noiseTex", mNoise);
	
	
	//Update vel shader
	mVelShader.uniform("maxControllers", activeControllersCount);
	mVelShader.uniform("prevControllers", prevControllers, 16);
	mVelShader.uniform("controllers", controllers, 16);
	mVelShader.uniform("controllerMinIndices", minIndices, 16);
	mVelShader.uniform("controllerMaxIndices", maxIndices, 16);
	
	//Hard coded - delete this TODO
	mVelShader.uniform("controller1", (*mCloudControllers)[0]->mLoc / particleTexRes);
	mVelShader.uniform("controller2", (*mCloudControllers)[1]->mLoc / particleTexRes);
	mVelShader.uniform("controller3", (*mCloudControllers)[2]->mLoc / particleTexRes);
	mVelShader.uniform("controller4", (*mCloudControllers)[3]->mLoc / particleTexRes);
	
	
//	/*
	 if(mLeapController->hasFingers){
	 for(int i = 0; i < mLeapController->fingerPositions.size(); i++){
	 float leap_x = mLeapController->fingerPositions[i].x;
	 float leap_y = mLeapController->fingerPositions[i].y;
	 
	 if(leap_x > minX && leap_x < maxX && leap_y > minY && leap_y < maxY){
	 float newX = ( leap_x - minX ) / ( maxX - minX ) * currentWindowSize.x;
	 float newY = ( leap_y - minY ) / ( maxY - minY ) * currentWindowSize.y;
	 newY = currentWindowSize.y - newY;
		 console() << newX << ", " << newY << endl;
	 switch (i)
	 {
	 case 0:
	 mVelShader.uniform("finger1", Vec2f(newX, newY)); //leap_x, leap_y));
	 break;
	 case 1:
	 mVelShader.uniform("finger2", Vec2f(newX, newY));
	 break;
	 case 2:
	 mVelShader.uniform("finger3", Vec2f(newX, newY));
	 break;
	 case 3:
	 mVelShader.uniform("finger4", Vec2f(newX, newY));
	 break;
	 case 4:
	 mVelShader.uniform("finger5", Vec2f(newX, newY));
	 break;
	 default:
	 break;
	 }
	 
	 //mVelShader.uniform("finger1", Vec2f(newX, newY)); //leap_x, leap_y));
	 mVelShader.uniform("checkUserInput",  mLeapController->numActiveFingers);
	 }
	 }
	 //float leap_x = mLeapController->avgPos.x;
	 //float leap_y = mLeapController->avgPos.y;
	 }
	 else{
	mVelShader.uniform("mousePos", mMousePos);
	mVelShader.uniform("checkUserInput", mMouseDownInt);
	}
//    */
	
	mVelShader.uniform("scaleX",(float)PARTICLES_X);
	mVelShader.uniform("scaleY",(float)PARTICLES_Y);
	
	ColorA( 0.0f, 0.0f, 0.0f, 0.0f );
	
	glBegin(GL_QUADS);
	glTexCoord2f( 0.0f, 0.0f); glVertex2f( 0.0f, 0.0f);
	glTexCoord2f( 0.0f, 1.0f); glVertex2f( 0.0f, PARTICLES_X);
	glTexCoord2f( 1.0f, 1.0f); glVertex2f( PARTICLES_X, PARTICLES_Y);
	glTexCoord2f( 1.0f, 0.0f); glVertex2f( PARTICLES_Y, 0.0f);
	glEnd();
	
	mVelShader.unbind();
	
	mFbo[mBufferOut].unbindTexture();
	
	mVelTex.unbind();
	mPosTex.unbind();
	mNoiseTex.unbind();
	
	mFbo[mBufferIn].unbindFramebuffer();
	
	mBufferIn = (mBufferIn + 1) % 2;
	mBufferOut = (mBufferIn + 1) % 2;
}

/*
 the draw method displays the last filled buffer
 */
void CloudParticle::draw(){
	//gl::setMatricesWindow( getWindowSize() );
	//gl::setViewport( getWindowBounds() );
	
	//gl::clear( ColorA( 0.0f, 0.0f, 0.0f, 0.0f ) );
	
	gl::enableAlphaBlending();
	glDisable(GL_DEPTH_TEST);
	
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	
	mFbo[mBufferIn].bindTexture(0,0);
	mFbo[mBufferIn].bindTexture(1,1);
	mFbo[mBufferIn].bindTexture(2,2);
	
	mSpriteTex.bind(3);
	mPosShader.bind();
	
	mPosShader.uniform("posTex", mPos);
	mPosShader.uniform("velTex", mVel);
	mPosShader.uniform("infTex", mInfo);
	mPosShader.uniform("oPosTex", 4);
	mPosShader.uniform("spriteTex", mSprite);
	mPosShader.uniform("scaleX",(float)PARTICLES_X);
	mPosShader.uniform("scaleY",(float)PARTICLES_Y);

	//update pos shader
	mPosShader.uniform("maxControllers", activeControllersCount);
	mPosShader.uniform("controllers", controllers, 16);
	mPosShader.uniform("controllerMinIndices", minIndices, 16);
	mPosShader.uniform("controllerMaxIndices", maxIndices, 16);
	
	//gl::color(ColorA(1.0f,1.0f,1.0f,0.0f));
	gl::pushMatrices();
	
	//	glScalef(getWindowWidth() / (float)PARTICLES , getWindowHeight() / (float)PARTICLES ,1.0f);
	//glScalef(0.5, 0.5, 0.5);
	//console() << "Loc: " << mLoc << endl;
	gl::translate(mLoc);
	// draw particles
	gl::draw( mVbo );
	gl::popMatrices();
	
	mPosShader.unbind();
	
	mSpriteTex.unbind();
	
	mFbo[mBufferIn].unbindTexture();
	
	gl::disableAlphaBlending();
}

void CloudParticle::mouseDown( MouseEvent event ){
	console() << event.getPos() << endl;
	
	//mPosShader.uniform("checkUserInput", 200.0f);
	
	mMouseDown = true;
	mMouseDownInt = -1;
	mMousePos = event.getPos();
	
	//normalised:
	mMousePosNormalised = Vec2f(mMousePos.x/getWindowWidth(), mMousePos.y/getWindowHeight());
}

void CloudParticle::mouseUp( MouseEvent event ){
	//mPosShader.uniform("checkUserInput", 100.0f);
	mMouseDown = false;
	mMouseDownInt = 0;
}

void CloudParticle::mouseDrag( MouseEvent event ){
	if(mMouseDown){
		mMousePos = event.getPos();
	}
}