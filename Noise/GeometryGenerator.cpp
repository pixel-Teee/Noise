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
	//���ȷֲ�irregularity��spikeyness
	static std::uniform_real_distribution<float> uniform2(0.2f, 0.8f);
	//��[4, 10)֮�����ɰ뾶
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

//[x, y]�����������
//aveRadius��ÿ�����㵽Բ�ĵ�ƽ���뾶
//irregularity��ÿ������ļ����
//spikeyness��ÿ�����㵽Բ�ĵİ뾶�Ŷ���
//numVerts����������
MeshData GeometryGeneratory::CreatePolygon(float center_x, float center_y, float aveRadius, float irregularity, float spikeyness, uint32_t numVerts)
{
	static std::default_random_engine e;

	//��˹�ֲ�
	static std::normal_distribution<float> gauss;

	//���ȷֲ�
	static std::uniform_real_distribution<float> uniform;

	//���ÿ�����������
	MeshData meshData;

	meshData.Vertices.resize(static_cast<size_t>(numVerts + 1));
	meshData.Indices.resize(static_cast<size_t>(numVerts * 3));

	//ӳ�䵽[0, 2PI]
	irregularity = clip(irregularity, 0.0f, 1.0f) * 2.0f * PI / numVerts;

	//ӳ�䵽[0, aveRadius]
	spikeyness = clip(spikeyness, 0.0f, 1.0f) * aveRadius;

	//��������½��Ŷ�
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

		//��������
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
