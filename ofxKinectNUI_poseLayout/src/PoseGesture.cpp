#include "PoseGesture.h"

void PoseGesture::setup ( string _label , int _gestureIndex ) 
{
	gestureIndex = _gestureIndex ;
	label = _label ; 
	lastGestureActivateTime = -1.0f ; 
	gestureHoldTime = 0.5f ; 
	angleTolerance = 4.0f ; 
	bGestureActive = false ; 
	totalAngleDiff = 0.0f  ; 
	gestureCompletion = 0.0f ; 
	debugColor = ofColor::fromHsb( ofRandom(255 ) , 255 , 255 ) ;  
	poseTimer.setup( 25 , debugColor ); 

}

void PoseGesture::update ( ) 
{
	poseTimer.normalizedTime = gestureCompletion ; 
	poseTimer.update() ; 
}

void PoseGesture::drawTimer ( float x , float y ) 
{
	ofSetColor ( 255 ) ; 
	poseTimer.draw( x , y ) ; 
}

void PoseGesture::draw( BasicSkeleton skeleton ) 
{
	for ( int i = 0 ; i < anglesAlignment.size() ; i++ ) 
	{
		if ( anglesAlignment[i] == true ) 
		{
			//anglesAlignment
			ofPoint p = skeleton.joints[ jointIndicies[i] ].position ; 
			ofSetColor( debugColor ) ; 
			ofCircle( p , 10 ) ; 
		}
	}
}

void PoseGesture::addJoint( int index , float angle ) 
{
	jointIndicies.push_back( index ) ; 
	angles.push_back( angle ) ; 
	anglesAlignment.push_back ( false ) ; 
}

void PoseGesture::checkGesture( BasicSkeleton skeleton )
{
	totalAngleDiff = 0.0f ; 
	bool bBroken = false ; 
	for ( int i = 0 ; i < angles.size() ; i++ ) 
	{
		
		float angleDiff = skeleton.joints[  jointIndicies[i]  ].connectionAngle - angles[i] ;  
		if ( abs(angleDiff) > angleTolerance ) 
		{
			//ANGLE IS TOO FAR OFF
			angleDiff = 0 ; 
			anglesAlignment[i] = false ; 
			bBroken = true ; 
		//	return ; 
		}
		else
		{
			anglesAlignment[i] = true ; 
		}
		totalAngleDiff += angleDiff ; 
	}

	/*
		float angleTolerance ;					//Threshold ( error tolerance ) of angle for a trigger 
		float gestureHoldTime ;				//Users have to hold the gesture for a short amount of time

		float lastGestureActivateTime ;		//For counting time

		ofEvent<string> POSE_START ;
		ofEvent<string> POSE_END ; 
*/
	//PoseGestureEvents
	if ( bBroken == false ) 
	{
		if ( bGestureActive == false ) 
		{
			if ( lastGestureActivateTime < 0 ) 
			{
				lastGestureActivateTime = ofGetElapsedTimef() ; 
				//cout << "JUST STARTED POSE ! should be done @ " << ofGetElapsedTimef() + gestureHoldTime << endl ; 
				ofNotifyEvent( PoseGestureEvents::Instance()->POSE_START , label ) ; 
				gestureCompletion = 0.0f ; 
			}
			else
			{
				gestureCompletion = ( ofGetElapsedTimef() - lastGestureActivateTime ) / (  gestureHoldTime ) ; 
				//cout <<  ( ofGetElapsedTimef() - lastGestureActivateTime ) << " / " << gestureHoldTime << endl ; 
				//cout << "gesture compelte % " << gestureCompletion * 100.0f << endl; 
				if ( ofGetElapsedTimef() > ( lastGestureActivateTime + gestureHoldTime ) ) 
				{
					//Held for long enough ! The gesture should be active
					lastGestureActivateTime = -2 ; 
					bGestureActive = true ; 
					ofNotifyEvent( PoseGestureEvents::Instance()->POSE_RECOGNIZED , label ) ; 
				}
			}
		}
	}
	else
	{
		//Be sure to fire an end event if there was any progress
		if( gestureCompletion > 0.0f ) 
			ofNotifyEvent( PoseGestureEvents::Instance()->POSE_END , label ) ; 
		gestureCompletion= 0.0f ; 
		lastGestureActivateTime = -2 ; 
		bGestureActive = false ;
	}

}

void PoseGesture::exportToXml( string xmlPath ) 
{
	ofxXmlSettings xml ; 
	xml.addValue( "gestureName" , label ) ; 
	xml.addTag( "gesture" ) ; 

	xml.pushTag( "gesture" , 0 ) ; 
		for ( int i = 0 ; i < angles.size() ; i++ ) 
		{
			xml.addValue( "angle" , angles[i] ) ; 
			xml.addValue( "jointIndex" , jointIndicies[i] ) ; 
		}
	xml.popTag( ) ;

	string fullPath =   xmlPath + ".xml"  ; //, false ) ; 
	bool bResult = xml.saveFile(  fullPath ) ; 
	cout << "result from saving to : " << fullPath << " :: " << bResult << " ____ exported XML file " << endl ; 


}

void PoseGesture::clear( ) 
{
	cout << "CLEARING POSE GESTURE" << endl ; 
	jointIndicies.clear() ; 
	angles.clear() ; 
}

/*
	string label ; 
	vector<int> jointIndicies ; 
	vector<float> angles ; 

	float threshold ;				//Threshold ( error tolerance ) of angle for a trigger 
	float gestureHoldTime ;			//Users have to hold the gesture for a short amount of time
	float lastGestureActivateTime ;	//For counting time
	bool bActive ;		
*/

