#pragma once

#include <optional>
#include <array>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "utils.h"

#include "keytime.h"

enum LightType {
	Spotlight,
	Pointlight
};

enum Color {
	White,
	Red,
	Green,
	Blue,
	Yellow
};

struct Light
{
	LightType light_type;
	Color color;
	std::array<float, 4> position;
	int ilight;
	std::optional<std::array<float, 3>> direction;
	float quad_attenuation;
	Keytimes pos_y;

	void init();
};

#include "world.h"

void draw_light();

