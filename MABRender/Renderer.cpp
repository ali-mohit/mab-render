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


void Renderer::Render()
{
	// render every pixel
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			glm::vec2 coord = { (float)x/(float)m_FinalImage->GetWidth() , (float)y / (float)m_FinalImage->GetHeight()};
			coord = coord * 2.0f - 1.0f;

			glm::vec4 color = PerPixel(coord);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utilities::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
	glm::vec3 rayOrigin = glm::vec3(0, 0, 1);
	glm::vec3 rayDirection = glm::vec3(coord.x, coord.y, -1.0f);
	rayDirection = glm::normalize(rayDirection);

	float radius = 0.5f;
	glm::vec3 sphereOrigin = glm::vec3(0, 0, 0);
	// (Dx^2 + Dy^2 + Dz^2)t^2 + 2(OxDx + OyDy + OzDz)t + (Ox^2 + Oy^2 + Oz^2 - R^2) = 0
	// Where
	// R = Radius
	// So = Center of the spehere
	// O = Ray Origin
	// D = Ray Direction
	// t = Hit Distance

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayDirection, rayOrigin);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	//Quadratic formula discriminant
	float discriminant = b * b - 4.0f * a * c;

	if (discriminant < 0.0f) {
		return glm::vec4(0, 0, 0, 1);
	}
	
	// hitPoints = (-b +- sqrt(discriminant))/2a
	float t_closest = (-b - glm::sqrt(discriminant)) / 2.0f * a;
	//float t_farest = (-b + glm::sqrt(discriminant)) / 2.0f * a;

	glm::vec3 hitPoint = rayOrigin + rayDirection * t_closest;
	glm::vec3 normalRay = glm::normalize((hitPoint - sphereOrigin));

	glm::vec3 rayLightDirection = glm::normalize(glm::vec3(-1, -1, -1));

	float cosAngle = glm::max(glm::dot(normalRay, -rayLightDirection), 0.0f);

	glm::vec3 sphereColor = glm::vec3(1.0f,0.0f,1.0f);
	sphereColor *= cosAngle;
	return glm::vec4(sphereColor, 1.0f);
	
}
