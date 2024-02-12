#include "material.h"

void Material::SetMaterial() const
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient.data());
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse.data());
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular.data());
	glMaterialf(GL_FRONT, GL_SHININESS, shininess * 128.0);

	glMaterialfv( GL_BACK, GL_EMISSION, Array3( 0., 0., 0. ) );
	glMaterialfv( GL_BACK, GL_AMBIENT, MulArray3( .4f, (float *)WHITE ) );
	glMaterialfv( GL_BACK, GL_DIFFUSE, MulArray3( 1., (float *)WHITE ) );
	glMaterialfv( GL_BACK, GL_SPECULAR, Array3( 0., 0., 0. ) );
	glMaterialf ( GL_BACK, GL_SHININESS, 2.f );
}
