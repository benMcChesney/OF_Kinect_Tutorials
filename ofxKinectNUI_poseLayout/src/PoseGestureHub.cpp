#pragma once

#include "PoseGestureHub.h"

void PoseGestureHub::setup ( ) 
{
	bDetectGestures = false ; 
	numExported = 0; 
	poseAngleTolerance = 15 ; 
	bDrawOverlays = false ; 
}
		
void PoseGestureHub::update ( BasicSkeleton skeleton ) 
{
	for ( int i = 0 ; i < poses.size() ; i++ ) 
	{
		poses[i]->update() ; 
		if ( bDetectGestures == true ) 
			poses[i]->checkGesture( skeleton ) ;
	}
}


void PoseGestureHub::draw ( BasicSkeleton skeleton  ) 
{
	if ( bDrawOverlays ) 
	{
		for ( int i = 0 ; i < poses.size() ; i++ )
		{
			poses[i]->draw( skeleton ) ; 
		}
	}
}

void PoseGestureHub::setPoseAngleTolerance( float tolerance ) 
{
	poseAngleTolerance = tolerance ; 
	if ( poses.size()  > 0 ) 
	{
		for ( int i = 0 ; i < poses.size() ; i++ )
		{
			poses[i]->angleTolerance = poseAngleTolerance ; 
		}
	}
}

void PoseGestureHub::addJointToPose( int jointIndex , float angle ) 
{
	if ( poses.size()  > 0 ) 
	{
		cout << "adding joint to : pose # :"  << poses.size() -1 << endl ; 
		poses[ (poses.size()-1) ]->addJoint( jointIndex , angle ) ;  
	}
}

//void loadPose( string xmlPath ) ; 
void PoseGestureHub::loadPose( string xmlPath ) 
{
	cout << "load Pose not written yet" << endl ; 
	ofxXmlSettings xml ; 
	bool bLoadResult = xml.loadFile( xmlPath ) ; 

	if ( bLoadResult == false ) 
	{
		cout << "xml @ " << xmlPath << " did not load ! " << endl ; 
	}
	string name = xml.getValue( "gestureName" , "no name" ) ;
	PoseGesture * pose = new PoseGesture( ) ; 
	pose->setup( name , poses.size() ) ; 
	
	xml.pushTag( "gesture" ) ; 
	int numJoints = xml.getNumTags( "angle" ) ; 
	for ( int i = 0 ; i < numJoints ; i++ ) 
	{
		float angle = xml.getValue( "angle" , 0.0f , i ) ; 
		int index = xml.getValue( "jointIndex" , 0 , i ) ; 
		pose->addJoint( index , angle ) ; 
	}

	pose->angleTolerance = poseAngleTolerance ; 
	poses.push_back( pose ) ; 
	cout << "added a pose with #" << numJoints << " of angles/joints" << endl ;

	xml.popTag() ; 
}

void PoseGestureHub::exportAllPoses ( ) 
{
	for ( int i = 0 ; i < poses.size() ; i++ ) 
	{
		numExported++ ; 
		poses[i]->exportToXml( poses[i]->label + "_pose" ) ; 
	}
}

void PoseGestureHub::addNewPose ( string _label )
{
	//ofSystemSaveDialog(...)
	//ofFileDialogResult ofSystemSaveDialog(string defaultName, string messageName)

	ofFileDialogResult result = ofSystemSaveDialog( "DEFAULT_POSE_NAME" , "name your new POSE!" ) ; 
	string poseName = result.getName() ; 

	if ( poses.size() > 0 ) 
	{
		for ( int i = 0 ; i < poses.size()  ; i++ ) 
		{
			if ( poses[i]->label.compare( poseName ) == 0 ) 
			{
				cout << "The pose name '" << poseName << "' already exists! adding a _ + a random Number" << endl ; 
				poseName += "_"+ ofToString( ofRandom( 0 , 1000 ) )  ; 
				break ; 
			}
		}
	}

	cout << "adding a new pose ! there are now : " << poses.size() + 1 << endl ; 
	PoseGesture * pose = new PoseGesture() ; 
	pose->setup( poseName , poses.size() ) ; 
	poses.push_back( pose ) ; 
}

void PoseGestureHub::drawDebug( ) 
{
	ofPushMatrix() ;
	float xOffset = ofGetWidth() / 2 - 100 ; 
		for ( int a = 0 ; a < poses.size() ; a++ ) 
		{
			float radius = poses[a]->poseTimer.radius ; 
			float paddedRadius = radius * 2.25 ; 
			float xPadding = paddedRadius * 2.0f ; 
			poses[a]->drawTimer( xOffset + paddedRadius + a * xPadding , paddedRadius ) ; 
			ofDrawBitmapStringHighlight( poses[a]->label , xOffset + paddedRadius + a * xPadding , paddedRadius + 45 , ofColor::black , poses[a]->poseTimer.color ) ; 
		}
	ofPopMatrix() ; 

}

PoseGesture* PoseGestureHub::getPoseByLabel( string label ) 
{
	//

	for ( int i = 0 ; i < poses.size() ; i++ ) 
	{
		//Found a match !
		if ( label.compare( poses[i]->label ) == 0 )  
			return poses[i] ; 
	}

	//Nothing found, return NULL
	return NULL ; 
}
