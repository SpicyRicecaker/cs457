#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctype.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "utils.h"

#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))

inline void _DrawConeLatLng(int ilat, int ilng, int numlats, int numlngs, float radbot, float radtop, float height);
void OsuCone(float radBot, float radTop, float height, int slices, int stacks);
