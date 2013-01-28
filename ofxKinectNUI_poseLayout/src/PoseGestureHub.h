#pragma once

#include "ofMain.h"
#include "PoseGesture.h"
#include "BasicSkeleton.h"
#include "ofEvents.h"

/* the "brains" of the pose gesture operation */

class PoseGestureHub 
{
	public : 
		PoseGestureHub( ) { } 
		~PoseGestureHub( ) { } 

		void setup ( ) ; 
		void update ( BasicSkeleton skeleton ) ; 
		void draw ( BasicSkeleton skeleton ) ; 

		//Load in a new XML pose
		void loadPose( string xmlPath ) ; 
		void exportAllPoses ( ) ; 
		void addNewPose ( string _label ) ;
		void addJointToPose( int jointIndex , float angle ) ; 

		//Keep all our poses in a vector, they are POINTERS
		vector<PoseGesture*> poses ; 
		PoseGesture* getPoseByLabel( string label ) ; 

		void setPoseAngleTolerance( float tolerance ) ;		//ofxUI handler
		void setPoseHoldTime( float time ) ;				//ofxUI handler
		float poseHoldTime ;		//time a pose is "held" for a trigger
		bool bDetectGestures ;		//do we bother looking for gestures ?
		int numExported ;			//useless should remove

		void drawDebug( ) ;

		float poseAngleTolerance ; 
		bool bDrawOverlays ; 


};