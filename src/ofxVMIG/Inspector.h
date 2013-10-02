#pragma once
#include "ofxCvGui/Panels/Base.h"
#include "ofxUI.h"
#include "Inspectable.h"

namespace ofxVMIG {
	class Inspector : public ofxCvGui::Panels::Base {
	public:
		Inspector();
		void setFocus(Inspectable&);
		void clearFocus();
	protected:
		void mouse(ofxCvGui::MouseArguments&);
		void boundsChange(ofxCvGui::BoundsChangeArguments&);
		void onUIChange(ofxUIEventArgs&);
		ofxUICanvas widgets;
		Inspectable* focused;
	};
}