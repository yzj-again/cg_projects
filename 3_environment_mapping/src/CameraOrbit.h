#pragma once

#include "ofMain.h"

class CameraOrbit
{
public:

	////MODEL transform:

	////VIEW transform:

	//camera position by orbit model
	float m_azimuth;
	float m_zenith;
	float m_zenith_thresh = PI/2*99/100;
	float m_radius;
	float m_radius_thresh = 5;
	glm::vec3 m_center;

	////PROJECTION transform:

	//perspective projection parameters
	float m_aspect;
	float m_fovy;
	float m_znear, m_zfar;

	CameraOrbit()
	{
		Reset();
	}
	void Reset()
	{
		m_azimuth = PI;
		m_zenith = 0;
		m_radius = 5;
		m_center = glm::vec3(0, 0, 0);
		m_aspect = 1;
		m_fovy = glm::radians(45.0f);
		m_znear = 0.01;
		m_zfar = 1000;
	}

	glm::mat4 ModelMatrix();
	glm::mat4 ViewMatrix();
	glm::mat4 ProjectionMatrix();

	//rotate horizontally or vertically 
	void RotateHorizotal(float degree);
	void RotateVertical(float degree);

	void Zoom(float scale);  //zoom-in / zoom-out (change radius)

	glm::vec3 Pos();  //pos on orbit
};