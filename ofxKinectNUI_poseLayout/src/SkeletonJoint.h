#pragma once
#include "ofMain.h"

class SkeletonJoint 
{
	public :
		SkeletonJoint ( ) { } 
		~SkeletonJoint ( ) { }  ;

		void setup ( int _jointIndex , int _connectionIndex , string _label ) ; 
		void update ( ) ; 
		void updateAngle ( ofPoint connectionPosition ) ; 
		void draw ( ) ; 

		int jointIndex ; 
		int connectionIndex ; 
		float connectionAngle ; 
		string label ; 

		ofPoint position ; 

		void drawConnection( ofPoint connectionPosition ) ;
		void drawLabel ( ) ; 
		void drawAngle ( ) ; 

		bool bHover ; 
};