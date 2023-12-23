#include <iostream>
#include "Renderer.h"
#include "Utilities.cpp"
#include "Walnut/Random.h"


void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (this->m_FinalImage) {

		// No Resize necessary
		if (width == m_FinalImage->GetWidth() && height == m_FinalImage->GetHeight())
			return;
		m_FinalImage->Resize(width, height);

	}
	else {
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}
	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];

	ResetFrameIndex();

}


void Renderer::Render(const Scene& scene, const Camera& camera, std::string& logStr)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;
	
	if (m_FramIndex == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetHeight() * m_FinalImage->GetWidth() * sizeof(glm::vec4));

	// render every pixel
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			glm::vec4 newColor = PerPixel(x, y, logStr);
			
			glm::vec4 oldAvgColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];

			oldAvgColor *= (float)(m_FramIndex - 1) / (float)(m_FramIndex);
			newColor /= m_FramIndex;
			glm::vec4 avgColor = newColor + oldAvgColor;
			m_AccumulationData[x + y * m_FinalImage->GetWidth()] = avgColor;

			avgColor = glm::clamp(avgColor, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utilities::ConvertToRGBA(avgColor);
		}
	}

	m_FinalImage->SetData(m_ImageData);

	if (m_Settings.Accumulate)
		m_FramIndex++;
	else
		m_FramIndex = 1;
	
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y, std::string& logStr)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[y * m_FinalImage->GetWidth() + x];


	uint32_t bounces = 5;
	glm::vec3 finalColor(0.0f);
	float multiplier = 1.0f;

	for (uint32_t i = 0; i < bounces; i++) {
		Renderer::HitPayload payload = TraceRay(ray, logStr);

		// Check if miss happened
		if (payload.HitDistance < 0.0f) {
			glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
			finalColor += skyColor * multiplier;

			break;
		}

		if (payload.IsIntoTheObject) {
			finalColor = glm::vec4(-payload.WorldPosition, 1);
			break;
		}

		glm::vec3 rayLightDirection = glm::normalize(glm::vec3(-1, -1, -1));
		float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -rayLightDirection), 0.0f);

		const Sphere& sphere = m_ActiveScene->SphereList[payload.ObjectIndex];
		const MaterialDescription& material = m_ActiveScene->MaterialList[sphere.MaterialIndex];

		glm::vec3 sphereColor = material.Albedo;
		sphereColor *= lightIntensity;

		finalColor += sphereColor * multiplier;

		multiplier *= 0.5f;

		ray.Origin = payload.WorldPosition + (payload.WorldNormal * 0.0001f);
		ray.Direction = glm::reflect(
			ray.Direction,
			payload.WorldNormal + material.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f)
		);
	}

	
	return glm::vec4(finalColor, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray, std::string& logStr)
{
	//logStr = std::to_string(t_closest);

	// (Dx^2 + Dy^2 + Dz^2)t^2 + 2(OxDx + OyDy + OzDz)t + (Ox^2 + Oy^2 + Oz^2 - R^2) = 0
	// Where
	// R = Radius
	// So = Center of the spehere
	// O = Ray Origin
	// D = Ray Direction
	// t = Hit Distance

	float hitDistance = std::numeric_limits<float>::max();
	bool isIntoTheObject = false;

	int hitObjectIndex = -1;
	for (size_t i = 0; i < m_ActiveScene->SphereList.size(); i++) {
		const Sphere& sphere = m_ActiveScene->SphereList[i];
		float radius = sphere.Radius;

		glm::vec3 camOrigin = ray.Origin - sphere.Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(camOrigin, ray.Direction);
		float c = glm::dot(camOrigin, camOrigin) - radius * radius;

		//Quadratic formula discriminant
		float discriminant = b * b - 4.0f * a * c;

		if (discriminant < 0.0f) {
			continue;
		}

		// hitPoints = (-b +- sqrt(discriminant))/2a
		bool isInsideObject = false;
		float t_target = (-b - glm::sqrt(discriminant)) / 2.0f * a;
		/*if (t_target < 0.0f) {
			t_target = (-b + glm::sqrt(discriminant)) / 2.0f * a;

			if (t_target < 0) {
				continue;
			}
			isInsideObject = true;
		}*/

		if (t_target > 0 && t_target < hitDistance) {
			hitDistance = t_target;
			hitObjectIndex = (int)i;
			isIntoTheObject = isInsideObject;
		}
	}
	
	if (hitObjectIndex < 0) {
		return Miss(ray);
	}
	
	return ClosestHit(ray, hitDistance, isIntoTheObject, hitObjectIndex);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, bool isIntoTheObject, int objectIndex)
{
	glm::vec3 sphereOrigin = glm::vec3(0, 0, 0);

	Renderer::HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;
	payload.IsIntoTheObject = isIntoTheObject;

	const Sphere& closestSphere = m_ActiveScene->SphereList[objectIndex];

	glm::vec3 camOrigin = ray.Origin - closestSphere.Position;
	payload.WorldPosition = camOrigin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize((payload.WorldPosition - sphereOrigin));

	payload.WorldPosition += closestSphere.Position;

	return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload = HitPayload();

	payload.HitDistance = -1.0f;
	return payload;
}


