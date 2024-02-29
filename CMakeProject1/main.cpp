#include <stdio.h>

#include <vector>
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable : 4996)
#endif

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "material.h"
// #include "setlight.cpp"
#include "osusphere.h"
#include "osucone.h"
// #include "osutorus.cpp"
// #include "loadobjfile.cpp"
#include "keytime.h"
#include "utils.h"

#include "world.h"
#include "scene.h"
#include "camera.h"
#include "display_lists.h"

//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// title of these windows:

const char* WINDOWTITLE = "OpenGL -- Andy Graphics";
const char* GLUITITLE = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

// defaults!
const int INIT_WINDOW_SIZE = 600;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT = 4;
const int MIDDLE = 2;
const int RIGHT = 1;

struct Point
{
  float x;
  float y;
  float z;
};

// window background color (rgba):

const GLfloat BACKCOLOR[] = { 0., 0., 0., 1. };

// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
// #define DEMO_Z_FIGHTING
// #define DEMO_DEPTH_BUFFER

// function prototypes:

void
Animate();
float
ElapsedSeconds();
void
InitGraphics();
void
InitMenus();
void
Keyboard(unsigned char, int, int);
void
MouseButton(int, int, int, int);
void
MouseMotion(int, int);
void
Resize(int, int);
void
Visibility(int);

// utility to create an array from 3 separate values:

// these are here for when you need them -- just uncomment the ones you need:

// main program:

int
main(int argc, char* argv[])
{
  // turn on the glut package:
  // (do this before checking argc and argv since glutInit might
  // pull some command line arguments out)

  glutInit(&argc, argv);

  // setup all the graphics stuff:

  InitGraphics();

  // create the display lists that **will not change**:
  world.Init();

  // init all the global variables used by Display( ):
  // this will also post a redisplay

  // setup all the user interface stuff:

  InitMenus();

  // draw the scene once and wait for some interaction:
  // (this will never return)

  glutSetWindow(world.MainWindow);
  glutMainLoop();

  // glutMainLoop( ) never actually returns
  // the following line is here to make the compiler happy:

  return 0;
}

// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate()
{
  // put animation stuff in here -- change some global variables for Display( )
  // to find:

  int program_running_time_ns = glutGet(GLUT_ELAPSED_TIME);

  int delta_time_ns = program_running_time_ns - world.last_time_stamp;
  // as soon as we get delta_time_ns, update our last timestamp
  world.last_time_stamp = program_running_time_ns;

  world.world_time += delta_time_ns;

  // makes the value of ms between 0 and MS_PER_CYCLE-1
  world.Time = (float)(world.world_time % MS_PER_CYCLE) /
               (float)MS_PER_CYCLE; // makes the value of Time between 0. and
                                    // slightly less than 1.
  // std::cout << "animate " << world.Time << std::endl;

  // for example, if you wanted to spin an object in Display( ), you might call:
  // glRotatef( 360.f*Time,   0., 1., 0. );

  // force a call to Display( ) next time it is convenient:

  glutSetWindow(world.MainWindow);
  glutPostRedisplay();
}

// return the number of seconds since the start of the program:

float
ElapsedSeconds()
{
  // get # of milliseconds since the start of the program:

  int ms = glutGet(GLUT_ELAPSED_TIME);

  // convert it to seconds:

  return (float)ms / 1000.f;
}

// initialize the glui window:

