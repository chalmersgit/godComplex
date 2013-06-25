#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Font.h"
#include "cinder/Text.h"

#include <vector>


#include "VectorSet.h"
#include "CloudController.h"
#include "CloudParticle.h"

#include <boost\shared_ptr.hpp>

using namespace ci;
using namespace std;

class VectorFlowField{
public:
	void setup();
	void update();
	void draw();
	void setDisplayUI();
	void setDrawFlow(bool b);
	void invokePrintFlowField();
	void cloudControllerFollow();
	void checkRespawn();
	
	void mouseDown(ci::app::MouseEvent event );
	void mouseUp(ci::app::MouseEvent event );
	void mouseDrag(ci::app::MouseEvent event );
	
	Channel32f mChannel;
	gl::Texture mTexture;
	gl::Texture windDirectionIcon;
	
	shared_ptr<VectorSet> mParticleController;
	
	int numAliveControllers;
	shared_ptr<vector<shared_ptr<CloudController>>> mCloudControllers;
	shared_ptr<CloudParticle> mCloudParticle;
	
	bool mDrawParticles;
	bool mDrawImage;
	bool mDrawFlowField;
	bool mDisplayUI;
	
	Vec2i mMouseLoc;
	Vec2f mWindDirection;
	
	float mTheta;
	float mPrevTime;
	float gustPrevTime;
	float offset;
	
	float mPrevTimeController;
	
	float windSpeed;
	float windGust;

};