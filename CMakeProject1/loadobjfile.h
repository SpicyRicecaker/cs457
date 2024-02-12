#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <vector>
#include <math.h>
#include <ctype.h>

#include "utils.h"
// TODO why does displaylists have random constants i need to reference lol
#include "display_lists.h"

// delimiters for parsing the obj file:

#define OBJDELIMS " \t"


struct Vertex
{
	float x, y, z;
};


struct Normal
{
	float nx, ny, nz;
};


struct TextureCoord
{
	float s, t, p;
};


struct face
{
	int v, n, t;
};

int LoadObjFile(char* name);
char* ReadRestOfLine(FILE* fp);
void ReadObjVTN(char* str, int* v, int* t, int* n);

