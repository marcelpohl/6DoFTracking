#include "Camera.h"

Camera::Camera(GLFWwindow* window, glm::vec3* center, int width, int height)
{
	m_window = window;

	m_center = *center;
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);

	m_theta = glm::pi<float>() / 2.0f;
	m_phi = 0.0f;
	m_radius = 2.0f;
	m_sensitivity = 0.01f;
	m_xOld = width/2.0f;
	m_yOld = height/2.0f;
	m_roll = 0.0f;

	m_viewMatrix = glm::lookAt(m_center + m_cameraPos, m_center, m_up);
}

Camera::~Camera()
{
}

void Camera::setCenter(glm::vec3* center)
{
	m_center = *center;
}

void Camera::setUp(glm::vec3* up)
{
	m_up = *up;
}

void Camera::setRadius(float radius)
{
	if (radius > 0.1f) {
		m_radius = radius;
	}
}

void Camera::setPhi(float phi)
{
	m_phi = phi;
}

void Camera::setTheta(float theta)
{
	m_theta = theta;
}

void Camera::setSensitiviy(float sensitivity)
{
	m_sensitivity = glm::abs(sensitivity);
}


void Camera::setRoll(float roll)
{
	m_roll = roll;
}

void Camera::setViewMatrix(glm::mat4 viewMat)
{
	m_viewMatrix = viewMat;
}

glm::vec3* Camera::getCameraPos()
{
	return &m_cameraPos;
}


void Camera::update(double deltaTime)
{
	double x, y;

	glfwGetCursorPos(m_window, &x, &y);
	if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		float changeX = ((float)x - m_xOld) * m_sensitivity;
		float changeY = ((float)y - m_yOld) * m_sensitivity;

		m_theta -= changeY;
		if (m_theta < 0.01f) m_theta = 0.01f;
		else if (m_theta > glm::pi<float>() - 0.01f) m_theta = glm::pi<float>() - 0.01f;

		m_phi -= changeX;
		if (m_phi < 0) m_phi += 2 * glm::pi<float>();
		else if (m_phi > 2 * glm::pi<float>()) m_phi -= 2 * glm::pi<float>();
	}

	m_xOld = (float)x;
	m_yOld = (float)y;

	if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
		m_radius -= 5.0f * (float)deltaTime;
	if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
		m_radius += 5.0f * (float)deltaTime;
	if (m_radius < 0.1f) m_radius = 0.1f;

	// TODO: roll?
	if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
		m_roll += 2.0f * (float)deltaTime;
	if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		m_roll -= 2.0f * (float)deltaTime;

	m_cameraPos.x = m_center.x + m_radius * sin(m_theta) * sin(m_phi);
	m_cameraPos.y = m_center.y + m_radius * cos(m_theta);
	m_cameraPos.z = m_center.z + m_radius * sin(m_theta) * cos(m_phi);

	m_viewMatrix = glm::rotate(glm::lookAt(m_cameraPos, m_center, m_up), m_roll, m_cameraPos - m_center);
}

glm::mat4* Camera::getViewMatrix()
{
	return &m_viewMatrix;
}

