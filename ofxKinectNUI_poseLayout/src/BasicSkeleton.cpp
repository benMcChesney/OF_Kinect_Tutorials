#include "BasicSkeleton.h"

void BasicSkeleton::setup ( ) 
{
	bDetected = false ; 
	loadLabelsFromXml( )  ; 
	loadDefaultLayoutPositions( ) ; 
	bDrawLabels = true ; 
	bDrawAngles = true ; 

	osc = new ofxOscReceiver( ) ; 
	osc->setup( 54321 ) ; 

	skeletonBounds = ofRectangle( 300 , 300 , 300 , 300 ) ; 
	bListenToOsc = false ; 
}

void BasicSkeleton::update ( )
{
	for ( int i = 0 ; i < joints.size() ; i++ ) 
	{
		joints[i].update( ) ; 
	}

	for ( int i = 0 ; i < joints.size() ; i++ ) 
	{
		joints[i].updateAngle( joints[joints[i].connectionIndex].position ) ; 
	}
}

void BasicSkeleton::updateOsc( ) 
{
	if ( bListenToOsc == false ) return ; 
	//return ; 
	int loop = 0 ; 
	while( osc->hasWaitingMessages() )
	{
		//cout << " waiting for message" << endl ; 
		ofxOscMessage m;
		osc->getNextMessage(&m);
		
		
		//cout << "message address : " << m.getAddress() << endl ;
		if ( m.getAddress() == "skeletonData/" ) 
		{
			loop++ ; 
			int numArgs = m.getNumArgs() ; 
			//cout << "number of arguments " << numArgs << endl ; 
			//int jointNum = 0 ; 
			int argNum = 0 ; 
			//cout << "loop : " << loop++ << endl ; 
			for ( int j = 0 ; j < 20 ; j++ ) 
			{
				//cout << "@ joint : " << j << endl ; 
				joints[j].position.x = m.getArgAsFloat( argNum ) * skeletonBounds.width + skeletonBounds.x ; 
				joints[j].position.y = ( -m.getArgAsFloat( argNum+1 ) ) * skeletonBounds.height + skeletonBounds.y ;

			//	cout << " raw x : " <<  m.getArgAsFloat( argNum ) << " , raw y : " <<  m.getArgAsFloat( argNum+1 ) << endl ;
				argNum += 2 ;
			}
			return ; 
			//cout << "currentFrame : " << ofToString( ofGetFrameNum() ) << endl ; 
		}
	}
}

void BasicSkeleton::draw( ) 
{
	ofSetColor( 255 , 12 , 0 ) ; 
	for ( int i = 0 ; i < joints.size() ; i++ ) 
	{
		ofCircle( joints[i].position , 5 ) ; 
		//joints[i].drawConnection( joints[joints[i].connectionIndex].position ) ; 
	}

	for ( int i = 0 ; i < joints.size() ; i++ ) 
	{
		joints[i].drawConnection( joints[joints[i].connectionIndex].position ) ; 
	}

	if ( bDrawLabels ) 
	{
		for ( int i = 0 ; i < joints.size() ; i++ ) 
		{
			joints[i].drawLabel( ) ; 
		}
	}

	if ( bDrawAngles ) 
	{
		for ( int i = 0 ; i < joints.size() ; i++ ) 
		{
			joints[i].drawAngle( ) ; 
		}
	}

	for ( int i = 0 ; i < joints.size() ; i++ ) 
	{
		joints[i].draw( ) ; 
	}
}

void BasicSkeleton::loadLabelsFromXml( ) 
{
	jointXml.loadFile( "jointLabels.xml" ) ; 

	int numJoints = jointXml.getNumTags( "joint" ) ;
	for ( int i = 0 ; i < numJoints ; i++ ) 
	{
		string label = jointXml.getAttribute("joint" , "label" , "noLabel" , i ) ; 
		cout << "label : " << label << endl ; 
		int connectionIndex =  jointXml.getAttribute("joint" , "connect" , -25 , i ) ; 
		cout << "connectionIndex : " << connectionIndex << endl; 
		SkeletonJoint joint ; 
		joint.setup( i , connectionIndex , label ) ; 
		joints.push_back( joint ) ; 
	}
}

void BasicSkeleton::loadDefaultLayoutPositions( ) 
{
	ofxXmlSettings xml ;
	xml.loadFile( "jointDefaultPosition.xml" ) ; 

	for ( int i = 0 ; i < joints.size() ; i++ ) 
	{
		float x = xml.getValue( "jointX" , -5 , i ) ; 
		float y = xml.getValue( "jointY" , -5 , i ) ; 
		joints[i].position.x = x ; 
		joints[i].position.y = y ; 
	}
}

void BasicSkeleton::saveDefaultLayoutPositions( ) 
{
	ofxXmlSettings xml ; 

	for ( int i = 0 ; i < joints.size() ; i++ ) 
	{
		xml.setValue( "jointX" , joints[i].position.x , i ) ; 
		xml.setValue( "jointY" , joints[i].position.y , i ) ; 
	}

	xml.saveFile( "jointDefaultPosition.xml" ) ; 
	cout << "saved the default positions! " << endl ; 
}

int BasicSkeleton::getClosestJointIndexTo( ofPoint location ) 
{
	float closestDistance = 100000 ; 
	int closestIndex = -1 ; 
	for ( int i = 0 ; i < joints.size() ; i++ ) 
	{
		joints[i].bHover = false ; 
		float distance = joints[i].position.distance( location ) ; 
		if ( distance < closestDistance ) 
		{
			closestDistance = distance ; 
			closestIndex = i ;
		}
	}

	joints[closestIndex].bHover = true ; 
	return closestIndex ; 
}
