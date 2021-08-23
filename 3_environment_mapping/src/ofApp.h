#pragma once

#include "ofMain.h"
#include "ofxEasyCubemap.h"
#include "CameraOrbit.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		ofMesh mesh;
		ofMesh cube_mesh;
		ofxEasyCubemap cubemap;
		ofShader shader;
		ofShader skyboxShader;

		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		glm::mat4 modelViewMatrix;
		glm::mat4 modelViewProjectionMatrix;
		glm::mat4 normalMatrix;
		glm::vec3 eyePos;
		glm::vec3 mesh_center_pt;


		int mouseLastX, mouseLastY;
		int mousePressedButton;
		float sliderX, sliderY;
		float rotateSensitivity = 10.f;
		float zoomSensitivity = 10.f;
		float translateSensitivity = 3.f;

		CameraOrbit camOrbit;
};
