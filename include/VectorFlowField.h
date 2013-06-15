#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"

#include <vector>


#include "VectorSet.h"
#include "CloudController.h"
#include "CloudParticle.h"


using namespace ci;
using namespace std;

class VectorFlowField{
public:
	void setup();
	void update();
	void draw();
	void setDrawFlow(bool b);
	void invokePrintFlowField();
	void cloudControllerFollow();
	void checkRespawn();
	
	void mouseDown(ci::app::MouseEvent event );
	void mouseUp(ci::app::MouseEvent event );
	void mouseDrag(ci::app::MouseEvent event );
	
	Channel32f mChannel;
	gl::Texture mTexture;
	
	VectorSet* mParticleController;
	
	int numAliveControllers;
	vector<CloudController*>* mCloudControllers;
	//CloudController* mCloudController; //NOTE: leave commented out
	CloudParticle* mCloudParticle;
	
	bool mDrawParticles;
	bool mDrawImage;
	bool mDrawFlowField;
	
	Vec2i mMouseLoc;
	Vec2f mWindDirection;
	
	float mTheta;
	float mPrevTime;
	
	float mPrevTimeController;
};