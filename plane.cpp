#include "plane.h"

GLuint make_grid(float len, int steps, float y_0) {
	GLuint GridDL = glGenLists(1);
	glNewList(GridDL, GL_COMPILE);
	// grid is pointing up
	glNormal3f(0., 1., 0.);

	float x_0 = -len / 2;
	float z_0 = -len / 2;

	float dx = len / steps;
	float dz = len / steps;

	for (int row = 1; row <= steps; row++) {
		// get the coordinate of the top left of the current square
		// get the coordinate of the top right
		// get the coordinate of the bottom left
		// get the coordinate of the bottom right

		// if on the first square, draw tl, tr, bl, br
		// if on any other square, draw tr, then br

		// now draw tl, tr, bl, br
		// use a triangle strip to draw the quads, row by row
		float z = (row - 1) * dz + z_0;
		glBegin(GL_QUAD_STRIP);
		for (int column = 1; column <= steps; column++) {
			float x = (column - 1) * dx + x_0;

			if (column == 1) {
				glVertex3f(x, y_0, z); //tl
				glVertex3f(x + dx, y_0, z); // tr
				glVertex3f(x, y_0, z + dz); // bl
				glVertex3f(x + dx, y_0, z + dz); // br
			}
			else {
				glVertex3f(x + dx, y_0, z); // tr
				glVertex3f(x + dx, y_0, z + dz); // br
			}
		}
		glEnd();
	}
	glEndList();

	return GridDL;
}

