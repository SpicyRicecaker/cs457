#include "world.h"

Scene&
World::get_current_scene()
{
  return scenes[current_scene_idx];
}

// as defined in world.h
World world = World{ .MainWindow = NULL,
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
  bool valid =
    pattern.Create("../shaders/pattern.vert", "../shaders/pattern.frag");
  if (!valid)
    fprintf(stderr, "Could not create the Pattern shader!\n");
  else
    fprintf(stderr, "Pattern shader created!\n");

  // init keytimes
  {
    for (auto [t, v] : (float[][2]){
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

    for (auto [t, v] : (float[][2]){
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

    for (auto [t, v] : (float[][2]){
           { 0.0, 0.19 },
           { 0.2, 0.19 },
           { 0.4, 0.19 },
           { 0.6, 0.19 },
           { 0.8, 0.19 },
           { 1.0, 0.19 },
         }) {
      world.uTol.AddTimeValue(t, v);
    }

    for (auto [t, v] : (float[][2]){
           { 0.0, 1.00 },
           { 0.2, 2.00 },
           { 0.4, 3.00 },
           { 0.6, 2.00 },
           { 0.8, 1.00 },
           { 1.0, 1.00 },
         }) {
      world.uNoiseAmp.AddTimeValue(t, v);
    }

    for (auto [t, v] : (float[][2]){
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

  // set the uniform variables that will not change:

  pattern.Use();
  // step 5 of project 6
  pattern.SetUniformVariable("uKa", 0.1f);
  pattern.SetUniformVariable("uKd", 0.5f);
  pattern.SetUniformVariable("uKs", 0.4f);
  pattern.SetUniformVariable("uColor", 1.f, 0.5f, 0.f);
  pattern.SetUniformVariable("uSpecularColor", 1.f, 1.f, 1.f);
  pattern.SetUniformVariable("uShininess", 12.f);

  pattern.SetUniformVariable("uTime", world.Time);

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
