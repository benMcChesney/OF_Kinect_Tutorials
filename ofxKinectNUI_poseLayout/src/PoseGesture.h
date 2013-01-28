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


		int gestureIndex ;		//it's own index
		string label ;			//it's own label
		vector<int> jointIndicies ;		//joints that matter
		vector<float> angles ;			//angles of joints that matter
		vector<bool> anglesAlignment ;	// !!!!!!!!!!! If angles are in alignment NOT IMPLEMENTED !!!!!!

		float totalAngleDiff ;	//Total angle difference between all important angles

		void checkGesture( BasicSkeleton skeleton ) ; 

		float angleTolerance ;				//Threshold ( error tolerance ) of angle for a trigger 
		float gestureHoldTime ;				//Users have to hold the gesture for a short amount of time
		float lastGestureActivateTime ;		//For counting time

		bool bGestureActive ;				//if this gesture is currently active
		float gestureCompletion ;			// between 0 and 1 of the hold duration

		void exportToXml( string xmlPath ) ; 

		VisualTimer poseTimer ;				//Visual timer for countdowns
		ofColor debugColor ;				//having coordinates debug colors makes the eyes hurt less
		
};