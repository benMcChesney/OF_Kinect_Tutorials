#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
    // turn on smooth lighting //
    ofSetSmoothLighting(true);
    
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
#ifdef USE_TWO_KINECTS
	kinect2.init();
	kinect2.open();
#endif
	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	

	ofSetFrameRate(60);

    bKinectOpen = true ; 
    usecamera = false;
    
    setup_ofxUI() ;
    
    maxCursors = 2 ;
    for ( int i = 0 ; i < maxCursors ; i++ )
    {
        cursors.push_back( ofVec2f() ) ; 
    }
}

void testApp::setup_ofxUI()
{
    //Setup ofxUI
    float dim = 24;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    guiWidth = 210-xInit;
    
    gui = new ofxUICanvas(0, 0, guiWidth+xInit, ofGetHeight());
	gui->addWidgetDown(new ofxUILabel("KINECT PARAMS", OFX_UI_FONT_MEDIUM ));
    gui->addWidgetDown(new ofxUIRangeSlider(guiWidth, dim, 0.0, 255.0, farThreshold, nearThreshold, "DEPTH RANGE"));
    gui->addWidgetDown(new ofxUIRangeSlider(guiWidth, dim, 0.0, ((kinect.width * kinect.height ) / 2 ), minBlobSize , maxBlobSize, "BLOB SIZE"));
    
    gui->addWidgetDown(new ofxUIToggle("THRESHOLD OPENCV" , bThreshWithOpenCV , dim , dim ) ) ;
    
    gui->addWidgetDown(new ofxUISlider(guiWidth, dim,  -30.0f  , 30.0f  , angle , "MOTOR ANGLE")) ;
    gui->addWidgetDown(new ofxUIToggle("OPEN KINECT" , bKinectOpen , dim , dim ) ) ;
    
    gui->addWidgetDown(new ofxUISlider(guiWidth, dim,  1.0 , 5.0 , cursorXSensitivity , "X SENSITIVITY")) ;
    gui->addWidgetDown(new ofxUISlider(guiWidth, dim,  1.0 , 5.0 , cursorYSensitivity , "Y SENSITIVITY")) ;

    gui->addWidgetDown(new ofxUIToggle("RESTRICT CURSORS" , bRestrictCursors , dim , dim ) ) ;
    
    gui->addWidgetDown(new ofxUISlider(guiWidth, dim,  0.0 , 50.0f , cursorBorderPadding , "CURSOR BORDER PADDING")) ;
    gui->addWidgetDown(new ofxUIToggle("USE CAMERA" , usecamera , dim , dim ) ) ;
    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings("GUI/kinectSettings.xml") ;
}

