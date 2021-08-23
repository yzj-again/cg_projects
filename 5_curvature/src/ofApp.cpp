#include "ofApp.h"

//--------------------------------------------------------------
ofFloatColor num2color(float num)
{
	if (num >= 0 && num < 0.25)
		return ofFloatColor(0, num, 1, 1);
	else if (num >= 0.25 && num < 0.5)
		return ofFloatColor(0, 1, 1-num, 1);
	else if (num >= 0.5 && num < 0.75)
		return ofFloatColor(num, 1, 0, 1);
	else if (num >= 0.75 && num <= 1)
		return ofFloatColor(1, 1-num, 0, 1);
	else if (num < 0)
		return ofFloatColor(0, 0, 0, 1);
}
//--------------------------------------------------------------
void ofApp::setup(){
	//ofBackground(128, 128, 128);
	
	/*Transform a halfedge-based mesh into OfMesh*/
	//static float cylinder_r;
	float min_curvature = 1e10;
	float max_curvature = 0;
	list<float> curvatureList;

	for (auto face : obj_mesh.faces) {
		vector<Halfedge*> edges = face->Edges();
		assert(edges.size() >= 3);
		glm::vec3 v1 = glm::normalize(edges[0]->v->pos - edges[1]->v->pos);
		glm::vec3 v2 = glm::normalize(edges[2]->v->pos - edges[1]->v->pos);
		glm::vec3 normal = glm::cross(v1, v2);
		static float cylinder_r = glm::length(v1 - v2) / 2;

		vector<vector<Halfedge*>> triplets;
		if (edges.size() == 3) {  //for triangles
			triplets.push_back(vector<Halfedge*>{edges[0], edges[1], edges[2]});
			for (int i = 0; i < 3; i++) {
				Cylinder(edges[i]->v->pos, edges[(i + 1) % 3]->v->pos, cylinder_r, mesh_wire, glm::vec3(0, 0, 0), 3);
			}
		}
		else if (edges.size() == 4) {  //for quads
			triplets.push_back(vector<Halfedge*>{edges[0], edges[1], edges[2]});
			triplets.push_back(vector<Halfedge*>{edges[2], edges[3], edges[0]});
			for (int i = 0; i < 4; i++) {
				Cylinder(edges[i]->v->pos, edges[(i + 1) % 4]->v->pos, cylinder_r, mesh_wire, glm::vec3(0, 0, 0), 3);
			}
		}
		for (auto triplet : triplets) {
			for (int i = 0; i < 3; i++) {
				mesh.addVertex(triplet[i]->v->pos);
				mesh.addNormal(normal);


				//float curvature = triplet[i]->v->AngleDefect();
				//float curvature = triplet[i]->v->UniformMeanCurvature();
				float curvature = triplet[i]->v->CotMeanCurvature();
				if (curvature != -100) {
					if (abs(curvature) < 1e-5) {
						curvature = 0.f;
					}
					min_curvature = glm::min(min_curvature, curvature);
					max_curvature = glm::max(max_curvature, curvature);
				}
				else {
					//curvature = 0.f;
				}
				curvatureList.push_back(curvature);
			}
		}
	}

	/*Add colors*/
	float valueScale = max_curvature - min_curvature + 1e-5;
	for (float curvature : curvatureList) {
		float value = (curvature - min_curvature) / valueScale;
		mesh.addColor(num2color(value));
	}
	cout << min_curvature << endl;
	cout << max_curvature << endl;

	/*Post-precessing for better visualizatoin*/
	glm::vec3 mesh_center_pt;
	float max_value=0;
	glm::vec3 vert;

	// find the center of the mesh
	auto vert_ptr = mesh.getVerticesPointer();
	glm::vec3 sum_vert(0);
	for (int i = 0; i < mesh.getNumVertices(); i++) {
		vert = *(vert_ptr + i);

		max_value = glm::max(max_value, vert[0]);
		max_value = glm::max(max_value, vert[1]);
		max_value = glm::max(max_value, vert[2]);
		sum_vert += vert;
	}
	mesh_center_pt = sum_vert / mesh.getNumVertices();

	//move and rescale the mesh
	for (int i = 0; i < mesh.getNumVertices(); i++) {
		*(vert_ptr + i) = (*(vert_ptr + i) - mesh_center_pt) / max_value;
	}
	
	//move and rescale the mesh's wireframe
	vert_ptr = mesh_wire.getVerticesPointer();
	for (int i = 0; i < mesh_wire.getNumVertices(); i++) {
		*(vert_ptr + i) = (*(vert_ptr + i) - mesh_center_pt) / max_value;
	}

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
	normalMatrix = glm::inverseTranspose(glm::mat4(modelMatrix));

	// mesh
	shader.begin();
	ofEnableDepthTest();
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 0.7f);

	shader.setUniform3f("eyePos", eyePos);
	shader.setUniformMatrix4f("modelMatrix", modelMatrix);
	shader.setUniformMatrix4f("modelViewProjectionMatrix", modelViewProjectionMatrix);
	shader.setUniformMatrix4f("normalMatrix", normalMatrix);

	shader.setUniform4f("mat_ambient", glm::vec4(1, 1, 1, 0));
	shader.setUniform4f("mat_diffuse", glm::vec4(1, 0, 0, 1));
	shader.setUniform1f("k_shininess", 64.0);
	shader.setUniform4f("mat_specular", glm::vec4(0.5, 0.5, 0.5, 1));
	shader.setUniform4f("mat_emissive", glm::vec4(0, 0, 0, 0));

	shader.setUniform4f("light.position", lightPosition);
	shader.setUniform4f("light.ambient", glm::vec4(0, 0, 0, 1));
	shader.setUniform4f("light.diffuse", glm::vec4(1, 1, 1, 0));
	shader.setUniform4f("light.specular", glm::vec4(1, 1, 1, 0));

	mesh.draw();

	ofDisableDepthTest();
	glDisable(GL_POLYGON_OFFSET_FILL);
	shader.end();

	// wireframe
	shader.begin();
	ofEnableDepthTest();

	shader.setUniform3f("eyePos", eyePos);
	shader.setUniformMatrix4f("modelMatrix", modelMatrix);
	shader.setUniformMatrix4f("modelViewProjectionMatrix", modelViewProjectionMatrix);
	shader.setUniformMatrix4f("normalMatrix", normalMatrix);

	shader.setUniform4f("mat_ambient", glm::vec4(1, 1, 1, 0));
	shader.setUniform4f("mat_diffuse", glm::vec4(1, 0, 0, 1));
	shader.setUniform1f("k_shininess", 64.0);
	shader.setUniform4f("mat_specular", glm::vec4(1, 1, 1, 1));
	shader.setUniform4f("mat_emissive", glm::vec4(0, 0, 0, 0));

	shader.setUniform4f("light.position", -lightPosition);
	shader.setUniform4f("light.ambient", glm::vec4(0, 0, 0, 1));
	shader.setUniform4f("light.diffuse", glm::vec4(1, 1, 1, 0));
	shader.setUniform4f("light.specular", glm::vec4(1, 1, 1, 0));

	mesh_wire.draw();

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
