#pragma once

#include "ofMain.h"
#include "BasicSkeleton.h"
#include "PoseGestureHub.h"
#include "PoseGestureEvents.h"
#include "ofxUI.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		//holds all our skeleton data and listens to OSC
		BasicSkeleton skeleton ; 

		//Takes in the skeleton data and loads all gesture XMLs
		PoseGestureHub poseHub ;

		//Event Handlers
		void poseRecognizedEventHandler( string &args ) ; 
		void poseStartEventHandler( string &args ) ; 
		void poseEndEventHandler( string &args ) ; 

		//Easy UI to make thigns easier to change
		ofxUICanvas * gui ;   
		void setupUI () ; 
		void guiEvent(ofxUIEventArgs &e);
};
