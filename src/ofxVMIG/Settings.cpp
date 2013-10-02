#include "Settings.h"
#include "ofSystemUtils.h"

using namespace std;

namespace ofxVMIG {
	namespace Settings {
#pragma mark Node
		//----------
		Node& Node::operator[](const std::string& address) {
			return this->getNodeByAddress(address);
		}

		//----------
		Node& Node::getNodeByAddress(const string& address) {
			auto findSlash = address.find('/');
			if(findSlash == string::npos) {
				//this level
				return this->getNodeByName(address);
			} else {
				auto name = address.substr(0, findSlash);
				auto subAddress = address.substr(findSlash + 1, address.size() - findSlash - 1);
				auto& node = this->getNodeByName(name);
				return node[subAddress];
			}
		}

		//----------
		Node& Node::getNodeByName(const string& name) {
			if (subNodes.find(name) == this->subNodes.end()) {
				this->addSubNodeByName(name);
			}
			return this->subNodes[name];
		}

		//----------
		Node& Node::addSubNodeByName(const string& name) {
			this->subNodes.insert(std::pair<string, Node>(name, Node()));
			return this->subNodes[name];
		}

		//----------
		Json::Value Node::serialize() {
			Json::Value json;
			
		}

		//----------
		void Node::deserialize(Json::Value& json) {
		}

		//----------
		void Node::load(const std::string& filenameConst) {
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
			string contents(std::istreambuf_iterator<char>(file),
				std::istreambuf_iterator<char>());
			file.close();

			Json::Reader reader;
			Json::Value root;
			reader.parse(contents, root);
			this->deserialize(root);
		}

#pragma mark Store
		//----------

	}
}