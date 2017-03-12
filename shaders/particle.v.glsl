#version 410

struct LightInfo {
  vec3 position;
  vec3 La;
  vec3 Ld;
  vec3 Ls;
};

uniform mat4 mv_mat;
uniform mat4 mvp_mat;
uniform mat3 normal_mat;
uniform LightInfo light;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 light_vec_v;
out vec3 cam_vec_v;
out vec3 normal_vec_v;

void main() {
  vec3 Position = position;
  Position.z=Position.z+5*gl_InstanceID;
  gl_Position = mvp_mat * vec4(Position, 1);

  vec4 eye = mv_mat * vec4(position, 1);

  light_vec_v = normalize(light.position - eye.xyz);
  cam_vec_v = normalize(-eye.xyz);
  normal_vec_v = normalize(normal_mat * normal);
}
