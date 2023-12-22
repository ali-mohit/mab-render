#pragma once


#include "vector"
#include "glm/glm.hpp"

struct MaterialDescription {
	glm::vec3 Albedo{ 1.0f, 1.0f, 1.0f };
	float Roughness = 1.0f;
	float Metalic = 0.0f;
};

struct Sphere {
	glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
	float Radius = 0.5f;

	int MaterialIndex = 0;
};

struct Scene {
	std::vector<Sphere> SphereList;
	std::vector<MaterialDescription> MaterialList;
};