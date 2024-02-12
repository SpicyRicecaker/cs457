#include "display_lists.h"

const Material Obsidian = {
  // ambient
  { 0., 0., 0., 1. },
  // diffuse
  { 0.2262, 0.2272, 0.6584, 1. },
  // specular
  { 0., 0., 0., 1. },
  // shininess
  0.
};

const Material ObsidianRed = {
  // ambient
  { 0., 0., 0., 1. },
  // diffuse
  { 0.5503, 0.0742, 0.1376, 1. },
  // specular
  // { 0.332741 + .4 , 0.328634 + .4, 0.346435 + .4, 1.	},
  { 1., 1., 1., 1. },
  // shininess
  0.7
};

const Material ObsidianGreen = {
  // ambient
  { 0., 0., 0., 1. },
  // diffuse
  { 0.0043, 0.3912, 0.0366, 1. },
  // specular
  { .6, .6, .6, 1. },
  // shininess
  0.2
};

// this obsidian is going to be more dull
const Material ObsidianBlue = {
  // ambient
  { 0., 0., 0., 1. },
  // diffuse
  { 0.2262, 0.2272, 0.6584, 1. },
  // specular
  { 0., 0., 0., 1. },
  // shininess
  0.
};

// line width for the axes:

DisplayLists
DisplayLists::New(Camera cam)
{
  if (world.DebugOn != 0)
    fprintf(stderr, "Starting InitLists.\n");

  glutSetWindow(world.MainWindow);

  GLuint CarouselCircleList = glGenLists(1);
  glNewList(CarouselCircleList, GL_COMPILE);
  //  float monokai_red[3] = { 1, 0.38, 0.533 };
  float white[3] = { 1., 1., 1. };
  draw_circle(0., 2., 60, white);
  glEndList();

  GLuint BackgroundCircleList = glGenLists(1);

  glNewList(BackgroundCircleList, GL_COMPILE);
  for (int i = 0; i < 5; i++) {
    background_circle(i, 15., cam);
  }
  glEndList();

  std::vector<float*> colors;
  colors.push_back(new float[3]{ 0.6192, 0.8817, 0.5049 });
  colors.push_back(new float[3]{ 0.5365, 0.7942, 0.4226 });
  colors.push_back(new float[3]{ 0.4654, 0.7189, 0.3513 });
  colors.push_back(new float[3]{ 0.3816, 0.6299, 0.2658 });
  colors.push_back(new float[3]{ 0.3186, 0.5631, 0.1994 });

  GLuint ColorCircleList = glGenLists(1);
  glNewList(ColorCircleList, GL_COMPILE);
  for (int i = 0; i < 5; i++) {
    color_circle(
      10., -50., -60. + i * 30., colors[i][0], colors[i][1], colors[i][2]);
  }
  glEndList();

  GLuint HorseList = glGenLists(1);
  glNewList(HorseList, GL_COMPILE);
  glPushMatrix();
  glRotatef(90.f, 0., 1., 0.);
  glTranslatef(0., -1.1f, 0.f);
  glBegin(GL_TRIANGLES);
  for (int i = 0; i < HORSEntris; i++) {
    struct point p0 = HORSEpoints[HORSEtris[i].p0];
    struct point p1 = HORSEpoints[HORSEtris[i].p1];
    struct point p2 = HORSEpoints[HORSEtris[i].p2];

    // fake "lighting" from above:

    float p01[3] = {}, p02[3] = {}, n[3] = {};
    p01[0] = p1.x - p0.x;
    p01[1] = p1.y - p0.y;
    p01[2] = p1.z - p0.z;
    p02[0] = p2.x - p0.x;
    p02[1] = p2.y - p0.y;
    p02[2] = p2.z - p0.z;
    Cross(p01, p02, n);
    Unit(n, n);
    n[1] = (float)fabs(n[1]);
    // simulating a glColor3f( 1., 1., 0. ) = purple:
    // float purple[3] = { 0.05375 + .5, 0.05, 0.06625 + .5 };
    float purple[3] = { 1., 1., 1. };
    glColor3f(purple[0] * n[1], purple[1] * n[1], purple[2] * n[1]);
    glNormal3f(n[0], n[1], n[2]);

    glVertex3f(p0.x, p0.y, p0.z);
    glVertex3f(p1.x, p1.y, p1.z);
    glVertex3f(p2.x, p2.y, p2.z);
  }
  glEnd();
  glPopMatrix();
  glEndList();

  GLuint CylinderList = glGenLists(1);
  glNewList(CylinderList, GL_COMPILE);
  OsuCone(1., 1., 100, 100, 100);
  glEndList();

  GLuint TorusList = glGenLists(1);
  glNewList(TorusList, GL_COMPILE);
  OsuTorus(1., 1.2, 100, 100);
  glEndList();

  GLuint SphereList = glGenLists(1);
  glNewList(SphereList, GL_COMPILE);
  // radius, slices, stacks
  OsuSphere(1., 300, 300);
  glEndList();

  GLuint PlaneList = make_grid(1., 1000, 0.);

  // create the axes:
  GLuint AxesList = glGenLists(1);
  glNewList(AxesList, GL_COMPILE);
  glLineWidth(AXES_WIDTH);
  Axes(1.5);
  glLineWidth(1.);
  glEndList();

  std::vector<Planet> planets = {
    Planet{ 0, 0, "noise3d.064.tex", 'e', 1.0 },
    Planet{ 0, 0, "noise3d.064.tex", 'm', 0.5 },
  };

  for (auto& planet : planets) {
    int nums, numt, nump;
    // gets current working directory on unix
    // {
    //   char cwd[PATH_MAX];
    //   if (getcwd(cwd, sizeof(cwd)) != NULL) {
    //     std::cout << "Current working directory: " << cwd << std::endl;
    //   } else {
    //     perror("getcwd() error");
    //   }
    // }
    // std::cout << "adding planet " << std::format("./textures/{}.bmp",
    // planet.name)
    //           << std::endl;
    std::string file_name = std::format("../textures/{}", planet.name);
    const char* file = file_name.c_str();

    unsigned char* texture = ReadTexture3D(file, &nums, &numt, &nump);
    if (texture == NULL) {
      fprintf(stderr, "Cannot open texture '%s'\n", file);
    } else {
      fprintf(stderr, "Opened '%s'", file);
    }
    // create an id for texture
    glGenTextures(1, &planet.texture);
    // bind to gpu
    glBindTexture(GL_TEXTURE_3D, planet.texture);
    // ?
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // behavior when s, t not in [0, 1]
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    // sampling behavior, interpolated or nearest, or something else?
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D,
                 0,
                 GL_RGBA,
                 nums,
                 numt,
                 nump,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 texture);

    // create sphere w/ texture & radius, and add to surface

    planet.dl = glGenLists(1);
    // we will just have to scale later using the radius
    glNewList(planet.dl, GL_COMPILE);
    // MarsTex must have already been created when
    // this is called glPushMatrix();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_3D, planet.texture);
    // a dl can call another dl that has been previously created
    glCallList(SphereList);
    glEndList();
  }

  return DisplayLists{ .AxesList = AxesList,
                       .CarouselCircleList = CarouselCircleList,
                       .BackgroundCircleList = BackgroundCircleList,
                       .ColorCircleList = ColorCircleList,
                       // ConeList,
                       .HorseList = HorseList,
                       // .BoatList = BoatList,
                       .PlaneList = PlaneList,
                       .SphereList = SphereList,
                       // .Strawberry = Strawberry,
                       // .Blueberry = Blueberry,
                       // .Avocado = Avocado,
                       // GridDL,
                       .CylinderList = CylinderList,
                       .TorusList = TorusList,
                       .planets = planets };
}

