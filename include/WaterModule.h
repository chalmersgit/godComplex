
#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"


class WaterModule{
public:
	WaterModule();
	void draw(float);
	void makeRipples();
	void setMakeRipples();
	void keyDown(ci::app::KeyEvent event);
	void mouseDown(ci::app::MouseEvent event);
	void mouseDrag(ci::app::MouseEvent event);
	void mouseUp(ci::app::MouseEvent event);
	void setup();
private:
	//Convenience method for drawing fullscreen rect
	//with tex coord
	void drawFullScreenRect();
	
	//FBOs to ping pong
	ci::gl::Fbo mFbo[2];
	size_t mFboIndex;
	
	//Shaders
	ci::gl::GlslProg mShaderGpGpu;
	ci::gl::GlslProg mShaderRefraction;
	
	//Refraction texture
	ci::gl::Texture mTexture;
	
	//Mouse             //TODO: bring to main app level
	ci::Vec2i mMouse;
	bool mMouseDown;
	
	//True renders input to screen
	bool mShowInput;
	
	bool makeRipples_bool;
	
};


