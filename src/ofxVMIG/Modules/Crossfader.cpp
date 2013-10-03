#include "Crossfader.h"
#include "ofxCvGui/Assets.h"

using namespace ofxCvGui;

namespace ofxVMIG {
	namespace Modules {
		//----------
		void Crossfader::BaseMode::updatePreview() {
			this->preview.allocate(64,64);

			this->preview.begin();
			ofClear(0,0,0,0);
			ofPushStyle();
			ofFill();
			ofSetLineWidth(0);

			float A, B, position;

			ofSetColor(255,100,100);
			ofBeginShape();
			ofVertex(0,32);
			for(int x=0; x<64; x+=2) {
				position = ofMap(x, 0, 64, -1.0f, 1.0f);
				this->crossfade(position, A, B);
				ofVertex(x, 32.0f - A * 32.0f);
			}
			ofVertex(64,32);
			ofEndShape(true);

			ofSetColor(100,255,100);
			ofBeginShape();
			ofVertex(0,32);
			for(int x=0; x<64; x+=2) {
				position = ofMap(x, 0, 64, -1.0f, 1.0f);
				this->crossfade(position, A, B);
				ofVertex(x, 32.0f + B * 32.0f);
			}
			ofVertex(64,32);
			ofEndShape(true);

			ofPopStyle();
			this->preview.end();
		}

		//----------
		void Crossfader::DefaultMode::crossfade(float position, float& A, float& B) {
			A = ofMap(position, -1.0f, +1.0f, 1.0f, 0.0f, true);
			B = ofMap(position, -1.0f, +1.0f, 0.0f, 1.0f, true);
		}

		//----------
		void Crossfader::HalfRangeMode::crossfade(float position, float& A, float& B) {
			A = ofMap(position, -0.5f, +0.5f, 1.0f, 0.0f, true);
			B = ofMap(position, -0.5f, +0.5f, 0.0f, 1.0f, true);
		}

		//----------
		void Crossfader::ThroughBlackMode::crossfade(float position, float& A, float& B) {
			A = ofMap(position, -1.0f, 0.0f, 1.0f, 0.0f, true);
			B = ofMap(position, 0.0f, +1.0f, 0.0f, 1.0f, true);
		}

		//----------
		void Crossfader::CutMode::crossfade(float position, float& A, float& B) {
			A = position <= 0.0f ? 1.0f : 0.0f;
			B = position > 0.0f ? 1.0f : 0.0f;
		}

