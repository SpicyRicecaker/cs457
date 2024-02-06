#include "plane.h"

GLuint make_grid(float len, int steps, float z_0) {
	GLuint GridDL = glGenLists(1);
	glNewList(GridDL, GL_COMPILE);
	// grid is pointing up
	// glNormal3f(0., 1., 0.);

	float x_0 = -len / 2;
	float y_0 = -len / 2;

	float dx = len / steps;
	float dy = len / steps;

	for (int row = 1; row <= steps; row++) {
		// get the coordinate of the top left of the current square
		// get the coordinate of the top right
		// get the coordinate of the bottom left
		// get the coordinate of the bottom right

		// if on the first square, draw tl, tr, bl, br
		// if on any other square, draw tr, then br

		// now draw tl, tr, bl, br
		// use a triangle strip to draw the quads, row by row
		float y = (row - 1) * dy + y_0;
		glBegin(GL_QUAD_STRIP);
		for (int column = 1; column <= steps; column++) {
			float x = (column - 1) * dx + x_0;

			if (column == 1) {
				glVertex3f(x, y, z_0); //tl
				glVertex3f(x + dx, y, z_0); // tr
				glVertex3f(x, y + dy, z_0); // bl
				glVertex3f(x + dx, y + dy, z_0); // br
			}
			else {
				glVertex3f(x + dx, y, z_0); // tr
				glVertex3f(x + dx, y + dy, z_0); // br
			}
		}
		glEnd();
	}
	glEndList();

	return GridDL;
}

