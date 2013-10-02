#pragma once
#include "ofxBlackMagic.h"
#include "ofxVideoHubControl.h"
#include "ofxCvGui/Panels/ElementHost.h"
#include "ofxVMIG/Inspectable.h";

namespace ofxVMIG {
	namespace Modules {
		class Channel : public Element {
		public:
			Channel(int index);
		protected:
			int index;
		};

		class VideoMatrix : public ofxCvGui::Panels::ElementHost, public Inspectable {
		public:
			VideoMatrix();
			void update();
			void populateInspector(ofxUICanvas &canvas) override;
			void inspectorCallback(ofxUIEventArgs &args) override;
		protected:
			ofxBlackmagic::Input inputs[4];
			ofxVideoHubControl videoHub;
		};
	}
}