#pragma once
#include "ofxBlackMagic.h"
#include "ofxVMIG/Module.h"
#include "ofxVideoHubControl.h"

namespace ofxVMIG {
	namespace Modules {
		class VideoMatrix : public Module {
			void setup() override;
			void update() override;
			void draw(float width, float height) override;
			void populateInspector(ofxUICanvas &canvas) override;
			void inspectorCallback(ofxUIEventArgs &args) override;
		protected:
			ofxBlackmagic::Input inputs[4];
			ofxVideoHubControl videoHub;
		};
	}
}