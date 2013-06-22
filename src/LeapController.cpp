/******************************************************************************\
* Copyright (C) 2012-2013 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/
/*
#include <iostream>
#include "Leap.h"
using namespace Leap;

class LeapController : public Listener {
  public:console()
	virtual void onInit(const Controller&);
	virtual void onConnect(const Controller&);
	virtual void onDisconnect(const Controller&);
	virtual void onExit(const Controller&);
	virtual void onFrame(const Controller&);
	virtual void onFocusGained(const Controller&);
	virtual void onFocusLost(const Controller&);
};*/

#include "LeapController.h"
#include "cinder/app/AppBasic.h"

#include "cinder/gl/gl.h"  
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace ci::app;

LeapController::LeapController(){
	hasFingers = false;
	numActiveFingers = 0;
	for(int i = 0; i < 40; i++){
		fingerPositions.push_back(Vec2f(0.0f, 0.0f));
		fingerVelocities.push_back(Vec2f(0.0f, 0.0f));
	}
}

void LeapController::onInit(const Controller& controller) {
  console() << "Leap Initialized" << std::endl;
}

void LeapController::onConnect(const Controller& controller) {
  console() << "Leap Connected" << std::endl;
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

void LeapController::onDisconnect(const Controller& controller) {
  //Note: not dispatched when running in a debugger.
  console() << "Leap Disconnected" << std::endl;
}

void LeapController::onExit(const Controller& controller) {
  console() << "Leap Exited" << std::endl;
}

void LeapController::onFrame(const Controller& controller) {
	//console() << "Leap: onFrame" << std::endl;
	// Get the most recent frame and report some basic information
	const Frame frame = controller.frame();
  
	/*
	console() << "Frame id: " << frame.id()
			<< ", timestamp: " << frame.timestamp()
			<< ", hands: " << frame.hands().count()
			<< ", fingers: " << frame.fingers().count()
			<< ", tools: " << frame.tools().count()
			<< ", gestures: " << frame.gestures().count() << std::endl;
	*/

	if (!frame.hands().empty()) {
		const FingerList fingers = frame.fingers();
		for(int k = 0; k < fingers.count(); k++){
			if (!fingers.empty()){
				// Calculate the hand's average finger tip position
				for (int i = 0; i < fingers.count(); ++i) {
					ci::Vec2f currentFingerPos(fingers[i].tipPosition().x, fingers[i].tipPosition().y);
					fingerPositions[i] = currentFingerPos;

					ci::Vec2f currentFingerVel(fingers[i].tipVelocity().x, (fingers[i].tipVelocity().y*-1.0));
					fingerVelocities[i] = currentFingerVel;
				}
				avgPos = fingers[0].tipPosition(); 
				numActiveFingers = fingers.count();
				hasFingers = true;
			}
			else{
				hasFingers = false;
			}
		}

		/*
		// Get the first hand
		const Hand hand = frame.hands()[0];

		// Check if the hand has any fingers
		const FingerList fingers = hand.fingers();
		if (!fingers.empty()){
			// Calculate the hand's average finger tip position
			
			for (int i = 0; i < fingers.count(); ++i) {
				ci::Vec2f currentFingerPos(fingers[i].tipPosition().x, fingers[i].tipPosition().y);
				fingerPositions[i] = currentFingerPos;

				ci::Vec2f currentFingerVel(fingers[i].tipVelocity().x, (fingers[i].tipVelocity().y*-1.0));
				fingerVelocities[i] = currentFingerVel;
			}
			avgPos = fingers[0].tipPosition(); 
			numActiveFingers = fingers.count();
			//console() << "Hand has " << fingers.count() << " fingers, average finger tip position" << avgPos << std::endl;

			hasFingers = true;
		}
		else{
			hasFingers = false;
		}
		*/

		/*
		// Get the hand's sphere radius and palm position
		console() << "Hand sphere radius: " << hand.sphereRadius()
				  << " mm, palm position: " << hand.palmPosition() << std::endl;

		// Get the hand's normal vector and direction
		const Vector normal = hand.palmNormal();
		const Vector direction = hand.direction();

		// Calculate the hand's pitch, roll, and yaw angles
		console() << "Hand pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
				  << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
				  << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;
		*/
	}
	else{
		hasFingers = false;
	}

	/*
	// Get gestures
	const GestureList gestures = frame.gestures();
	for (int g = 0; g < gestures.count(); ++g) {
		Gesture gesture = gestures[g];

	switch (gesture.type()) {
		case Gesture::TYPE_CIRCLE:
		{
			CircleGesture circle = gesture;
			std::string clockwiseness;

			if (circle.pointable().direction().angleTo(circle.normal()) <= PI/4) {
				clockwiseness = "clockwise";
			} else {
				clockwiseness = "counterclockwise";
			}
			// Calculate angle swept since last frame
			float sweptAngle = 0;
			if (circle.state() != Gesture::STATE_START) {
				CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
				sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
			}
			console() << "Circle id: " << gesture.id()
				<< ", state: " << gesture.state()
				<< ", progress: " << circle.progress()
				<< ", radius: " << circle.radius()
				<< ", angle " << sweptAngle * RAD_TO_DEG
				<<  ", " << clockwiseness << std::endl;
			break;
		}
		case Gesture::TYPE_SWIPE:
		{
			SwipeGesture swipe = gesture;
			console() << "Swipe id: " << gesture.id()
			<< ", state: " << gesture.state()
			<< ", direction: " << swipe.direction()
			<< ", speed: " << swipe.speed() << std::endl;
			break;
		}
		case Gesture::TYPE_KEY_TAP:
		{
			KeyTapGesture tap = gesture;
			console() << "Key Tap id: " << gesture.id()
			<< ", state: " << gesture.state()
			<< ", position: " << tap.position()
			<< ", direction: " << tap.direction()<< std::endl;
			break;
		}
		case Gesture::TYPE_SCREEN_TAP:
		{
			ScreenTapGesture screentap = gesture;
			console() << "Screen Tap id: " << gesture.id()
			<< ", state: " << gesture.state()
			<< ", position: " << screentap.position()
			<< ", direction: " << screentap.direction()<< std::endl;
			break;
		}
		default:
			console() << "Unknown gesture type." << std::endl;
			break;
		}
	}

	if (!frame.hands().empty() || !gestures.empty()) {
		console() << std::endl;
	}*/
}

void LeapController::onFocusGained(const Controller& controller) {
  console() << "Leap Focus Gained" << std::endl;
}

void LeapController::onFocusLost(const Controller& controller) {
  console() << "Leap Focus Lost" << std::endl;
}
