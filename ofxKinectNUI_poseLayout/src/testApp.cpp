#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
	skeleton.setup( ) ; 

	//Start Clear
	poseHub.setup( ) ;
	
	//load premade configs from XML
	poseHub.loadPose( "jesus_pose.xml" ) ; 
	poseHub.loadPose( "standing_pose.xml" ) ; 
	poseHub.loadPose( "tStance_pose.xml" ) ;
	poseHub.loadPose( "raisedHands_pose.xml" ) ; 

	ofSetFrameRate( 30 ) ; 

	//Add an event listener so we know when we need to change state
	ofAddListener( PoseGestureEvents::Instance()->POSE_RECOGNIZED , this , &testApp::poseRecognizedEventHandler ) ; 
	ofAddListener( PoseGestureEvents::Instance()->POSE_START , this , &testApp::poseStartEventHandler ) ; 
	ofAddListener( PoseGestureEvents::Instance()->POSE_END , this , &testApp::poseEndEventHandler ) ; 

	setupUI( ) ; 
}

//--------------------------------------------------------------
void testApp::update(){

	skeleton.update( ) ; 
	poseHub.update( skeleton ) ;
	skeleton.updateOsc() ; 
	ofSetWindowTitle( "FPS:" + ofToString( ofGetFrameRate() ) );
}

//------------------------------------------------v--------------
void testApp::draw(){
	skeleton.draw( ) ; 

	poseHub.draw( skeleton ) ; 
	poseHub.drawDebug( ) ; 

}

void testApp::poseRecognizedEventHandler( string &args ) 
{
	cout << "POSE '" << args << "' IS RECOGNIZED ! " << endl ; 

	PoseGesture * pg = poseHub.getPoseByLabel( args ) ; 

	float percentTint = 0.75 ; 
	//Always check in case somethign goes arwy
	if ( pg != NULL ) 
		ofBackground( pg->debugColor.r * percentTint, pg->debugColor.g * percentTint  , pg->debugColor.b * percentTint  ) ; 
}

void testApp::poseStartEventHandler( string &args ) 
{
	cout << "POSE '" << args << "' IS STARTING  ! " << endl ; 
}

void testApp::poseEndEventHandler( string &args )
{
	cout << "POSE '" << args << "' IS ENDING  ! " << endl ; 
	ofBackground( 128 ) ; 
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){

	//ofSaveScreen( ofToString(ofGetUnixTime() )+ ".png" ) ;
	switch ( key ) 
	{
		case 's':
		case 'S':
			//skeleton.saveDefaultLayoutPositions( ) ; 
			break; 

		case 'e':
		case 'E':
			poseHub.exportAllPoses( ) ; 
			break ; 

		case 'n':
		case 'N':
			poseHub.addNewPose( "NEW POSE" ) ; 
			break; 

	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

	if ( x < 300 ) return ; 
		skeleton.getClosestJointIndexTo( x , y ) ; 
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
	if ( x < 300 ) return ; 
	if ( button == 0 ) 
	{
		int closestIndex = skeleton.getClosestJointIndexTo( x , y ) ; 
		skeleton.joints[ closestIndex ].position = ofPoint ( x , y ) ; 
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
	if ( x < 300 ) return ; 
	int closestIndex = skeleton.getClosestJointIndexTo( x , y ) ; 
	if ( button == 0 ) 
	{
		skeleton.joints[ closestIndex ].position = ofPoint ( x , y ) ; 
	}
	if ( button == 1 ) 
	{
		cout << "ADDING A JOINT + ANGLE TO POSE!" << endl ; 
		poseHub.addJointToPose( closestIndex , skeleton.joints[closestIndex].connectionAngle ) ; 
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
	if ( x < 300 ) return ; 
	if ( button == 0 ) 
	{
		int closestIndex = skeleton.getClosestJointIndexTo( x , y ) ; 
		skeleton.joints[ closestIndex ].position = ofPoint ( x , y ) ; 
	}
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::setupUI ()
{
	//Setup OFXUI and all the default variables
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = 255-xInit; 
	float dim = 20; 
	gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight()); 
	gui->addWidgetDown(new ofxUILabel("'N' to add NEW pose", OFX_UI_FONT_LARGE)); 
	gui->addWidgetDown(new ofxUILabel("'E' to EXPORT all poses", OFX_UI_FONT_LARGE)); 
	gui->addWidgetDown(new ofxUIToggle( dim , dim , skeleton.bListenToOsc ,"LISTEN TO OSC" ) );  
	gui->addWidgetDown(new ofxUIToggle( dim , dim , skeleton.bDrawLabels ,"DRAW LABELS" ) );
	gui->addWidgetDown(new ofxUIToggle( dim , dim , poseHub.bDetectGestures ,"CHECK FOR GESTURES" ) );
	gui->addWidgetDown(new ofxUISlider(length-xInit,dim, 0.0, 90.0f , poseHub.poseAngleTolerance , "ANGLE TOLERANCE")); 
	gui->addWidgetDown(new ofxUISlider(length-xInit,dim, 0.05, 2.0f , poseHub.poseHoldTime , "POSE HOLD DURATION")); 
 	
	//Add new sliders here !
	ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
	gui->loadSettings( "GUI/settings.xml" ) ;
}
	
void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
	//cout << "got event from: " << name << endl; 	
	
	if ( name == "ANGLE TOLERANCE" ) poseHub.setPoseAngleTolerance(  ((ofxUISlider *) e.widget)->getScaledValue() ) ; 
	if ( name == "POSE HOLD DURATION" ) poseHub.setPoseHoldTime(  ((ofxUISlider *) e.widget)->getScaledValue() ) ; 

	if ( name == "LISTEN TO OSC" ) skeleton.bListenToOsc = ((ofxUIToggle *) e.widget)->getValue() ;
	if ( name == "DRAW LABELS" ) skeleton.bDrawLabels = ((ofxUIToggle *) e.widget)->getValue() ;
	if ( name == "CHECK FOR GESTURES"  ) poseHub.bDetectGestures = ((ofxUIToggle *) e.widget)->getValue() ;

	//Handle new sliders here !

	/* non-compressed example
	if(name == "RED")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		cout << "RED " << slider->getScaledValue() << endl; 
		//red = slider->getScaledValue(); 
	}*/

	gui->saveSettings( "GUI/settings.xml" ) ;
}