#include "CameraOrbit.h"

extern int g_window_w;
extern int g_window_h;

glm::mat4 CameraOrbit::ModelMatrix()
{
	return glm::mat4();  //just return identify matrix
}

glm::mat4 CameraOrbit::ViewMatrix()
{
	return glm::lookAt(Pos(), m_center, glm::vec3(0, 1, 0));
}

glm::mat4 CameraOrbit::ProjectionMatrix()
{
	return glm::perspective(m_fovy, m_aspect, m_znear, m_zfar);
}

void CameraOrbit::RotateHorizotal(float degree)
{
	m_azimuth += degree;
}

void CameraOrbit::RotateVertical(float degree)
{
	m_zenith += degree;
	if (m_zenith > m_zenith_thresh)
		m_zenith = m_zenith_thresh;
	if (m_zenith < -m_zenith_thresh)
		m_zenith = -m_zenith_thresh;
}

void CameraOrbit::Zoom(float scale)
{
	m_radius += scale;

	if (m_radius < 0.01)
		m_radius = (float)0.01;

	if (m_radius > m_radius_thresh)
		m_radius = m_radius_thresh;
}

glm::vec3 CameraOrbit::Pos()
{
	//note: y-z axis flipped sphere coordinate
	glm::vec3 pos = m_center + m_radius * glm::vec3(cos(m_zenith) * cos(m_azimuth), sin(m_zenith), cos(m_zenith) * sin(m_azimuth));
	return pos;
}