void
InitMenus()
{
  if (world.DebugOn != 0)
    fprintf(stderr, "Starting InitMenus.\n");

  glutSetWindow(world.MainWindow);

  int numColors = sizeof(Colors) / (3 * sizeof(float));

  int colormenu = glutCreateMenu(DoColorMenu);
  for (int i = 0; i < numColors; i++) {
    glutAddMenuEntry(ColorNames[i], i);
  }

  int axesmenu = glutCreateMenu(DoAxesMenu);
  glutAddMenuEntry("Off", 0);
  glutAddMenuEntry("On", 1);

  int depthcuemenu = glutCreateMenu(DoDepthMenu);
  glutAddMenuEntry("Off", 0);
  glutAddMenuEntry("On", 1);

  int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
  glutAddMenuEntry("Off", 0);
  glutAddMenuEntry("On", 1);

  int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
  glutAddMenuEntry("Off", 0);
  glutAddMenuEntry("On", 1);

  int debugmenu = glutCreateMenu(DoDebugMenu);
  glutAddMenuEntry("Off", 0);
  glutAddMenuEntry("On", 1);

  int projectionmenu = glutCreateMenu(DoProjectMenu);
  if (world.current_scene_idx == CAMERA_SCENE) {
    glutAddMenuEntry("Orthographic", ORTHO);
  }
  glutAddMenuEntry("Perspective", PERSP);

  int scenemenu = glutCreateMenu(DoSceneMenu);
  glutAddMenuEntry("Camera", CAMERA_SCENE);
  glutAddMenuEntry("World", WORLD_SCENE);

  int mainmenu = glutCreateMenu(DoMainMenu);
  glutAddSubMenu("Axes", axesmenu);
  glutAddSubMenu("Axis Colors", colormenu);
  glutAddSubMenu("Scene", scenemenu);

#ifdef DEMO_DEPTH_BUFFER
  glutAddSubMenu("Depth Buffer", depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
  glutAddSubMenu("Depth Fighting", depthfightingmenu);
#endif

  glutAddSubMenu("Depth Cue", depthcuemenu);
  glutAddSubMenu("Projection", projectionmenu);
  glutAddMenuEntry("Reset", RESET);
  glutAddSubMenu("Debug", debugmenu);
  glutAddMenuEntry("Quit", QUIT);

  // attach the pop-up menu to the right mouse button:
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics()
{
  if (world.DebugOn != 0)
    fprintf(stderr, "Starting InitGraphics.\n");

  // request the display modes:
  // ask for monokai_red-green-blue-alpha color, double-buffering, and
  // z-buffering:

  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  // set the initial window configuration:

  world.ScreenWidth = glutGet(GLUT_SCREEN_WIDTH);
  world.ScreenHeight = glutGet(GLUT_SCREEN_HEIGHT);

  int bottom_margin = 100;
  glutInitWindowSize(world.ScreenHeight - bottom_margin,
                     world.ScreenHeight - bottom_margin);

  glutInitWindowPosition(
    world.ScreenWidth / 2 - glutGet(GLUT_INIT_WINDOW_WIDTH) / 2,
    world.ScreenHeight / 2 - glutGet(GLUT_INIT_WINDOW_HEIGHT) / 2 -
      bottom_margin / 2);

  // open the window and set its title:

  world.MainWindow = glutCreateWindow(WINDOWTITLE);
  glutSetWindowTitle(WINDOWTITLE);

  // set the framebuffer clear values:

  glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

  // setup the callback functions:
  // DisplayFunc -- redraw the window
  // ReshapeFunc -- handle the user resizing the window
  // KeyboardFunc -- handle a keyboard input
  // MouseFunc -- handle the mouse button going down or up
  // MotionFunc -- handle the mouse moving with a button down
  // PassiveMotionFunc -- handle the mouse moving with a button up
  // VisibilityFunc -- handle a change in window visibility
  // EntryFunc	-- handle the cursor entering or leaving the window
  // SpecialFunc -- handle special keys on the keyboard
  // SpaceballMotionFunc -- handle spaceball translation
  // SpaceballRotateFunc -- handle spaceball rotation
  // SpaceballButtonFunc -- handle spaceball button hits
  // ButtonBoxFunc -- handle button box hits
  // DialsFunc -- handle dial rotations
  // TabletMotionFunc -- handle digitizing tablet motion
  // TabletButtonFunc -- handle digitizing tablet button hits
  // MenuStateFunc -- declare when a pop-up menu is in use
  // TimerFunc -- trigger something to happen a certain time from now
  // IdleFunc -- what to do when nothing else is going on

  glutSetWindow(world.MainWindow);
  glutDisplayFunc(Display);
  glutReshapeFunc(Resize);
  glutKeyboardFunc(Keyboard);
  glutMouseFunc(MouseButton);
  glutMotionFunc(MouseMotion);
  glutPassiveMotionFunc(MouseMotion);
  // glutPassiveMotionFunc( NULL );
  glutVisibilityFunc(Visibility);
  glutEntryFunc(NULL);
  glutSpecialFunc(NULL);
  glutSpaceballMotionFunc(NULL);
  glutSpaceballRotateFunc(NULL);
  glutSpaceballButtonFunc(NULL);
  glutButtonBoxFunc(NULL);
  glutDialsFunc(NULL);
  glutTabletMotionFunc(NULL);
  glutTabletButtonFunc(NULL);
  glutMenuStateFunc(NULL);
  glutTimerFunc(-1, NULL, 0);

  // setup glut to call Animate( ) every time it has
  // 	nothing it needs to respond to (which is most of the time)
  // we don't need to do this for this program, and really should set the
  // argument to NULL but, this sets us up nicely for doing animation

  if (world.Frozen) {
    glutIdleFunc(NULL);
  } else {
    glutIdleFunc(Animate);
  }

  // init the glew package (a window must be open to do this):
  // ALWAYS RUN THIS CODE, EVEN WHEN NOT ON WINDOWS!
  // Glew allows dynamic opengl function loading, whatever that means
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "glewInit Error\n");
  } else
    fprintf(stderr, "GLEW initialized OK\n");
  fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  // all other setups go here, such as GLSLProgram and KeyTime setups:
}

