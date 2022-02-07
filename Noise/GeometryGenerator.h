#pragma once

//Math
#include <glm/glm.hpp>

#include <vector>

#include "BaseType.h"
#include "Model.h"

class GeometryGeneratory
{
public:
	static std::vector<Model> BuildGeometry();
private:
	static MeshData CreatePolygon(float x, float y, float aveRadius, float irregularity, float spikeyness, uint32_t numVerts);
};