//--------------------------------------------------------------
void testApp::update() {
	
     
    //don't move the points if we are using the camera
    if(!usecamera){
        ofVec3f sumOfAllPoints(0,0,0);
        for(int i = 0; i < points.size(); i++){
            points[i].z -= 4;
            sumOfAllPoints += points[i];
        }
        center = sumOfAllPoints / points.size();
    }

    
	ofBackground(100, 100, 100);
	
    ofSetWindowTitle( "Kinect Workshop Start - FPS:"+ ofToString( ofGetElapsedTimef() ) ) ; 
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
        // findContours( ofxCvGrayscaleImage&  input, int minArea, int maxArea, int nConsidered, bool bFindHoles, bool bUseApproximation ) ;
        
		contourFinder.findContours(grayImage, minBlobSize , maxBlobSize , maxCursors , false);
        
        //Reset the cursors
        cursors.clear() ;
        
        //For each openCV blob normalize it and scale it by the x + y sensitivty
        //and then scale it by the window space , and prevent it from going off screen like a cursor
        for ( int i = 0 ; i < contourFinder.nBlobs ; i++ )
        {
            //Get the centroid ( which is relative to the depthImage
            ofVec2f iCentroid = contourFinder.blobs[i].centroid ;
            
            //Normalize the centroid ( make it between 0 and 1 ) 
            iCentroid.x /= kinect.width ;
            iCentroid.y /= kinect.height ;
            
            //offset coordinates so that the sensitivity scales in all directions ( 0 , 0 ) in the middle
            iCentroid += ofVec2f( -0.5 , -0.5 ) ;
            
            //Scale it by cursor sensitivity
            iCentroid.x *= cursorXSensitivity ;
            iCentroid.y *= cursorYSensitivity ;
            
            //remove the offset but keep the sensitivity scaing
            iCentroid -= ofVec2f( -0.5 , -0.5 ) ;
            

            iCentroid.x *= ofGetWidth() ;
            iCentroid.y *= ofGetHeight() ;
            
            if ( bRestrictCursors == true )
            {
                iCentroid.x = ofMap ( iCentroid.x , 0.0 , ofGetWidth() , cursorBorderPadding , ofGetWidth() - cursorBorderPadding , true ) ;
                iCentroid.y = ofMap ( iCentroid.y , 0.0 , ofGetHeight() , cursorBorderPadding , ofGetHeight() - cursorBorderPadding , true ) ;
            }
            cursors.push_back( iCentroid ) ;
        }
	}
    
    if ( cursors.size() > 0 )
    {
        for ( int i = 0 ; i < cursors.size() ; i++)
        {
            ofVec3f point = cursors[i] ;
            //if we are using the camera, the mouse moving should rotate it around the whole sculpture
            if(usecamera){
                float rotateAmount = ofMap(point.x, 0, point.y , 0, 360);
                ofVec3f furthestPoint;
                if (points.size() > 0) {
                    furthestPoint = points[0];
                }
                else
                {
                    furthestPoint = ofVec3f(point.x, point.y, 0);
                }
                
                ofVec3f directionToFurthestPoint = (furthestPoint - center);
                ofVec3f directionToFurthestPointRotated = directionToFurthestPoint.rotated(rotateAmount, ofVec3f(0,1,0));
                camera.setPosition(center + directionToFurthestPointRotated);
                camera.lookAt(center);
            }
            //otherwise add points like before
            else{
                points.push_back( point );
            }
        }

    }
	
    //setup  light animations
    
    ofQuaternion xRotation , yRotation ;
    xRotation.makeRotate( sin( ofGetElapsedTimef() ), 1.0f, 0.0f, 0.0f ) ;
    yRotation.makeRotate( cos( ofGetElapsedTimef() ), 0.0f, 1.0f, 0.0f ) ;
    
    ofQuaternion lightRotation = xRotation * yRotation ;
    
    light.setGlobalOrientation( lightRotation ) ;
}

