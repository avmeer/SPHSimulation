#version 410

struct LightInfo {
  vec3 position;
  vec3 La;
  vec3 Ld;
  vec3 Ls;
};
struct MatInfo {
  vec3 Ka;
  vec3 Kd;
  vec3 Ks;
  float shiny;
};

uniform LightInfo light;
uniform MatInfo mat;

in vec3 light_vec_v;
in vec3 cam_vec_v;
in vec3 normal_vec_v;

layout (location = 0) out vec4 color_f;

void main() {
  vec3 normal = normalize(normal_vec_v);
  if (!gl_FrontFacing) {
    normal = -normal;
  }
  vec3 halfway = normalize(light_vec_v + cam_vec_v);
  float diffuse_wt = max(dot(light_vec_v, normal), 0);
  float specular_wt = pow(max(dot(normal, halfway), 0), 4 * mat.shiny);

  vec3 ambient = light.La * mat.Ka;
  vec3 diffuse = light.Ld * mat.Kd * diffuse_wt;
  vec3 specular = light.Ls * mat.Ks * specular_wt;

  color_f = vec4(ambient + diffuse + specular, 1);
  if (!gl_FrontFacing) {
    color_f.rgb = color_f.bgr;
  }
}