// the keyboard callback:

void
Keyboard(unsigned char c, int x, int y)
{
  if (world.DebugOn != 0)
    fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

  switch (c) {
    case 'q':
    case 'Q':
    case ESCAPE:
      DoMainMenu(QUIT); // will not return here
      break;            // happy compiler
    case 'f':
    case 'F':
      world.Frozen = !world.Frozen;
      if (world.Frozen) {
        // wow, pretty hand*LY* that glut has a function like this
        glutIdleFunc(NULL);
      } else {
        world.last_time_stamp = glutGet(GLUT_ELAPSED_TIME);
        glutIdleFunc(Animate);
      }
      break;
    case 'p':
    case 'P': {
      for (auto& l : world.lights) {
        l.light_type = LightType::Pointlight;
      }
      break;
    }
    // case 's':
    // case 'S': {
    //   for (auto& l : world.lights) {
    //     l.light_type = LightType::Spotlight;
    //   }
    //   break;
    // }
    case 'a':
    case 'A': {
      for (auto& l : world.lights) {
        l.quad_attenuation += 0.1;
      }
      break;
    }
    case 'd':
    case 'D': {
      for (auto& l : world.lights) {
        l.quad_attenuation -= 0.1;
      }
      break;
    }
    case 'b':
    case 'B': {
      world.useBumpMapping = !world.useBumpMapping;
      break;
    }
    case 'w':
    case 'W': {
      world.current_scene_idx = WORLD_SCENE;
      world.SceneNeedsUpdate = true;
      break;
    }
    case 'c':
    case 'C': {
      world.get_current_scene().crying = !world.get_current_scene().crying;
      break;
    }
    case 't':
    case 'T': {
      world.get_current_scene().update_time = !world.get_current_scene().update_time;
      break;
    }
    case 'k':
    case 'K': {
      world.get_current_scene().update_keytimes = !world.get_current_scene().update_keytimes;
      break;
    }

    case 'n':
    case 'N': {
      world.useXYZ = !world.useXYZ;
      break;
    }

    default: {
      int planet_i = 0;
      for (auto& p : world.display_lists.planets) {
        if (p.key == c) {
          world.get_current_scene().current_planet = planet_i;
        }
        planet_i++;
      }

      break;
    }
  }

  // force a call to Display( ):

  glutSetWindow(world.MainWindow);
  glutPostRedisplay();
}