//--------------------------------------------------------------
void testApp::draw() {
	
	ofSetColor(255, 255, 255);
    ofPushMatrix() ; 
	ofTranslate( guiWidth + 10 , 0 ) ;

               
    ofEnableAlphaBlending() ; 
    ofPushMatrix() ;
    ofTranslate( ofGetWidth() - 210 - guiWidth - 10  , ofGetHeight() - 160 ) ;
    // draw from the live kinect
    kinect.drawDepth(0 , 0, 200 , 150 );
        
    ofSetColor( 255 , 65 ) ;
    kinect.draw(0, 0, 200 , 150);
    contourFinder.draw(0, 0, 200, 150);
    ofPopMatrix() ;
        

	
    
    ofPopMatrix() ;

    //If there are any cursors
    if ( cursors.size() > 0 )
    {
        //Iterate through
        for ( int i = 0 ; i < cursors.size() ; i++ )
        {
            //Give each a unique color based on it's index
            float ratio = (float) i / ( float) cursors.size() ;
            ofSetColor( ofColor::fromHsb(225.0f * ratio , 255 , 255 ) ) ;
            ofCircle( cursors[i] , 12 ) ; 
        }
    }
    //to debug to the screen use
    //ofDrawBitmapString( "STRING" , x , y ) ;
    
    //if we're using the camera, start it.
	//everything that you draw between begin()/end() shows up from the view of the camera
    if(usecamera){
        camera.begin();
    }
	ofSetColor(0);
	//do the same thing from the first example...
    ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	for(int i = 1; i < points.size(); i++){
        
		//find this point and the next point
		ofVec3f thisPoint = points[i-1];
		ofVec3f nextPoint = points[i];
        
		//get the direction from one to the next.
		//the ribbon should fan out from this direction
		ofVec3f direction = (nextPoint - thisPoint);
        
		//get the distance from one point to the next
		float distance = direction.length();
        
		//get the normalized direction. normalized vectors always have a length of one
		//and are really useful for representing directions as opposed to something with length
		ofVec3f unitDirection = direction.normalized() + 0.1f ;
        
		//find both directions to the left and to the right
		ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0,0,1));
		ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0,0,1));
        
		//use the map function to determine the distance.
		//the longer the distance, the narrower the line.
		//this makes it look a bit like brush strokes
		float thickness = ofMap(distance, 0, 60, 40, 10, true);
        
		//calculate the points to the left and to the right
		//by extending the current point in the direction of left/right by the length
		ofVec3f leftPoint = thisPoint+toTheLeft*thickness;
		ofVec3f rightPoint = thisPoint+toTheRight*thickness;
        
		//add these points to the triangle strip
		mesh.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
		mesh.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
        
        mesh.addColor ( ofColor::fromHsb( sin ( (float)i ) * 40.0f + 128.0f, 255.0f , 255.0f ) ) ;
        mesh.addColor ( ofColor::fromHsb( sin ( (float)i ) * 40.0f + 128.0f, 255.0f , 255.0f ) ) ;
	}
    
    // enable lighting //
    ofEnableLighting();
    
    glEnable(GL_DEPTH_TEST);
    
    // the position of the light must be updated every frame,
    // call enable() so that it can update itself //
    light.enable();

    
	//end the shape
	mesh.draw();
    
    light.disable() ;
    
    glDisable(GL_DEPTH_TEST) ;
    ofDisableLighting() ;
    
	//if we're using the camera, take it away
    if(usecamera){
    	camera.end();
    }

    
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();
	
	if(name == "DEPTH RANGE")
	{
		ofxUIRangeSlider *slider = (ofxUIRangeSlider *) e.widget;
        farThreshold = slider->getScaledValueLow() ;
        nearThreshold = slider->getScaledValueHigh() ; 
	}
    
    if(name == "BLOB SIZE")
	{
		ofxUIRangeSlider *slider = (ofxUIRangeSlider *) e.widget;
        minBlobSize = slider->getScaledValueLow() ;
        maxBlobSize = slider->getScaledValueHigh() ;
	}
    
    if(name == "THRESHOLD OPENCV" )
	{
		ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        bThreshWithOpenCV = toggle->getValue() ;
	}
    

    if(name == "MOTOR ANGLE" )
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget;
        angle = slider->getScaledValue() ;
        kinect.setCameraTiltAngle(angle);
	}
    
    if(name == "OPEN KINECT" )
	{
		ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        bKinectOpen = toggle->getValue() ;
        if ( bKinectOpen == true )
            kinect.open() ;
        else
            kinect.close() ;
	}
    
    if(name == "X SENSITIVITY" )
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget;
        cursorXSensitivity = slider->getScaledValue() ;
	}
    
    if(name == "Y SENSITIVITY" )
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget;
        cursorYSensitivity = slider->getScaledValue() ;
	}
    
    if(name == "RESTRICT CURSORS" )
	{
		ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        bRestrictCursors = toggle->getValue() ;
	}

    if(name == "CURSOR BORDER PADDING" )
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget;
        cursorBorderPadding = slider->getScaledValue() ;
	}
   
    if(name == "USE CAMERA" )
	{
		ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        usecamera = toggle->getValue() ;
	}
   
    gui->saveSettings("GUI/kinectSettings.xml") ; 
}

//--------------------------------------------------------------
void testApp::exit() {
	kinect.close();
	
#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
    
	switch (key) {
		
        // example of how to create a keyboard event
            
		case 'A':
        case 'a':
            //...do things
            break ; 
			
	}
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}
