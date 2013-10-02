#pragma once
#include "ofxVMIG/Module.h"

namespace ofxVMIG {
	namespace Modules {
		class VideoMatrix : public Module {
			void setup() override;
			void update() override;
			void draw(float width, float height) override;
			void populateInspector(ofxUICanvas &canvas) override;
			void inspectorCallback(ofxUIEventArgs &args) override;
		};
	}
}