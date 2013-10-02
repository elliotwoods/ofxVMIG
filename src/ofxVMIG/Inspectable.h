#pragma once
#include "ofxUI.h"

namespace ofxVMIG {
	class Inspectable {
	public:
		// Called when module needs to populate an ofxUICanvas
		virtual void populateInspector(ofxUICanvas &canvas) { };
        
        // Called when a value is chaning in the inspector
		virtual void inspectorCallback(ofxUIEventArgs &args) { };
	};
}