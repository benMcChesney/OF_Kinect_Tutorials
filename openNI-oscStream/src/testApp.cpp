#include "testApp.h"

class myUser : public ofxOpenNIUser {
public:
    void test(){
        cout << "test" << endl;
    }
};

//--------------------------------------------------------------
void testApp::setup() {
    
 
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    /*
    openNIRecorder.setup();
    openNIRecorder.addDepthGenerator();
    openNIRecorder.addImageGenerator();
    openNIRecorder.setRegister(true);
    openNIRecorder.setMirror(true);
    openNIRecorder.addUserGenerator();
    openNIRecorder.setMaxNumUsers(2);
    //openNIRecorder.start();
    */
    
    openNIPlayer.setup();
	openNIPlayer.start();
    openNIPlayer.startPlayer("skeleton.oni");
    openNIPlayer.setRegister(true);
    openNIPlayer.setMirror(true);
    openNIPlayer.addUserGenerator();
    openNIPlayer.setMaxNumUsers(2);

    //Setup the OSC network and the port
    oscSender.setup("localhost", 12345 ) ;
    
    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
}

//--------------------------------------------------------------
void testApp::update(){
    //openNIRecorder.update();
    openNIPlayer.update();
    
    //We will send the entire skeleton X + Y data in one message
    ofxOscMessage m ;
    m.setAddress( "skeletonData/" ) ; 
    
    // Make sure we have A user
    int currentTrackedUsers = openNIPlayer.getNumTrackedUsers( ) ;
    if ( currentTrackedUsers > 0 )
    {
        //Only send the first user for now
        ofxOpenNIUser& user = openNIPlayer.getTrackedUser( 0 );
        float w = openNIPlayer.getWidth() ;
        float h = openNIPlayer.getHeight() ; 
        int numJoints = user.getNumJoints() ;
        for ( int i = 0 ; i < numJoints ; i++ )
        {
            ofxOpenNIJoint iJoint = user.getJoint( (Joint) i ) ;
            ofPoint p = iJoint.getProjectivePosition() ;
            m.addFloatArg( p.x / w ) ;
            m.addFloatArg( p.y / h ) ;
        }
    }
    
    oscSender.sendMessage( m ) ;

}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(255, 255, 255);
    
    ofPushMatrix();
    
    //Some weird blend modes are called, so we isolate it to make sure
    //it doens't mess up rendering anything after
    ofPushStyle() ;
        openNIPlayer.drawDebug(0, 0);
    ofPopStyle() ;
    
    ofEnableAlphaBlending() ;
    ofFill( );
    
    
    int currentTrackedUsers = openNIPlayer.getNumTrackedUsers( ) ;
    if ( currentTrackedUsers > 0 )
    {
        ofxOpenNIUser& user = openNIPlayer.getTrackedUser( 0 );
        int numJoints = user.getNumJoints() ;
        //cout <<" THERE IS A USER! with " << numJoints << endl ;
        float hueIncrement = 1.0f / (float)numJoints ; 
        for ( int i = 0 ; i < numJoints ; i++ )
        {
            float hue = ( i + 1 ) * hueIncrement ; 
            ofxOpenNIJoint iJoint = user.getJoint( (Joint) i ) ;

            ofSetColor( ofColor::fromHsb( hue , 255 , 255 ) ) ;
            ofPoint p = iJoint.getProjectivePosition() ;
            //We ignore Z because it's scale is way crazier than pixel space
            ofCircle(  p.x , p.y , 6 ) ;
            
            ofDrawBitmapStringHighlight( ofToString(i) + ":" + iJoint.getName() , p.x , p.y - 8 ) ;
        }
    }


    ofPushMatrix();
    
	ofSetColor(0, 255, 0);
	string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate());
	verdana.drawString(msg, 20, 2 * 480 - 20);
}

//--------------------------------------------------------------
void testApp::userEvent(ofxOpenNIUserEvent & event){
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user" << event.id << "from device" << event.deviceID;
}

//--------------------------------------------------------------
void testApp::gestureEvent(ofxOpenNIGestureEvent & event){
    ofLogNotice() << event.gestureName << getGestureStatusAsString(event.gestureStatus) << "from device" << event.deviceID << "at" << event.timestampMillis;
}

//--------------------------------------------------------------
void testApp::exit(){
   // openNIRecorder.stop();
    openNIPlayer.stop();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

    int cloudRes = -1;
    switch (key) {
        case ' ':
            /*
            if(!openNIRecorder.isRecording()){
                openNIRecorder.startRecording(ofToDataPath("test.oni"));
            }else{
                openNIRecorder.stopRecording();
            }
             */
            break;
        case 'p':
            openNIPlayer.startPlayer("test.oni");
            break;
        case '/':
            openNIPlayer.setPaused(!openNIPlayer.isPaused());
            break;
        case 'm':
            openNIPlayer.firstFrame();
            break;
        case '<':
        case ',':
            openNIPlayer.previousFrame();
            break;
        case '>':
        case '.':
            openNIPlayer.nextFrame();
            break;
        case 'x':
            //openNIRecorder.stop();
            openNIPlayer.stop();
            break;
        case 't':
            //openNIRecorder.toggleRegister();
            break;
    }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}