#pragma once 
#include <glm/vec3.hpp>

class Light
{
public:
	Light(const glm::vec3& pos, const glm::vec3& col);
	glm::vec3 position;
	glm::vec3 color;
};
