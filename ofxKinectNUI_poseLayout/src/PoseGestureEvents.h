#pragma once

#include "ofEvents.h"
#include "ofMain.h"

/* keeping all the events in a seperate class alleviates some inheritance issues */

class PoseGestureEvents
{
public : 
	static PoseGestureEvents* Instance()
        {
             static PoseGestureEvents inst ;
             return &inst ;
        }

		ofEvent<string> POSE_RECOGNIZED ;
		ofEvent<string> POSE_START ;
		ofEvent<string> POSE_END ; 
};
		

		