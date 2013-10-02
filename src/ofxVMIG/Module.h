#pragma once
#include "ofxUI.h"
#include <memory>

namespace ofxVMIG {
    class Module {
    public:
		virtual void setup() { }
		virtual void update() { };
		virtual void draw(float width, float height) { };
    };

	typedef std::shared_ptr<Module> ModulePtr;
}