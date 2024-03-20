#version 460 core

out vec4 FragColor;

in vec3 v_n;
in vec3 v_wc;
in vec2 v_st;
uniform vec3 eye;

uniform float amplitude;

uniform sampler2D texture_1;
uniform sampler2D texture_2;

const vec3 light_color = vec3(1., 1., 1.);
const vec3 light_pos = vec3(5., 5., 5.);

uniform float r_a;
uniform float r_b;
uniform float r_c;
uniform float r_d;

// MOD PERLIN {
// Note: Code inspired by Flafla2 https://adrianb.io/2014/08/09/perlinnoise.html,
// which is a modification of the original code & algorithm by Ken Perlin.
// I adapted it for use on the GPU

float fade(float t) {
  // Fade function as defined by Ken Perlin.  This eases coordinate values
  // so that they will ease towards integral values.  This ends up smoothing
  // the final output.

  // 6t^5 - 15t^4 + 10t^3
  return t * t * t * (t * (t * 6. - 15.) + 10.);         
}

int inc(int num) {
  return num + 1;
}

// Doubled permutation to avoid overflow
int p[512];                                                    

// Source: http://riven8192.blogspot.com/2010/08/calculate-perlinnoise-twice-as-fast.html
// calculates the dot product of the vector from corner to point with the corner gradient vector
float grad(int hash, float x, float y, float z) {
  switch(hash & 0xF) {
    case 0x0: return  x + y;
    case 0x1: return -x + y;
    case 0x2: return  x - y;
    case 0x3: return -x - y;
    case 0x4: return  x + z;
    case 0x5: return -x + z;
    case 0x6: return  x - z;
    case 0x7: return -x - z;
    case 0x8: return  y + z;
    case 0x9: return -y + z;
    case 0xA: return  y - z;
    case 0xB: return -y - z;
    case 0xC: return  y + x;
    case 0xD: return -y + z;
    case 0xE: return  y - x;
    case 0xF: return -y - z;
    default: return 0; // never happens
  }
}

// perlin
float perlin(vec3 pos) {
  // assert: p in [0.,1.]x[0.,1.]x[0.,1.]
  // this is obviously not always the case, however for this convoluted program it is


  // 8 bit = 256
  int xi = int(pos.x) & 255;
  int yi = int(pos.y) & 255;
  int zi = int(pos.z) & 255;

  float xf = pos.x - floor(pos.x);
  float yf = pos.y - floor(pos.y);
  float zf = pos.z - floor(pos.z);

  // values which will be used to avg the weight values 
  float u = fade(xf);
  float v = fade(yf);
  float w = fade(zf);

  // hash each corner
  int aaa, aba, aab, abb, baa, bba, bab, bbb;
  aaa = p[p[p[    xi ]+    yi ]+    zi ];
  aba = p[p[p[    xi ]+inc(yi)]+    zi ];
  aab = p[p[p[    xi ]+    yi ]+inc(zi)];
  abb = p[p[p[    xi ]+inc(yi)]+inc(zi)];
  baa = p[p[p[inc(xi)]+    yi ]+    zi ];
  bba = p[p[p[inc(xi)]+inc(yi)]+    zi ];
  bab = p[p[p[inc(xi)]+    yi ]+inc(zi)];
  bbb = p[p[p[inc(xi)]+inc(yi)]+inc(zi)];

  float x1, x2, y1, y2;

  x1 = mix(grad(aaa, xf, yf, zf), grad(baa, xf - 1, yf, zf), u);                                   
  x2 = mix(grad(aba, xf, yf - 1, zf), grad(bba, xf - 1, yf - 1, zf), u);
  y1 = mix(x1, x2, v);
  x1 = mix(grad(aab, xf, yf, zf - 1), grad(bab, xf - 1, yf, zf - 1), u);
  x2 = mix(grad(abb, xf, yf - 1, zf - 1), grad (bbb, xf - 1, yf - 1, zf - 1), u);
  y2 = mix(x1, x2, v);
    
  // val in [-1,1]
  float val = mix(y1, y2, w);
  // val in [0, 1]
  float val_normalized = (val + 1.) / 2.;

  return val_normalized;
}

// Hash lookup table as defined by Ken Perlin.  This is a randomly
// arranged array of all numbers from 0-255 inclusive.
const int[] permutation = { 151,160,137,91,90,15,                 
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,    
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};


void perlin_init() {
    // initialize a 512 permutation array
    for(int x = 0; x < 512; x++) {
        p[x] = permutation[x % 256];
    }
}

float octave_perlin(vec3 pos, int octaves, float persistence) {
    float total = 0.;
    float frequency = 1.;
    float amplitude = 1.;

    float maxValue = 0.;  // Used for normalizing result to 0.0 - 1.0
    for(int i = 0; i < octaves; i++) {
        total += perlin(frequency * pos) * amplitude;
        
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= 2.;
    }
    
    return total / maxValue;
}

// ENDMOD PERLIN

