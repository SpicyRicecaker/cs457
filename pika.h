#pragma once

#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Eyes {
	// center
	glm::vec2 c;
	// distance	
	float d;
	// size
	float s;
};

struct Highlights {
	// center
	glm::vec2 c;
	// distance	
	float d;
	// size
	float s;
};

struct Mouth {
	// center
	glm::vec2 c;
	// radii
	glm::vec2 r;
};

struct Nose {
	// center
	glm::vec2 c;
	// height	
	float h;
	// rotation
	float theta;
};

struct Pouches {
	// center
	glm::vec2 c;
	// distance	
	float d;
	// size
	float s;
};

struct Ears {
	// center
	glm::vec2 c;
	// height
	float h;
	// theta
	float theta;
  // distance
  float d;
};

struct Highlights2 {
	// center
	glm::vec2 c;
	// distance	
	float d;
	// size
	float s;
};

struct PikaColors {
  glm::vec4 mouthred;
  glm::vec4 yellow;
  glm::vec4 pouchred;
};

struct Pika {
  Eyes eyes;
  Highlights highlights;
	Highlights2 highlights2;
  Mouth mouth;
  Nose nose;
  Pouches pouches;
  Ears ears;
};
