#include "world.h"

Scene&
World::get_current_scene()
{
  return scenes[current_scene_idx];
}

// as defined in world.h
World world = World{
  .MainWindow = NULL,
  .current_scene_idx = NULL,
  .scenes = {},
  .world_time = 0,
  .last_time_stamp = 0,
  .Time = 0.,
  .Ticks = 0,
  .SceneNeedsUpdate = false,
  .Xmouse = NULL,
  .Ymouse = NULL,
  .ScreenWidth = NULL,
  .ScreenHeight = NULL,
  .Frozen = false,
  .ActiveButton = NULL,
  .DebugOn = false,
  .ShiftHeld = false,
  .AltHeld = false,
  .lights = { Light{
    .light_type = LightType::Pointlight,
    .color = Color::White,
    .position = { 0., 13., 0., 1. },
    .ilight = GL_LIGHT0,
    .direction = { { 0., -1., 0. } },
    .quad_attenuation = 0.,
  } },
  .colors = { { 1., 1., 1., 1. },
              { 1., 0., 0., 1. },
              { 0., 1., 0., 1. },
              { 1., 1., 0., 1. },
              { 0., 0., 1., 1. } },
  .useXYZ = false,
  .useBumpMapping = true
};

void
World::Init()
{
  scenes.push_back(Scene{
    .AxesOn = 0,
    .DepthCueOn = 0,
    .DepthBufferOn = 1,
    .DepthFightingOn = 0,
    .NowColor = YELLOW,
    .NowProjection = PERSP,
    .Scale = 1.0,
    .ShadowsOn = 0,
    .XRot = 0.,
    .YRot = 0.,
    .XTrans = 0.,
    .YTrans = 0.,
    .ZTrans = 0.,
    .label = std::string("camera scene"),
    .camera =
      Camera{ .eye = glm::vec3(0., .6, 0.),
              .center = glm::vec3(0., 0., 0.),
              // how do we ensure that the normal is updated accordingly?
              .up = glm::vec3(0., 1., 0.),
              .right = glm::vec3(1., 0., 0.) },
    .projection_matrix = glm::mat4(1.),
    .view_matrix = glm::mat4(1.),
    .texture_mode = true,
    .lighting_mode = false,
    .current_planet = MARS,
  });

  scenes.push_back(Scene{ .AxesOn = 0,
                          .DepthCueOn = 0,
                          .DepthBufferOn = 1,
                          .DepthFightingOn = 0,
                          .NowColor = YELLOW,
                          .NowProjection = PERSP,
                          .Scale = 1.0,
                          .ShadowsOn = 0,
                          .XRot = 0.,
                          .YRot = 0.,
                          .XTrans = 0.,
                          .YTrans = 0.,
                          .ZTrans = 0.,
                          .label = std::string("overall scene"),
                          .camera = Camera{ .eye = glm::vec3(0., 2., 4.),
                                            .center = glm::vec3(0., 0., 0.),
                                            .up = glm::vec3(0., 1., 0.),
                                            .right = glm::vec3(1., 0., 0.) },
                          .projection_matrix = glm::mat4(1.),
                          .view_matrix = glm::mat4(1.),
                          .texture_mode = false,
                          .lighting_mode = false,
                          .update_time = true,
                          .update_keytimes = true,
                          .crying = false,
                          .current_planet = EARTH,
                          .lighting_texture_combo_mode = 0 });

  for (auto& l : world.lights) {
    // initialize the lights
    l.init();
  }

  world.current_scene_idx = WORLD_SCENE;
  // all other setups go here, such as GLSLProgram and KeyTime setups:

  pattern.Init();
  std::string vert = std::format("{}/shaders/pattern.vert", DIR_PREFIX);
  std::string frag = std::format("{}/shaders/pattern.frag", DIR_PREFIX);
  bool valid = pattern.Create((char*) vert.c_str(), (char*) frag.c_str());
  if (!valid)
    fprintf(stderr, "Could not create the Pattern shader!\n");
  else
    fprintf(stderr, "Pattern shader created!\n");

  // set the uniform variables that will not change:
  init_keytimes();

  pattern.Use();
  // step 5 of project 6
  pattern.SetUniformVariable((char*)"uKa", 0.1f);
  pattern.SetUniformVariable((char*)"uKd", 0.5f);
  pattern.SetUniformVariable((char*)"uKs", 0.4f);
  pattern.SetUniformVariable((char*)"uColor", 1.f, 0.5f, 0.f);
  pattern.SetUniformVariable((char*)"uSpecularColor", 1.f, 1.f, 1.f);
  pattern.SetUniformVariable((char*)"uShininess", 12.f);

  pattern.SetUniformVariable((char*)"uTime", world.Time);

  world.pikacolors = PikaColors{
    // dummy alpha value to avoid padding issues in shader
    .mouthred = glm::vec4(232. / 255., 123. / 255., 124. / 255., 1.),
    .yellow = glm::vec4(251. / 255., 212. / 255., 89. / 255., 1.),
    .pouchred = glm::vec4(225. / 255., 91. / 255., 66. / 255., 1.)
  };

  world.pattern.SetUniformBufferObject(world.pikacolors);

  pattern.UnUse();

  DisplayLists display_lists =
    DisplayLists::New(world.get_current_scene().camera);
  world.display_lists = display_lists;
}

