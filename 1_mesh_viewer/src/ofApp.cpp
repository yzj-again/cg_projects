#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//ofBackground(128, 128, 128);

	string file_name = "torus.ply";
	mesh.load(file_name);
	vector<glm::vec3> vertices = mesh.getVertices();
	vector<ofMeshFace> faces = mesh.getUniqueFaces();

	vector<glm::vec3>::iterator it;
	glm::vec3 max_xyz = vertices[0];
	glm::vec3 min_xyz = vertices[0];
	glm::vec3 sum_vert(0);

	for (it = vertices.begin(); it != vertices.end(); it++) {
		glm::vec3 vert = *it;
		for (int i = 0; i <= 2; i++) {
			if (vert[i] > max_xyz[i])
				max_xyz[i] = vert[i];
			else{
				if (vert[i] < min_xyz[i])
					min_xyz[i] = vert[i];
			}
		}
		sum_vert += vert;
	}
	glm::vec3 center_pt = sum_vert / vertices.size();

	cout << "Loaded model: " << file_name << endl
		<< "Number of vertices: " << vertices.size() << endl
		<< "Number of faces: " << faces.size() << endl
		<< "Bounding boxes: " << endl
			<< "  min_xyz: (" << min_xyz << ")" << endl
			<< "  max_xyz: (" << max_xyz << ")" << endl
		<< "Center Point: (" << center_pt << ")" << endl;
	
	shader.load("Blinn-Phong.vert", "Blinn-Phong.frag");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	projectionMatrix = glm::perspective(0.6f, float(ofGetWindowWidth()) / ofGetWindowHeight(), 1.f, 150.0f);
	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
	normalMatrix = glm::inverseTranspose(glm::mat4(modelViewMatrix));
	
	shader.begin();
	ofEnableDepthTest();

	shader.setUniformMatrix4f("modelViewMatrix", modelViewMatrix);
	shader.setUniformMatrix4f("modelViewProjectionMatrix", modelViewProjectionMatrix);
	shader.setUniformMatrix4f("normalMatrix", normalMatrix);
	
	shader.setUniform4f("mat_ambient", glm::vec4(1, 1, 1, 0));
	shader.setUniform4f("mat_diffuse", glm::vec4(1, 0, 0, 1));
	shader.setUniform1f("k_shininess", 64.0);
	shader.setUniform4f("mat_specular", glm::vec4(1, 1, 1, 1));
	shader.setUniform4f("mat_emissive", glm::vec4(0, 0, 0, 0));
	
	shader.setUniform4f("light.position", modelViewMatrix * lightPosition);
	shader.setUniform4f("light.ambient", glm::vec4(0, 0, 0, 1));
	shader.setUniform4f("light.diffuse", glm::vec4(1, 1, 1, 0));
	shader.setUniform4f("light.specular", glm::vec4(1, 1, 1, 0));

	mesh.draw();
	
	ofDisableDepthTest();
	shader.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'r') {
		lastModelMatrix = glm::mat4(1);
		modelMatrix = translateMatrix * trackballMatrix * glm::inverse(translateMatrix) * lastModelMatrix;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	float trackBallUnitLength = glm::min(ofGetWindowWidth(), ofGetWindowHeight());
	sliderX = (x - mousePressedX) / trackBallUnitLength;
	sliderY = (y - mousePressedY) / trackBallUnitLength;

	switch (mousePressedButton) {
		case 0:
			trackballMatrix = glm::rotate(glm::mat4(1), glm::radians(rotateSensitivity * sliderX), glm::vec3(0.0f, 1.0f, 0.0f));
			trackballMatrix = glm::rotate(trackballMatrix, glm::radians(rotateSensitivity * sliderY), glm::vec3(1.0f, 0.0f, 0.0f));
			break;
		case 1:
			trackballMatrix[3][0] = sliderX * translateSensitivity;
			trackballMatrix[3][1] = -sliderY * translateSensitivity;
			break;
		case 2:
			trackballMatrix[3][3] = glm::exp(sliderX * zoomSensitivity);
			break;
	}
	//cout << "Dragged" << endl << trackballMatrix << endl;
	translateMatrix[3][0] = lastModelMatrix[3][0] / lastModelMatrix[3][3];
	translateMatrix[3][1] = lastModelMatrix[3][1] / lastModelMatrix[3][3];
	modelMatrix = translateMatrix * trackballMatrix * glm::inverse(translateMatrix) * lastModelMatrix;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	mousePressedButton = button;
	mousePressedX = x;
	mousePressedY = y;
	//cout << "Pressed" << endl << lastModelMatrix << endl;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	lastModelMatrix = translateMatrix * trackballMatrix * glm::inverse(translateMatrix) * lastModelMatrix;
	trackballMatrix = glm::mat4(1);
	//cout << "Release" << endl << lastModelMatrix << endl;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
