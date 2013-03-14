#pragma once

/* base class for dealing with skeleton data */


#include "ofMain.h"
#include "SkeletonJoint.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"

class BasicSkeleton
{
	public :
		BasicSkeleton( ) { } 
		~BasicSkeleton( ) { } 

		void setup ( bool _bUseOpenNI = true ) ;
		void update ( ) ;
		void updateOsc( ) ; 
		void draw( ) ; 
    
		//Loading default XML data
		void loadLabelsFromXml( string path ) ;
		void loadDefaultLayoutPositions( string path ) ;
		void saveDefaultLayoutPositions( ) ; 

		//Keep a vector of Skeleton Joints
		vector< SkeletonJoint > joints ; 
		ofxXmlSettings jointXml ; 

		//If there is a skeleton active --- NOT IMPLEMENTED
		bool bDetected ; 

		//Helper methods for drag + drop mouse stuff
		int getClosestJointIndexTo( ofPoint location ) ; 
		int getClosestJointIndexTo( float x , float y ) { return getClosestJointIndexTo( ofPoint( x , y ) ) ; } 

		//Debug drawing
		bool bDrawLabels ; 
		bool bDrawAngles ;
        bool bUseOpenNI ;

		void toggleLabels() ;

		//OSC reciever
		ofxOscReceiver * osc ; 
		bool bListenToOsc ; 

		//The skeleton comes in as between -1 and 1 , which is useless
		//so we scale it into a rectangle so it's human readable
		ofRectangle skeletonBounds ; 
};