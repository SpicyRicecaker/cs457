#pragma once
#include <cstdio>
#include <math.h>
#include <array>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

void PrintMat4(glm::mat4 mat);

void PrintVec3(glm::vec3 vec);

float* Array3(float a, float b, float c);

float* MulArray3(float factor, float array0[]);

float* MulArray3(float factor, float a, float b, float c);

std::array<float, 4> MulArray3(float c, std::array<float, 4> a);

// for lighting:
const float WHITE[4] = { 1., 1., 1., 1. };

void HsvRgb(float[3], float[3]);
void Cross(float[3], float[3], float[3]);
float Dot(float[3], float[3]);
float Unit(float[3], float[3]);
float Unit(float[3]);
void HsvRgb(float hsv[3], float rgb[3]);
float* cross(float A[3], float B[3]);

float radians_to_degrees(float r);

void DoRasterString(float, float, float, char*);
void DoStrokeString(float, float, float, float, char*);

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

extern const char* ColorNames[];

// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
{
	{ 1., 0., 0. },		// monokai_red
	{ 1., 1., 0. },		// purple
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE = GL_LINEAR;
const GLfloat FOGDENSITY = 0.30f;
const GLfloat FOGSTART = 1.5f;
const GLfloat FOGEND = 4.f;
