#pragma once

#include "ofMain.h"
#include "SkeletonJoint.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"

class BasicSkeleton
{
	public :
		BasicSkeleton( ) { } 
		~BasicSkeleton( ) { } 

		void setup ( ) ;
		void update ( ) ;
		void updateOsc( ) ; 
		void draw( ) ; 

		void loadLabelsFromXml( ) ;
		void loadDefaultLayoutPositions( ) ;
		void saveDefaultLayoutPositions( ) ; 

		vector< SkeletonJoint > joints ; 
		ofxXmlSettings jointXml ; 
		bool bDetected ; 

		int getClosestJointIndexTo( ofPoint location ) ; 
		int getClosestJointIndexTo( float x , float y ) { return getClosestJointIndexTo( ofPoint( x , y ) ) ; } 

		bool bDrawLabels ; 
		bool bDrawAngles ; 
		void toggleLabels() ; 

		ofxOscReceiver * osc ; 
		bool bListenToOsc ; 

		ofRectangle skeletonBounds ; 

	
};