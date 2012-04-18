//
//  SimpleKinect.cpp
//  ofxKinectExample
//
//  Created by Ben McChesney on 4/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "SimpleKinect.h"

void SimpleKinect::setup ( ) 
{
    // enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	kinect.open();
	
#ifdef USE_TWO_KINECTS
	kinect2.init();
	kinect2.open();
#endif
	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	bThreshWithOpenCV = true;
	
	
	
	// zero the tilt on startup
	angle = 0;
	//kinect.setCameraTiltAngle(angle);
	
	// start from the front
	bDrawPointCloud = false;
    maxNumCursors = 4 ; 
    
    //Add the cursors to the vector
    for ( int i = 0 ; i < maxNumCursors ; i++ ) 
    {
        KinectCursor kc ; 
        kc.enabled = false ; 
        kc.position = ofPoint ( ) ; 
        kc.nPosition = ofPoint ( ) ; 
        kCursors.push_back( kc ) ; 
    }
    
    
    kinectXml.loadFile( "kinectSettings.xml" ) ; 
    nearThreshold = kinectXml.getValue("nearThreshold", 15 ) ; 
    farThreshold = kinectXml.getValue("farThreshold", 215 ) ; 
}

void SimpleKinect::update ( ) 
{
    kinect.update();
	
	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
		
		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
		
		// we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds
		if(bThreshWithOpenCV) {
			grayThreshNear = grayImage;
			grayThreshFar = grayImage;
			grayThreshNear.threshold(nearThreshold, true);
			grayThreshFar.threshold(farThreshold);
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
		} else {
			
			// or we do it ourselves - show people how they can work with the pixels
			unsigned char * pix = grayImage.getPixels();
			
			int numPixels = grayImage.getWidth() * grayImage.getHeight();
			for(int i = 0; i < numPixels; i++) {
				if(pix[i] < nearThreshold && pix[i] > farThreshold) {
					pix[i] = 255;
				} else {
					pix[i] = 0;
				}
			}
		}
		
		// update the cv images
		grayImage.flagImageChanged();
		
		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayImage, 400, (kinect.width*kinect.height)/2, maxNumCursors , false );
	}
    
    //ADDED
    int blobSize = contourFinder.blobs.size() ; 
    for ( int i = 0 ; i < maxNumCursors ; i++ ) 
    {
        if ( i < blobSize ) 
        {
            //contourFinder stores the center of each detected blob
            ofPoint center = contourFinder.blobs[i].centroid ; 
            //This point is relative to the sie of the feed so we need normalize it ( make it easier to work with ) 
            
            //C++ has operators that allow for simple calculations between classes
            center.x /= kinect.getWidth() ; 
            center.y /= kinect.getHeight() ; 
            
            kCursors[i].enabled = true ; 
            kCursors[i].nPosition = center ; 
             //OF has built in functions for a lot of things, like getting the windows size
            kCursors[i].position = ofPoint (  center.x * ofGetWidth() , center.y * ofGetHeight() ) ; 
        }
        else
        {
            kCursors[i].enabled = false ; 
        }
    }

	
    
     
#ifdef USE_TWO_KINECTS
	kinect2.update();
#endif
}

void SimpleKinect::draw ( ) 
{
  	if( bDrawPointCloud ) {
		easyCam.begin();
		drawPointCloud();
		easyCam.end();
	} else {
		// draw from the live kinect
		kinect.drawDepth(10, 10, 400, 300);
		kinect.draw(420, 10, 400, 300);
		
		grayImage.draw(10, 320, 400, 300);
		contourFinder.draw(10, 320, 400, 300);
        
        ofEnableAlphaBlending() ; 
        ofSetColor ( 255 , 0 , 0 , 125 ) ; 
                
		
#ifdef USE_TWO_KINECTS
		//kinect2.draw(420, 320, 400, 300);
        kinect2.draw(420, 320 );
#endif
	}
    
    ofEnableAlphaBlending() ; 
    ofSetColor ( 0 , 0 , 255 , 195 ) ; 
    for ( int i = 0 ; i < kCursors.size() ; i++ ) 
    {
        if ( kCursors[i].enabled == true ) 
        {
            ofPoint p = kCursors[i].position ; 
            ofCircle( p.x , p.y , 35 ) ;   
        }
    }
}

void SimpleKinect::drawDebug()
{
    // draw instructions
	ofSetColor(255, 255, 255 );
	stringstream reportStream;
	reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
	<< ofToString(kinect.getMksAccel().y, 2) << " / "
	<< ofToString(kinect.getMksAccel().z, 2) << endl
	<< "press p to switch between images and point cloud, rotate the point cloud with the mouse" << endl
	<< "using opencv threshold = " << bThreshWithOpenCV <<" (press spacebar)" << endl
	<< "set near threshold " << nearThreshold << " (press: + -)" << endl
	<< "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
	<< ", fps: " << ofGetFrameRate() << endl
	<< "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl
	<< "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl;
	ofDrawBitmapString(reportStream.str(),20,652);
    
   
}

void SimpleKinect::keyPressed( int key )
{
    switch (key) {
		case ' ':
			bThreshWithOpenCV = !bThreshWithOpenCV;
			break;
			
		case'p':
			bDrawPointCloud = !bDrawPointCloud;
			break;
			
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
            kinectXml.setValue("farThreshold", farThreshold ) ; 
			break;
			
		case '<':
		case ',':
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
            kinectXml.setValue("farThreshold", farThreshold ) ; 
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
            kinectXml.setValue("nearThreshold", nearThreshold ) ; 
			break;
			
		case '-':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
            kinectXml.setValue("nearThreshold", nearThreshold ) ; 
			break;
			
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
    
    kinectXml.saveFile( "kinectSettings.xml" ) ; 
}

void SimpleKinect::exit( )
{
    //	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	
#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}
void SimpleKinect::drawPointCloud() 
{
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				mesh.addColor(kinect.getColorAt(x,y));
				mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
			}
		}
	}
	glPointSize(3);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards' 
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	glEnable(GL_DEPTH_TEST);
	mesh.drawVertices();
	glDisable(GL_DEPTH_TEST);
	ofPopMatrix();
}
