#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
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
    
    setup_ofxUI() ;
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
    gui->addWidgetDown(new ofxUIToggle("DRAW POINT CLOUD" , bDrawPointCloud , dim , dim ) ) ;
    
    gui->addWidgetDown(new ofxUISlider(guiWidth, dim,  -30.0f  , 30.0f  , angle , "MOTOR ANGLE")) ;
    gui->addWidgetDown(new ofxUIRangeSlider(guiWidth, dim, 0 , 2000 , pointCloudMinZ , pointCloudMaxZ, "Z RANGE")) ;
    gui->addWidgetDown(new ofxUIToggle("OPEN KINECT" , bKinectOpen , dim , dim ) ) ;
    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings("GUI/kinectSettings.xml") ;
}

//--------------------------------------------------------------
void testApp::update() {
	
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
        
		contourFinder.findContours(grayImage, minBlobSize , maxBlobSize , 20, false);
	}
	
#ifdef USE_TWO_KINECTS
	kinect2.update();
#endif
}

//--------------------------------------------------------------
void testApp::draw() {
	
	ofSetColor(255, 255, 255);
    ofPushMatrix() ; 
	ofTranslate( guiWidth + 10 , 0 ) ;
	if(bDrawPointCloud) {
		easyCam.begin();
		drawPointCloud();
		easyCam.end();
	} else {
		// draw from the live kinect
		kinect.drawDepth(10, 10, 400, 300);
		kinect.draw(420, 10, 400, 300);
		
		grayImage.draw(10, 320, 400, 300);
		contourFinder.draw(10, 320, 400, 300);
		
#ifdef USE_TWO_KINECTS
		kinect2.draw(420, 320, 400, 300);
#endif
	}
    
    ofPopMatrix() ;
	
    //to debug to the screen use
    //ofDrawBitmapString( "STRING" , x , y ) ;
    
    
}

void testApp::drawPointCloud()
{
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
    
    /* 
     Change the color based on time. You can use ofGetElapsedTimef() which returns
     a float for how many seconds this app has been running
     
     in can be used such as :
        sin( ofGetElapsedTimef() )
        ofNoise( ofGetElapsedTimef() )
     
     for interesting repeating animation patterns
     
     ofColor has a function called "fromHSB( hue , saturation , brightness )" that allows for easy color offset
     */
    
	int step = 2;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				
                ofVec3f vertex = kinect.getWorldCoordinateAt(x, y) ;
                if ( vertex.z > pointCloudMinZ && vertex.z < pointCloudMaxZ )
                {
                    mesh.addVertex( vertex );
                    
                    //Offset the color here
                    ofColor col = kinect.getColorAt(x,y) ; // + ?
                    
                    mesh.addColor( col );
                }
				
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
    
    if(name == "Z RANGE" )
	{
		ofxUIRangeSlider *slider = (ofxUIRangeSlider *) e.widget;
        pointCloudMinZ = slider->getScaledValueLow() ;
        pointCloudMaxZ = slider->getScaledValueHigh() ;
	}
    
    if(name == "THRESHOLD OPENCV" )
	{
		ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        bThreshWithOpenCV = toggle->getValue() ;
	}
    
    if(name == "DRAW POINT CLOUD" )
	{
		ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        bDrawPointCloud = toggle->getValue() ;
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
