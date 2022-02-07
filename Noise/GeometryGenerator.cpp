#include "GeometryGenerator.h"

#include <random>

#include "Model.h"

#define PI 3.1415926535f

float clip(float x, float min, float max)
{
	if(min > max) return x;
	else if(x < min) return min;
	else if(x > max) return max;
	else return x;
}

std::vector<Model> GeometryGeneratory::BuildGeometry()
{
	//GeometryGeneratory geoGen;

	float xSteps = 100.0f / 100.0f;
	float ySteps = 100.0f / 10.0f;

	static std::default_random_engine e;
	//均匀分布irregularity和spikeyness
	static std::uniform_real_distribution<float> uniform2(0.2f, 0.8f);
	//在[4, 10)之间生成半径
	static std::uniform_int_distribution<int> uniform(4, 10);

	std::vector<Model> models;

	for (float x = -50.0f; x <= 49.0f; x += xSteps)
	{
		for (float y = -50.0f; y <= 49.0f; y += ySteps)
		{
			int numVertex = uniform(e);
			MeshData meshData = CreatePolygon(0.0f, 0.0f, 1.0f, uniform2(e), uniform2(e), numVertex);

			models.emplace_back(std::move(meshData));
		}
	}
	return models;
}

//[x, y]：多边形中心
//aveRadius：每个顶点到圆心的平均半径
//irregularity：每个顶点的间隔度
//spikeyness：每个顶点到圆心的半径扰动度
//numVerts：顶点数量
MeshData GeometryGeneratory::CreatePolygon(float center_x, float center_y, float aveRadius, float irregularity, float spikeyness, uint32_t numVerts)
{
	static std::default_random_engine e;

	//高斯分布
	static std::normal_distribution<float> gauss;

	//均匀分布
	static std::uniform_real_distribution<float> uniform;

	//存放每个顶点的数据
	MeshData meshData;

	meshData.Vertices.resize(static_cast<size_t>(numVerts + 1));
	meshData.Indices.resize(static_cast<size_t>(numVerts * 3));

	//映射到[0, 2PI]
	irregularity = clip(irregularity, 0.0f, 1.0f) * 2.0f * PI / numVerts;

	//映射到[0, aveRadius]
	spikeyness = clip(spikeyness, 0.0f, 1.0f) * aveRadius;

	//间隔的上下界扰动
	std::vector<float> angleSteps;
	float lower = (2 * PI / numVerts) - irregularity;
	float upper = (2 * PI / numVerts) + irregularity;
	uniform.param(std::uniform_real<float>::param_type(lower, upper));

	float sum = 0;
	for (uint32_t i = 0; i < numVerts; ++i)
	{
		float tmp = uniform(e);
		angleSteps.push_back(tmp);
		sum = sum + tmp;
	}

	float k = sum / (2 * PI);
	for(uint32_t i = 0; i < numVerts; ++i)
		angleSteps[i] = angleSteps[i] / k;

	uniform.param(std::uniform_real<float>::param_type(0.0f, 2 * PI));
	gauss.param(std::normal_distribution<float>::param_type(aveRadius, spikeyness));

	float angle = uniform(e);

	for (uint32_t i = 0; i < numVerts; ++i)
	{
		float r_i = clip(gauss(e), 0.0f, 2 * aveRadius);
		float x = center_x + r_i * cos(angle);
		float y = center_y + r_i * sin(angle);
		meshData.Vertices[i].Position = glm::vec3(x, y, 0.0f);

		glm::vec3 Normal = glm::vec3(x, y, -1.0f);
		meshData.Vertices[i].Normal = Normal;

		//纹理坐标
		x += 0.5f;
		y += 0.5f;
		meshData.Vertices[i].TexCoord = glm::vec2(x, y);

		angle = angle + angleSteps[i];
	}

	Vertex tmpVertex;
	tmpVertex.Position = glm::vec3(0.0f, 0.0f, -5.0f);
	tmpVertex.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
	tmpVertex.TexCoord = glm::vec2(0.5f, 0.5f);

	for (uint32_t i = 0, k = 0; i < meshData.Indices.size(); i += 3, ++k)
	{
		meshData.Indices[i] = numVerts;
		meshData.Indices[static_cast<size_t>(i + 1)] = k + 1;
		meshData.Indices[static_cast<size_t>(i + 2)] = k;
	}

	meshData.Indices[meshData.Indices.size() - 2] = 0;

	return meshData;
}
