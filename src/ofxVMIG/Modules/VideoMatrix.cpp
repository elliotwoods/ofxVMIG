#include "VideoMatrix.h"

namespace ofxVMIG {
	namespace Modules {
		//----------
		void VideoMatrix::setup() {
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
		}

		//----------
		void VideoMatrix::update() {
			this->videoHub.update();
		}
	}
}