#pragma once

#include "world.h"
#include "utils.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <iostream>

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};


void	DoAxesMenu(int id);
void	DoColorMenu(int id);
void	DoDepthBufferMenu(int id);
void	DoDepthFightingMenu(int id);
void	DoDepthMenu(int id);
void	DoDebugMenu(int id);
void	DoMainMenu(int id);
void	DoSceneMenu(int id);
void	DoProjectMenu(int id);
