#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() 
{
	ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(60);
    
    ofBackground( 0 , 0 , 0 ) ; 
    simpleKinect.setup() ; 
}

//--------------------------------------------------------------
void testApp::update() 
{
    simpleKinect.update() ; 
}

//--------------------------------------------------------------
void testApp::draw() {
	
	ofSetColor(255, 255, 255);
	simpleKinect.draw() ; 
    simpleKinect.drawDebug() ; 
    
    //We can access the cursor list from the simple kinect class to draw our own cursors !
    ofEnableAlphaBlending() ; 
    ofSetHexColor( 0xF7E967 ) ; 
    vector<KinectCursor> kCursors = simpleKinect.kCursors ; 
    for ( int i = 0 ; i < kCursors.size() ; i++ ) 
    {
        if ( kCursors[i].enabled == true ) 
        {
            ofPoint p = kCursors[i].nPosition ; 
            p.x *= 400 ; 
            p.y *= 300 ; 
            ofCircle( p.x , p.y , 16 ) ;   
        }
    }

}

//--------------------------------------------------------------
void testApp::exit() {
    simpleKinect.exit() ; 
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

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}
