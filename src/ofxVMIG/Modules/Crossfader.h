#pragma once
#include "ofxCvGui/Panels/ElementHost.h"
#define VMIG_CROSSFADE_ROW_HEIGHT 20.0f
#define VMIG_CROSSFADE_ROW_SPACING 40.0f
#define VMIG_CROSSFADE_HANDLE_HEIGHT 80.0f
#define VMIG_CROSSFADE_INDENT 100.0f
#define VMIG_CROSSFADE_MODE_BUTTON_SPACING 80.0f
#define VMIG_CROSSFADE_MODE_BUTTON_SIZE 50.0f
#define VMIG_CROSSFADE_MODE_BUTTON_Y_OFFSET 80.0f
#define VMIG_CROSSFADE_AUTO_FADE_BUTTON_SIZE 50.0f
#define VMIG_CROSSFADE_AUTO_FADE_BUTTON_SPACING 60.0f

namespace ofxVMIG {
	namespace Modules {
		class Crossfader : public ofxCvGui::Panels::ElementHost {
		public:
			//------------
			class BaseMode {
			public:
				virtual void crossfade(float position, float& A, float& B) = 0;
				virtual string getName() = 0;
				void updatePreview();
				ofFbo preview;
			};

			//------------
			class DefaultMode : public BaseMode {
			public:
				void crossfade(float position, float& A, float& B) override;
				string getName() override { return "Crossfade"; }
			};

			//------------
			class HalfRangeMode : public BaseMode {
			public:
				void crossfade(float position, float& A, float& B) override;
				string getName() override { return "Crossfade"; }
			};

			//------------
			class ThroughBlackMode : public BaseMode {
			public:
				void crossfade(float position, float& A, float& B) override;
				string getName() override { return "Through Black"; }
			};

			//------------
			class CutMode : public BaseMode {
			public:
				void crossfade(float position, float& A, float& B) override;
				string getName() override { return "Cut"; }
			};

			//------------
			enum AutoFade {
				AutoFade_None,
				AutoFade_A,
				AutoFade_B
			};

			Crossfader(int targetWidth, int targetHeight);
			void setPosition(float);
			void setA(ofTexture&);
			void setB(ofTexture&);
			ofFbo& getTarget();

		protected:
			void drawFade();
			float getX(float position);
			float getPosition(float x);
			void drawTicks(float position);
			float position;
			ofxCvGui::ElementPtr slider;
			ofPtr<BaseMode> defaultMode, halfRangeMode, throughBlackMode, cutMode;
			ofPtr<BaseMode> selectedMode;

			int targetWidth, targetHeight;
			ofTexture* A;
			ofTexture* B;
			ofFbo target;
			bool draggingCrossfader;

			AutoFade autoFade;
			float autoFadeSpeed;
		};
	}
}