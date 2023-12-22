#pragma once

#include "Walnut/Image.h"
#include "memory"
#include "glm/glm.hpp"
#include "Camera.h"
#include "Scene.h"

#include "Ray.h"

class Renderer {
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene,const Camera& camera, std::string& log);
	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; };
private:
	struct HitPayload {
		float HitDistance;
		glm::vec3 WorldNormal;
		glm::vec3 WorldPosition;
		
		int ObjectIndex;
		bool IsIntoTheObject;
	};

	glm::vec4 PerPixel(uint32_t x, uint32_t y, std::string& logStr); // RayGen shader
	HitPayload TraceRay(const Ray& ray, std::string& log);

	HitPayload ClosestHit(const Ray& ray, float hitDistance, bool isIntoTheObject, int objectIndex);
	HitPayload Miss(const Ray& ray);

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	uint32_t* m_ImageData = nullptr;
};

