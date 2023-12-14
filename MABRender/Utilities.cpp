#include "Walnut/Image.h"
#include "glm/glm.hpp"


namespace Utilities {
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		return (uint32_t)((a << 24) | (b << 16) | (g << 8) | r);
	}
}