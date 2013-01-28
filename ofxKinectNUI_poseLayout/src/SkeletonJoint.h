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

		int jointIndex ;			//It's own skeleton index
		int connectionIndex ;		//It's target index
		float connectionAngle ;		//Angle from itself to target ( degrees ? ) 
		string label ;				//Joint label such as 'head'

		ofPoint position ;			//Position in SCREEN SPACE
			
		void drawConnection( ofPoint connectionPosition ) ;
		void drawLabel ( ) ; 
		void drawAngle ( ) ; 

		bool bHover ;			//If on mouse move it's the closest point... I had plans for this but now I forget
};