// called when the mouse button transitions down or up:

void
MouseButton(int button, int state, int x, int y)
{
  // printf("HANDLING MOUSE BUTTON\n");
  Scene& scene = world.get_current_scene();

  int b = 0; // LEFT, MIDDLE, or RIGHT

  if (world.DebugOn)
    fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);

  // get the proper button bit mask:

  switch (button) {
    case GLUT_LEFT_BUTTON: {
      if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
        world.ShiftHeld = true;
      }
      if (glutGetModifiers() & GLUT_ACTIVE_ALT) {
        world.AltHeld = true;
      }

      b = LEFT;
      break;
    }

    case GLUT_MIDDLE_BUTTON:
      b = MIDDLE;
      break;

    case GLUT_RIGHT_BUTTON:
      b = RIGHT;
      break;

    case SCROLL_WHEEL_UP:
      scene.Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
      // keep object from turning inside-out or disappearing:
      if (scene.Scale < MINSCALE)
        scene.Scale = MINSCALE;
      break;

    case SCROLL_WHEEL_DOWN:
      scene.Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
      // keep object from turning inside-out or disappearing:
      if (scene.Scale < MINSCALE)
        scene.Scale = MINSCALE;
      break;

    default:
      b = 0;
      fprintf(stderr, "Unknown mouse button: %d\n", button);
  }

  // button down sets the bit, up clears the bit:

  if (state == GLUT_DOWN) {
    world.Xmouse = x;
    world.Ymouse = y;
    world.ActiveButton |= b; // set the proper bit
  } else {
    world.ShiftHeld = false;
    world.AltHeld = false;
    world.ActiveButton &= ~b; // clear the proper bit
  }

  // printf("the thngy was cliked");
  // fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", ShiftHeld, AltHeld, 0, 0);
  // printf("Hi %d %d", ShiftHeld, AltHeld);

  glutSetWindow(world.MainWindow);
  glutPostRedisplay();
}

// called when the mouse moves while a button is down:

void
MouseMotion(int x, int y)
{
  Scene& scene = world.get_current_scene();

  int dx = x - world.Xmouse; // change in mouse coords
  int dy = y - world.Ymouse;

  if ((world.ActiveButton & LEFT) != 0) {
    if (world.AltHeld && world.ShiftHeld) {
      scene.XTrans += dx;
      scene.YTrans += dy;
      scene.ZTrans += 0;
    } else {
      scene.XRot += (ANGFACT * dy);
      scene.YRot += (ANGFACT * dx);
    }
  }

  if ((world.ActiveButton & MIDDLE) != 0) {
    scene.Scale += SCLFACT * (float)(dx - dy);

    // keep object from turning inside-out or disappearing:

    if (scene.Scale < MINSCALE)
      scene.Scale = MINSCALE;
  }

  world.Xmouse = x; // new current position
  world.Ymouse = y;

  glutSetWindow(world.MainWindow);
  glutPostRedisplay();
}

float
radians_to_degrees(float r)
{
  return (r / (2 * M_PI)) * 360.;
}

// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

// called when user resizes the window:

void
Resize(int width, int height)
{
  // don't really need to do anything since window size is
  // checked each time in Display( ):

  glutSetWindow(world.MainWindow);
  glutPostRedisplay();
}

// handle a change to the window's visibility:

void
Visibility(int state)
{
  if (world.DebugOn != 0)
    fprintf(stderr, "Visibility: %d\n", state);

  if (state == GLUT_VISIBLE) {
    glutSetWindow(world.MainWindow);
    glutPostRedisplay();
  } else {
    // could optimize by keeping track of the fact
    // that the window is not visible and avoid
    // animating or redrawing it ...
  }
}

// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );
