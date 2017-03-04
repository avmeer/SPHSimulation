#version 410

uniform mat4 mv_mat;
uniform mat4 mvp_mat;
uniform mat4 normal_mat;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

void main() {
  gl_Position = mvp_mat * vec4(position, 1);
}
