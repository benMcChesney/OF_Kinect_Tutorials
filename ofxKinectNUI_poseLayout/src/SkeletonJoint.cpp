#include "SkeletonJoint.h"

void SkeletonJoint::setup ( int _jointIndex , int _connectionIndex , string _label ) 
{
	jointIndex = _jointIndex ; 
	connectionIndex = _connectionIndex ; 
	label = _label ; 
	position = ofPoint ( ofRandom( 0 , ofGetWidth() ) , ofRandom ( 0 , ofGetHeight() ) ) ; 
}

void SkeletonJoint::update ( ) 
{

}

void SkeletonJoint::updateAngle ( ofPoint connectionPosition ) 
{
	//float angleRad = connectionPosition.angle( position  ) ; 
	//connectionAngle = angleRad ;//ofRadToDeg( angleRad ) ; 

	connectionAngle =  -180 + ofRadToDeg( atan2( connectionPosition.y - position.y, connectionPosition.x - position.x ) ) ;  
	if ( connectionAngle < 0 ) connectionAngle += 360.0f ; 

	connectionAngle = 360.0f - connectionAngle ; 
}

void SkeletonJoint::draw ( )
{
	ofPushMatrix() ; 
		ofTranslate( position ) ; 
		if ( bHover == true )
		{
			ofSetColor( 255 , 255 ) ; 
			ofCircle( 0 , 0 , 10 ) ; 
		}
	ofPopMatrix() ; 
}

void SkeletonJoint::drawConnection( ofPoint connectionPosition ) 
{
	int numSubConnections = 8 ; 
	
	ofColor start = ofColor( 255 , 0 , 0 ) ; 
	ofColor end = ofColor( 0 , 0 , 255 ) ; 

	float maxThickness  = numSubConnections * 2.0f ;
	ofEnableSmoothing() ; 
	ofPushStyle() ; 
	for ( int i = 0 ; i < numSubConnections ; i++ ) 
	{
		float prevRatio = ( (float)i  ) / ( float ) numSubConnections ; 
		float ratio = ( (float)i + 1.0f ) / ( float ) numSubConnections ; 
		ofColor prevColor = start.lerp( end , prevRatio ) ; 
		ofColor color = start.lerp( end , ratio ) ; 
	
		ofPoint prevLerpPoint  = position.interpolated( connectionPosition , prevRatio ) ; 
		ofPoint lerpPoint  = position.interpolated( connectionPosition , ratio ) ; 

		ofSetColor( prevColor ) ;
		ofSetLineWidth( maxThickness * ratio ) ; 

		ofFill() ; 
		ofLine ( prevLerpPoint , lerpPoint ) ; 
		ofNoFill( ) ; 
		ofLine ( prevLerpPoint , lerpPoint ) ; 
	}
	ofPopStyle() ; 
}

void SkeletonJoint::drawLabel ( ) 
{
	string status = ofToString( jointIndex ) +":" + label ; // "\nx:" + ofToString( position.x ) + "\ny:" + ofToString( position.y ) ; 
	//if ( jointIndex == 0 ) 
		//status += "\nangle : " + ofToString( connectionAngle ) ; 
	int offsetX = status.size() * -4 ; 
	ofPushMatrix() ; 
		ofTranslate( position ) ; 
		ofDrawBitmapStringHighlight( status , offsetX , -25 ) ;
	ofPopMatrix() ; 

}

void SkeletonJoint::drawAngle ( ) 
{
	return ; 

	ofPushStyle() ; 
		ofSetColor( 0 , 255 , 0 ) ; 
		ofSetLineWidth( 2 ) ; 
		float angleLength = 15;


		
		//float x = cos ( ofDegToRad( connectionAngle ) ) * angleLength ; 
		//float y = sin ( ofDegToRad( connectionAngle ) ) * angleLength ;
		/*
		ofPoint newEndPoint ( cos ( nodes[i].angle ) * radius , sin ( nodes[i].angle ) * radius ) ;
		*/
		ofPushMatrix() ; 
			ofTranslate( position ) ; 
			ofRotateZ( 360 - connectionAngle ) ; 
			ofRect( -2 , 0 , 4 , -angleLength ) ; 
			ofCircle( 0 , -angleLength , 5 ) ; 
		ofPopMatrix() ; 
		//ofPoint angleP = ofPoint ( y , x ) ; 
		//ofLine ( position , position + angleP ) ;  
		//draw Angle !
		//ofCircle( position + angleP , 5 ) ; 

	ofPopStyle() ; 
}