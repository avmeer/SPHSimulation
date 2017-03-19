#version 430

uniform float curr_time;
uniform int flip;
uniform int phase;

layout (local_size_x = 256) in;
layout (std430, binding = 0) buffer pos0 {
  vec4 FlipPosition[];
};
layout (std430, binding = 1) buffer vel0 {
  vec4 FlipVelocity[];
};
layout (std430, binding = 2) buffer den0 {
  float FlipDensity[];
};
layout (std430, binding = 3) buffer prs0 {
  float FlipPressure[];
};
layout (std430, binding = 4) buffer pos1 {
  vec4 FlopPosition[];
};
layout (std430, binding = 5) buffer vel1 {
  vec4 FlopVelocity[];
};
layout (std430, binding = 6) buffer den1 {
  float FlopDensity[];
};
layout (std430, binding = 7) buffer prs1 {
  float FlopPressure[];
};

void kernel_func(in vec4 pi, in vec4 pj, in float h, out float value) {
  // This is the Gaussian kernel.
  float x = distance(pi, pj);
  value = exp(-1 * x * x / h / h);
  value /= h;
  value /= sqrt(3.14159);
  value *= 32;

  // This is a cubic spline kernel, which is supposed to be more efficient.
  // value = 0;
  // if (q <= 1) {
  //   value = (1 - 1.5 * q * q * (1 - 0.5 * q)) / 3.14159 / h / h / h;
  // } else if (q <= 2) {
  //   float tmq = 2 - q;
  //   value = 0.25 * tmq * tmq * tmq / 3.14159 / h / h / h;
  // }
}

void gradient_kernel_func(in vec4 pi, in vec4 pj, in float h, out vec4 value) {
  // These are the derivatives of the Gaussian kernel.
  float a = distance(pi, pj);
  float b = exp(-1 * a * a / h / h);
  b /= (h * h * h);
  b /= sqrt(3.14159);
  b *= 2;
  b *= 10;

  value.x = b * (pj.x - pi.x);
  value.y = b * (pj.y - pi.y);
  value.z = b * (pj.z - pi.z);
}

void get_in_pos(in int i, out vec4 in_pos) {
  if (flip == 0) {
    in_pos = FlipPosition[i];
  } else {
    in_pos = FlopPosition[i];
  }
}

void get_in_vel(in int i, out vec4 in_vel) {
  if (flip == 0) {
    in_vel = FlipVelocity[i];
  } else {
    in_vel = FlopVelocity[i];
  }
}

void get_in_den(in int i, out float in_del) {
  if (flip == 0) {
    in_del = FlopDensity[i];
  } else {
    in_del = FlipDensity[i];
  }
}

void get_in_prs(in int i, out float in_prs) {
  if (flip == 0) {
    in_prs = FlopPressure[i];
  } else {
    in_prs = FlipPressure[i];
  }
}

void main() {
  float mass = 0.01;
  vec4 gravity = vec4(0, -0.01, 0, 0);
  float h = 10;
  float rho_0 = 4;
  float k = 0.5;

  int index = int(gl_GlobalInvocationID.x);
  vec4 in_pos, in_vel, out_pos, out_vel;
  float in_den, in_prs, out_den, out_prs;
  get_in_pos(index, in_pos);
  get_in_vel(index, in_vel);
  get_in_den(index, in_den);
  get_in_prs(index, in_prs);
  out_pos = in_pos;
  out_vel = in_vel;
  out_den = in_den;
  out_prs = in_prs;

  if (phase == 0) {
    out_den = 0;
    for (int i = 0; i < 2000; i += 1) {
      float w;
      vec4 other_pos;
      get_in_pos(i, other_pos);
      kernel_func(in_pos, other_pos, h, w);
      out_den += mass * w;
    }

    out_prs = k * (pow(out_den / rho_0, 7) - 1);
  } else if (phase == 1) {
    vec4 f_pressure, f_viscosity, f_other;

    for (int i = 0; i < 2000; i += 1) {
      vec4 dw;
      vec4 other_pos;
      get_in_pos(i, other_pos);
      gradient_kernel_func(in_pos, other_pos, h, dw);

      float other_den, other_prs;
      get_in_den(i, other_den);
      get_in_prs(i, other_prs);
      dw *= (in_prs / in_den / in_den + other_prs / other_den / other_den);
      dw *= mass;

      f_pressure += dw;
    }
    f_pressure *= -mass;

    f_viscosity = vec4(0, 0, 0, 0);

    f_other = mass * gravity;

    vec4 f = f_pressure + f_viscosity + f_other;
    f /= mass;
    // f *= delta_t;

    out_vel = in_vel + f;
  }

  out_pos += out_vel;

  if (flip == 0) {
    FlopPosition[index] = out_pos;
    FlopVelocity[index] = out_vel;
    FlopDensity[index] = out_den;
    FlopPressure[index] = out_prs;
  } else {
    FlipPosition[index] = out_pos;
    FlipVelocity[index] = out_vel;
    FlipDensity[index] = out_den;
    FlipPressure[index] = out_prs;
  }
}
