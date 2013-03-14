#pragma once

/*
 
 BASED off of the original ofxKinect example !
 github.com/ofTheo/ofxKinect
 
 I just added ofxUI and made some stuff a little easier to digest
 
 */

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "demoParticle.h"
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
    vector<ofPoint> cursors ;
    float cursorXSensitivity ;
    float cursorYSensitivity ;
    bool bRestrictCursors ;
    float cursorBorderPadding ;
    
    particleMode currentMode;
    string currentModeStr;
    
    void resetParticles();
    vector <demoParticle> p;
    vector <ofPoint> attractPoints;
    vector <ofPoint> attractPointsWithMovement;
    
};