void
draw_circle(float height, float radius, int steps, float color[3])
{
  // generate a new 600 points, using a triangle strip.
  // alternate points between the two vectors
  glBegin(GL_LINE_STRIP);

  glColor3fv(color);
  float degrees_diff = 2. * M_PI / steps;

  glNormal3f(0., 1., 0.);

  for (int i = 0; i <= steps; i++) {
    float x = radius * cos((i * degrees_diff));
    float y = radius * sin((i * degrees_diff));

    glVertex3f(x, height, y);
  }

  glEnd();
}

// draws a background circle
void
background_circle(float number, float dradius, Camera cam)
{
  std::cout << "setting up circle" << std::endl;
  float radius = 45. + number * dradius;
  int steps = 60.;
  float dtheta = 2. * M_PI / steps;
  float back = -100.;

  std::vector<float*> b;

  float left = 0.1;
  float right = 0.2;
  float range = right - left;
  int segments = 5;
  for (int i = 0; i < segments; i++) {
    float a = right - (range) * (static_cast<float>(i) / segments);
    b.push_back(new float[3]{ a, a, a });
  }

  glBegin(GL_TRIANGLE_FAN);
  glNormal3f(0, 0., 1.);

  int idx = number;

  glColor3f(b[idx][0], b[idx][1], b[idx][2]);

  glVertex3f(0., 0. + cam.eye.y, back + -0.1 * idx);

  for (int i = 0; i <= steps; i++) {
    float x = radius * cos(i * dtheta);
    float y = radius * sin(i * dtheta);

    glVertex3f(x, y + cam.eye.y, back + -0.1 * idx);
  }
  glEnd();
}

