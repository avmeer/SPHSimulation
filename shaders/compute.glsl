#version 430

uniform float time;

layout (local_size_x = 10) in;
layout (std430, binding = 0) buffer positions {
  vec4 Particle[];
};

void main() {
  uint index = gl_GlobalInvocationID.x;
  // Particle[index].y = index;
  // Particle[index].y = Particle[index].y * 2;
  vec4 new_pos = Particle[index];
  new_pos.y = new_pos.y * 2;
  Particle[index] = new_pos;
}
