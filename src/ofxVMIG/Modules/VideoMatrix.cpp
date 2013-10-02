#include "VideoMatrix.h"

using namespace ofxCvGui;

namespace ofxVMIG {
	namespace Modules {
#pragma mark Channel
		//----------
		Channel::Channel(int index) {
			this->index = index;
		}

#pragma mark VideoMatrix
		//----------
		VideoMatrix::VideoMatrix() {
			auto deviceList = ofxBlackmagic::Iterator::getDeviceList();
			if (deviceList.size() != 4) {
				ofSystemAlertDialog("VMIG is configured for a single DeckLink Quad. Please ensure that you have 4 working DeckLink inputs.\nWe'll now quit.");
				ofExit();
			}
			try {
				for(int i=0; i<4; i++) {
					this->inputs[i].startCapture(deviceList[i], bmdModeHD1080p25);
				}
			} catch (std::exception e) {
				ofLogError() << e.what();
				ofSystemAlertDialog("DeckLink initialisation failed:\n" + string(e.what()));
				ofExit();
			}

			this->videoHub.setup();

			for(int i=0; i<32; i++) {
				auto newChannel = ofPtr<Channel>(new Channel(i));
				this->elementGroup->add(newChannel);
			}
		}

		//----------
		void VideoMatrix::update() {
			this->videoHub.update();
		}

		//----------
		void VideoMatrix::populateInspector(ofxUICanvas &canvas) {
		}

		//----------
		void VideoMatrix::inspectorCallback(ofxUIEventArgs &args) {
		}
	}
}