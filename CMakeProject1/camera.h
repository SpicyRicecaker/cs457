#pragma once

#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

struct Camera
{
	glm::vec3 eye; // position of the current camera
	glm::vec3 center; // direction where the camera is pointing to
	glm::vec3 up; // camera normal
	glm::vec3 right;
};

