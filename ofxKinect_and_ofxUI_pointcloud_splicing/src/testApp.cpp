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
    
    gui->addWidgetDown(new ofxUIToggle("DRAW POINT CLOUD" , bDrawPointCloud , dim , dim ) ) ;
    gui->addWidgetDown(new ofxUISlider(guiWidth, dim,  0.00001f  , 0.5f  , normalizedRange.x , "NORMALIZED RANGE X" )) ;
    gui->addWidgetDown(new ofxUISlider(guiWidth, dim,  0.0f  , 0.5f  , normalizedRange.y , "NORMALIZED RANGE Y" )) ;
    gui->addWidgetDown(new ofxUISlider(guiWidth, dim,  0.0f  , 0.5f  , normalizedRange.z , "NORMALIZED RANGE Z" )) ;
    
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
    
    //ofColor offset = ?
    
    float normalizedZ = sin( ofGetElapsedTimef() ) * 0.35   + 0.5 ;
    float currentZ = ofLerp( pointCloudMinZ , pointCloudMaxZ , normalizedZ ) ; 
    float minZ = ofLerp( pointCloudMinZ , pointCloudMaxZ , normalizedZ - normalizedRange.z ) ;
    float maxZ = ofLerp( pointCloudMinZ , pointCloudMaxZ , normalizedZ + normalizedRange.z ) ;
    
    float normalizedY = cos( ofGetElapsedTimef() * 0.25 ) * 0.5   + 0.5 ;
    float currentY = ofLerp( 0 , kinect.getHeight() , normalizedY ) ;
    float minY = ofLerp( 0 , kinect.getHeight() , normalizedY - normalizedRange.y ) ;
    float maxY = ofLerp( 0 , kinect.getHeight() , normalizedY + normalizedRange.y ) ;

    
    float normalizedX = cos( ofGetElapsedTimef() * 0.125 ) * 0.5   + 0.5 ;
    float currentX = ofLerp( 0 , kinect.getHeight() , normalizedX ) ;
    float minX = ofLerp( 0 , kinect.getWidth() , normalizedX - normalizedRange.x ) ;
    float maxX = ofLerp( 0 , kinect.getWidth() , normalizedX + normalizedRange.x ) ;

    
	int step = 2;
    
    ofColor offset = ofColor::fromHsb( (int)(ofGetFrameNum() * 0.3) % 255 , 255 , 255 ) ;
    ofColor offset2 = ofColor::fromHsb( (int)(ofGetFrameNum() * 0.5) % 120 , 255 , 255 ) ;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				
                ofVec3f vertex = kinect.getWorldCoordinateAt(x, y) ;
               
                if ( vertex.z > pointCloudMinZ && vertex.z < pointCloudMaxZ )
                {
                     float alpha = 80 ;
                    ofColor col = kinect.getColorAt(x,y) + offset ;
                    if ( vertex.z > minZ && vertex.z < maxZ && vertex.y > minY && vertex.y < maxY )
                    {
                        alpha = 255 ;
                        
                    }
                    
                    int xRange = normalizedRange.x * kinect.getWidth() + 1 ;
                    int xMod = x % xRange ; // ;
                    //Make stripes !
                    int offset = sin ( ofGetElapsedTimef() ) * 0.5 + 0.5 ;
                    if ( xMod > xRange/4 + xRange/4 * offset && xMod < xRange * .75 + xRange/4 * offset  )
                    {
                        col -= offset2 ; 
                    }
                    
                    mesh.addVertex( vertex );
                    
                    //Offset the color here
                    
                    col.a = alpha ;
                    
                    mesh.addColor( col );
				}
			}
		}
	}
    
    ofEnableBlendMode( OF_BLENDMODE_ADD ) ;
	glPointSize( ofNoise( ofGetElapsedTimef() ) * 12 + 8 );
	ofPushMatrix();
        // the projected points are 'upside down' and 'backwards' 
        ofScale(1, -1, -1);
        ofTranslate(0, 0, -1000); // center the points a bit
        glEnable(GL_DEPTH_TEST);
            mesh.draw();
        glDisable(GL_DEPTH_TEST);
	ofPopMatrix();
     ofEnableBlendMode( OF_BLENDMODE_ALPHA ) ;
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
    
    if(name ==  "NORMALIZED RANGE X" ) normalizedRange.x = ((ofxUISlider *) e.widget)->getScaledValue() ;
    if(name ==  "NORMALIZED RANGE Y" ) normalizedRange.y = ((ofxUISlider *) e.widget)->getScaledValue() ;
    if(name ==  "NORMALIZED RANGE Z" ) normalizedRange.z = ((ofxUISlider *) e.widget)->getScaledValue() ;
    
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
