#include "ofMain.h"
#include "ofApp.h"


//========================================================================
int main() {
	ofGLWindowSettings glSettings;
	glSettings.setSize(720, 720);
	glSettings.windowMode = OF_WINDOW;
	glSettings.setGLVersion(4, 1);
	ofCreateWindow(glSettings);

	ofRunApp(new ofApp());
}