float luminance_of(vec3 v) {
	float luminance = dot(vec3(0.299, 0.587, 0.114), v);
	return luminance;
}

// basically smoothstep but sharp
vec3 color_ramp_linear(vec3 color1, float pos_color1, vec3 color2, float pos_color2, float luminance) {
    // float luminance = luminance_of(colorin);

    vec3 out_color = vec3(0., 0., 0.);

    if (luminance <= pos_color1) {
      out_color = color1;
    } else if (luminance <= pos_color2) {
      out_color = mix(color1, color2, (luminance - pos_color1) / (pos_color2 - pos_color1));
    } else {
      out_color = color2;
    }

    return out_color;
}

// basically smoothstep but sharp
vec3 color_ramp_linear_3(
  vec3 color1, float pos_color1, 
  vec3 color2, float pos_color2, 
  vec3 color3, float pos_color3, 
  float luminance
) {
    // float luminance = luminance_of(colorin);

    vec3 out_color = vec3(0., 0., 0.);

    if (luminance <= pos_color1) {
      out_color = color1;
    } else if (luminance <= pos_color2) {
      out_color = mix(color1, color2, (luminance - pos_color1) / (pos_color2 - pos_color1));
    } else if (luminance <= pos_color3) {
      out_color = mix(color2, color3, (luminance - pos_color2) / (pos_color3 - pos_color2));
    } else {
      out_color = color3;
    }

    return out_color;
}

// basically smoothstep but sharp
vec3 color_ramp_linear_4(
  vec3 color1, float pos_color1, 
  vec3 color2, float pos_color2, 
  vec3 color3, float pos_color3, 
  vec3 color4, float pos_color4, 
  float luminance
) {
    // float luminance = luminance_of(colorin);

    vec3 out_color = vec3(0., 0., 0.);

    if (luminance <= pos_color1) {
      out_color = color1;
    } else if (luminance <= pos_color2) {
      out_color = mix(color1, color2, (luminance - pos_color1) / (pos_color2 - pos_color1));
    } else if (luminance <= pos_color3) {
      out_color = mix(color2, color3, (luminance - pos_color2) / (pos_color3 - pos_color2));
    } else if (luminance <= pos_color4) {
      out_color = mix(color3, color4, (luminance - pos_color3) / (pos_color4 - pos_color3));
    } else {
      out_color = color4;
    }

    return out_color;
}


vec3 wave_texture() {
	vec3 wave_color = vec3(0., 0., 0.);

	int num_stripes = 10;

	float a = 0.;
	float b = 1.;
	float range = b - a;

	float dx = range / float(num_stripes);

  // v_st
	int stripe_num = int((v_st.s / range) /*ratio of 0 to 1*/ * float(num_stripes));

	float stripe_center = a + float(stripe_num) * dx + dx / 2.;

	float stripe_radius = 0.01;
	// float tolerance = 0.01;
	float d = abs(v_st.s - stripe_center);
	float t = d / stripe_radius;

	// wave_one_color = mix(vec3(0., 0., 0.), vec3(1., 1., 1.), d / stripe_radius);
	// https://dev.to/thehalfbloodprince/stripes-in-shaders-3n5n

	// 2pi / b
	// if we want 50 stripes...
	// float f = fract( A * (cameraSpacePosXNormalized / 2.) );

	// float xpos = d / stripe_radius;
	// float p = -2.8;
	// float r = 3.1;
	// float t = exp(r * xpos + p) / (1. + exp(r * xpos + p));
	// float t = smoothstep( 0.5-uP-uTol, 0.5-uP+uTol, f ) - smoothstep( 0.5+uP-uTol, 0.5+uP+uTol, f );

	wave_color = mix(vec3(0., 0., 0.), vec3(1., 1., 1.), (log(t)+2.)/3.2);
	wave_color = clamp(wave_color, vec3(0., 0., 0.), vec3(1., 1., 1.));

	return wave_color;

	// float s = A * sin(B * cameraSpacePosXNormalized) + C;
	// if (s < 0.902) {
	// 	s /= C;
	// }

	// wave_one_color = mix(vec3(0., 0., 0.), vec3(1., 1., 1.), s);
}



