#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//ofBackground(128, 128, 128);

	cube_mesh.load("cube.ply");
	
	//string file_name = "torus.ply";
	//string file_name = "cube.ply";
	string file_name = "bunny.ply";
	//string file_name = "ball.ply";
	
	mesh.load(file_name);
	auto vert_ptr = mesh.getVerticesPointer();

	glm::vec3 max_xyz = *vert_ptr;
	glm::vec3 min_xyz = *vert_ptr;
	glm::vec3 sum_vert(0);

	for (int i=0; i < mesh.getNumVertices(); i++) {
		glm::vec3 vert = *(vert_ptr + i);
		for (int j = 0; j <= 2; j++) {
			if (vert[j] > max_xyz[j])
				max_xyz[j] = vert[j];
			else{
				if (vert[j] < min_xyz[j])
					min_xyz[j] = vert[j];
			}
		}
		sum_vert += vert;
	}
	mesh_center_pt = sum_vert / mesh.getNumVertices();

	cout
		<< "Loaded model: " << file_name << endl
		<< "Number of vertices: " << mesh.getNumVertices() << endl
		<< "Number of faces: " << mesh.getNumIndices() << endl
		<< "Bounding boxes: " << endl
			<< "  min_xyz: (" << min_xyz << ")" << endl
			<< "  max_xyz: (" << max_xyz << ")" << endl
		<< "Center Point: (" << mesh_center_pt << ")" << endl;
	
	cubemap.load("nvlobby_front.jpg", "nvlobby_back.jpg", "nvlobby_right.jpg", "nvlobby_left.jpg", "nvlobby_top.jpg", "nvlobby_bottom.jpg");
	//cubemap.load("cube_front.jpg", "cube_back.jpg", "cube_right.jpg", "cube_left.jpg", "cube_top.jpg", "cube_bottom.jpg");
	//cubemap.load("cube2_front.jpg", "cube2_back.jpg", "cube2_right.jpg", "cube2_left.jpg", "cube2_top.jpg", "cube2_bottom.jpg");
	
	shader.load("env_mapping.vert", "env_mapping.frag");
	skyboxShader.load("skybox.vert", "skybox.frag");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	eyePos = camOrbit.Pos();

	// SkyBox Shader
	modelMatrix = camOrbit.ModelMatrix();
	modelMatrix = glm::scale(modelMatrix, glm::vec3(6.0f, 6.0f, 6.0f)) * modelMatrix;

	viewMatrix = camOrbit.ViewMatrix();
	projectionMatrix = camOrbit.ProjectionMatrix();

	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
	normalMatrix = glm::inverseTranspose(glm::mat4(modelMatrix));

	skyboxShader.begin();
	glDepthFunc(GL_LEQUAL);
	ofEnableDepthTest();

	shader.setUniformMatrix4f("modelMatrix", modelMatrix);
	skyboxShader.setUniformMatrix4f("modelViewProjectionMatrix", modelViewProjectionMatrix);
	skyboxShader.setUniformTexture("envMap", cubemap.getTexture(), 0);

	cube_mesh.draw();

	ofDisableDepthTest();
	glDepthFunc(GL_LESS);
	skyboxShader.end();
	
	// Object Shader
	modelMatrix = camOrbit.ModelMatrix();
	modelMatrix = glm::translate(glm::mat4(1), -mesh_center_pt) * modelMatrix;
	modelMatrix = glm::rotate(glm::mat4(1), glm::radians(270.f), glm::vec3(1.0f, 0.0f, 0.0f)) * modelMatrix;
	viewMatrix = camOrbit.ViewMatrix();
	projectionMatrix = camOrbit.ProjectionMatrix();

	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
	normalMatrix = glm::inverseTranspose(glm::mat4(modelMatrix));

	shader.begin();
	ofEnableDepthTest();

	shader.setUniformMatrix4f("modelMatrix", modelMatrix);
	shader.setUniformMatrix4f("modelViewProjectionMatrix", modelViewProjectionMatrix);
	shader.setUniformMatrix4f("normalMatrix", normalMatrix);
	shader.setUniform3f("eyePos", eyePos);
	
	mesh.draw();
	
	ofDisableDepthTest();
	shader.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'r') {
		camOrbit.Reset();
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
	sliderX = (x - mouseLastX) / trackBallUnitLength;
	sliderY = (y - mouseLastY) / trackBallUnitLength;
	mouseLastX = x;
	mouseLastY = y;

	switch (mousePressedButton) {
		case 0:
			camOrbit.RotateHorizotal(rotateSensitivity * sliderX);
			camOrbit.RotateVertical(rotateSensitivity * sliderY);
			break;
		/*case 1:
			break;*/
		case 2:
			camOrbit.Zoom(zoomSensitivity * sliderX);
			break;
	}

	/*cout << "Dragged" << endl
		<< "pos: " << camOrbit.Pos() << endl
		<< "m_azimuth: " << camOrbit.m_azimuth << endl
		<< "m_zenith: " << camOrbit.m_zenith << endl
		<< "m_radius: " << camOrbit.m_radius << endl
		<< endl;*/
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	mousePressedButton = button;
	mouseLastX = x;
	mouseLastY = y;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
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
