#ifndef VISUALTIMER_H
#define VISUALTIMER_H

#include "ofMain.h"

class VisualTimer
{
    public:
        VisualTimer();
        virtual ~VisualTimer();

        float normalizedTime ;

        void setup ( float _radius , ofColor _color ) ;
        void update ( ) ;
        void draw ( float _x , float _y ) ;

        float radius ;
        ofColor color ;
    protected:
    private:
};

#endif // VISUALTIMER_H
