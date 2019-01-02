#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string.h>
#include <random>
#include <chrono>

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>


#include "Phong.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Mesh.h"
#include "FBO.h"
#include "ShaderPostProcessing.h"

#include "Texture.h"

extern const std::string RESOURCES_PATH = "E:\\programming\\TIG\\TIG\\assets\\";
extern const std::string SHADERS_PATH = "E:\\programming\\TIG\\TIG\\shader\\";
extern const std::string DATA_PATH = "E:\\programming\\Sun3DDataset\\SUNRGBD\\";

const int WIDTH = 640;
const int HEIGHT = 480;

struct Pose {
	float phi;
	float theta;
	float roll;
	float radius;
};

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

Camera* camera = nullptr;
bool firstMouse = true;
float lastX = WIDTH / 2;
float lastY = HEIGHT / 2;


void saveImageToFile(FBO* fbo);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void handleInput(GLFWwindow* window, FBO* fbo);


int main()
{
	// initialize and configure glfw
	//------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



	// glad: load all OpenGL function pointers
	//----------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}



	// initialize camera
	//----------------------------------------
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::mt19937 rng(seed);
	std::uniform_real_distribution<float> distributionFullCircle(0, 2 * glm::pi<float>());
	std::uniform_real_distribution<float> distributionHalfCircle(0, glm::pi<float>());
	std::uniform_real_distribution<float> distributionRadius(1.0f, 3.0f);

	Pose observedPose;
	observedPose.phi = distributionFullCircle(rng);
	observedPose.theta = distributionHalfCircle(rng);
	observedPose.roll = distributionFullCircle(rng);
	observedPose.radius = distributionRadius(rng);



	camera = new Camera(window, &glm::vec3(0.0f, 0.0f, 0.0f), WIDTH, HEIGHT);
	camera->setPhi(observedPose.phi);
	camera->setTheta(observedPose.theta);
	camera->setRadius(observedPose.radius);



	// load shaders and create shader program
	//---------------------------------------

	Phong phongShader((SHADERS_PATH + "simple.vert").c_str(), (SHADERS_PATH + "simple.frag").c_str());
	ShaderPostProcessing depthBlendShader((SHADERS_PATH + "depth.vert").c_str(), (SHADERS_PATH + "depth.frag").c_str());
	ShaderPostProcessing postProcessShader((SHADERS_PATH + "postProcess.vert").c_str(), (SHADERS_PATH + "postProcess.frag").c_str());
	ShaderPostProcessing sfqShader((SHADERS_PATH + "textureToQuad.vert").c_str(), (SHADERS_PATH + "textureToQuad.frag").c_str());



	// define lights
	//-------------------------
	Light* light1 = new Light(glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
	phongShader.addLight(light1);
	Light* light2 = new Light(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
	phongShader.addLight(light2);
	Light* light3 = new Light(glm::vec3(0.0f, -0.5f, -1.0f), glm::vec3(0.3f, 0.3f, 0.8f), glm::vec3(0.6f, 0.6f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
	phongShader.addLight(light3);



	// define Material
	//-------------------------
	Material* material = new Material(glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f);
	material->setTexture((RESOURCES_PATH + "container2.png").c_str(), MapType::Diffuse);
	material->setTexture((RESOURCES_PATH + "container2_specular.png").c_str(), MapType::Specular);
	phongShader.setMaterial(material);



	// load objects
	//-------------------------
	Mesh* object = new Mesh((RESOURCES_PATH + "box.obj").c_str(), material);
	Mesh* scene = new Mesh((RESOURCES_PATH + "scene.obj").c_str(), material);



	// load input images
	//-------------------------
	Texture colorTex((DATA_PATH + "kv1\\b3dodata\\img_0063\\fullres\\img_0063.jpg").c_str(), true);
	Texture depthTex((DATA_PATH + "kv1\\b3dodata\\img_0063\\fullres\\img_0063_abs.png").c_str(), true);



	// gaussian noise texture
	//-------------------------
	std::vector<float> gaussNoiseData;
	std::uniform_real_distribution<float> sigmaDistr(0.0f, 2.0f);
	float mean = 0.0f;
	//float sigma = sigmaDistr(rng);
	float sigma = 10.0f;

	long size = colorTex.getWidth() * colorTex.getWidth();
	std::normal_distribution<float> normalDist(mean, sigma);
	for (int i = 0; i < size; i++) {
		float val = (normalDist(rng) / 255) + 0.5f;
		gaussNoiseData.push_back(val);
	}
	Texture* gaussNoiseTexture = new Texture(colorTex.getWidth(), colorTex.getHeight(), TextureType::Grey, gaussNoiseData);



	// FBO for post processing
	//------------------------
	FBO *fbo = new FBO(depthTex.getWidth(), depthTex.getHeight());
	fbo->attachColorTexture(1);
	fbo->attachStencilTexture();

	FBO *fbo2 = new FBO(depthTex.getWidth(), depthTex.getHeight());
	fbo2->attachColorTexture(1);

	depthBlendShader.use();
	depthBlendShader.addTexture(colorTex.getId(), "backgroundColorTexture");
	depthBlendShader.addTexture(depthTex.getId(), "backgroundDepthTexture");

	postProcessShader.use();
	postProcessShader.addTexture(fbo->getColorTexture(0), "screenTexture");
	postProcessShader.addTexture(gaussNoiseTexture->getId(), "noiseTexture");
	postProcessShader.set1i("radius", 1);
	postProcessShader.set1i("width", depthTex.getWidth());
	postProcessShader.set1i("height", depthTex.getHeight());



	sfqShader.use();
	sfqShader.addTexture(fbo2->getColorTexture(0), "screenTexture");
	//sfqShader.addTexture(fbo->getStencilTexture(), "screenTexture");
	//sfqShader.addTexture(gaussNoiseTexture->getId(), "screenTexture");



	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	lastFrame = (float)glfwGetTime();



	// main render loop
	//-----------------
	while (!glfwWindowShouldClose(window)) {
		
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		handleInput(window, fbo2);


		// render scene into FBO
		fbo->bind();

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glm::mat4 model(1.0f);
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));

		camera->update(deltaTime);
		glm::mat4* view = camera->getViewMatrix();

		glm::mat4 projection(1.0f);
		projection = glm::perspective(glm::radians(60.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 10.0f);

		phongShader.use();
		phongShader.setMat4f("model", model);
		phongShader.setMat4f("view", *view);
		phongShader.setMat4f("projection", projection);
		phongShader.set3f("camPos", *camera->getCameraPos());

		object->Draw(&phongShader);


		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);

		depthBlendShader.use();

		depthBlendShader.set1f("near", 0.1f);
		depthBlendShader.set1f("far", 10.0f);
		depthBlendShader.update();
		depthBlendShader.draw();


		fbo2->bind();

		postProcessShader.use();
		postProcessShader.update();
		postProcessShader.draw();

		fbo2->unbind();


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sfqShader.use();
		sfqShader.update();
		sfqShader.draw();


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// cleanup
	//--------
	delete material;
	delete light1;
	delete light2;
	delete light3;
	delete object;
	delete fbo;

	glfwTerminate();
	return 0;
}


void saveImageToFile(FBO* fbo)
{
	unsigned long imageSize = WIDTH * HEIGHT * 3;
	unsigned char *data = new unsigned char[imageSize];
	unsigned char *depthData = new unsigned char[imageSize];

	fbo->bind();
	glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, data);
	glReadPixels(0, 0, WIDTH, HEIGHT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthData);
	fbo->unbind();

	stbi_flip_vertically_on_write(1);
	stbi_write_tga("E:\\programming\\TIG\\test_color.tga", WIDTH, HEIGHT, 3, data);
	stbi_write_tga("E:\\programming\\TIG\\test_depth.tga", WIDTH, HEIGHT, 1, depthData);

	delete data;
	delete depthData;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void handleInput(GLFWwindow* window, FBO* fbo)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		saveImageToFile(fbo);
}