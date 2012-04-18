#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground( 15 , 15 , 15 ) ; 
    ofSetFrameRate(60);
    
    simpleKinect.setup() ; 
}

//--------------------------------------------------------------
void testApp::update() {
	
	ofBackground(15, 15, 15);
	
    simpleKinect.update() ; 
    string windowString = "near : " + ofToString( simpleKinect.nearThreshold ) + " far : " + ofToString ( simpleKinect.farThreshold ) + "  @ " + ofToString( ofGetFrameRate() ) + " fps " ; 
    ofSetWindowTitle( windowString ) ; 

}

//--------------------------------------------------------------
void testApp::draw() {
	
    ofEnableAlphaBlending() ; 
	ofSetColor(255, 255, 255);
    simpleKinect.draw( ) ; 
	
}
     
 /*
 //ADDED
 int blobSize = contourFinder.blobs.size() ; 
 for ( int i = 0 ; i < blobSize ; i++ ) 
 {
 //contourFinder stores the center of each detected blob
 ofPoint center = contourFinder.blobs[i].centroid ; 
 //This point is relative to the sie of the feed so we need normalize it ( make it easier to work with ) 
 
 //C++ has operators that allow for simple calculations between classes
 // ofPoint nCenter = ( center.x / 400.0f , center.y / 300.0f ) ; 
 
 //OF has built in functions for a lot of things, like getting the windows size
 center.x /= kinect.getWidth() ; 
 center.y /= kinect.getHeight() ; 
 
 //Draw red circle in the box
 ofSetColor ( 255 , 0 , 0 , 125 ) ; 
 ofCircle( 10 + center.x * 200 , 10 + center.y * 150 , 12 ) ; 
 
 
 //Draw a blue circle on the screen
 ofSetColor ( 0 , 0 , 255 , 125 ) ; 
 ofCircle( center.x * ofGetWidth() , center.y * ofGetHeight() , 18 ) ; 
 }*/


//--------------------------------------------------------------
void testApp::exit() {
    
	simpleKinect.kinect.close();

}

//--------------------------------------------------------------
void testApp::keyPressed (int key) 
{
	simpleKinect.keyPressed( key ) ; 
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

