
#include "WaterModule.h"

#include "cinder/ImageIo.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const Vec2i kWindowSize = Vec2i(1920, 1080);
const Vec2f kPixel = Vec2f::one() / Vec2f(kWindowSize);

WaterModule::WaterModule(){
	makeRipples_bool = true;
}

void WaterModule::setMakeRipples(){
	makeRipples_bool = true;
}

void WaterModule::mouseDown(MouseEvent event)
{
	mMouseDown = true;
	mouseDrag(event);
}


void WaterModule::keyDown(KeyEvent event)
{
	switch(event.getCode()){
		case KeyEvent::KEY_i:
			mShowInput = !mShowInput;
			break;
	}
}

void WaterModule::mouseDrag(MouseEvent event)
{
	mMouse = event.getPos();
}

void WaterModule::mouseUp(MouseEvent event)
{
	mMouseDown = false;
}


void WaterModule::setup()
{
	//Set flags                     TODO:bring to main app level
	mMouse = Vec2i::zero();
	mMouseDown = false;
	mShowInput = false;
	
	//Load shaders
	try{
		mShaderGpGpu = gl::GlslProg(loadResource(RES_PASS_THRU_VERT), loadResource(RES_GPGPU_FRAG));
	}catch(gl::GlslProgCompileExc ex){
		console() << "Unable to compile GPGPU shader:\n" << ex.what() << "\n";
		return;
	}
	try{
		mShaderRefraction = gl::GlslProg(loadResource(RES_PASS_THRU_VERT), loadResource(RES_REFRACTION_FRAG));
	}catch(gl::GlslProgCompileExc ex){
		console() << "Unable to compile refraction shader:\n" << ex.what() << "\n";
		return;
	}
	
	//Load refraction texture
	{
		gl::Texture::Format format;
		format.setInternalFormat(GL_RGBA32F_ARB);
		mTexture = gl::Texture(loadImage(loadResource(RES_TEXTURE)));
		mTexture.setWrap(GL_REPEAT, GL_REPEAT);
	}
	
	{
		//Set up format with 32-bit color for high resolution data
		gl::Fbo::Format format;
		format.enableColorBuffer(true);
		format.enableDepthBuffer(false);
		format.setColorInternalFormat(GL_RGBA32F_ARB);
		
		//Create two frame buffer objects to ping ping
		mFboIndex = 0;
		for(size_t n = 0; n < 2; ++n){
			mFbo[n] = gl::Fbo(kWindowSize.x, kWindowSize.y, format);
			mFbo[n].bindFramebuffer();
			gl::setViewport(mFbo[n].getBounds());
			gl::clear();
			mFbo[n].unbindFramebuffer();
			mFbo[n].getTexture().setWrap(GL_REPEAT,GL_REPEAT);
		}
	}
}

void WaterModule::drawFullScreenRect()
{
	//Begin drawing
	gl::begin(GL_TRIANGLES);
	
	//Define quad vertices
	Area bounds = getWindowBounds();
	Vec3f vert0((float)bounds.x1, (float)bounds.y1, 0.0f );
	Vec3f vert1((float)bounds.x2, (float)bounds.y1, 0.0f );
	Vec3f vert2((float)bounds.x1, (float)bounds.y2, 0.0f );
	Vec3f vert3((float)bounds.x2, (float)bounds.y2, 0.0f );
	
	
	
	//Define quad texture coords
	Vec3f uv0(0.0f, 0.0f, 0.0f);
	Vec3f uv1(1.0f, 0.0f, 0.0f);
	Vec3f uv2(0.0f, 1.0f, 0.0f);
	Vec3f uv3(1.0f, 1.0f, 0.0f);
	
	//Draw quad (two triangles)
	gl::texCoord(uv0);
	gl::vertex(vert0);
	gl::texCoord(uv2);
	gl::vertex(vert2);
	gl::texCoord(uv1);
	gl::vertex(vert1);
	
	gl::texCoord(uv1);
	gl::vertex(vert1);
	gl::texCoord(uv2);
	gl::vertex(vert2);
	gl::texCoord(uv3);
	gl::vertex(vert3);
	
	//end drawing
	gl::end();
}