		//----------
		Crossfader::Crossfader(int targetWidth, int targetHeight) {
			this->caption = "Crossfader";

			this->slider = this->elementGroup->addBlank();
			this->slider->onDraw += [this] (DrawArguments& args) {
				const float yCenter = this->getHeight() / 2.0f;
				const float width = this->getWidth();
				const float xCenter = width / 2.0f;

				ofPushStyle();

				//--
				//major ticks
				//
				ofSetColor(100);
				ofSetLineWidth(1.0f);
				float x = this->getX(-1.0f);
				ofLine(x, yCenter - 50.0f, x, yCenter + 50.0f);
				x = this->getX(0.0f);
				ofLine(x, yCenter - 50.0f, x, yCenter + 50.0f);
				x = this->getX(1.0f);
				ofLine(x, yCenter - 50.0f, x, yCenter + 50.0f);
				//
				//--

				//--
				//coloured ticks
				//
				for(float position = -1.0f; position <= 1.0f; position += 0.01f) {
					this->drawTicks(position);
				}
				//
				//--

				//--
				//handle
				//
				ofSetLineWidth(1.0f);
				float handleX = this->getX(this->position);

				ofFill();
				ofEnableAlphaBlending();
				ofSetColor(0,0,0,50);
				ofRect(handleX - 10.0f, yCenter - VMIG_CROSSFADE_HANDLE_HEIGHT / 2.0f, 20.0f, VMIG_CROSSFADE_HANDLE_HEIGHT);

				ofNoFill();
				ofSetColor(255);
				ofLine(handleX, yCenter - VMIG_CROSSFADE_ROW_HEIGHT / 2.0f, handleX, yCenter - VMIG_CROSSFADE_ROW_HEIGHT);
				ofLine(handleX, yCenter + VMIG_CROSSFADE_ROW_HEIGHT / 2.0f, handleX, yCenter + VMIG_CROSSFADE_ROW_HEIGHT);
				ofPopStyle();
				//
				//--
			};

			//--
			//select mode buttons
			//
			vector<ElementPtr> modeButtons;
			auto selectDefault = this->elementGroup->addBlank();
			auto selectHalfRange = this->elementGroup->addBlank();
			auto selectThroughBlack = this->elementGroup->addBlank();
			auto selectCut = this->elementGroup->addBlank();
			modeButtons.push_back(selectThroughBlack);
			modeButtons.push_back(selectDefault);
			modeButtons.push_back(selectHalfRange);
			modeButtons.push_back(selectCut);

			selectDefault->onDraw += [this] (DrawArguments& args) {
				ofEnableAlphaBlending();
				this->defaultMode->preview.draw(0,0,args.parentBounds.width, args.parentBounds.height);
				ofDisableAlphaBlending();
				if (this->selectedMode == this->defaultMode) {
					ofPushStyle();
					ofSetColor(255);
					ofNoFill();
					ofSetLineWidth(1.0f);
					ofRect(0,0,args.parentBounds.width, args.parentBounds.height);
					ofPopStyle();
				}
			};
			selectHalfRange->onDraw += [this] (DrawArguments& args) {
				ofEnableAlphaBlending();
				this->halfRangeMode->preview.draw(0,0,args.parentBounds.width, args.parentBounds.height);
				ofDisableAlphaBlending();
				if (this->selectedMode == this->halfRangeMode) {
					ofPushStyle();
					ofSetColor(255);
					ofNoFill();
					ofSetLineWidth(1.0f);
					ofRect(0,0,args.parentBounds.width, args.parentBounds.height);
					ofPopStyle();
				}
			};
			selectThroughBlack->onDraw += [this] (DrawArguments& args) {
				ofEnableAlphaBlending();
				this->throughBlackMode->preview.draw(0,0,args.parentBounds.width, args.parentBounds.height);
				ofDisableAlphaBlending();
				if (this->selectedMode == this->throughBlackMode) {
					ofPushStyle();
					ofSetColor(255);
					ofNoFill();
					ofSetLineWidth(1.0f);
					ofRect(0,0,args.parentBounds.width, args.parentBounds.height);
					ofPopStyle();
				}
			};
			selectCut->onDraw += [this] (DrawArguments& args) {
				ofEnableAlphaBlending();
				this->cutMode->preview.draw(0,0,args.parentBounds.width, args.parentBounds.height);
				ofDisableAlphaBlending();
				if (this->selectedMode == this->cutMode) {
					ofPushStyle();
					ofSetColor(255);
					ofNoFill();
					ofSetLineWidth(1.0f);
					ofRect(0,0,args.parentBounds.width, args.parentBounds.height);
					ofPopStyle();
				}
			};
			
			selectDefault->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->selectedMode = this->defaultMode;
				}
			};
			selectHalfRange->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->selectedMode = this->halfRangeMode;
				}
			};
			selectThroughBlack->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->selectedMode = this->throughBlackMode;
				}
			};
			selectCut->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->selectedMode = this->cutMode;
				}
			};
			//
			//--


			//--
			//auto fade buttons
			//
			auto autoToA_1 = this->elementGroup->addBlank();
			auto autoToA_2 = this->elementGroup->addBlank();
			auto autoToA_5 = this->elementGroup->addBlank();
			auto autoToA_10 = this->elementGroup->addBlank();
			
			auto autoToB_1 = this->elementGroup->addBlank();
			auto autoToB_2 = this->elementGroup->addBlank();
			auto autoToB_5 = this->elementGroup->addBlank();
			auto autoToB_10 = this->elementGroup->addBlank();

			autoToA_1->onDraw += [] (DrawArguments&) {
				ofxCvGui::AssetRegister.drawText("1s", 0, 0, "", true, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE);
			};
			autoToA_2->onDraw += [] (DrawArguments&) {
				ofxCvGui::AssetRegister.drawText("2s", 0, 0, "", true, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE);
			};
			autoToA_5->onDraw += [] (DrawArguments&) {
				ofxCvGui::AssetRegister.drawText("5s", 0, 0, "", true, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE);
			};
			autoToA_10->onDraw += [] (DrawArguments&) {
				ofxCvGui::AssetRegister.drawText("10s", 0, 0, "", true, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE);
			};
			autoToB_1->onDraw += [] (DrawArguments&) {
				ofxCvGui::AssetRegister.drawText("1s", 0, 0, "", true, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE);
			};
			autoToB_2->onDraw += [] (DrawArguments&) {
				ofxCvGui::AssetRegister.drawText("2s", 0, 0, "", true, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE);
			};
			autoToB_5->onDraw += [] (DrawArguments&) {
				ofxCvGui::AssetRegister.drawText("5s", 0, 0, "", true, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE);
			};
			autoToB_10->onDraw += [] (DrawArguments&) {
				ofxCvGui::AssetRegister.drawText("10s", 0, 0, "", true, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE);
			};

			autoToA_1->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->autoFade = AutoFade_A;
					this->autoFadeSpeed = (-1.0f - this->position) / 1.0f;
				}
			};
			autoToA_2->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->autoFade = AutoFade_A;
					this->autoFadeSpeed = (-1.0f - this->position) / 2.0f;
				}
			};
			autoToA_5->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->autoFade = AutoFade_A;
					this->autoFadeSpeed = (-1.0f - this->position) / 5.0f;
				}
			};
			autoToA_10->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->autoFade = AutoFade_A;
					this->autoFadeSpeed = (-1.0f - this->position) / 10.0f;
				}
			};
			autoToB_1->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->autoFade = AutoFade_B;
					this->autoFadeSpeed = (+1.0f - this->position) / 1.0f;
				}
			};
			autoToB_2->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->autoFade = AutoFade_B;
					this->autoFadeSpeed = (+1.0f - this->position) / 2.0f;
				}
			};
			autoToB_5->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->autoFade = AutoFade_B;
					this->autoFadeSpeed = (+1.0f - this->position) / 5.0f;
				}
			};
			autoToB_10->onMouse += [this] (MouseArguments& args) {
				if (args.isLocalPressed()) {
					this->autoFade = AutoFade_B;
					this->autoFadeSpeed = (+1.0f - this->position) / 10.0f;
				}
			};
			//
			//--

			this->onMouse += [this] (MouseArguments& args) {
				if (args.action == MouseArguments::Pressed) {
					this->draggingCrossfader = (args.local.y >= this->getHeight() / 2.0f - VMIG_CROSSFADE_HANDLE_HEIGHT &&
						args.local.y <= this->getHeight() / 2.0f + VMIG_CROSSFADE_HANDLE_HEIGHT);
				}

				if (args.action == MouseArguments::Dragged && this->draggingCrossfader) {
					float dx = args.movement.x;
					float dPosition = this->getPosition(args.local.x) - this->getPosition(args.local.x - dx);
					this->setPosition(this->position + dPosition);
				}
			};

			this->onBoundsChange += [this, modeButtons, autoToA_1, autoToA_2, autoToA_5, autoToA_10, autoToB_1, autoToB_2, autoToB_5, autoToB_10] (BoundsChangeArguments&) {
				this->slider->setBounds(ofRectangle(0,0,this->getWidth(), this->getHeight()));
				float spread = modeButtons.size() * VMIG_CROSSFADE_MODE_BUTTON_SPACING;
				for(int i=0; i<modeButtons.size(); i++) {
					auto & button = modeButtons[i];
					float x = ofMap(i, 0, modeButtons.size() - 1, -spread / 2.0f, spread / 2.0f) + this->getWidth() / 2.0f;
					button->setBounds(ofRectangle(x - VMIG_CROSSFADE_MODE_BUTTON_SIZE / 2.0f, this->getHeight() / 2.0f + VMIG_CROSSFADE_MODE_BUTTON_Y_OFFSET, VMIG_CROSSFADE_MODE_BUTTON_SIZE, VMIG_CROSSFADE_MODE_BUTTON_SIZE));
				}

				float autoToAx = VMIG_CROSSFADE_INDENT - VMIG_CROSSFADE_AUTO_FADE_BUTTON_SPACING - VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE / 2.0f;
				autoToA_1->setBounds(ofRectangle(autoToAx, this->getHeight() / 2.0f - 2 * VMIG_CROSSFADE_AUTO_FADE_BUTTON_SPACING - VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE / 2.0f,
					VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE));
				autoToA_2->setBounds(ofRectangle(autoToAx, this->getHeight() / 2.0f - VMIG_CROSSFADE_AUTO_FADE_BUTTON_SPACING - VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE / 2.0f,
					VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE));
				autoToA_5->setBounds(ofRectangle(autoToAx, this->getHeight() / 2.0f - VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE / 2.0f,
					VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE));
				autoToA_10->setBounds(ofRectangle(autoToAx, this->getHeight() / 2.0f + VMIG_CROSSFADE_AUTO_FADE_BUTTON_SPACING - VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE / 2.0f,
					VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE));

				float autoToBx = this->getWidth() - VMIG_CROSSFADE_INDENT + VMIG_CROSSFADE_AUTO_FADE_BUTTON_SPACING - VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE / 2.0f;
				autoToB_1->setBounds(ofRectangle(autoToBx, this->getHeight() / 2.0f - 2 * VMIG_CROSSFADE_AUTO_FADE_BUTTON_SPACING - VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE / 2.0f,
					VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE));
				autoToB_2->setBounds(ofRectangle(autoToBx, this->getHeight() / 2.0f - VMIG_CROSSFADE_AUTO_FADE_BUTTON_SPACING - VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE / 2.0f,
					VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE));
				autoToB_5->setBounds(ofRectangle(autoToBx, this->getHeight() / 2.0f - VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE / 2.0f,
					VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE));
				autoToB_10->setBounds(ofRectangle(autoToBx, this->getHeight() / 2.0f + VMIG_CROSSFADE_AUTO_FADE_BUTTON_SPACING - VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE / 2.0f,
					VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE, VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE));
			};

			this->onUpdate += [this] (UpdateArguments&) {
				this->drawFade();
				if (this->autoFade != AutoFade_None) {
					this->position += this->autoFadeSpeed * ofGetLastFrameTime();
					if (this->position < -1.0f || this->position > 1.0f) {
						this->setPosition(this->position); // this will clear auto and clamp
					}
				}
			};

			this->defaultMode = ofPtr<BaseMode>(new DefaultMode());
			this->halfRangeMode = ofPtr<BaseMode>(new HalfRangeMode());
			this->throughBlackMode = ofPtr<BaseMode>(new ThroughBlackMode());
			this->cutMode = ofPtr<BaseMode>(new CutMode());
			this->selectedMode = this->defaultMode;

			this->defaultMode->updatePreview();
			this->halfRangeMode->updatePreview();
			this->throughBlackMode->updatePreview();
			this->cutMode->updatePreview();

			this->position = 0.0f;
			this->A = 0;
			this->B = 0;
			this->targetWidth = targetWidth;
			this->targetHeight = targetHeight;
			this->target.allocate(this->targetWidth, this->targetHeight, GL_RGBA);
			this->draggingCrossfader = false;
		}

		//----------
		void Crossfader::setPosition(float position) {
			this->position = ofClamp(position, -1.0f, 1.0f);
			this->autoFade = AutoFade_None;
		}
		
		//----------
		void Crossfader::setA(ofTexture& A) {
			this->A = &A;
		}

		//----------
		void Crossfader::setB(ofTexture& B) {
			this->B = &B;
		}

		//----------
		ofFbo& Crossfader::getTarget() {
			return this->target;
		}

		//----------
		void Crossfader::drawFade() {
			this->target.begin();
			ofClear(0);
			ofEnableBlendMode(OF_BLENDMODE_ADD);
			
			float alphaA, alphaB;
			this->selectedMode->crossfade(this->position, alphaA, alphaB);
			if(this->A != 0) {
				ofSetColor(255, 255, 255, 255.0f * alphaA);
				this->A->draw(0, 0, this->targetWidth, this->targetHeight);
			}
			if(this->B != 0) {
				ofSetColor(255, 255, 255, 255.0f * alphaB);
				this->B->draw(0, 0, this->targetWidth, this->targetHeight);
			}

			ofDisableBlendMode();
			this->target.end();
		}

		//----------
		float Crossfader::getX(float position) {
			return ofMap(position, -1.0f, 1.0f, VMIG_CROSSFADE_INDENT, this->getWidth() - VMIG_CROSSFADE_INDENT);;
		}

		//----------
		float Crossfader::getPosition(float x) {
			return ofMap(x, VMIG_CROSSFADE_INDENT, this->getWidth() - VMIG_CROSSFADE_INDENT, -1.0f, 1.0f, true);;
		}

		//----------
		void Crossfader::drawTicks(float position)  {
			float alphaA, alphaB;
			this->selectedMode->crossfade(position, alphaA, alphaB);

			const float x = this->getX(position);
			const float yCenter = this->getHeight() / 2.0f;

			//center
			ofSetColor(255);
			ofLine(x, yCenter - VMIG_CROSSFADE_ROW_HEIGHT / 2.0f, x, yCenter + VMIG_CROSSFADE_ROW_HEIGHT / 2.0f);
			
			//A
			ofSetColor(255,100,100);
			ofLine(x, yCenter - VMIG_CROSSFADE_ROW_SPACING, x, yCenter - VMIG_CROSSFADE_ROW_SPACING - alphaA * VMIG_CROSSFADE_ROW_HEIGHT);

			//B
			ofSetColor(100,255,100);
			ofLine(x, yCenter + VMIG_CROSSFADE_ROW_SPACING, x, yCenter + VMIG_CROSSFADE_ROW_SPACING + alphaB * VMIG_CROSSFADE_ROW_HEIGHT);
		}
	}
}