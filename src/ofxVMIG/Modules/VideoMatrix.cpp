#include "VideoMatrix.h"
#include "ofxCvGui/Assets.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)
#define ADD_MODE(x) this->modeList.insert(pair<string, BMDDisplayMode>(STRINGIFY(x), x)); this->reverseModeList.insert(pair<BMDDisplayMode, string>(x, STRINGIFY(x)));

using namespace ofxCvGui;

namespace ofxVMIG {
	namespace Modules {
#pragma mark Channel
		//----------
		bool Channel::modeListInitialised = false;

		//----------
		map<string, BMDDisplayMode> Channel::modeList = map<string, BMDDisplayMode>();
		map<BMDDisplayMode, string> Channel::reverseModeList = map<BMDDisplayMode, string>();

		//----------
		Channel::Channel(int index) {
			this->index = index;
			this->onDraw += [this] (DrawArguments&) {
				this->preview.draw(0,0,this->getWidth(), this->getHeight());
				AssetRegister.drawText(ofToString(this->index + 1), 20, this->getHeight() - 50, "", 30, 30);
				AssetRegister.drawText(this->label, 60, this->getHeight() - 50, "", true, 30, 30);
			};

			if (!modeListInitialised) {
				//should use IDeckLinkInput::DoesSupportVideoMode to check
				ADD_MODE(bmdModeNTSC);
				ADD_MODE(bmdModeNTSC2398);
				ADD_MODE(bmdModePAL);
				ADD_MODE(bmdModeNTSCp);
				ADD_MODE(bmdModePALp);
				ADD_MODE(bmdModeHD1080p2398);
				ADD_MODE(bmdModeHD1080p24);
				ADD_MODE(bmdModeHD1080p25);
				ADD_MODE(bmdModeHD1080p2997);
				ADD_MODE(bmdModeHD1080p30);
				ADD_MODE(bmdModeHD1080i50);
				ADD_MODE(bmdModeHD1080i5994);
				ADD_MODE(bmdModeHD1080i6000);
				//ADD_MODE(bmdModeHD1080p50);
				//ADD_MODE(bmdModeHD1080p5994);
				//ADD_MODE(bmdModeHD1080p6000);
				ADD_MODE(bmdModeHD720p50);
				ADD_MODE(bmdModeHD720p5994);
				ADD_MODE(bmdModeHD720p60);
			}

			for(auto mode : this->modeList) {
				this->modeSelection.insert(pair<string, bool>(mode.first, false));
			}
			this->setMode(bmdModePAL);
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

		//----------
		void Channel::populateInspector(ofxUICanvas& canvas) {
			canvas.addLabel("Select video mode:");
			for(auto & mode : this->modeList) {
				canvas.addToggle(mode.first, & (this->modeSelection[mode.first]));
			}
		}
		
		//----------
		void Channel::inspectorCallback(ofxUIEventArgs &args) {
			for(auto mode : this->modeSelection) {
				if (mode.first == args.getName()) {
					this->setMode(args.getName());
					break; //break for
				}
			}
		}

		//----------
		void Channel::setMode(string mode) {
			for(auto & modeSelection : this->modeSelection) {
				modeSelection.second = mode == modeSelection.first;
			}
			this->mode = mode;
			auto enumMode = this->getMode();
			this->onModeChange(enumMode);
		}

		//----------
		void Channel::setMode(BMDDisplayMode mode) {
			this->setMode(reverseModeList[mode]);
		}

		//----------
		BMDDisplayMode Channel::getMode() {
			if (this->mode == "") {
				return bmdModeUnknown;
			} else {
				return this->modeList[mode];
			}
		}

		//----------
		string Channel::getModeString() {
			return this->mode;
		}

#pragma mark VideoMatrix
		void drawMarker(string label, float x) {
			const float y = 30;
			ofPushStyle();
			ofFill();
			ofSetLineWidth(0);
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

			auto takeVideoWallButton = this->elementGroup->addBlank();
			takeVideoWallButton->onDraw += [this] (DrawArguments& args) {
				ofPushStyle();
				if (this->selectionVideoWall == this->selectionPreview) {
					ofFill();
					ofSetColor(10, 10, 50);
					ofRect(0,0,args.parentBounds.getWidth(), args.parentBounds.getHeight());
				}
				ofPopStyle();
				AssetRegister.drawText("Take Video Wall", 0, 0, "", false, VMIG_VIDEO_MATRIX_TAKE_HEIGHT, 200);
			};
			takeVideoWallButton->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->setVideoWall(this->selectionPreview);
				}
			};
			//
			//--

			this->onBoundsChange += [this, takeAButton, takeBButton, takeVideoWallButton] (BoundsChangeArguments& args) {
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
				takeVideoWallButton->setBounds(ofRectangle(400, this->getHeight() - VMIG_VIDEO_MATRIX_TAKE_HEIGHT, 200, VMIG_VIDEO_MATRIX_TAKE_HEIGHT));
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
						this->onSetInspectorFocus(*this->channels[i]);
					}
				};

