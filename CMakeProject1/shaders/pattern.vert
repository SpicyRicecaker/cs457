// make this 120 for the mac:
#version 460 compatibility

#define M_PI 3.1415926535897932384626433832795

// out variables to be interpolated in the rasterizer and sent to each fragment shader:
out  vec3  vN;	  // normal vector
out  vec3  vL;	  // vector from point to light
out  vec3  vE;	  // vector from point to eye
out  vec2  vST;	  // (s,t) texture coordinates
out  vec3  vMCposition;

// where the light is:
const vec3 LightPosition = vec3(  0., 5., 5. );

// z = A\cos(2\piBr + C)e^{-Dr}
// uA : amplitude
// uB : period
// uC : phase shift
// uD : decay rate
// r  : radius = sqrt(x^2 + y^2)

// how to get the tangent vector at each point?
// f(x,y) = z = ...
// f(x,y) - z = 0
// \nabla f(x,y,z) = (df/dx, df/dy, df/dz)
// \nabla f(x,y,z) = (df/dx, df/dy, -1)
// \nabla f(x,y,z) = (df/dr * dr/dx, df/dr * dr/dy, -1)

uniform float uA, uB, uC, uD;

void
main()
{
	// // interpolated s & t coordinates?
	// vST = gl_MultiTexCoord0.st;

	vec4 vert = gl_Vertex;
	// float r = sqrt(pow(vert.x, 2) + pow(vert.y, 2));
	// vert.z = uA * cos(2. * M_PI * uB * r + uC) * exp(-uD * r);

	// float df_dr = -uA * sin(2. * M_PI * uB * r + uC) * 2. * M_PI * uB * exp(-uD * r) 
	//               + uA * cos(2. * M_PI * uB * r + uC) * exp(-uD * r);

	// float dr_dx = vert.x * sqrt(pow(vert.x, 2) + pow(vert.y, 2));
	// float dr_dy = vert.y * sqrt(pow(vert.x, 2) + pow(vert.y, 2));

  // 	// why do we need this
	// vec4 ECposition = gl_ModelViewMatrix * vert;
	// // vector normal to current vertex?
	// // gl_NormalMatrix ensures that normals are transformed correctly normals
	// // are direction vectors while vertices are position vectors, so they need
	// // to be treated differently
	// vN = normalize( gl_NormalMatrix * vec3(df_dr * dr_dx, df_dr * dr_dy, 1.));  // normal vector
	// // vector from light to point
	// vL = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	// vE = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	// // add coordinates for later
	// vMCposition = vert.xyz;
	// // map vertex position from world space to 2d screen space (monitor resolution) basically
	// gl_Position = gl_ModelViewProjectionMatrix * vert;
	gl_Position = gl_ModelViewProjectionMatrix * vert;
}
