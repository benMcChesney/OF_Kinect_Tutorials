#include "VisualTimer.h"

VisualTimer::VisualTimer()
{
    //ctor
}

VisualTimer::~VisualTimer()
{
    //dtor
}

void VisualTimer::setup ( float _radius , ofColor _color )
{
    radius = _radius ;
    color = _color ;
}

void VisualTimer::update ( )
{
    //normalizedTime = ( sin ( ofGetElapsedTimef() ) + 1.0f ) / 2.0f ;

}

void VisualTimer::draw ( float _x , float _y )
{
    ofEnableAlphaBlending() ;
    float numVertices = 80 ;

    int lastVert = normalizedTime * numVertices ;
    ofPushMatrix( ) ;
        ofTranslate( _x , _y ) ;

        ofSetColor( 255 , 255 , 255 , 80) ;
        ofFill();
        ofCircle( 0 , 0,  radius ) ;

        ofPath path ;
        path.setFilled ( false ) ;
        path.setStrokeColor( color ) ;
        path.setStrokeWidth( 5 ) ;
        for ( int i = 0 ; i < lastVert ; i++ )
        {
            float theta = ( float ) i / numVertices  * TWO_PI;
            float x = sin ( theta ) * radius ;
            float y = cos ( theta ) * radius ;
            if ( i == 0 )
                path.moveTo( x , y ) ;
            path.lineTo( x , y ) ;
        }

        path.draw( ) ;

    ofPopMatrix() ;
    ofColor color ;


    //

}


