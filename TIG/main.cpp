#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string.h>
#include <random>
#include <chrono>

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Phong.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Mesh.h"
#include "FBO.h"
#include "ShaderPostProcessing.h"

#include "Texture.h"

namespace fs = std::experimental::filesystem::v1;

extern const std::string RESOURCES_PATH = "E:\\programming\\TIG\\TIG\\assets\\";
extern const std::string SHADERS_PATH = "E:\\programming\\TIG\\TIG\\shader\\";
extern const std::string DATA_PATH = "E:\\programming\\Sun3DDataset\\SUNRGBD\\kv1\\NYUdata\\";
extern const std::string EXPORT_PATH = "E:\\programming\\TIG\\export\\";

const int WIDTH = 640;
const int HEIGHT = 480;

const float ZNEAR = 0.1f;
const float ZFAR = 10.0f;

Material* material1 = nullptr;
Material* material2 = nullptr;

Mesh* object1;

Phong* phongShader = nullptr;
ShaderPostProcessing* depthBlendShader = nullptr;
ShaderPostProcessing* postProcessShader = nullptr;
//ShaderPostProcessing sfqShader;

void generateImages(GLFWwindow* window, std::string sources, std::string dest, std::string filename);

void saveImageToFile(FBO* fbo, std::string filename, int width, int height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void handleInput(GLFWwindow* window);


int main()
{
	//cv::Mat image;
	//image = cv::imread(DATA_PATH + +"kv1\\b3dodata\\img_0063\\fullres\\img_0063_abs.png", cv::IMREAD_ANYDEPTH);
	//auto nrChannels = image.channels();
	//auto width = image.cols;
	//auto height = image.rows;

	//if (!image.data) {
	//	return 0;
	//}

	//cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE); // Create a window for display.
	//cv::imshow("Display window", image); // Show our image inside it.

	//cv::waitKey(0); // Wait for a keystroke in the window
	//return 0;


	// initialize and configure glfw
	//------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

	
	// load shaders and create shader program
	//---------------------------------------
	phongShader = new Phong((SHADERS_PATH + "simple.vert").c_str(), (SHADERS_PATH + "simple.frag").c_str());
	depthBlendShader = new ShaderPostProcessing((SHADERS_PATH + "depth.vert").c_str(), (SHADERS_PATH + "depth.frag").c_str());
	postProcessShader = new ShaderPostProcessing((SHADERS_PATH + "postProcess.vert").c_str(), (SHADERS_PATH + "postProcess.frag").c_str());
	//sfqShader = ShaderPostProcessing((SHADERS_PATH + "textureToQuad.vert").c_str(), (SHADERS_PATH + "textureToQuad.frag").c_str());


	// define Material
	//-------------------------
	material1 = new Material(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 32.0f);
	material1->setTexture((RESOURCES_PATH + "export3dcoat_lambert3SG_color.png").c_str(), MapType::Diffuse);
	material1->setTexture((RESOURCES_PATH + "export3dcoat_lambert3SG_metalness.png").c_str(), MapType::Specular);
	material1->setTexture((RESOURCES_PATH + "materialball_ao.png").c_str(), MapType::AmbientOcclusion);
	


	// load objects
	//-------------------------
	object1 = new Mesh((RESOURCES_PATH + "export3dcoat.obj").c_str(), material1);
	//Mesh* scene = new Mesh((RESOURCES_PATH + "scene.obj").c_str(), material);

	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


	int images = 0;
	for (auto& dir : fs::directory_iterator(DATA_PATH)) {
		auto path = dir.path();
		if (path.filename().string().substr(0, 1) == ".") {
			continue;
		}
		std::cout << "Processing Image " << path.filename() << std::endl;
		generateImages(window, path.string(), EXPORT_PATH, path.filename().string());
		images++;
		if (images >= 10) {
			break;
		}
	}

	
	// cleanup
	//--------
	delete material1;
	//delete material2;
	delete object1;

	delete phongShader;
	delete depthBlendShader;
	delete postProcessShader;

	glfwTerminate();
	return 0;
}


void generateImages(GLFWwindow* window, std::string sources, std::string dest, std::string filename)
{
	// load input images
	//-------------------------
	Texture colorTex((sources + "\\fullres\\" + filename + ".jpg").c_str(), true);
	Texture depthTex((sources + "\\fullres\\" + filename + ".png").c_str(), true);

	int imgWidth = colorTex.getWidth();
	int imgHeight = colorTex.getHeight();


	// initialize camera
	//----------------------------------------
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::mt19937 rng(seed);
	std::uniform_real_distribution<float> distributionFullCircle(0, 2 * glm::pi<float>());
	std::uniform_real_distribution<float> distributionHalfCircle(0, glm::pi<float>());
	std::uniform_real_distribution<float> distributionRadius(2.0f, 3.0f);

	float phi = distributionFullCircle(rng);
	float theta = distributionHalfCircle(rng);
	float roll = distributionFullCircle(rng);
	float radius = distributionRadius(rng);

	Camera* camera = new Camera(window, &glm::vec3(0.0f, 0.0f, 0.0f), imgWidth, imgHeight);
	camera->setPhi(phi);
	camera->setTheta(theta);
	camera->setRadius(radius);
	camera->setRoll(roll);

	camera->update(0);
	auto observedPose = camera->getViewMatrix();

	std::uniform_real_distribution<float> distributionTranslation(-0.02f, 0.02f);
	std::uniform_real_distribution<float> distributionRotation(-10.0f, 10.0f);
	glm::vec3 translation(distributionTranslation(rng), distributionTranslation(rng), distributionTranslation(rng));
	glm::vec3 rotation(glm::radians(distributionRotation(rng)), glm::radians(distributionRotation(rng)), glm::radians(distributionRotation(rng)));

	auto predictedPose = glm::translate(
		glm::rotate(
			glm::rotate(
				glm::rotate(*observedPose, -rotation.z, glm::vec3(1.0f, 0.0f, 0.0f)),
				-rotation.y,
				glm::vec3(0.0f, 1.0f, 0.0f)),
			-rotation.x,
			glm::vec3(0.0f, 0.0f, 1.0f)
		),
		-translation
	);


	// define light sources
	//-------------------------
	phi = distributionFullCircle(rng);
	theta = distributionHalfCircle(rng);
	if (theta < 0.01f) theta = 0.01f;
	else if (theta > glm::pi<float>() - 0.01f) theta = glm::pi<float>() - 0.01f;

	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
	lightPos.x = glm::sin(theta) * glm::sin(phi);
	lightPos.y = glm::cos(theta);
	lightPos.z = glm::sin(theta) * glm::cos(phi);
	Light* lightObs = new Light(lightPos, glm::vec3(0.65f, 0.65f, 0.65f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.26f, 0.26f, 0.26f));

	auto predLightPos = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f) * predictedPose;
	Light* lightPred = new Light(glm::normalize(glm::vec3(predLightPos.x, predLightPos.y, predLightPos.z)), glm::vec3(0.65f, 0.65f, 0.65f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.26f, 0.26f, 0.26f));


	// gaussian noise texture
	//-------------------------
	std::vector<float> gaussNoiseData;
	std::uniform_real_distribution<float> sigmaDistr(0.0f, 2.0f);
	float mean = 0.0f;
	float sigma = sigmaDistr(rng);
	//float sigma = 2.0f;

	long size = imgWidth * imgHeight;
	std::normal_distribution<float> normalDist(mean, sigma);
	for (int i = 0; i < size; i++) {
		float val = (normalDist(rng) / 255) + 0.5f;
		gaussNoiseData.push_back(val);
	}
	Texture* gaussNoiseTexture = new Texture(imgWidth, imgHeight, TextureType::Grey, gaussNoiseData);


	// additional post processing parameters
	//--------------------------
	std::uniform_real_distribution<float> colShiftDistr(-0.05f, 0.05f);
	float colorShift = colShiftDistr(rng);
	int blurRadius = 1;


	// FBO for post processing
	//------------------------
	FBO *fbo = new FBO(imgWidth, imgHeight);
	fbo->attachRGBDTexture();
	fbo->attachStencilTexture();

	FBO *fbo2 = new FBO(imgWidth, imgHeight);
	fbo2->attachRGBDTexture();

	depthBlendShader->use();
	depthBlendShader->addTexture(colorTex.getId(), "backgroundColorTexture");
	depthBlendShader->addTexture(depthTex.getId(), "backgroundDepthTexture");

	postProcessShader->use();
	postProcessShader->addTexture(fbo->getColorTexture(0), "screenTexture");
	postProcessShader->addTexture(gaussNoiseTexture->getId(), "noiseTexture");
	postProcessShader->set1i("radius", blurRadius);
	postProcessShader->set1i("width", imgWidth);
	postProcessShader->set1i("height", imgHeight);
	postProcessShader->set1f("colorShift", colorShift);

	//sfqShader.use();
	//sfqShader.addTexture(fbo2->getColorTexture(0), "screenTexture");
	//sfqShader.addTexture(fbo->getStencilTexture(), "screenTexture");
	//sfqShader.addTexture(gaussNoiseTexture->getId(), "screenTexture");


	// build projection matrix from intrinsics
	//-----------------
	std::string input;
	std::ifstream file;
	std::vector<std::string> intrinsics;

	file.open(sources + "\\fullres\\intrinsics.txt");
	while (file >> input) {
		intrinsics.push_back(input);
	}
	file.close();

	glm::mat4 projection(1.0f);

	projection[0][0] = 2.0f * std::stof(intrinsics[0]) / imgWidth;
	projection[0][1] = 0.0f;
	projection[0][2] = 0.0f;
	projection[0][3] = 0.0f;

	projection[1][0] = 0.0f;
	projection[1][1] = -2.0f * std::stof(intrinsics[4]) / imgWidth;
	projection[1][2] = 0.0f;
	projection[1][3] = 0.0f;

	projection[2][0] = 1.0f - 2.0f * std::stof(intrinsics[2]) / imgWidth;
	projection[2][1] = 2.0 * std::stof(intrinsics[5]) / imgHeight - 1.0;
	projection[2][2] = (ZFAR + ZNEAR) / (ZNEAR - ZFAR);
	projection[2][3] = -1.0;

	projection[3][0] = 0.0f;
	projection[3][1] = 0.0f;
	projection[3][2] = 2.0 * ZFAR * ZNEAR / (ZNEAR - ZFAR);
	projection[3][3] = 0.0f;

	//projection = glm::perspective(glm::radians(60.0f), (float)imgWidth / (float)imgHeight, 0.1f, 10.0f);


	//----------------------------------------
	// render observed Pose image
	//----------------------------------------
	fbo->bind();

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glm::mat4 model(1.0f);
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

	phongShader->use();	

	phongShader->clearLights();
	phongShader->addLight(lightObs);

	phongShader->setMaterial(material1);
	phongShader->setMat4f("model", model);
	phongShader->setMat4f("view", *observedPose);
	phongShader->setMat4f("projection", projection);
	phongShader->set3f("camPos", *camera->getCameraPos());
	phongShader->set1f("near", ZNEAR);
	phongShader->set1f("far", ZFAR);

	object1->Draw(phongShader);


	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	depthBlendShader->use();
	depthBlendShader->update();
	depthBlendShader->draw();


	fbo2->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	postProcessShader->use();
	postProcessShader->update();
	postProcessShader->draw();

	fbo2->unbind();

	saveImageToFile(fbo2, dest + filename + "_observed", imgWidth, imgHeight);



	//----------------------------------------
	// render predicted Pose image
	//----------------------------------------
	fbo->bind();

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	camera->update(0);

	phongShader->use();

	phongShader->clearLights();
	phongShader->addLight(lightPred);

	phongShader->setMaterial(material1);
	phongShader->setMat4f("model", model);
	phongShader->setMat4f("view", predictedPose);
	phongShader->setMat4f("projection", projection);
	phongShader->set3f("camPos", *camera->getCameraPos());
	phongShader->set1f("near", ZNEAR);
	phongShader->set1f("far", ZFAR);

	object1->Draw(phongShader);
	
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);


	fbo2->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	postProcessShader->use();
	postProcessShader->update();
	postProcessShader->draw();

	fbo2->unbind();

	saveImageToFile(fbo2, dest + filename + "_predicted", imgWidth, imgHeight);


	delete fbo;
	delete fbo2;
	delete lightObs;
	delete lightPred;
}


void saveImageToFile(FBO* fbo, std::string filename, int width, int height)
{
	unsigned long imageSize = width * height* 4;
	unsigned char *data = new unsigned char[imageSize];

	cv::Mat temp(height, width, CV_8UC4);
	cv::Mat img(height, width, CV_8UC4);


	fbo->bind();
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, temp.data);
	fbo->unbind();

	cv::flip(temp, img, 0);
	
	//cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
	//cv::imshow("Display window", img);
	//cv::waitKey(0);

	std::string path = filename + ".tga";
	stbi_flip_vertically_on_write(1);
	stbi_write_tga(path.c_str(), width, height, 4, data);

	delete data;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void handleInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}