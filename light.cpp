#include "light.h"

void
draw_light()
{
  Scene& scene = world.get_current_scene();
  Camera& cam = scene.camera;
  glShadeModel(GL_FLAT);
  glEnable(GL_LIGHTING);

  for (auto& l : world.lights) {
    // draw sphere at that position
    std::array<float, 4>& l_color = world.get_color(l.color);

    glm::mat4 model = glm::mat4(1.);

    // 2. rotate
    model = glm::rotate(model, glm::radians(360 * world.Time), glm::vec3(0., 1., 0.));

    // 1. move slightly off center
    model = glm::translate(model, glm::vec3(10., 0., 0.));


    glLoadMatrixf(&(scene.view_matrix * model)[0][0]);

    switch (l.light_type) {
      case LightType::Pointlight: {
        glLightfv(l.ilight,
                  GL_POSITION,
                  Array3(l.position[0], l.position[1], l.position[2]));
        glLightf(l.ilight, GL_SPOT_CUTOFF, 180.f);
        glLightfv(l.ilight, GL_AMBIENT, MulArray3(0.1f, 1.f, 1.f, 1.f));
        glLightfv(l.ilight, GL_DIFFUSE, MulArray3(1.f, 1.0f, 1.0f, 1.0f));
        glLightfv(l.ilight, GL_SPECULAR, MulArray3(1.0f, 1.f, 1.f, 1.f));
        glLightf(l.ilight, GL_CONSTANT_ATTENUATION, 1.);
        glLightf(l.ilight, GL_LINEAR_ATTENUATION, 0.);
        glLightf(l.ilight, GL_QUADRATIC_ATTENUATION, l.quad_attenuation);
        glEnable(l.ilight);
        break;
      }
      case LightType::Spotlight: {
        glLightfv(l.ilight, GL_POSITION, l.position.data());
        glLightfv(l.ilight, GL_SPOT_DIRECTION, (*l.direction).data());
        glLightf(l.ilight, GL_SPOT_EXPONENT, 1.f);
        glLightf(l.ilight, GL_SPOT_CUTOFF, 45.f);
        glLightfv(l.ilight, GL_AMBIENT, Array3(0., 0., 0.));
        glLightfv(l.ilight, GL_DIFFUSE, l_color.data());
        glLightfv(l.ilight, GL_SPECULAR, l_color.data());
        glLightf(l.ilight, GL_CONSTANT_ATTENUATION, 1.);
        glLightf(l.ilight, GL_LINEAR_ATTENUATION, 0.);
        glLightf(l.ilight, GL_QUADRATIC_ATTENUATION, 0.);
        glEnable(l.ilight);
        break;
      }
    }
  }
  glLoadMatrixf(&(scene.view_matrix)[0][0]);
}

void
Light::init()
{
  // create the keyframes for stuff
  pos_y.Init();
  pos_y.AddTimeValue(0.0, 6.);
  pos_y.AddTimeValue(1.0, 0.);
}