void
color_circle(float radius, float x0, float y0, float r, float g, float b)
{
  int steps = 60.;
  float dtheta = 2. * M_PI / steps;
  float back = -100.;

  float left = 0.1;
  float right = 0.2;
  float range = right - left;

  glBegin(GL_TRIANGLE_FAN);
  glNormal3f(0, 0., 1.);

  glColor3f(r, g, b);
  glVertex3f(x0, y0, 0.);
  for (int i = 0; i <= steps; i++) {
    float x = radius * cos(i * dtheta);
    float y = radius * sin(i * dtheta);

    glVertex3f(x0 + x, y0 + y, 0.);
  }
  glEnd();
}

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes(float len)
{
  // printf("hello world\n");
  glBegin(GL_LINE_STRIP);
  glVertex3f(len, 0., 0.);
  glVertex3f(0., 0., 0.);
  glVertex3f(0., len, 0.);
  glEnd();
  glBegin(GL_LINE_STRIP);
  glVertex3f(0., 0., 0.);
  glVertex3f(0., 0., len);
  glEnd();

  float fact = LENFRAC * len;
  float base = BASEFRAC * len;

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < 4; i++) {
    int j = xorder[i];
    if (j < 0) {

      glEnd();
      glBegin(GL_LINE_STRIP);
      j = -j;
    }
    j--;
    glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
  }
  glEnd();

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < 5; i++) {
    int j = yorder[i];
    if (j < 0) {

      glEnd();
      glBegin(GL_LINE_STRIP);
      j = -j;
    }
    j--;
    glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
  }
  glEnd();

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < 6; i++) {
    int j = zorder[i];
    if (j < 0) {

      glEnd();
      glBegin(GL_LINE_STRIP);
      j = -j;
    }
    j--;
    glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
  }
  glEnd();
}

