#version 460 core

out vec4 FragColor;

in vec3 v_n;
in vec3 v_wc;
uniform vec3 eye;

uniform float amplitude;

const vec3 light_color = vec3(1., 1., 1.);
const vec3 light_pos = vec3(5., 5., 5.);

void main()
{
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

  FragColor = vec4((ambient + diffuse + specular), 1.);
}
