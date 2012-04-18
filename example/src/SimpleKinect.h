//
//  SimpleKinect.h
//  ofxKinectExample
//
//  Created by Ben McChesney on 4/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
//  This class is just a simple wrapper for ofxKinect to not overwhelm beginners with code
//

#ifndef ofxKinectExample_SimpleKinect_h
#define ofxKinectExample_SimpleKinect_h

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

//ADDED
#include "ofxXmlSettings.h"

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

struct KinectCursor
{
    bool enabled ; 
    ofPoint nPosition ; 
    ofPoint position ; 
};

class SimpleKinect
{
    public :
        SimpleKinect () { } 
    
        vector<KinectCursor> kCursors ; 
        void setup( ) ; 
        void update( ) ; 
        void draw ( ) ; 
        void drawDebug ( ) ;
    	void keyPressed(int key) ;
        void exit() ; 
    
        ofxKinect kinect;
        
        ofxCvColorImage colorImg;
        
        ofxCvGrayscaleImage grayImage; // grayscale depth image
        ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
        ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
        
        ofxCvContourFinder contourFinder;
        
    	void drawPointCloud();

        //We can use openCV to break apart the user from the BG or cycle through the pixels ourselves
        bool bThreshWithOpenCV;
        bool bDrawPointCloud;
        
        int nearThreshold;
        int farThreshold;
        
        //Angle of the kinect ( this controls the motor at the base of the kinect ) 
        int angle;
        
        // used for viewing the point cloud
        ofEasyCam easyCam;
        
        //Storing the threshold values will make our lives easier
        ofxXmlSettings kinectXml ; 
        int maxNumCursors ; 
};

#endif