void
init_freq_keytimes()
{
  // init keytimes
  for (auto& [t, v] : std::vector<std::array<float, 2>> {
         { 0.0, 0.1 },
         { 0.2, 0.1 },
         { 0.4, 0.1 },
         { 0.6, 0.1 },
         { 0.8, 0.1 },
         { 1.0, 0.1 },
         //  { 0., 0. },
         //  { 0.25, .0000 },
         //  { 0.5, .0050 },
         //  { 0.75, .0500 },
         //  { 1., .0000 },
       }) {
    world.uAd.AddTimeValue(t, v);
  }

  for (auto& [t, v] : std::vector<std::array<float, 2>>{
         { 0.0, 0.1 },
         { 0.2, 0.1 },
         { 0.4, 0.1 },
         { 0.6, 0.1 },
         { 0.8, 0.1 },
         { 1.0, 0.1 },
         //  { 0., 0. },
         //  { 0.8, .302 },
         //  { 0.9, .402 },
         //  { 1., .503 },
       }) {
    world.uBd.AddTimeValue(t, v);
  }

  for (auto& [t, v] : std::vector<std::array<float, 2>>{
         { 0.0, 0.19 },
         { 0.2, 0.19 },
         { 0.4, 0.19 },
         { 0.6, 0.19 },
         { 0.8, 0.19 },
         { 1.0, 0.19 },
       }) {
    world.uTol.AddTimeValue(t, v);
  }

  for (auto& [t, v] : std::vector<std::array<float, 2>>{
         { 0.0, 0.00 },
         { 0.2, 1.00 },
         { 0.4, 7.00 },
         { 0.6, 1.00 },
         { 0.8, 0.00 },
         { 1.0, 0.00 },
       }) {
    world.uNoiseAmp.AddTimeValue(t, v);
  }

  for (auto& [t, v] : std::vector<std::array<float,2>>{
         { 0.0, 1.00 },
         { 0.2, 2.00 },
         { 0.4, 3.00 },
         { 0.6, 2.00 },
         { 0.8, 1.00 },
         { 1.0, 1.00 },
       }) {
    world.uNoiseFreq.AddTimeValue(t, v);
  }
}

void
init_puddle_keytimes()
{
  float uA_final = .4;
  for (auto& [t, v] : std::vector<std::array<float,2>>{
         { 0.0, 1.0 },
         { 0.2, 2.0 },
         { 0.4, 3.0 },
         { 0.6, 2.0 },
         { 0.8, 1.0 },
         { 1.0, 1.0 },
       }) {
    world.uA.AddTimeValue(t, v);
  }
  float uB_final = (2.f * M_PI);
  for (auto& [t, v] : std::vector<std::array<float,2>>{
         { 0.0f, uB_final },
         { 0.2f, uB_final + .5f * (float)M_PI },
         { 0.4f, uB_final + 1.f * (float)M_PI },
         { 0.6f, uB_final + .5f * (float)M_PI},
         { 0.8f, uB_final },
         { 1.0f, uB_final },
       }) {
    world.uB.AddTimeValue(t, v);
  }
  for (auto& [t, v] : std::vector<std::array<float,2>>{
         { 0.0, 0.00 },
         { 0.2, 1.00 },
         { 0.4, 2.00 },
         { 0.6, 1.00 },
         { 0.8, 0.00 },
         { 1.0, 0.00 },
       }) {
    world.uC.AddTimeValue(t, v);
  }
  float uD_final = 4.;
  for (auto& [t, v] : std::vector<std::array<float,2>>{
         { 0.0, uD_final },
         { 0.2, uD_final },
         { 0.4, uD_final },
         { 0.6, uD_final },
         { 0.8, uD_final },
         { 1.0, uD_final },
       }) {
    world.uD.AddTimeValue(t, v);
  }
}

void
init_keytimes()
{
  init_freq_keytimes();
  init_puddle_keytimes();
}

std::array<float, 4>&
World::get_color(Color c)
{
  switch (c) {
    case Color::White: {
      return colors[0];
    }
    case Color::Red: {
      return colors[1];
    }
    case Color::Green: {
      return colors[2];
    }
    case Color::Yellow: {
      return colors[3];
    }
    case Color::Blue: {
      return colors[4];
    }
    default: {
      printf("VITAL ERROR\n");
      return colors[0];
    }
  }
}
