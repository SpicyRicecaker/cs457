// make this 120 for the mac:
#version 460 compatibility

// out variables to be interpolated in the rasterizer and sent to each fragment shader:

out  vec3  vN;	  // normal vector
out  vec3  vL;	  // vector from point to light
out  vec3  vE;	  // vector from point to eye
out  vec2  vST;	  // (s,t) texture coordinates
out  vec3  vMCposition;

// where the light is:

const vec3 LightPosition = vec3(  0., 5., 5. );

void
main( )
{
  // interpolated s & t coordinates?
	vST = gl_MultiTexCoord0.st;
  // why do we need this
	vec4 ECposition = gl_ModelViewMatrix * gl_Vertex;
  // vector normal to current vertex?
	vN = normalize( gl_NormalMatrix * gl_Normal );  // normal vector
  // vector from light to point
	vL = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	vE = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	// add coordinates for later
	vMCposition = gl_Vertex.xyz;
  // map vertex position from world space to 2d screen space (monitor resolution) basically
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
