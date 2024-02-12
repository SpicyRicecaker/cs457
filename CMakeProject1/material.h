#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <math.h>
#include "utils.h"
#include <array>

struct Material {
    std::array<float, 4> ambient;
    std::array<float, 4> diffuse;
    std::array<float, 4> specular;
    float shininess;

	void SetMaterial() const;
};

extern const Material Obsidian;
