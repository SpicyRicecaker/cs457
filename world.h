#pragma once
#include "scene.h"

#include <vector>

#include "display_lists.h"
#include "light.h"
#include "menu.h"
#include "glslprogram.h"
#include "pika.h"

// Scenes
const int CAMERA_SCENE = 0;
const int WORLD_SCENE = 1;

// for animation:
const int MS_PER_CYCLE = 4000; // 10000 milliseconds = 10 seconds

// which projection:
enum Projections
{
  ORTHO,
  PERSP
};

const float STAR_HEIGHT = 60.;
const float STAR_END_HEIGHT = 0.2;
const float STAR_HORIZONTAL_Z_DIST = -10.;

struct World
{
  int MainWindow; // window id for main graphics window
  int current_scene_idx;
  std::vector<Scene> scenes;
  int world_time;
  int last_time_stamp;
  float Time; // used for animation, this has a value between 0. and 1.
  int Ticks;
  bool SceneNeedsUpdate;
  int Xmouse;
  int Ymouse; // mouse values
  int ScreenWidth;
  int ScreenHeight;
  bool Frozen;
  int ActiveButton; // current button that is down
  int DebugOn;      // != 0 means to print debugging info
  bool ShiftHeld;
  bool AltHeld;
  std::vector<Light> lights;
  DisplayLists display_lists;
  std::vector<std::array<float, 4>> colors;
  GLSLProgram pattern;
  Pika pika;
  PikaColors pikacolors;
  Keytimes eyes_y_offset;
  Keytimes eyes_y_radius;
  Keytimes eyes_y_radius_crying;

  Keytimes uS;
  Keytimes uT;

  Scene& get_current_scene();
  std::array<float, 4>& get_color(Color c);
  void Init();
};

extern World world;
