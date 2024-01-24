// make this 120 for the mac:
#version 460 compatibility

// lighting uniform variables -- these can be set once and left alone:
uniform float   uKa, uKd, uKs;	 // coefficients of each type of lighting -- make sum to 1.0

// uAd	Ellipse diameter for s
// uBd	Ellipse diameter for t
// uTol	Width of the blend between ellipse and non-ellipse areas 
uniform float 	uAd, uBd, uTol;

uniform vec3    uColor;		 // object color
uniform vec3    uSpecularColor;	 // light color
uniform float   uShininess;	 // specular exponent

uniform float 	uTime;
uniform float 	uEyesYOffset, uEyesYRadius;
uniform float 	uEyesYRadiusCrying;
uniform	float 	uCrying;
uniform float 	uS, uT;		
// in variables from the vertex shader and interpolated in the rasterizer:

in  vec3  vN;		   // normal vector
in  vec3  vL;		   // vector from point to light
in  vec3  vE;		   // vector from point to eye
in  vec2  vST;		   // (s,t) texture coordinates

struct Eyes {
	// center
	vec2 c;
	// distance	
	float d;
	// size
	float s;
};

struct Highlights {
	// center
	vec2 c;
	// distance	
	float d;
	// size
	float s;
};


struct Mouth {
	// center
	vec2 c;
	// radii
	vec2 r;
};

struct Nose {
	// center
	vec2 c;
	// height	
	float h;
	// rotation
	float theta;
};

struct Pouches {
	// center
	vec2 c;
	// distance	
	float d;
	// size
	float s;
};

struct Ears {
	// center
	vec2 c;
	// height
	float h;
	// theta
	float theta;
	// distance
	float d;
};

struct Highlights2 {
	// center
	vec2 c;
	// distance	
	float d;
	// size
	float s;
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

// MUST be alphabetical LOL
// Think 6 is the data cap for uniform buffer objects
layout (binding = 0) uniform Block {
	Pika p;
};

struct PikaColors {
	vec4 mouthred;
  vec4 yellow;
	vec4 pouchred;
};

layout (binding = 1) uniform ColorBlock {
  PikaColors colors;
};


#define PI 3.1415926538
#define mirror_x .5

void
main( )
{
	// shrink the s size and move it to the right by .5
	// float s = vST.s / .5 - mirror_x;

	// bool right = false;

	// if (s > mirror_x) {
	// 	right = true;
	// 	// move to middle
	// 	s -= mirror_x;
	// 	s = -s;
	// 	s += mirror_x;
	// }
	
	float t = vST.t;
	float s = vST.s;

	// determine the color using the square-boundary equations:

	// uColor is the constant color of the object given no changes
	vec3 myColor = colors.yellow.rgb;
	// (uS0, uT0) is the center of the square, whilst uD is the length of the square
	// how do I create a circle instead?
	// if( uS0-uD/2. <= s  &&  s <= uS0+uD/2.  &&  uT0-uD/2. <= t  &&  t <= uT0+uD/2. )
	// {
	// 	myColor = vec3( 1., 1., 0. );
	// }

	// if ((pow(s - uSc, 2.) / uRs) + (pow(t - uTc, 2.) / uRt) <= 1.) {
	// 	myColor = vec3(1., 0., 0.);
	// }

	// equation of ellipse
	// if ((pow((s - uSc), 2.) / pow(uRs, 2.6)) + (pow((t - uTc), 2.) / pow(uRt, 2.)) <= 1.) {
	// 	myColor = vec3(1., 0., 0.);
	// }

	// pika's eyes
	if ((pow((s - (p.eyes.c.x - p.eyes.d)), 2.) / pow(p.eyes.s, 2.)) + (pow((t - p.eyes.c.y), 2.) / pow(p.eyes.s * uEyesYRadius, 2.)) <= 1.) {
		myColor = vec3(0., 0., 0.);
	}

	// apply the per-fragmewnt lighting to myColor:
	vec3 Normal = normalize(vN);
	vec3 Light  = normalize(vL);
	vec3 Eye    = normalize(vE);

	vec3 ambient = uKa * myColor;

	float dd = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
	vec3 diffuse = uKd * dd * myColor;

	float ss = 0.;
	if( dot(Normal,Light) > 0. )	      // only do specular if the light can see the point
	{
		vec3 ref = normalize(  reflect( -Light, Normal )  );
		ss = pow( max( dot(Eye,ref),0. ), uShininess );
	}
	vec3 specular = uKs * ss * uSpecularColor;
	gl_FragColor = vec4( ambient + diffuse + specular,  1. );
}

