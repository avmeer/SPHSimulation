#version 430

uniform float curr_time;
uniform int flip;

layout (local_size_x = 256) in;
layout (std430, binding = 0) buffer inpos {
  vec4 FlipPosition[];
};
layout (std430, binding = 1) buffer invel {
  vec4 FlipVelocity[];
};
layout (std430, binding = 2) buffer outpos {
  vec4 FlopPosition[];
};
layout (std430, binding = 3) buffer outvel {
  vec4 FlopVelocity[];
};

void kernel_func(in uint i, in uint j, in float h, out float value) {
  float q;
  if (flip == 0) {
    q = distance(FlipPosition[i], FlipPosition[j]) / h;
  } else {
    q = distance(FlopPosition[i], FlopPosition[j]) / h;
  }

  if (q <= 1) {
    value = 1;
  } else {
    value = 0;
  }

  // value = 0;
  // if (q <= 1) {
  //   value = (1 - 1.5 * q * q * (1 - 0.5 * q)) / 3.14159 / h / h / h;
  // } else if (q <= 2) {
  //   float tmq = 2 - q;
  //   value = 0.25 * tmq * tmq * tmq / 3.14159 / h / h / h;
  // }
}

void main() {
  float mass = 0.01;

  uint index = gl_GlobalInvocationID.x;
  vec4 in_pos, in_vec, out_pos, out_vec;
  if (flip == 0) {
    in_pos = FlipPosition[index];
    in_vec = FlipVelocity[index];
  } else {
    in_pos = FlopPosition[index];
    in_vec = FlopVelocity[index];
  }
  out_pos = in_pos;
  out_vec = in_vec;

  // out_pos.y = cos(
  //   curr_time * 0.01 +
  //   in_pos.x * 0.5 +
  //   in_pos.z * 0.5
  // ) + 2;

  float rho = 0;
  for (int i = 0; i < 2000; i += 1) {
    float y;
    kernel_func(index, i, 20, y);
    rho = rho + mass * y;
  }
  out_pos.y = rho;

  if (flip == 0) {
    FlopPosition[index] = out_pos;
    FlopVelocity[index] = out_vec;
  } else {
    FlipPosition[index] = out_pos;
    FlipVelocity[index] = out_vec;
  }
}
