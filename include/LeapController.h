#include "cinder/app/AppBasic.h"
#include <iostream>
#include "Leap.h"
#include <vector>

using namespace Leap;

class LeapController : public Listener {
  public:
	LeapController();
	//LeapController(int velocityShader);
	virtual void onInit(const Controller&);
	virtual void onConnect(const Controller&);
	virtual void onDisconnect(const Controller&);
	virtual void onExit(const Controller&);
	virtual void onFrame(const Controller&);
	virtual void onFocusGained(const Controller&);
	virtual void onFocusLost(const Controller&);

	bool hasFingers;
	Vector avgPos;
	int numActiveFingers;
	std::vector<ci::Vec2f> fingerPositions;
	std::vector<ci::Vec2f> fingerVelocities;

};

