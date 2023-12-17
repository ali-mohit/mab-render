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

}


void Renderer::Render(const Scene& scene, const Camera& camera, std::string& logStr)
{

	Ray ray;
	ray.Origin = camera.GetPosition();
	// render every pixel
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			
			ray.Direction = camera.GetRayDirections()[y * m_FinalImage->GetWidth() + x];
			
			glm::vec4 color = RayTrace(scene, ray, logStr);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utilities::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::RayTrace(const Scene& scene, const Ray& ray, std::string& logStr)
{
	//logStr = std::to_string(t_closest);

	glm::vec3 sphereOrigin = glm::vec3(0, 0, 0);
	// (Dx^2 + Dy^2 + Dz^2)t^2 + 2(OxDx + OyDy + OzDz)t + (Ox^2 + Oy^2 + Oz^2 - R^2) = 0
	// Where
	// R = Radius
	// So = Center of the spehere
	// O = Ray Origin
	// D = Ray Direction
	// t = Hit Distance

	if(scene.SphereList.size() == 0)
		return glm::vec4(0, 0, 0, 1);

	const Sphere* closestSphere = nullptr;
	float hitDistance = std::numeric_limits<float>::max();
	bool isIntoTheObject = false;

	for (const Sphere& sphere : scene.SphereList) {
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
		if (t_target < 0) {
			t_target = (-b + glm::sqrt(discriminant)) / 2.0f * a;

			if (t_target < 0) {
				continue;
			}
			isInsideObject = true;
		}

		if (t_target < hitDistance) {
			hitDistance = t_target;
			closestSphere = &sphere;
			isIntoTheObject = isInsideObject;
		}
	}
	
	if (closestSphere == nullptr) {
		return glm::vec4(0, 0, 0, 1);
	}
	
	glm::vec3 camOrigin = ray.Origin - closestSphere->Position;
	glm::vec3 hitPoint = camOrigin + ray.Direction * hitDistance;

	if (isIntoTheObject) {
		return glm::vec4(-hitPoint, 1);
	}
	
	glm::vec3 normalRay = glm::normalize((hitPoint - sphereOrigin));

	glm::vec3 rayLightDirection = glm::normalize(glm::vec3(-1, -1, -1));

	float cosAngle = glm::max(glm::dot(normalRay, -rayLightDirection), 0.0f);
	
	glm::vec3 sphereColor = closestSphere->Albedo;
	sphereColor *= cosAngle;

	return glm::vec4(sphereColor, 1.0f);
	
}
