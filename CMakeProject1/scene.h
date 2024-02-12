#pragma once

#include "camera.h"

#include <string>

#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "keytime.h"

struct Scene
{
  // MutableGlobalVariables:
  int AxesOn;          // != 0 means to draw the axes
  int DepthCueOn;      // != 0 means to use intensity depth cueing
  int DepthBufferOn;   // != 0 means to use the z-buffer
  int DepthFightingOn; // != 0 means to force the creation of z-fighting
  int NowColor;        // index into Colors[ ]
  int NowProjection;   // ORTHO or PERSP
  float Scale;         // scaling factor
  int ShadowsOn;       // != 0 means to turn shadows on
  float XRot, YRot;    // rotation angles in degrees
  float XTrans, YTrans, ZTrans;
  std::string label;
  Camera camera;
  // think it goes 3d -> 2d here
  glm::mat4 projection_matrix;
  glm::mat4 view_matrix;
	bool texture_mode;
	bool lighting_mode;
  bool update_time;
  bool update_keytimes;
  bool crying;
	int current_planet;
  int lighting_texture_combo_mode;
};
