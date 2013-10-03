#include "VideoMatrix.h"
#include "ofxCvGui/Assets.h"

using namespace ofxCvGui;

namespace ofxVMIG {
	namespace Modules {
#pragma mark Channel
		//----------
		Channel::Channel(int index) {
			this->index = index;
			this->onDraw += [this] (DrawArguments&) {
				this->preview.draw(0,0,this->getWidth(), this->getHeight());
				AssetRegister.drawText(ofToString(this->index), 50, this->getHeight() - 50, "", 30, 30);
				AssetRegister.drawText(this->label, 90, this->getHeight() - 50, "", true, 30, 30);
			};
		}

		//----------
		void Channel::setLabel(string label) {
			this->label = label;
		}

		//----------
		string Channel::getLabel() const {
			return this->label;
		}

		//----------
		void Channel::setPreview(ofTexture& texture) {
			const int resolution = 512;
			if (!this->preview.isAllocated()) {
				this->preview.allocate(resolution, resolution, GL_RGBA);
			}

			this->preview.begin();
			texture.draw(0,0,resolution,resolution);
			this->preview.end();
		}

#pragma mark VideoMatrix
		void drawMarker(string label, float x) {
			const float y = 30;
			ofPushStyle();
			ofFill();
			ofSetLineWidth(0);
			if (label == "A") {
				ofSetColor(50, 10, 10);
			} else if (label == "B") {
				ofSetColor(10, 50, 10);
			} else {
				ofSetColor(50);
			}
			ofCircle(x, y, 15);
			ofSetColor(255);
			AssetRegister.drawText(label, x - 20, y - 20, "", false, 40, 40);
			ofPopStyle();
		}

		//----------
		VideoMatrix::VideoMatrix() {
			auto deviceList = ofxBlackmagic::Iterator::getDeviceList();
			if (deviceList.size() != 4) {
				ofSystemAlertDialog("VMIG is configured for a single DeckLink Quad. Please ensure that you have 4 working DeckLink inputs.\nWe'll now quit.");
				ofExit();
			}
			try {
				for(int i=0; i<4; i++) {
					this->inputs.push_back(ofPtr<ofxBlackmagic::Input>(new ofxBlackmagic::Input()));
					this->inputs.back()->startCapture(deviceList[i], bmdModePAL);
				}
			} catch (std::exception e) {
				ofLogError() << e.what();
				ofSystemAlertDialog("DeckLink initialisation failed:\n" + string(e.what()));
				ofExit();
			}


			//--
			//takes
			//
			auto takeAButton = this->elementGroup->addBlank();
			takeAButton->onDraw += [this] (DrawArguments& args) {
				ofPushStyle();
				if (this->selectionA == this->selectionPreview) {
					ofFill();
					ofSetColor(50, 10, 10);
					ofRect(0,0,args.parentBounds.getWidth(), args.parentBounds.getHeight());
				}
				ofPopStyle();
				AssetRegister.drawText("Take A", 0, 0, "", false, VMIG_VIDEO_MATRIX_TAKE_HEIGHT, 100);
			};
			takeAButton->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->setA(this->selectionPreview);
				}
			};

