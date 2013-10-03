#pragma once
#include "ofxBlackMagic.h"
#include "ofxVideoHubControl.h"
#include "ofxCvGui/Panels/ElementHost.h"
#include "ofxVMIG/Inspectable.h";

#define VMIG_CHANNEL_COUNT 16
#define VMIG_PREVIEW_CHANNEL 6
#define VMIG_VIDEO_MATRIX_TAKE_HEIGHT 60.0f

namespace ofxVMIG {
	namespace Modules {
		class Channel : public ofxCvGui::Element {
		public:
			Channel(int index);
			void setLabel(string);
			string getLabel() const;
			void setPreview(ofTexture&);
		protected:
			ofFbo preview;
			int index;
			string label;
		};

		class VideoMatrix : public ofxCvGui::Panels::ElementHost, public Inspectable {
		public:
			VideoMatrix();
			void update();
			void populateInspector(ofxUICanvas &canvas) override;
			void inspectorCallback(ofxUIEventArgs &args) override;
			ofxBlackmagic::Input& getInput(int index);
			void setA(int channel);
			void setB(int channel);
			void setPreview(int channel);
		protected:
			vector<ofPtr<ofxBlackmagic::Input>> inputs;
			vector<ofPtr<Channel> > channels;
			ofxVideoHubControl videoHub;
			int channelInC, channelInD;

			int selectionA, selectionB, selectionPreview;
		};
	}
}