void main()
{
  perlin_init();

  float ambient_strength = 0.1;
  float diffuse_strength = 0.5;
  float specular_strength = 0.5;
  float specular_factor = 1.;

  vec3 v_light_dir = normalize(-v_wc + light_pos);
  vec3 v_eye_dir = normalize(-v_wc + eye);

  vec3 ambient = ambient_strength * light_color;

  float diff = max(dot(v_light_dir, v_n), 0.);
  vec3 diffuse = diffuse_strength * diff * light_color;

  vec3 reflect_ray = reflect(-v_light_dir, v_n);
  float spec = pow(max(dot(reflect_ray, v_eye_dir), 0.), specular_factor);
  vec3 specular = specular_strength * spec * light_color;

  // vec3 tex_color_1 = texture(texture_1, v_st).rgb;
  // vec3 tex_color_2 = texture(texture_2, v_st).rgb;

  vec3 phong = ambient + diffuse + specular;

  // right now, the closer it is to the center, the darker it is
  // this is good, however, if it is at the edge we need to it be really light
  // an exponential function

  // attempt to implement voronoi distance to edge, but without the jitter, so it's literally a square grid
  float cell_width = 0.02;
  // get center cell based on current position
  // code inspired by blender source:
  // https://github.com/blender/blender/blob/3135e766ec1055be7f08a93cff51fe1ed8b453d1/intern/cycles/kernel/svm/voronoi.h#L551
  // except it's without the randomness/jitter, so it's basically a 1000x1000 rubix cube, boring 3d square grid

  vec3 generator = floor(v_wc / cell_width) * cell_width + 0.5 * cell_width;

  // 1 is a relatively huge number
  float min_dist = 1.;

  vec3 to_closest_generator ;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      for (int k = -1; k <= 1; k++) {
        vec3 neighbor_generator = generator + cell_width * vec3(float(i), float(j), float(k));

        vec3 to_neighbor_generator = -v_wc + neighbor_generator;
        if (length(to_neighbor_generator) < min_dist) {
          min_dist = length(to_neighbor_generator);
          to_closest_generator = to_neighbor_generator;
        }
      }
    }
  }

  min_dist = 1.;

  // find distance edge, normalized to [0,1] <- this is the important part
  for (int k = -1; k <= 1; k++) {
    for (int j = -1; j <= 1; j++) {
      for (int i = -1; i <= 1; i++) {
        vec3 neighbor_generator = generator + cell_width * vec3(float(i), float(j), float(k));

        vec3 to_neighbor_generator = -v_wc + neighbor_generator;

        vec3 perpendicular_to_edge = -to_closest_generator + to_neighbor_generator;

        if (dot(perpendicular_to_edge, perpendicular_to_edge) > 0.000001f) {
          float distance_to_edge = dot(normalize((to_closest_generator + to_neighbor_generator) / 2.0f),
                                     normalize(perpendicular_to_edge));
          min_dist = min(min_dist, distance_to_edge);
        }
      }
    }
  }

  float voronoi = min_dist;

  float voronoi_and_noise = mix(voronoi, octave_perlin(v_wc, 12, 0.86), 0.878);

  // 0.379, 0.438
  vec3 vor_noi_cr = color_ramp_linear(vec3(1., 1., 1.), 0.379, vec3(0., 0., 0.), 0.438, voronoi_and_noise);
  // vec3 vor_noi_cr = color_ramp_linear(vec3(1., 1., 1.), 0.063, vec3(0., 0., 0.), 0.253, voronoi_and_noise);

  vec3 wave_cr = color_ramp_linear_3(
    vec3(0.6941, 0.6941, 0.6941),
    0., 
    vec3(0.7333, 0.7373, 0.7373),
    0.114,
    vec3(1., 1., 1.),
    1., 
    luminance_of(wave_texture())
  );

  // vec3 wav_noi_lerp = mix(wave_cr, octave_perlin(v_wc + vec2(0.001, 0.001), 12, 0.86) * vec3(1., 1., 1.), 0.5);
  vec3 wav_noi_lerp = mix(wave_cr, octave_perlin(v_wc + vec3(0.0001, 0.0001, 0.0001), 12, 0.86) * vec3(1., 1., 1.), 0.5);

  // THIS IS THE MELON WITHOUT THE SUFF YUMMY :)
  vec3 wav_noi_lerp_cr = color_ramp_linear_4(
    vec3(0.1333, 0.2118, 0.1137),
    0.598, 
    vec3(0.2902, 0.4392, 0.1961),
    0.685,
    vec3(0.3843, 0.5725, 0.2510),
    0.711,
    vec3(0.6392, 0.6902, 0.3765),
    0.781, 
    luminance_of(wav_noi_lerp)
  );

  vec3 wav_noi_lerp_cr_bw = color_ramp_linear(
    vec3(0., 0., 0.),
    0.246, 
    vec3(1., 1., 1.),
    0.369, 
    luminance_of(wav_noi_lerp_cr)
  );

  vec3 full_skeleton_bw = min(clamp(5. * vor_noi_cr, vec3(0., 0., 0.), vec3(1., 1., 1.)), wav_noi_lerp_cr_bw);

  vec3 out_color = mix(wav_noi_lerp_cr, vec3(0.7333, 0.6941, 0.5490), luminance_of(full_skeleton_bw));
  // float wave_and_noise = mix(luminance_of(wave_texture())
  // FragColor = vec4(octave_perlin(v_wc, 7, 2.) * vec3(1., 1., 1.), 1.);

  // FragColor = vec4(vor_noi_cr * phong, 1.); // debug perlin noise

  FragColor = vec4(out_color, 1.);
  // FragColor = vec4(octave_perlin(v_wc, 12, 0.86) * vec3(1., 1., 1.), 1.); // debug perlin noise
}
