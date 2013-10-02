#pragma once
#include <string>
#include <map>
#include "ofUtils.h"
#include "json/json.h"

namespace ofxVMIG {
	namespace Settings {
		class Node {
		public:
			typedef std::map<string, Node> NodeMap;
			Node& operator[](const std::string& address);
			Node& getNodeByAddress(const std::string& address);
			Node& getNodeByName(const std::string& name);
			Node& addSubNodeByName(const std::string& name);
			
			Json::Value serialize();
			void deserialize(Json::Value&);

			void save(const std::string& filename = "");
			void load(const std::string& filename = "");

			template<typename T>
			Node& operator<<(const T& value) {
				auto stream = stringstream();
				stream << value;
				this->value = stream.str();
			}

			///return false if value was blank
			template<typename T>
			bool operator>>(T& value) { 
				if (this->value == "") {
					return false;
				} else {
					stringstream(this->value) >> value;
				}
			}

			string value;
		protected:
			 NodeMap subNodes;
		};
	}
}