void WaterModule::makeRipples(){
	gl::color(ColorAf(1.0f, 0.0f, 0.0f, 1.0f));
	
//    gl::drawSolidCircle(Vec2f(964.0f, 42.0f), 20.0f, 12);
	gl::drawSolidCircle(Vec2f(664.0f, 206.0f), 20.0f, 12);
	gl::drawSolidCircle(Vec2f(547.0f, 444.0f), 20.0f, 12);
	gl::drawSolidCircle(Vec2f(679.0f, 694.0f), 20.0f, 12);
	gl::drawSolidCircle(Vec2f(911.0f, 829.0f), 20.0f, 12);
	gl::drawSolidCircle(Vec2f(1209.0f, 711.0f), 20.0f, 12);
	gl::drawSolidCircle(Vec2f(1371.0f, 596.0f), 20.0f, 12);
	gl::drawSolidCircle(Vec2f(1483.0f, 246.0f), 20.0f, 12);
	gl::drawSolidCircle(Vec2f(1632.0f, 555.0f), 20.0f, 12);
	gl::drawSolidCircle(Vec2f(1689.0f, 969.0f), 20.0f, 12);
	gl::drawSolidCircle(Vec2f(1248.0f, 177.0f), 20.0f, 12);
	gl::drawSolidCircle(Vec2f(1141.0f, 433.0f), 20.0f, 12);
	gl::drawSolidCircle(Vec2f(0.0f, 0.0f), 40.0f, 12);
	gl::drawSolidCircle(Vec2f(0.0f, 1080.0f), 40.0f, 12);
	gl::drawSolidCircle(Vec2f(0.0f, 540.0f), 40.0f, 12);
	
	
	
	gl::color(Color::white());
}

void WaterModule::draw(float waterHeight)
{
	
	
	
	//GPGPU pass
	
	//Enable textures
	gl::enable(GL_TEXTURE_2D);
	gl::color(Colorf::white());
	
	
	//Bind the other FBO to draw onto it
	size_t pong = (mFboIndex+1) % 2;
	mFbo[pong].bindFramebuffer();
	
	
	glPushMatrix();
	//set up the window to match the FBO
	gl::setViewport(mFbo[mFboIndex].getBounds());
	gl::setMatricesWindow(mFbo[mFboIndex].getSize(), false);
	//gl::setMatricesWindowPersp(mFbo[mFboIndex].getSize());
	gl::clear();
	
	
	//Bind the texture from the FBO on which we last wrote data
	mFbo[mFboIndex].bindTexture();
	
	//Bind and configure GPU shader
	mShaderGpGpu.bind();
	mShaderGpGpu.uniform("buffer", 0);
	mShaderGpGpu.uniform("pixel", kPixel);
	
	//Draw a fullscreen rectangle to process data
	drawFullScreenRect();
	
	//End shader output
	mShaderGpGpu.unbind();
	
	//Unbind and disable textures
	mFbo[mFboIndex].unbindTexture();
	gl::disable(GL_TEXTURE_2D);
	
	//Draw mouse input into red channel
	/*
	 if(mMouseDown){
	 gl::color(ColorAf(1.0f, 0.0f, 0.0f, 1.0f));
	 
	 
	 gl::drawSolidCircle(Vec2f(mMouse), 20.0f, 12);      //TODO: this is where we have to implement the vector flow field
	 gl::color(Color::white());
	 }
	 */
	if(makeRipples_bool){
		makeRipples();
		makeRipples_bool = false;
	}
	
	
	glPopMatrix();
	//Stop drawing to FBO
	mFbo[pong].unbindFramebuffer();
	
	//Swap FBO's
	mFboIndex = pong;
	
	////////////////////////////////////////////////////
	
	//Refraction pass
	
	//clear screen and set to viewport
	gl::clear(Color::black());
	gl::setViewport(getWindowBounds());
	//gl::setMatricesWindow(getWindowSize());  //NOTE: remeber this big dog error thrower
	gl::setMatricesWindowPersp(getWindowSize());
	
	//this flag draws the raw data without refraction
	if(mShowInput){
		//gl::draw(mFbo[mFboIndex].getTexture());
	}else{
		//bind the FBO we last rendered as a texture
		mFbo[mFboIndex].bindTexture(0, 0);
		
		//bind and enable the refraction texture
		gl::enable(GL_TEXTURE_2D);
		mTexture.bind(1);
		
		//bind and configure the refraction shader
		mShaderRefraction.bind();
		mShaderRefraction.uniform("buffer", 0);
		mShaderRefraction.uniform("pixel", kPixel);
		mShaderRefraction.uniform("tex", 1);
		
		//fill the screen with the shader output
		
		//        gl::drawCube(Vec3f(0, 0, 0), Vec3f(841.0f, 1.0f, 600.0f));
		//        gl::drawSolidRect(getWindowBounds());
		//        gl::rotate(Vec3f(180.0, 0.0, 0.0));
		//        gl::enableWireframe();
		drawFullScreenRect();
		
		
		//unbind and disable the texture
		//end shader output
		mShaderRefraction.unbind();
		mTexture.unbind();
		gl::disable(GL_TEXTURE_2D);
		mFbo[mFboIndex].unbindTexture();
		
	}
	
}


//CINDER_APP_BASIC( waterModule, RendererGl(RendererGl::AA_MSAA_32) )
