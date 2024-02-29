#include "plane.h"

GLuint
make_grid(float len, int steps, float z_0)
{
  GLuint GridDL = glGenLists(1);
  GLuint GridTexture;
  glNewList(GridDL, GL_COMPILE);
  {
    int nums, numt, nump;
    std::string file_name = std::format("{}/textures/noise3d.064.tex", DIR_PREFIX);
    const char* file = file_name.c_str();

    unsigned char* texture = ReadTexture3D(file, &nums, &numt, &nump);
    if (texture == NULL) {
      fprintf(stderr, "Cannot open texture '%s'\n", file);
    } else {
      fprintf(stderr, "Opened '%s'", file);
    }
    // create an id for texture
    glGenTextures(1, &GridTexture);
    // bind to gpu
    glBindTexture(GL_TEXTURE_3D, GridTexture);
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

    // we will just have to scale later using the radius
    glNewList(GridDL, GL_COMPILE);
    // MarsTex must have already been created when
    // this is called glPushMatrix();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_3D, GridTexture);
    // a dl can call another dl that has been previously created
  }
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

      glTexCoord2f((float)(row - 1) / (float)steps,
                   (float)(column - 1) / (float)steps);
      glVertex3f(x, y, z_0); // tr
      glTexCoord2f((float)(row - 1) / (float)steps,
                   (float)(column - 1) / (float)steps);
      glVertex3f(x, y + dy, z_0); // br
    }
    glEnd();
  }
  glEndList();

  return GridDL;
}
