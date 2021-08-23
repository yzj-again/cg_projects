#pragma once

#include "ofMain.h"
#include <glm/gtx/string_cast.hpp>

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
		ofShader shader;
		ofImage diffuseTex;
		ofImage specTex;
		ofImage normalTex;

		glm::mat4 modelMatrix = glm::mat4(1.0f); // current model pose after last dragging
		glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glm::mat4 projectionMatrix;
		glm::mat4 modelViewMatrix;
		glm::mat4 modelViewProjectionMatrix;
		glm::mat4 normalMatrix;
		glm::vec4 lightPosition = glm::vec4(-0.5, 1, 1, 0);
		glm::mat4 lastModelMatrix = glm::mat4(1.0f); // model pose after last mouse releasing
		glm::mat4 trackballMatrix = glm::mat4(1.0f); // rotation, zooming, translation all in one
		glm::mat4 translateMatrix = glm::mat4(1.0f); // translate model to orgin before rotation

		int mousePressedX, mousePressedY;
		int mousePressedButton;
		float sliderX, sliderY;
		float rotateSensitivity = 240.f;
		float zoomSensitivity = 2.f;
		float translateSensitivity = 3.f;
};
