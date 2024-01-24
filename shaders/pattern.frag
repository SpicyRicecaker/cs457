// make this 120 for the mac:
#version 460 compatibility

// lighting uniform variables -- these can be set once and left alone:
uniform float   uKa, uKd, uKs;	 // coefficients of each type of lighting -- make sum to 1.0
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
	float s = vST.s / .5 - mirror_x;

	bool right = false;

	if (s > mirror_x) {
		right = true;
		// move to middle
		s -= mirror_x;
		s = -s;
		s += mirror_x;
	}
	
	float t = vST.t;

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
	if (uCrying == 0.) {
		if ((pow((s - (p.eyes.c.x - p.eyes.d)), 2.) / pow(p.eyes.s, 2.)) + (pow((t - p.eyes.c.y), 2.) / pow(p.eyes.s * uEyesYRadius, 2.)) <= 1.) {
			myColor = vec3(0., 0., 0.);
			// pika's eye highlights (they should only ever show up if they're in the eyes!)
			if ((pow((s - (p.highlights.c.x - p.highlights.d)), 2.) / pow(p.highlights.s, 2.)) + (pow((t - p.highlights.c.y), 2.) / pow(p.highlights.s, 2.)) <= 1.) {
				myColor = vec3(1., 1., 1.);
			}
		}
	} else {
		if ((pow((s - (p.eyes.c.x - p.eyes.d)), 2.) / pow(p.eyes.s, 2.)) + (pow((t - p.eyes.c.y), 2.) / pow(p.eyes.s * uEyesYRadiusCrying, 2.)) <= 1.) {
			myColor = vec3(0., 0., 0.);
			// pika's eye highlights (they should only ever show up if they're in the eyes!)
			if ((pow((s - (p.highlights.c.x - p.highlights.d)), 2.) / pow(p.highlights.s, 2.)) + (pow((t - p.highlights.c.y), 2.) / pow(p.highlights.s, 2.)) <= 1.) {
				myColor = vec3(1., 1., 1.);
			}
		}
	}

	// pika's mouth
	if ((pow((s - p.mouth.c.x), 2.) / pow(p.mouth.r.x, 2.)) + (pow((t - p.mouth.c.y), 2.) / pow(p.mouth.r.y, 2.)) <= 1. && t <= p.mouth.c.y ) {
		myColor = colors.mouthred.rgb;
	}

	// pika's nose
	{
		mat3x2 pt;

		for (int k = 0; k < 3; k++) {
			pt[k][0] = p.nose.h * cos(k * ((2. * PI) / 3.) + p.nose.theta) + p.nose.c.x;
			pt[k][1] = p.nose.h * sin(k * ((2. * PI) / 3.) + p.nose.theta) + p.nose.c.y;
		}

		if (t - pt[0][1] >= ((pt[1][1] - pt[0][1]) / (pt[1][0] - pt[0][0])) * (s - pt[0][0])
		&&	t - pt[1][1] <= ((pt[2][1] - pt[1][1]) / (pt[2][0] - pt[1][0])) * (s - pt[1][0])
		&&	t - pt[2][1] >= ((pt[0][1] - pt[2][1]) / (pt[0][0] - pt[2][0])) * (s - pt[2][0])) {
			myColor = vec3(0., 0., 0.);
		}
	}
	
	// pika's pouches
	if ((pow((s - (p.pouches.c.x - p.pouches.d)), 2.) / pow((.03 * cos(4. * 2. * PI * uTime) + 1.) * p.pouches.s, 2.)) + (pow((t - p.pouches.c.y), 2.) / pow((.03 * cos(4. * 2. * PI * uTime) + 1.) * p.pouches.s, 2.)) <= 1.) {
		myColor = colors.pouchred.rgb;
	}

	// pika's tears
	if (uCrying != 0.) {
		if ((pow(((s + uS) - (p.highlights2.c.x - p.highlights2.d)), 2.) / pow(p.highlights2.s, 2.)) + (pow(((t + uT) - p.highlights2.c.y), 2.) / pow(p.highlights2.s, 2.)) <= 1.) {
			myColor = vec3(1., 1., 1.);
		}
	}

	// pika's ears
	{
			mat3x2 pt;

			for (int k = 0; k < 3; k++) {
				pt[k][0] = p.ears.h * cos(k * ((2. * PI) / 3.) + p.ears.theta) + p.ears.c.x - p.ears.d;
				pt[k][1] = p.ears.h * sin(k * ((2. * PI) / 3.) + p.ears.theta) + p.ears.c.y;
			}

			if (t - pt[0][1] <= ((pt[1][1] - pt[0][1]) / (pt[1][0] - pt[0][0])) * (s - pt[0][0])
			&&	t - pt[1][1] <= ((pt[2][1] - pt[1][1]) / (pt[2][0] - pt[1][0])) * (s - pt[1][0])
			&&	t - pt[2][1] >= ((pt[0][1] - pt[2][1]) / (pt[0][0] - pt[2][0])) * (s - pt[2][0])) {
				myColor = vec3(0., 0., 0.);
			}
	}

	// float bridge = 0.3;

	// if (pow(s - uSc - bridge / 2., 2.) + pow(t - uTc, 2.) <= pow(uRs / 4., 2.)) {
	// 	myColor = vec3(0., 0., 0.);
	// }

	// if (pow(s - uSc + bridge / 2., 2.) + pow(t - uTc, 2.) <= pow(uRs / 4., 2.)) {
	// 	myColor = vec3(0., 0., 0.);
	// }



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

