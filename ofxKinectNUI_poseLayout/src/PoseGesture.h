#pragma once

#include "ofMain.h"
#include "BasicSkeleton.h"
#include "VisualTimer.h"
#include "PoseGestureEvents.h"

class PoseGesture
{
	public : 
		PoseGesture() { } 
		~PoseGesture( ) { }

		void setup ( string _label , int _gestureIndex ) ; 
		void update ( ) ; 
		void draw ( BasicSkeleton skeleton ) ; 
		void drawTimer ( float x , float y ) ; 
		void addJoint( int index , float angle ) ;
		void clear( ) ; 

		int gestureIndex ; 
		string label ; 
		vector<int> jointIndicies ; 
		vector<float> angles ; 
		vector<bool> anglesAlignment ; 

		float totalAngleDiff ; 

		void checkGesture( BasicSkeleton skeleton ) ; 

		float angleTolerance ;					//Threshold ( error tolerance ) of angle for a trigger 
		float gestureHoldTime ;				//Users have to hold the gesture for a short amount of time
		float lastGestureActivateTime ;		//For counting time

		bool bGestureActive ; 
		float gestureCompletion ; 

		void exportToXml( string xmlPath ) ; 

		VisualTimer poseTimer ; 
		ofColor debugColor ;
		
};