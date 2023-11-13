#include "Renderer.h"
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
			m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	glm::vec3 rayDirection = glm::vec3(coord.x, coord.y, -1.0f);
	//rayDirection = glm::normalize(rayDirection);

	glm::vec3 rayOrigin = glm::vec3(0, 0, -1);
	float radius = 0.5f;

	// (Dx^2 + Dy^2 + Dz^2)t^2 + 2(OxDx + OyDy + OzDz)t + (Ox^2 + Oy^2 + Oz^2 - R^2) = 0
	// Where
	// R = Radius
	// O = Ray Origin
	// D = Ray Direction
	// t = Hit Distance

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayDirection, rayOrigin);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	//Quadratic formula discriminant
	float discriminant = b * b - 4.0f * a * c;

	if (discriminant >= 0.0f) {
		return 0xffAAFF00;
	}
	return 0xff000000;
	
}
