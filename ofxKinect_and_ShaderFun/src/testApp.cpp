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

    bDrawShader = false ; 
    shader.load("shader/twist.vert" , "shader/twist.frag" ) ;
}

void testApp::setup_ofxUI()
{
    //In 3d Math we use quatnerions for rotating on multiple axis
    //Setup ofxUI
    float dim = 24;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    guiWidth = 210-xInit;
    
    gui = new ofxUICanvas(0, 0, guiWidth+xInit, ofGetHeight());
	gui->addWidgetDown(new ofxUILabel("KINECT PARAMS", OFX_UI_FONT_MEDIUM ));
    gui->addWidgetDown(new ofxUIRangeSlider(guiWidth, dim, 0.0, 255.0, farThreshold, nearThreshold, "DEPTH RANGE"));
    gui->addWidgetDown(new ofxUIRangeSlider(guiWidth, dim, 0.0, ((kinect.width * kinect.height ) / 2 ), minBlobSize , maxBlobSize, "BLOB SIZE"));
    
    gui->addWidgetDown(new ofxUIToggle("DRAW POINT CLOUD" , bDrawPointCloud , dim , dim ) ) ;
    
    gui->addWidgetDown(new ofxUISlider(guiWidth, dim,  -30.0f  , 30.0f  , angle , "MOTOR ANGLE")) ;
    gui->addWidgetDown(new ofxUIRangeSlider(guiWidth, dim, 0 , 2000 , pointCloudMinZ , pointCloudMaxZ, "Z RANGE")) ;
    gui->addWidgetDown(new ofxUIToggle("OPEN KINECT" , bKinectOpen , dim , dim ) ) ;
    gui->addWidgetDown(new ofxUIToggle("USE SHADER" , bDrawShader , dim , dim ) ) ;

    //
    gui->addWidgetDown(new ofxUISlider(guiWidth, dim,  0.0f  , 200.0f  , shaderVar1 , "SHADER FLOAT 1")) ;
    gui->addWidgetDown(new ofxUISlider(guiWidth, dim,  0.0f  , 200.0f  , shaderVar2 , "SHADER FLOAT 2")) ;
    gui->addWidgetDown(new ofxUISlider(guiWidth, dim,  0.0f  , 200.0f  , shaderVar3 , "SHADER FLOAT 3")) ;
    gui->addWidgetDown(new ofxUISlider(guiWidth, dim,  0.0f  , 200.0f  , shaderVar4 , "SHADER FLOAT 4")) ;

    shaderVar1 , shaderVar2 , shaderVar3 , shaderVar4 ;
    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings("GUI/kinectSettings.xml") ;
}

//--------------------------------------------------------------
void testApp::update() {
	
	ofBackground(100, 100, 100);
	
    ofSetWindowTitle( "Kinect Workshop Start - FPS:"+ ofToString( ofGetElapsedTimef() ) ) ; 
	kinect.update();
	

    ofVec2f nMouse = ofVec2f( ((float) mouseX / (float)ofGetWidth()) - 0.5 , ((float) mouseY / (float)ofGetHeight() ) - 0.5 ) ;
   /// cout << "nMouse :" << nMouse << endl ;
    
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
        
        if ( bDrawShader )
        {
            shader.begin() ;
            //shader.setUniform1f("time", ofGetElapsedTimef() ) ;
            shader.setUniform1f("time", ofGetElapsedTimef() ) ;
            shader.setUniform1f("var1", shaderVar1 ) ;
            shader.setUniform1f("var2", shaderVar2 ) ;
            shader.setUniform1f("var3", shaderVar3 ) ;
            shader.setUniform1f("var4", shaderVar4 ) ;
        }
		easyCam.begin();
		drawPointCloud();
		easyCam.end();
        
        if ( bDrawShader )
            shader.end( ) ;
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
    ofPushMatrix();
    glEnable(GL_DEPTH_TEST);
    ofScale(1, -1, -1);
    ofTranslate(0, 0, -1000); // center the points a bit
    ofEnableBlendMode( OF_BLENDMODE_ALPHA ) ;

   

    ofColor offset = ofColor::fromHsb((ofGetFrameNum()/2  )%255, 255, 255 ) ;
   // ofEnableBlendMode( OF_BLENDMODE_ADD ) ;
	int step = 5 ;
    float boxSize = step ;//16.5f ;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				
                ofVec3f vertex = kinect.getWorldCoordinateAt(x, y) ;
                if ( vertex.z > pointCloudMinZ && vertex.z < pointCloudMaxZ )
                {
                    float normalizedZ = ofMap( vertex.z , pointCloudMinZ , pointCloudMaxZ , -360.0f , 360.0f ) ; 
                    //mesh.addVertex( vertex );
                    
                    //Offset the color here
                    ofColor col = kinect.getColorAt(x,y) + offset ; // + offset ;
                    
                    //mesh.addColor( col );
                    ofSetColor( col ) ;
                    ofPushMatrix() ;
                        ofQuaternion rot ;
                        ofQuaternion rotX = ofQuaternion( sin( ofGetElapsedTimef() + y + x * 2.5f ) * 360.0f , ofVec3f( 0.0f , 1.0f , 0.0f ) ) ;
                        ofQuaternion rotY = ofQuaternion( normalizedZ , ofVec3f( 1.0f , 0.0f , 0.0f ) ) ;
                        rot = rotX * rotY ;
                        ofVec3f axis ;
                        float angle ;
                        rot.getRotate( angle , axis ) ;
                                          
                        ofTranslate( vertex ) ; 
                        ofRotate( angle , axis.x , axis.y , axis.z ) ;
                        ofBox( ofVec3f( )  , boxSize ) ;
                    ofPopMatrix() ;
                }
				
			}
		}
	}
    glDisable(GL_DEPTH_TEST);
    ofPopMatrix();
    ofEnableBlendMode( OF_BLENDMODE_ADD ) ; 
	//glPointSize(3);
	        // the projected points are 'upside down' and 'backwards' 
       
        
            //mesh.drawVertices();
    
       	
     
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
    
    if(name == "USE SHADER" )
	{
		ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        bDrawShader = toggle->getValue() ;
	}

    
    if(name == "MOTOR ANGLE" )
	{
		ofxUISlider *slider =  ((ofxUISlider *) e.widget) ;
        angle = slider->getScaledValue() ;
        kinect.setCameraTiltAngle(angle);
	}
    
    if(name ==  "SHADER FLOAT 1" ) shaderVar1 = ((ofxUISlider *) e.widget)->getScaledValue() ;
    if(name ==  "SHADER FLOAT 2" ) shaderVar2 = ((ofxUISlider *) e.widget)->getScaledValue() ;
    if(name ==  "SHADER FLOAT 3" ) shaderVar3 = ((ofxUISlider *) e.widget)->getScaledValue() ;
    if(name ==  "SHADER FLOAT 4" ) shaderVar4 = ((ofxUISlider *) e.widget)->getScaledValue() ;
    
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
