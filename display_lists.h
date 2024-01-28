#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <vector>
#include <string>
#include <format>
#define _USE_MATH_DEFINES
#include <math.h>
#include <unistd.h>
#include <limits.h>

#include "camera.h"
#include "loadobjfile.h"

// add the carosel horse
#include "carousel_horse.h"
#include "osucone.h"
#include "osusphere.h"
#include "plane.h"
#include "osutorus.h"

#include "utils.h"
#include "bmp_to_texture.h"

///////////////////////////////////////   HANDY UTILITIES:
/////////////////////////////

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

// the stroke characters 'X' 'Y' 'Z' :
static float xx[] = { 0.f, 1.f, 0.f, 1.f };

static float xy[] = { -.5f, .5f, .5f, -.5f };

static int xorder[] = { 1, 2, -3, 4 };

static float yx[] = { 0.f, 0.f, -.5f, .5f };

static float yy[] = { 0.f, .6f, 1.f, 1.f };

static int yorder[] = { 1, 2, 3, -2, 4 };

static float zx[] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[] = { 1, 2, 3, 4, -5, 6 };

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// axes stuff?

const GLfloat AXES_WIDTH = 3.;

extern const Material Obsidian;
extern const Material ObsidianRed;
extern const Material ObsidianGreen;
extern const Material ObsidianBlue;

#define VENUS 0
#define EARTH 1
#define MARS 2
#define JUPITER 3
#define SATURN 4
#define URANUS 5
#define NEPTUNE 6

struct Planet
{
  GLuint dl;
  GLuint texture;
  std::string name;
  char key;
  float radius;
};

struct DisplayLists
{
  GLuint AxesList;
  GLuint CarouselCircleList;
  GLuint BackgroundCircleList;
  GLuint ColorCircleList;
  GLuint HorseList;
  GLuint BoatList;
  GLuint PlaneList;
  GLuint SphereList;
  // GLuint  Strawberry;
  // GLuint  Blueberry;
  // GLuint  Avocado;
  GLuint CylinderList;
  GLuint TorusList;
  std::vector<Planet> planets;
  static DisplayLists New(Camera cam);
};
#include "world.h"

void
draw_circle(float height, float radius, int steps, float color[3]);

void
Axes(float len);

void
color_circle(float radius, float x0, float y0, float r, float g, float b);

void
background_circle(float number, float dradius, Camera cam);

void
Display();
