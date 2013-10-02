#include "Settings.h"
#include "ofSystemUtils.h"

using namespace std;
using namespace ofxCvGui;

namespace ofxVMIG {
	//----------
	Settings::Settings() {
		this->elementGroup = ElementGroupPtr(new ElementGroup());
		this->saveButton = ofPtr<Utils::Button>(new Utils::Button);
		this->loadButton = ofPtr<Utils::Button>(new Utils::Button);

		this->onDraw += [this] (DrawArguments& args) {
			this->elementGroup->draw(args);
		};
		
		this->onMouse += [this] (MouseArguments& args) {
			this->elementGroup->mouseAction(args);
		};

		this->elementGroup->add(this->saveButton);
		this->elementGroup->add(this->loadButton);

		this->saveButton->onDrawUp += [this] (DrawArguments&) {
			ofDrawBitmapString("Save", this->saveButton->getWidth() - 16, this->saveButton->getHeight() / 2 + 5);
		};
		this->saveButton->onDrawDown += [this] (DrawArguments& args) {
			ofPushStyle();
			ofFill();
			ofSetColor(255);
			ofSetLineWidth(0);
			ofRect(0, 0, this->saveButton->getWidth(), this->saveButton->getHeight());
			ofPopStyle();

			this->saveButton->onDrawUp(args);
		};
		this->saveButton->onMouseReleased += [this] (MouseArguments& args) {
			this->save("settings.json");
		};
		
		this->loadButton->onDrawUp += [this] (DrawArguments&) {
			ofDrawBitmapString("Save", this->loadButton->getWidth() - 16, this->loadButton->getHeight() / 2 + 5);
		};
		this->loadButton->onDrawDown += [this] (DrawArguments& args) {
			ofPushStyle();
			ofFill();
			ofSetColor(255);
			ofSetLineWidth(0);
			ofRect(0, 0, this->loadButton->getWidth(), this->loadButton->getHeight());
			ofPopStyle();

			this->loadButton->onDrawUp(args);
		};
		this->loadButton->onMouseReleased += [this] (MouseArguments& args) {
			this->load("settings.json");
		};

		this->onBoundsChange += [this] (BoundsChangeArguments& args) {
			ofRectangle buttonBounds(0,0,args.bounds.width / 2.0f, args.bounds.height);
			this->loadButton->setBounds(buttonBounds);
			buttonBounds.x += buttonBounds.width;
			this->saveButton->setBounds(buttonBounds);
		};
	}

	//----------
	void Settings::load(const std::string& filenameConst) {
		auto filename = filenameConst;
		if (filename == "") {
			auto result = ofSystemLoadDialog("Load settings.json", false);
			if (result.bSuccess) {
				filename = result.getPath();
			} else {
				ofLogError() << "No file selected";
				return;
			}
		}

		ifstream file(ofToDataPath(filename).c_str());
		if (file.fail()) {
			ofLogError() << "Failed to open file";
			return;
		}
		auto contents = string(std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>());
		file.close();

		Json::Reader reader;
		this->json.clear();
		reader.parse(contents, this->json);
	}

	//----------
	void Settings::save(const std::string& filenameConst) {
		auto filename = filenameConst;
		if (filename == "") {
			auto result = ofSystemSaveDialog("Save settings.json", false);
			if (result.bSuccess) {
				filename = result.getPath();
			} else {
				ofLogError() << "No file selected";
				return;
			}
		}

		Json::StyledWriter writer;
		auto contents = writer.write(this->json);
	}

}