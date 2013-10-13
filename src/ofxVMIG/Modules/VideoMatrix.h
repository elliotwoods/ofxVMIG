#pragma once
#include "ofxBlackMagic.h"
#include "ofxVideoHubControl.h"
#include "ofxCvGui/Panels/ElementHost.h"
#include "ofxVMIG/Inspectable.h";

#define VMIG_CHANNEL_COUNT 16
#define VMIG_CHANNEL_SCROLL_TIME 4.0f
#define VMIG_PREVIEW_CHANNEL 6
#define VMIG_RECORD_CHANNEL 8
#define VMIG_VIDEO_WALL_CHANNEL 11
#define VMIG_VIDEO_MATRIX_TAKE_HEIGHT 60.0f

namespace ofxVMIG {
	namespace Modules {
		class Channel : public ofxCvGui::Element, public ofxVMIG::Inspectable {
		public:
			Channel(int index);
			int getIndex();
			void setLabel(string);
			string getLabel() const;
			void setPreview(ofTexture&);

			void populateInspector(ofxUICanvas &canvas) override;
			void inspectorCallback(ofxUIEventArgs &args) override;
			void setMode(string mode);
			void setMode(BMDDisplayMode mode);
			BMDDisplayMode getMode();
			string getModeString();

			ofxLiquidEvent<BMDDisplayMode> onModeChange;

		protected:
			ofFbo preview;
			int index;
			string label;
			static map<string, BMDDisplayMode> modeList;
			static map<BMDDisplayMode, string> reverseModeList; //should use multimap
			map<string, bool> modeSelection;
			string mode;
			static bool modeListInitialised;
		};

		class VideoMatrix : public ofxCvGui::Panels::ElementHost {
		public:
			VideoMatrix();
			void update();
			ofxBlackmagic::Input& getInput(int index);
			void setA(int channel);
			void setB(int channel);
			void setC(int channel);
			void setD(int channel);
			void set(int input, int channel);

			void setPreview(int channel);
			void setVideoWall(int channel);

			bool readbackConnections();

			void save() const;
			void load();

			ofxLiquidEvent<Inspectable> onSetInspectorFocus;
		protected:
			vector<ofPtr<ofxBlackmagic::Input>> inputs;
			vector<ofPtr<Channel> > channels;
			ofxVideoHubControl videoHub;
			int channelInC, channelInD;

			int selectionA, selectionB, selectionPreview, selectionVideoWall;
			bool needsSave;
		};
	}
}