			auto takeBButton = this->elementGroup->addBlank();
			takeBButton->onDraw += [this] (DrawArguments& args) {
				ofPushStyle();
				if (this->selectionB == this->selectionPreview) {
					ofFill();
					ofSetColor(10, 50, 10);
					ofRect(0,0,args.parentBounds.getWidth(), args.parentBounds.getHeight());
				}
				ofPopStyle();
				AssetRegister.drawText("Take B", 0, 0, "", false, VMIG_VIDEO_MATRIX_TAKE_HEIGHT, 100);
			};
			takeBButton->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->setB(this->selectionPreview);
				}
			};
			//
			//--

			this->onBoundsChange += [this, takeAButton, takeBButton] (BoundsChangeArguments& args) {
				const int itemCount = this->channels.size();
				float width = this->getWidth();
				float height = this->getHeight() - VMIG_VIDEO_MATRIX_TAKE_HEIGHT;
				float aspect = 16.0f / 9.0f;
				float availableArea = width * height;
				float areaPerItem = availableArea / itemCount;
				float widthPerItem = sqrt(areaPerItem * aspect);
				int cols = ceil(width / widthPerItem);
				int rows = ceil((float) itemCount / (float) cols);
				
				widthPerItem = width / cols;
				float heightPerItem = widthPerItem / aspect;
				
				int index = 0;
				for(auto element : this->elementGroup->getElements()) {
					if (dynamic_cast<Channel*>(element.get())) {
						float x = (index % cols) * widthPerItem;
						float y = (index / cols) * heightPerItem;
						ofRectangle bounds(x, y, widthPerItem, heightPerItem);
						element->setBounds(bounds);
						index++;
					}
				}

				takeAButton->setBounds(ofRectangle(200, this->getHeight() - VMIG_VIDEO_MATRIX_TAKE_HEIGHT, 100, VMIG_VIDEO_MATRIX_TAKE_HEIGHT));
				takeBButton->setBounds(ofRectangle(300, this->getHeight() - VMIG_VIDEO_MATRIX_TAKE_HEIGHT, 100, VMIG_VIDEO_MATRIX_TAKE_HEIGHT));
			};

			this->onUpdate += [this] (UpdateArguments&) {
				this->update();
			};

			this->onDraw += [this] (DrawArguments& args) {
				auto channel = this->channels[selectionPreview];
				ofxCvGui::AssetRegister.drawText(channel->getLabel(), 0, this->getHeight() - VMIG_VIDEO_MATRIX_TAKE_HEIGHT, "", true, VMIG_VIDEO_MATRIX_TAKE_HEIGHT, 200.0f);
			};

			this->videoHub.setup();
			this->videoHub.setAddress("192.168.1.96");
			for(int i=0; i<16; i++) {
				auto newChannel = ofPtr<Channel>(new Channel(i));
				this->channels.push_back(newChannel);
				this->elementGroup->add(newChannel);

				newChannel->onMouse += [i, this] (MouseArguments& args) {
					if (args.isLocalPressed()) {
						this->setPreview(i);
					}
				};

				newChannel->onDraw += [i, this] (DrawArguments& args) {
					if(this->selectionA == i) {
						drawMarker("A", 40);
						ofPushStyle();
						ofSetColor(200,100,100);
						ofNoFill();
						ofSetLineWidth(1.0f);
						ofRect(0,0,args.parentBounds.getWidth(), args.parentBounds.getHeight());
						ofPopStyle();
					}

					if(this->selectionB == i) {
						drawMarker("B", 80);
						ofPushStyle();
						ofSetColor(100,200,100);
						ofNoFill();
						ofSetLineWidth(1.0f);
						ofRect(0,0,args.parentBounds.getWidth(), args.parentBounds.getHeight());
						ofPopStyle();
					}
					
					if(this->selectionPreview == i) {
						drawMarker("P", 120);
						ofPushStyle();
						ofSetColor(255);
						ofNoFill();
						ofSetLineWidth(1.0f);
						ofRect(0,0,args.parentBounds.getWidth(), args.parentBounds.getHeight());
						ofPopStyle();
					}
				};
			}

			this->channelInC = -1;
			this->channelInD = -1;

			this->setA(8);
			this->setB(9);
			this->setPreview(8);
		}

		//----------
		void VideoMatrix::update() {
			this->videoHub.update();
			if (this->videoHub.isInputLabelsUpdate()) {
				auto labels = this->videoHub.getInputLabels();
				for(int i=0; i<this->channels.size(); i++) {
					if (i < labels.size()) {
						this->channels[i]->setLabel(labels[i]);
					}
				}
			}
			for(auto input : this->inputs) {
				input->update();
			}

			int tick = (int) ofGetElapsedTimef() / 2;
			tick = tick % (VMIG_CHANNEL_COUNT / 2);
			int newChannelInC =  (tick * 2) % VMIG_CHANNEL_COUNT;
			int newChannelInD = (tick * 2 + 1) % VMIG_CHANNEL_COUNT;

			//this could be coded better, go on then!!
			if (newChannelInC != this->channelInC) {
				if (this->channelInC >= 0) {
					this->channels[this->channelInC]->setPreview(this->inputs[2]->getTextureReference());
				}
				this->videoHub.setRoute(2, newChannelInC);
				this->channelInC = newChannelInC;
			}
			if (newChannelInD != this->channelInD) {
				if (this->channelInD >= 0) {
					this->channels[this->channelInD]->setPreview(this->inputs[3]->getTextureReference());
				}
				this->videoHub.setRoute(3, newChannelInD);
				this->channelInD = newChannelInD;
			}
		}

		//----------
		void VideoMatrix::populateInspector(ofxUICanvas &canvas) {
		}

		//----------
		void VideoMatrix::inspectorCallback(ofxUIEventArgs &args) {
		}

		//----------
		ofxBlackmagic::Input& VideoMatrix::getInput(int index) {
			return * this->inputs[index];
		}

		//----------
		void VideoMatrix::setA(int channel) {
			this->videoHub.setRoute(0, channel);
			this->videoHub.setRoute(4, channel);
			this->selectionA = channel;
		}

		//----------
		void VideoMatrix::setB(int channel) {
			this->videoHub.setRoute(1, channel);
			this->videoHub.setRoute(5, channel);
			this->selectionB = channel;
		}

		//----------
		void VideoMatrix::setPreview(int channel) {
			this->videoHub.setRoute(VMIG_PREVIEW_CHANNEL, channel);
			this->selectionPreview = channel;
		}
	}
}