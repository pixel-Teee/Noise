#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class UI
{
public:
	UI();
	UI(GLFWwindow* window);
	~UI();
	void SetWindow(GLFWwindow* window);
	void DrawUI();
	void BeginDraw();
	void EndDraw(GLFWwindow* window);
	bool ProcessInput();
};