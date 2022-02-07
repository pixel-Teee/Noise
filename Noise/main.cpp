#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

#include <iostream>
#include <random>
#include <string>
#include <sstream>

#include "Shader.h"
#include "Camera.h"
#include "UI.h"
#include "BaseType.h"
#include "GeometryGenerator.h"

using namespace cv;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void SetMaterial(const Shader& shader, int32_t index, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float shine);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 100.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//UI
UI ui;

int main()
{
	/*----------------------------
	---------Init Glfw------------
	------------------------------*/
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Noise", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	/*----------------------------
	---------Init Glfw------------
	------------------------------*/
	Shader ourShader("shaders/vertex.txt", "shaders/fragment.txt");

	GeometryGeneratory geo;
	std::vector<Model> models = geo.BuildGeometry();

	std::default_random_engine e;
	std::uniform_real_distribution<float> u(0.0f, 1.0f);
	std::uniform_int_distribution<int> u2(0, 2);
	//random every noise's pos
	std::vector<glm::mat4> worldPos;
	std::vector<int32_t> MatIndex;
	for (int32_t x = 0; x < 100; ++x)
	{
		for (int32_t y = 0; y < 10; ++y)
		{
			worldPos.push_back(glm::translate(glm::mat4(1.0f), glm::vec3((x + u(e)) - 50.0f, (y + u(e)) * 10 - 50.0f, 0.0f)));
			MatIndex.push_back(u2(e));
		}
	}

	ourShader.use();
	//brass material index
	glm::vec4 mat_ambient = { 0.329412f, 0.223529f, 0.027451f,1.0f };
	glm::vec4 mat_diffuse = { 0.780392f, 0.568627f, 0.113725f, 1.0f };
	glm::vec4 mat_specular = { 0.992157f, 0.941176f, 0.807843f, 1.0f };
	float shine = 27.8974f;
	SetMaterial(ourShader, 0, mat_ambient, mat_diffuse, mat_specular, shine);
	//bronze
	mat_ambient = { 0.2125f, 0.1275f, 0.054f, 1.0f };
	mat_diffuse = { 0.714f, 0.4284f, 0.18144f, 1.0f };
	mat_specular = { 0.393548f, 0.271906f, 0.166721f, 1.0f };
	shine = 25.6f;
	SetMaterial(ourShader, 1, mat_ambient, mat_diffuse, mat_specular, shine);
	//chrome
	mat_ambient = { 0.25f, 0.25f, 0.25f, 1.0f };
	mat_diffuse = { 0.4f, 0.4f, 0.4f, 1.0f };
	mat_specular = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
	shine = 76.8f;
	SetMaterial(ourShader, 2, mat_ambient, mat_diffuse, mat_specular, shine);
	
	/*
	struct Light
	{
		vec3 position;

		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
	};
	*/

	Light light;
	ui.SetWindow(window);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		ourShader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		ourShader.setVec3("viewPos", camera.Position);

		//light
		ourShader.setVec3("light.position", light.position);
		ourShader.setVec3("light.ambient", light.ambient);
		ourShader.setVec3("light.diffuse", light.diffuse);
		ourShader.setVec3("light.specular", light.specular);

		
		//render noise
		for (int32_t i = 0; i < 1000; ++i)
		{
			ourShader.setMat4("model", worldPos[i]);
			ourShader.setInt("materialIndex", MatIndex[i]);
			models[i].Draw();
		}		

		ui.BeginDraw();
		ImGui::Begin("Light Properties");
		ImGui::DragFloat3("LightPosition", glm::value_ptr(light.position), 20.0f, -200.0f, 200.0f);
		ImGui::ColorEdit3("LightAmbient", glm::value_ptr(light.ambient));
		ImGui::ColorEdit3("LightDiffuse", glm::value_ptr(light.diffuse));
		ImGui::ColorEdit3("LightSpecular", glm::value_ptr(light.specular));

		if (ImGui::Button("Capture"))
		{
			unsigned char* imageData;
			imageData = new unsigned char[SCR_WIDTH * SCR_HEIGHT * 4];

			glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_BGRA, GL_UNSIGNED_BYTE, imageData);

			Mat mat(SCR_HEIGHT, SCR_WIDTH, CV_8UC4, imageData);
			blur(mat, mat, Size(5, 5));
			imshow("test", mat);
		}
		ImGui::End();
		//ImGui::ShowDemoWindow();
		ui.EndDraw(window);
		
		glfwSwapBuffers(window);
		glfwPollEvents();

		Sleep(50.0f);
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (!ui.ProcessInput())
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (!ui.ProcessInput())
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!ui.ProcessInput())
	{
		camera.ProcessMouseScroll(static_cast<float>(yoffset));
	}
}

void SetMaterial(const Shader& shader, int32_t index, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float shine)
{
	std::stringstream ss;
	ss << index;
	std::string id;
	ss >> id;

	std::string materialName = "materials[" + id + "]";

	std::string m_ambient = materialName + ".ambient";
	std::string m_diffuse = materialName + ".diffuse";
	std::string m_specular = materialName + ".specular";
	std::string m_shine = materialName + ".shininess";

	shader.setVec4(m_ambient, ambient);
	shader.setVec4(m_diffuse, diffuse);
	shader.setVec4(m_specular, specular);
	shader.setFloat(m_shine, shine);
}

