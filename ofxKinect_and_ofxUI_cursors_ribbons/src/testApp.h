#pragma once

/*
 
 BASED off of the original ofxKinect example !
 github.com/ofTheo/ofxKinect
 
 I just added ofxUI and made some stuff a little easier to digest
 
 */

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"'

#include "ofxUI.h"

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

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
	void windowResized(int w, int h);
    
    void setup_ofxUI() ;
	
	ofxKinect kinect;
	
#ifdef USE_TWO_KINECTS
	ofxKinect kinect2;
#endif
	
	ofxCvColorImage colorImg;
	
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	
	ofxCvContourFinder contourFinder;
	
	bool bThreshWithOpenCV;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
    
    //added for ofxUI
    ofxUICanvas *gui;
    float guiWidth ; 
	void guiEvent(ofxUIEventArgs &e);
    
    float minBlobSize , maxBlobSize ;
    
    bool bKinectOpen ;
    
    //Each frame take the number of blobs and create cursors at their centroids
    vector<ofVec2f> cursors ;
    float cursorXSensitivity ;
    float cursorYSensitivity ;
    bool bRestrictCursors ;
    float cursorBorderPadding ;
    
    //this holds all of our points
    vector<ofVec3f> points;
    //this keeps track of the center of all the points
    ofVec3f center;
    
	//our camera objects for looking at the scene from multiple perspectives
	ofCamera camera;
	
	//if usecamera is true, we'll turn on the camera view
    bool usecamera;
    
    ofLight light ;
    
    int maxCursors ;
    
    
    
};