// draw the complete scene:
void
Display()
{
  Scene& scene = world.get_current_scene();
  Camera& cam = scene.camera;

  if (world.DebugOn != 0)
    fprintf(stderr, "Starting Display.\n");

  // set which window we want to do the graphics into:
  glutSetWindow(world.MainWindow);

  // erase the background:
  glDrawBuffer(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
#ifdef DEMO_DEPTH_BUFFER
  if (DepthBufferOn == 0)
    glDisable(GL_DEPTH_TEST);
#endif

  // set the viewport to be a square centered in the window:

  GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
  GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
  GLsizei v = vx < vy ? vx : vy; // minimum dimension
  GLint xl = (vx - v) / 2;
  GLint yb = (vy - v) / 2;
  glViewport(xl, yb, v, v);

  // set the viewing volume:
  // remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE
  // EYE USE gluOrtho2D( ) IF YOU ARE DOING 2D !

  glMatrixMode(GL_PROJECTION);
  if (scene.NowProjection == ORTHO) {
    scene.projection_matrix = glm::ortho(-2.f, 2.f, -2.f, 2.f, 0.1f, 1000.f);
  } else {
    scene.projection_matrix = glm::perspective(70.f, 1.f, 0.1f, 1000.f);
  }
  glLoadMatrixf(&(scene.projection_matrix[0][0]));
  // place the objects into the scene:

  glMatrixMode(GL_MODELVIEW);
  // rotate the scene:
  if (world.current_scene_idx == CAMERA_SCENE) {
    scene.view_matrix = glm::lookAt(cam.eye,
                                    // this should follow a target light
                                    cam.center,
                                    cam.up);

  } else {
    // set the eye position, look-at position, and up-vector:
    // printf("lights position x: %f\n", world.lights[0].position[0]);
    // gluLookAt(cam.XPos, cam.YPos, cam.ZPos, world.lights[0].position[0],
    // world.lights[0].position[1], world.lights[0].position[2], cam.XNorm,
    // cam.YNorm, cam.ZNorm);
    scene.view_matrix = glm::lookAt(cam.eye,
                                    // this should follow a target light
                                    cam.center,
                                    cam.up);
    // rotate the thing
    scene.view_matrix = glm::rotate(scene.view_matrix,
                                    (glm::float32)(glm::radians(scene.YRot)),
                                    glm::vec3(0., 1., 0.));
    scene.view_matrix = glm::rotate(scene.view_matrix,
                                    (glm::float32)(glm::radians(scene.XRot)),
                                    glm::vec3(1., 0., 0.));
    // uniformly scale the scene:

    // apply scale
    if (scene.Scale < MINSCALE)
      scene.Scale = MINSCALE;
    scene.view_matrix =
      glm::scale(scene.view_matrix, scene.Scale * glm::vec3(1., 1., 1.));
  }
  // scene view matrix is solidifed now
  // gluLookAt(cam.eye.x, cam.eye.y, cam.eye.z, cam.center.x, cam.center.y,
  // cam.center.z, cam.up.x, cam.up.y, cam.up.z);
  glLoadMatrixf(&(scene.view_matrix)[0][0]);

  // set the fog parameters:
  if (scene.DepthCueOn != 0) {
    glFogi(GL_FOG_MODE, FOGMODE);
    glFogfv(GL_FOG_COLOR, FOGCOLOR);
    glFogf(GL_FOG_DENSITY, FOGDENSITY);
    glFogf(GL_FOG_START, FOGSTART);
    glFogf(GL_FOG_END, FOGEND);
    glEnable(GL_FOG);
  } else {
    glDisable(GL_FOG);
  }

  // possibly draw the axes:

  if (scene.AxesOn != 0) {
    glColor3fv(&Colors[scene.NowColor][0]);
    glDisable(GL_LIGHTING);
    glCallList(world.display_lists.AxesList);
    glEnable(GL_LIGHTING);
  }

  // since we are using glScalef( ), be sure the normals get unitized:
  glEnable(GL_NORMALIZE);

  // enable transparency
  glEnable(GL_BLEND);
  // enable textures & lighting conditionally
  {
    draw_light();
    if (world.get_current_scene().texture_mode) {
      glEnable(GL_TEXTURE_2D);
    } else {
      glDisable(GL_TEXTURE_2D);
    }
    if (world.get_current_scene().lighting_mode) {
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    } else {
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    }
  }

  // render the plane
  {
    if (world.get_current_scene().update_time) {
      world.pattern.SetUniformVariable("uTime", world.Time);
    }
    if (world.get_current_scene().update_keytimes) {
      // printf("hello world\n");
      world.pattern.SetUniformVariable("uAd", world.uAd.GetValue(world.Time));
      world.pattern.SetUniformVariable("uBd", world.uBd.GetValue(world.Time));
      world.pattern.SetUniformVariable("uTol", world.uTol.GetValue(world.Time));

      world.pattern.SetUniformVariable("uNoiseAmp",
                                       world.uNoiseAmp.GetValue(world.Time));
      world.pattern.SetUniformVariable("uNoiseFreq",
                                       world.uNoiseFreq.GetValue(world.Time));
      world.pattern.SetUniformVariable(
        "uUseXYZforNoise", world.uUseXYZforNoise.GetValue(world.Time));

      // set values for puddle
      world.pattern.SetUniformVariable("uA", world.uA.GetValue(world.Time));
      world.pattern.SetUniformVariable("uB", world.uB.GetValue(world.Time));
      world.pattern.SetUniformVariable("uC", world.uC.GetValue(world.Time));
      world.pattern.SetUniformVariable("uD", world.uD.GetValue(world.Time));
      world.pattern.SetUniformVariable("uUseBumpMapping", world.useBumpMapping);
    }

    world.pattern.SetUniformVariable("Noise3", 3);
    world.pattern.Use();
    glCallList(world.display_lists.PlaneList);
    world.pattern.UnUse();
  }

  // disable everything in preparation for the next round of rendering
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);

  // other
  glColor3f(0., 1., 0.);
  // call this completely ignoring the view matrix
  // glCallList(world.display_lists.BackgroundCircleList);

#ifdef DEMO_Z_FIGHTING
  if (DepthFightingOn != 0) {
    // this is completely broken lol, hopefully don't use
    glPushMatrix();
    glRotatef(90.f, 0.f, 1.f, 0.f);
    glCallList(BoxList);
    glPopMatrix();
  }
#endif

  // draw some gratuitous text that just rotates on top of the scene:
  // i commented out the actual text-drawing calls -- put them back in if you
  // have a use for them a good use for thefirst one might be to have your name
  // on the screen a good use for the second one might be to have vertex numbers
  // on the screen alongside each vertex

  glDisable(GL_DEPTH_TEST);
  glColor3f(0.f, 1.f, 1.f);
  // DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );

  // draw some gratuitous text that is fixed on the screen:
  //
  // the projection matrix is reset to define a scene whose
  // world coordinate system goes from 0-100 in each axis
  //
  // this is called "percent units", and is just a convenience
  //
  // the modelview matrix is reset to identity as we don't
  // want to transform these coordinates

  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.f, 100.f, 0.f, 100.f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(1.f, 1.f, 1.f);

  glPushMatrix();
  glTranslatef(80., 5., 0.);
  glScalef(.03, .03, .03);
  DoRasterString(0.f, 0.f, 0.f, (char*)"Andy Li");
  glPopMatrix();

  // swap the double-buffered framebuffers:

  glutSwapBuffers();

  // be sure the graphics buffer has been sent:
  // note: be sure to use glFlush( ) here, not glFinish( ) !

  glFlush();
}