				newChannel->onDraw += [i, this] (DrawArguments& args) {
					if(this->selectionA == i) {
						ofPushStyle();
						ofSetColor(50,10,10);
						drawMarker("A", 40);
						ofSetColor(200,100,100);
						ofNoFill();
						ofSetLineWidth(1.0f);
						ofRect(0,0,args.parentBounds.getWidth(), args.parentBounds.getHeight());
						ofPopStyle();
					}

					if(this->selectionB == i) {
						ofPushStyle();
						ofSetColor(50,10,10);
						drawMarker("B", 80);
						ofSetColor(100,200,100);
						ofNoFill();
						ofSetLineWidth(1.0f);
						ofRect(0,0,args.parentBounds.getWidth(), args.parentBounds.getHeight());
						ofPopStyle();
					}
					
					if(this->selectionPreview == i) {
						ofPushStyle();
						ofSetColor(50);
						drawMarker("P", 120);
						ofSetColor(255);
						ofNoFill();
						ofSetLineWidth(1.0f);
						ofRect(0,0,args.parentBounds.getWidth(), args.parentBounds.getHeight());
						ofPopStyle();
					}

					if(this->selectionVideoWall == i) {
						ofPushStyle();
						ofSetColor(10,10,50);
						drawMarker("W", 160);
						ofSetColor(100,100,200);
						ofNoFill();
						ofSetLineWidth(1.0f);
						ofRect(0,0,args.parentBounds.getWidth(), args.parentBounds.getHeight());
						ofPopStyle();
					}
				};

				newChannel->onModeChange += [this, i] (BMDDisplayMode& mode) {
					if (i == this->selectionA) {
						this->inputs[0]->setMode(mode);
					}
					if (i == this->selectionB) {
						this->inputs[1]->setMode(mode);
					}
					this->needsSave = true;
				};
			}

			this->channelInC = -1;
			this->channelInD = -1;

			if (!this->readbackConnections()) {
				//failed to read routing from videohub
				this->setA(8);
				this->setB(9);
				this->setPreview(this->selectionA);
				this->setVideoWall(this->selectionA);
			}

			this->load();
			this->needsSave = false;
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


			//--
			//channel scroll
			//--
			//
			int tick = ofGetElapsedTimef() / VMIG_CHANNEL_SCROLL_TIME;
			tick = tick % (VMIG_CHANNEL_COUNT / 2);
			int newChannelInC =  (tick * 2) % VMIG_CHANNEL_COUNT;
			int newChannelInD = (tick * 2 + 1) % VMIG_CHANNEL_COUNT;

			
			if (newChannelInC != this->channelInC) {
				if (this->channelInC >= 0) {
					this->channels[this->channelInC]->setPreview(this->inputs[2]->getTextureReference());
				}
				this->setC(newChannelInC);
				this->channelInC = newChannelInC;
			}
			if (newChannelInD != this->channelInD) {
				if (this->channelInD >= 0) {
					this->channels[this->channelInD]->setPreview(this->inputs[3]->getTextureReference());
				}
				this->setD(newChannelInD);
				this->channelInD = newChannelInD;
			}
			//
			//--


			if (this->needsSave) {
				this->save();
				this->needsSave = false;
			}
		}

		//----------
		ofxBlackmagic::Input& VideoMatrix::getInput(int index) {
			return * this->inputs[index];
		}

		//----------
		void VideoMatrix::setA(int channel) {
			this->set(0, channel);
			this->selectionA = channel;
		}

		//----------
		void VideoMatrix::setB(int channel) {
			this->set(1, channel);
			this->selectionB = channel;
		}

		//----------
		void VideoMatrix::setC(int channel) {
			this->set(2, channel);
		}

		//----------
		void VideoMatrix::setD(int channel) {
			this->set(3, channel);
		}

		//----------
		void VideoMatrix::set(int input, int channel) {
			this->videoHub.setRoute(input, channel);
			if (input < 2) { // set monitors if A or B
				this->videoHub.setRoute(input + 4, channel);
			}
			auto mode = this->channels[channel]->getMode();
			if (mode != bmdModeUnknown) {
				this->inputs[input]->setMode(mode);
			}
		}

		//----------
		void VideoMatrix::setPreview(int channel) {
			this->videoHub.setRoute(VMIG_PREVIEW_CHANNEL, channel);
			this->selectionPreview = channel;
		}

		//----------
		void VideoMatrix::setVideoWall(int channel) {
			this->videoHub.setRoute(VMIG_VIDEO_WALL_CHANNEL, channel);
			this->selectionVideoWall = channel;
		}

		//----------
		bool VideoMatrix::readbackConnections() {
			//send some data, so we have a local cache of routing
			this->videoHub.setRoute(39, 39);
			while (!this->videoHub.isRoutingUpdate()) {
				this->videoHub.update();
				ofSleepMillis(1);
			}
			auto routing = this->videoHub.getRoutingMatrix();
			if (routing.empty()) {
				return false;
			}
			this->selectionA = routing[0];
			this->selectionB = routing[1];
			this->selectionPreview = routing[VMIG_PREVIEW_CHANNEL];
			this->selectionVideoWall = routing[VMIG_VIDEO_WALL_CHANNEL];
		}

		//----------
		void VideoMatrix::save() const {
			ofstream file;
			file.open(ofToDataPath("channelFormats.txt").c_str(), ios::out);
			for (int i=0; i<this->channels.size(); i++) {
				auto channel = this->channels[i];
				file << channel->getModeString() << endl;
			}
			file.close();
		}

		//----------
		void VideoMatrix::load() {
			ifstream file;
			file.open(ofToDataPath("channelFormats.txt").c_str(), ios::in);
			try {
				if (file.bad()) {
					throw(std::exception("File open failed"));
				}
				for (int i=0; i<this->channels.size(); i++) {
					string mode;
					file >> mode;
					if (file.eof()) {
						throw(std::exception("EOF detected"));
					}
					if (!mode.empty()) {
						auto channel = this->channels[i];
						channel->setMode(mode);
					}
				}
			} catch (std::exception e) {
				ofLogError() << e.what();
			}
			file.close();
		}
	}
}