#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "BaseType.h"

class Model
{
public:
	Model(const MeshData& meshData);

	void Draw();
private:
	MeshData m_meshData;
	uint32_t vao;
	uint32_t vbo;
	uint32_t ebo;
};
