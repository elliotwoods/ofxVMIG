#pragma once
#include <string>
#include <map>
#include "ofUtils.h"
#include "json/json.h"
#include "ofxCvGui.h"

namespace ofxVMIG {
	class Settings : public ofxCvGui::Panels::Base {
	public:
		Settings();
		void load(const std::string& filename = "");
		void save(const std::string& filename = "");
	protected:
		Json::Value json;
		ofxCvGui::ElementGroupPtr elementGroup;

		ofPtr<ofxCvGui::Utils::Button> saveButton;
		ofPtr<ofxCvGui::Utils::Button> loadButton;
	};
}