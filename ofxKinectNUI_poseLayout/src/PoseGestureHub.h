#pragma once

#include "ofMain.h"
#include "PoseGesture.h"
#include "BasicSkeleton.h"
#include "ofEvents.h"

class PoseGestureHub 
{
	public : 
		PoseGestureHub( ) { } 
		~PoseGestureHub( ) { } 

		void setup ( ) ; 
		void update ( BasicSkeleton skeleton ) ; 
		void draw ( BasicSkeleton skeleton ) ; 

		void loadPose( string xmlPath ) ; 
		void exportAllPoses ( ) ; 
		void addNewPose ( string _label ) ;
		void addJointToPose( int jointIndex , float angle ) ; 

		vector<PoseGesture*> poses ; 
		PoseGesture* getPoseByLabel( string label ) ; 

		void setPoseAngleTolerance( float tolerance ) ; 
		void setPoseHoldTime( float time ) ; 
		bool bDetectGestures ; 
		int numExported ; 

		void drawDebug( ) ; 

		float poseAngleTolerance ; 
		bool bDrawOverlays ; 


};