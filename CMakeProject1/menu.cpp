#include "menu.h"

void DoAxesMenu(int id)
{
	Scene& scene = world.get_current_scene();
	scene.AxesOn = id;

	glutSetWindow(world.MainWindow);
	glutPostRedisplay();
}

void DoSceneMenu(int id)
{
	world.current_scene_idx = id;
	world.SceneNeedsUpdate = true;

	glutSetWindow(world.MainWindow);
	glutPostRedisplay();
}


void
DoColorMenu(int id)
{
	Scene& scene = world.get_current_scene();
	scene.NowColor = id - RED;
	
	glutSetWindow(world.MainWindow);
	glutPostRedisplay();
}


void
DoDebugMenu(int id)
{
	world.DebugOn = id;

	glutSetWindow(world.MainWindow);
	glutPostRedisplay();
}


void
DoDepthBufferMenu(int id)
{
	Scene& scene = world.get_current_scene();
	scene.DepthBufferOn = id;

	glutSetWindow(world.MainWindow);
	glutPostRedisplay();
}


void
DoDepthFightingMenu(int id)
{
	Scene& scene = world.get_current_scene();
	scene.DepthFightingOn = id;

	glutSetWindow(world.MainWindow);
	glutPostRedisplay();
}


void
DoDepthMenu(int id)
{
	Scene& scene = world.get_current_scene();
	scene.DepthCueOn = id;

	glutSetWindow(world.MainWindow);
	glutPostRedisplay();
}

// main menu callback:

void
DoMainMenu(int id)
{
	switch (id)
	{
	case RESET:
		// Reset();
		world.Init();
		break;

	case QUIT:
		// gracefully close out the graphics:
		glutSetWindow(world.MainWindow);
		glFinish();
		// gracefully close the graphics window:
		glutDestroyWindow(world.MainWindow);
		// gracefully exit the program:
		exit(0);
		break;

	default:
		fprintf(stderr, "Menu::Don't know what to do with Main Menu ID %d\n", id);
	}

	glutSetWindow(world.MainWindow);
	glutPostRedisplay();
}


void
DoProjectMenu(int id)
{
	auto& scene = world.get_current_scene();
	scene.NowProjection = id;

	glutSetWindow(world.MainWindow);
	glutPostRedisplay();
}
