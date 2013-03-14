#include "BasicSkeleton.h"

void BasicSkeleton::setup ( bool _bUseOpenNI ) 
{
    bUseOpenNI = _bUseOpenNI ; 
	//Default stuff out
	bDetected = false ; 
	bDrawLabels = true ; 
	bDrawAngles = true ; 

	osc = new ofxOscReceiver( ) ; 
	osc->setup( 12345 ) ; 

	//I just eyeballed these numbers, seemed to center the skeleton on the screen well
	skeletonBounds = ofRectangle( 300 , 300 , 300 , 300 ) ; 
	bListenToOsc = false ; 
}

void BasicSkeleton::update ( )
{
	//Update the joints positions
	for ( int i = 0 ; i < joints.size() ; i++ ) 
	{
		joints[i].update( ) ; 
	}


	//with new positions, update the new angles
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
		if ( m.getAddress().compare("skeletonData/") == 0  )
		{
			loop++ ; 
			int numArgs = m.getNumArgs() ;
			int argNum = 0 ;
            
            int numJoints = 0 ;
            if ( !bUseOpenNI )
                numJoints = 20 ;
            if ( bUseOpenNI )
                numJoints = 15 ;
            
			//We have 20 joints !
			for ( int j = 0 ; j < numJoints ; j++ ) 
			{
				//cout << "@ joint : " << j << endl ; 
				joints[j].position.x = m.getArgAsFloat( argNum ) * skeletonBounds.width + skeletonBounds.x ;
                //Kinect SDK has the Y positions updside down
                if ( !bUseOpenNI )
                    joints[j].position.y = ( -m.getArgAsFloat( argNum+1 ) ) * skeletonBounds.height + skeletonBounds.y ;
                if ( bUseOpenNI )
                    joints[j].position.y = ( m.getArgAsFloat( argNum+1 ) ) * skeletonBounds.height + skeletonBounds.y ;
				argNum += 2 ;
			}
			return ; 
			//cout << "currentFrame : " << ofToString( ofGetFrameNum() ) << endl ; 
		}
	}
}

void BasicSkeleton::draw( ) 
{
	//Red signifies nothing, just easy to see
	ofSetColor( 255 , 12 , 0 ) ; 
	for ( int i = 0 ; i < joints.size() ; i++ ) 
	{
		ofCircle( joints[i].position , 5 ) ;  
	}

	//Draw all the connections , FAT end is the start, skinny end is the END
	for ( int i = 0 ; i < joints.size() ; i++ ) 
	{
		joints[i].drawConnection( joints[joints[i].connectionIndex].position ) ; 
	}

	//Draw debug labels if enabled
	if ( bDrawLabels ) 
	{
		for ( int i = 0 ; i < joints.size() ; i++ ) 
		{
			joints[i].drawLabel( ) ; 
		}
	}

	/* THIS DOES NOTHING !!!! */
	if ( bDrawAngles ) 
	{
		for ( int i = 0 ; i < joints.size() ; i++ ) 
		{
			joints[i].drawAngle( ) ; 
		}
	}

	//Normal draw!
	for ( int i = 0 ; i < joints.size() ; i++ ) 
	{
		joints[i].draw( ) ; 
	}
}

void BasicSkeleton::loadLabelsFromXml( string path ) 
{
    //OpenNI Layout : openNI_jointLabels
    //Kinect SDK Layout : kinectSDK_jointLabels.xml
	jointXml.loadFile( path ) ; 

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

void BasicSkeleton::loadDefaultLayoutPositions(  string path )
{
	ofxXmlSettings xml ;
	xml.loadFile( path ) ;

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
