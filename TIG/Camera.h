#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include <vector>




// An camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{

public:
	Camera(GLFWwindow* window, glm::vec3* center, int width, int height);
	~Camera();
	
	void setCenter(glm::vec3* center);
	void setUp(glm::vec3* up);
	void setRadius(float radius);
	void setPhi(float phi);
	void setTheta(float theta);
	void setSensitiviy(float sensitivity);

	void setViewMatrix(glm::mat4 viewMat);

	glm::vec3* getCameraPos();

	void update(double deltaTime);
	glm::mat4* getViewMatrix();

private:
	GLFWwindow* m_window;

	glm::mat4 m_viewMatrix;

	glm::vec3 m_cameraPos;
	glm::vec3 m_center;
	glm::vec3 m_up;

	float m_theta;
	float m_phi;
	float m_radius;
	float m_sensitivity;
	float m_xOld;
	float m_yOld;

};
#endif

