// make this 120 for the mac:
#version 460 compatibility

// lighting uniform variables -- these can be set once and left alone:
uniform float   uKa, uKd, uKs;	 // coefficients of each type of lighting -- make sum to 1.0

// uAd	Ellipse diameter for s
// uBd	Ellipse diameter for t
// uTol	Width of the blend between ellipse and non-ellipse areas 
uniform float 	uAd, uBd, uTol;
uniform float  	uNoiseAmp, uNoiseFreq;
uniform bool 	uUseXYZforNoise;
uniform bool 	uUseBumpMapping;
uniform sampler3D Noise3;
in vec3 vMC;

uniform vec3    uColor;		 // object color
uniform vec3    uSpecularColor;	 // light color
uniform float   uShininess;	 // specular exponent

uniform float 	uTime;
// in variables from the vertex shader and interpolated in the rasterizer:

in  vec3  vN;		   // normal vector
in  vec3  vL;		   // vector from point to light
in  vec3  vE;		   // vector from point to eye
in  vec2  vST;		   // (s,t) texture coordinates
// struct Game {};

// MUST be alphabetical LOL
// Think 6 is the data cap for uniform buffer objects
// layout (binding = 0) uniform Block {
// 	Game g;
// };

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

// vec3 mix(vec3 value0, vec3 value1, float t) {
//     return (1.0 - t) * value0 + t * value1;
// }

vec3
RotateNormal( float angx, float angy, vec3 n)
{
        float cx = cos( angx );
        float sx = sin( angx );
        float cy = cos( angy );
        float sy = sin( angy );

        // rotate about x:
        float yp =  n.y*cx - n.z*sx;    // y'
        n.z      =  n.y*sx + n.z*cx;    // z'
        n.y      =  yp;
        // n.x      =  n.x;

        // rotate about y:
        float xp =  n.x*cy + n.z*sy;    // x'
        n.z      = -n.x*sy + n.z*cy;    // z'
        n.x      =  xp;
        // n.y      =  n.y;

        return normalize( n );
}

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

	// vec4 nv;
	
	// // if (uUseXYZforNoise) {
    // //     nv = texture(Noise3, uNoiseFreq * vMCposition );
	// // } else {
    //     nv = texture(Noise3, uNoiseFreq * vec3(vST, 0.) );
	// // }

	// float n = nv.r + nv.g + nv.b + nv.a; // range is 1. -> 3.
	// n = n - 2.; // range is now -1. -> 1.
	// n *= uNoiseAmp;

	// vec3 myColor = colors.yellow.rgb;
	// if (n > -0.1) {
	// 	myColor = vec3(1., 1., 1.);
	// } else {
	// 	myColor = vec3(0., 0., 0.);
	// }

	// vST.s + n;
	// vST.t + n;

	// // copied from canvas slides
	// float Ar = uAd / 2.;
	// float Br = uBd / 2.;
	// int numins = int( vST.s / uAd );
	// int numint = int( vST.t / uBd );
	// float sc = numins * uAd + Ar;
	// float tc = numint * uBd + Br;

	// // copied from https://web.engr.oregonstate.edu/~mjb/cs557/Handouts/noise.1pp.pdf
	// float ds = vST.s - sc; // wrt ellipse center
	// float dt = vST.t - tc; // wrt ellipse center

	// float oldDist = sqrt( ds * ds + dt * dt );
	// float newDist = oldDist + n;
	// float scale = newDist / oldDist; // this could be < 1., = 1., or > 1.

	// ds *= scale; // scale by noise factor
	// ds /= Ar; // ellipse equation
	// dt *= scale; // scale by noise factor
	// dt /= Br; // ellipse equation

	// // use the smoothstep 
	// float d = ds * ds + dt * dt;
	// float t = smoothstep( 1.-uTol, 1.+uTol, d );

	// vec3 myColor = mix( colors.yellow.rgb, vec3(0., 0., 0.), t );

	// get noise values -1. to 1.

	vec3 fml = vec3(vST, 0.);

	vec4 nvx = texture( Noise3, uNoiseFreq * fml );
	float angx = nvx.r + nvx.g + nvx.b + nvx.a  -  2.;	// -1. to +1.
	angx *= uNoiseAmp;

	vec4 nvy = texture( Noise3, uNoiseFreq * vec3(fml.yx, 0.) );
	float angy = nvy.r + nvy.g + nvy.b + nvy.a  -  2.;	// -1. to +1.
	angy *= uNoiseAmp;

	vec3 myColor = colors.yellow.rgb;
	
	// float local_s = vST.s;
	// float local_t = vST.t;
	// // random ellipse
	// // vec2 ellipse_c = vec2(0.5, 0.5);

	// float t = smoothstep(1. - uTol, 1. + uTol, pow(local_s - sc, 2.) / pow(Ar, 2.) + pow(local_t - tc, 2.) / pow(Br, 2.));

	// // determine the color using the square-boundary equations:

	// // uColor is the constant color of the object given no changes
	// vec3 myColor = colors.yellow.rgb;
	// myColor = mix(myColor, vec3(0., 0., 0.), t);
	// vec3 myColor = vec3(n, n, n);
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


	// apply the per-fragmewnt lighting to myColor:
	vec3 Normal = normalize(vN);

	if (uUseBumpMapping) {
		Normal = RotateNormal(angx, angy, Normal);
		// Normal = RotateNormal(2., 2., Normal);
	}

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
