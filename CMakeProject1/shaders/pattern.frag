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

layout(binding = 0, std430) readonly buffer SSBO {
    // The layout here should match the data structure in C++
    vec4 random_points[];
};

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

  // get distance to closest cell
  float min_dist = 1.1;
  float second_min_dist = 1.0;

  for (int i = 0; i < random_points.length(); i++) {
    if (distance(v_wc, random_points[i].xyz) < min_dist) {
      min_dist = distance(v_wc, random_points[i].xyz);
    } else if (distance(v_wc, random_points[i].xyz) < second_min_dist) {
      second_min_dist = distance(v_wc, random_points[i].xyz);
    }
  }
  // right now, the closer it is to the center, the darker it is
  // this is good, however, if it is at the edge we need to it be really light
  // an exponential function

  // FragColor = vec4(octave_perlin(v_wc, 7, 2.) * vec3(1., 1., 1.), 1.);
  FragColor = vec4(((second_min_dist + min_dist)/2.) * vec3(1., 1., 1.), 1.); // debug perlin noise
}
