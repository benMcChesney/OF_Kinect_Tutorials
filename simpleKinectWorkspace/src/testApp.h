#pragma once

#include "ofMain.h"
#include "SimpleKinect.h"




//Based on Theo Watson's ofxKinect example but simplified

class testApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	

	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
    
    //All our kinect stuff goes into another class to make it simple !
    SimpleKinect simpleKinect ; 
    

};
