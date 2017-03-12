#version 430

uniform float curr_time;

layout (local_size_x = 10) in;
layout (std430, binding = 0) buffer positions {
  vec4 Particle[];
};

void main() {
  uint index = gl_GlobalInvocationID.x;
  Particle[index].y = cos(
    curr_time * 0.01 +
    Particle[index].x * 0.5 +
    Particle[index].z * 0.5
  ) + 2;
}
