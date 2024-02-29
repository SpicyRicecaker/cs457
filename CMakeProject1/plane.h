#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <string>
#include <format>

#include "bmp_to_texture.h"
#include "material.h"

GLuint make_grid(float width, int steps, float y_0);
