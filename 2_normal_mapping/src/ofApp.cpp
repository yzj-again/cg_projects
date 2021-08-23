#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//ofBackground(128, 128, 128);
	string file_name = "shield.ply";
	mesh.load(file_name);
	
	using namespace glm;

	const vec3* vertices = mesh.getVerticesPointer();
	const vec2* uvs = mesh.getTexCoordsPointer();
	const uint* triangles = mesh.getIndexPointer();

	std::vector <std::vector<vec3>> verticesTangentsLists;
	verticesTangentsLists.resize(mesh.getNumVertices());
	std::vector<vec3> verticesTangents;
	verticesTangents.resize(mesh.getNumVertices());

	for (int i = 0; i < mesh.getNumIndices() / 3; i += 3) {
		const vec3& v0 = vertices[triangles[i]];
		const vec3& v1 = vertices[triangles[i+1]];
		const vec3& v2 = vertices[triangles[i+2]];
		const vec2& uv0 = uvs[triangles[i]];
		const vec2& uv1 = uvs[triangles[i+1]];
		const vec2& uv2 = uvs[triangles[i+2]];

		const vec3 e1 = v1 - v0;
		const vec3 e2 = v2 - v0;
		const mat2x3 E = mat2x3(e1[0], e1[1], e1[2], e2[0], e2[1], e2[2]);
		const vec2 delta_uv1 = uv1 - uv0;
		const vec2 delta_uv2 = uv2 - uv0;

		const float& d_u1 = delta_uv1.x;
		const float& d_v1 = delta_uv1.y;
		const float& d_u2 = delta_uv2.x;
		const float& d_v2 = delta_uv2.y;

		const float coeff = 1 / (d_u1 * d_v2 - d_u2 * d_v1);

		const glm::vec3 T = normalize(E * vec2(d_v2, -d_v1));

		verticesTangentsLists[triangles[i]].push_back(T);
		verticesTangentsLists[triangles[i+1]].push_back(T);
		verticesTangentsLists[triangles[i+2]].push_back(T);
	}

	for (int i = 0; i < verticesTangentsLists.size(); i++) {
		verticesTangents[i] = vec3(0, 0, 0);
		if (verticesTangentsLists[i].size() > 0) {
			for (int j = 0; j < verticesTangentsLists[i].size(); j++){
				verticesTangents[i] += verticesTangentsLists[i][j];
			}
			verticesTangents[i] /= verticesTangentsLists[i].size();
			vec3& t = verticesTangents[i];
			mesh.addColor(ofFloatColor(t.x, t.y, t.z, 0.0));
		}
	}
	
	shader.load("Blinn-Phong.vert", "Blinn-Phong.frag");
	ofDisableArbTex();
	diffuseTex.load("shield_diffuse.png");
	specTex.load("shield_spec.png");
	normalTex.load("shield_normal.png");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	projectionMatrix = glm::perspective(ofDegToRad(45), float(ofGetWindowWidth()) / ofGetWindowHeight(), 1.f, 150.0f);
	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
	normalMatrix = glm::inverseTranspose(glm::mat4(modelViewMatrix));
	
	shader.begin();
	ofEnableDepthTest();

	shader.setUniformMatrix4f("modelViewMatrix", modelViewMatrix);
	shader.setUniformMatrix4f("modelViewProjectionMatrix", modelViewProjectionMatrix);
	shader.setUniformMatrix4f("normalMatrix", normalMatrix);
	
	shader.setUniform4f("mat_ambient", glm::vec4(1, 1, 1, 0));
	shader.setUniform4f("mat_diffuse", glm::vec4(1, 1, 1, 1));
	shader.setUniform1f("k_shininess", 3.0);
	shader.setUniform4f("mat_specular", glm::vec4(1, 1, 1, 1));
	shader.setUniform4f("mat_emissive", glm::vec4(0, 0, 0, 0));
	
	shader.setUniform4f("light.position", modelViewMatrix * lightPosition);
	shader.setUniform4f("light.ambient", glm::vec4(0, 0, 0, 1));
	shader.setUniform4f("light.diffuse", glm::vec4(1, 1, 1, 0));
	shader.setUniform4f("light.specular", glm::vec4(1, 1, 1, 0));

	shader.setUniformTexture("diffuseTex", diffuseTex, 0);
	shader.setUniformTexture("specTex", specTex, 1);
	shader.setUniformTexture("normalTex", normalTex, 2);
	
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
