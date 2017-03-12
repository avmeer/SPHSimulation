#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <ctime>

#include <string>
#include <fstream>

#include "./include/OpenGLUtils.h"
#include "./include/ShaderUtils.h"
#include "./include/ShaderProgram.h"

#define GROUND 0
#define PARTICLE 1

#define PARTICLES 2000
#define SPHERE_THETA_STEPS 10
#define SPHERE_PHI_STEPS 10

typedef struct {
  float x, y, z;
  float nx, ny, nz;
} vertex;

typedef struct {
  float x, y, z, w;
} position;

const vertex ground_verts[] = {
  {-15, 0, -15, 0, 1, 0},
  {-15, 0,  15, 0, 1, 0},
  { 15, 0,  15, 0, 1, 0},
  { 15, 0, -15, 0, 1, 0}
};

const unsigned short ground_indices[] = {
  0, 2, 1,
  0, 3, 2
};

int window_width, window_height;

GLint l_mouse, r_mouse;

glm::vec2 mouse(-9999, -9999);

glm::vec3 camera_angles(1.82, 2.01, 30);
glm::vec3 eye(10, 10, 10);
glm::vec3 look_at(0, 0, 0);
glm::vec3 up(0, 1, 0);

glm::vec3 light(3, 5, 3);
glm::vec3 light_color(1, 1, 1);

glm::vec3 ground_mat_a(0.0215, 0.1745, 0.0215);
glm::vec3 ground_mat_d(0.07568, 0.61424, 0.07568);
glm::vec3 ground_mat_s(0.633, 0.727811, 0.633);
float ground_mat_sh = 0.6 * 128;

glm::vec3 particle_mat_a(0, 0.1, 0.06);
glm::vec3 particle_mat_d(0, 0, 0.7);
glm::vec3 particle_mat_s(0.50196078, 0.50196078, 0.50196078);
float particle_mat_sh = 0.25 * 128;

CSCI441::ShaderProgram* ground_shader = NULL;
CSCI441::ShaderProgram* particle_shader = NULL;
GLuint compute_shader = NULL;

struct GULocs {
  GLint mv_mat;
  GLint mvp_mat;
  GLint normal_mat;
  GLint light_position;
  GLint light_La;
  GLint light_Ld;
  GLint light_Ls;
  GLint mat_Ka;
  GLint mat_Kd;
  GLint mat_Ks;
  GLint mat_shiny;
} ground_u;
struct GALocs {
  GLint position;
  GLint normal;
} ground_a;

struct PULocs {
  GLint mv_mat;
  GLint mvp_mat;
  GLint normal_mat;
  GLint light_position;
  GLint light_La;
  GLint light_Ld;
  GLint light_Ls;
  GLint mat_Ka;
  GLint mat_Kd;
  GLint mat_Ks;
  GLint mat_shiny;
} particle_u;
struct PALocs {
  GLint position;
  GLint normal;
} particle_a;

struct CULocs {
  GLint curr_time;
} compute_u;

GLuint vaods[2];

float curr_time;
position* particle_locs;
GLuint compute_vaods[1];
GLuint position_buffer;

double step;
clock_t prev, next;

void calculate_camera_pos() {
  eye.x = camera_angles.z * sinf(camera_angles.x) * sinf(camera_angles.y);
  eye.y = camera_angles.z * -cosf(camera_angles.y);
  eye.z = camera_angles.z * -cosf(camera_angles.x) * sinf(camera_angles.y);
}

void resize(int w, int h) {
  window_width = w;
  window_height = h;

  glViewport(0, 0, w, h);
}

void mouse_act(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) {
    l_mouse = state;
  } else if (button == GLUT_RIGHT_BUTTON) {
    r_mouse = state;
  }

  mouse.x = x;
  mouse.y = y;
}

void mouse_move(int x, int y) {
  if (l_mouse == GLUT_DOWN) {
    camera_angles.x += (x - mouse.x) * 0.005;
    camera_angles.y += (y - mouse.y) * 0.005;

    if (camera_angles.y <= 0) {
      camera_angles.y = 0.001;
    } else if (camera_angles.y >= M_PI) {
      camera_angles.y = M_PI - 0.001;
    }

    calculate_camera_pos();
  } else if (r_mouse == GLUT_DOWN) {
    camera_angles.z += (y - mouse.y) * 0.1;

    if (camera_angles.z < 1) {
      camera_angles.z = 1;
    } else if (camera_angles.z > 50) {
      camera_angles.z = 50;
    }

    calculate_camera_pos();
  }

  mouse.x = x;
  mouse.y = y;
}

void normal_keys(unsigned char key, int x, int y) {
  if (key == 'q' || key == 'Q' || key == 27) {
    exit(0);
  }
}

void update_particles() {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, position_buffer);

  glUseProgram(compute_shader);
  glUniform1f(compute_u.curr_time, curr_time);
  glDispatchCompute(PARTICLES / 10, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  GLfloat buffer_data[4 * PARTICLES];
  glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4 * PARTICLES * sizeof(GLfloat), buffer_data);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  for (int i = 0; i < 4 * PARTICLES; i += 4) {
    particle_locs[i / 4].x = buffer_data[i + 0];
    particle_locs[i / 4].y = buffer_data[i + 1];
    particle_locs[i / 4].z = buffer_data[i + 2];
    particle_locs[i / 4].w = buffer_data[i + 3];
  }
}

void render_timer(int value) {
  next = clock();
  double d = (next - prev) / (double)CLOCKS_PER_SEC;
  prev = next;
  double fps = 1.0 / d;

  step += d;
  if (step > 1) {
    char wtitle[27];
    sprintf(wtitle, "SPH simulation (FPS: %5.2f)", fps);
    glutSetWindowTitle(wtitle);
    step -= 1;
  }

  curr_time = prev;
  update_particles();
  glutPostRedisplay();

  glutTimerFunc((unsigned int)(1000.0 / 60.0), render_timer, 0);
}

void render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float ratio = (float)window_width / (float)window_height;
  glm::mat3 normal_mat;
  glm::mat4 m_mat, v_mat, p_mat, mv_mat, mvp_mat;

  p_mat = glm::perspective(45.0f, ratio, 0.001f, 100.0f);
  v_mat = glm::lookAt(eye, look_at, up);
  mv_mat = v_mat * m_mat;
  mvp_mat = p_mat * mv_mat;
  normal_mat = glm::transpose(glm::inverse(glm::mat3(mv_mat)));

  ground_shader->useProgram();

  glUniformMatrix4fv(ground_u.mv_mat, 1, GL_FALSE, &mv_mat[0][0]);
  glUniformMatrix4fv(ground_u.mvp_mat, 1, GL_FALSE, &mvp_mat[0][0]);
  glUniformMatrix3fv(ground_u.normal_mat, 1, GL_FALSE, &normal_mat[0][0]);
  glUniform3fv(ground_u.light_position, 1, &light[0]);
  glUniform3fv(ground_u.light_La, 1, &light_color[0]);
  glUniform3fv(ground_u.light_Ld, 1, &light_color[0]);
  glUniform3fv(ground_u.light_Ls, 1, &light_color[0]);
  glUniform3fv(ground_u.mat_Ka, 1, &ground_mat_a[0]);
  glUniform3fv(ground_u.mat_Ks, 1, &ground_mat_s[0]);
  glUniform3fv(ground_u.mat_Kd, 1, &ground_mat_d[0]);
  glUniform1f(ground_u.mat_shiny, ground_mat_sh);

  glBindVertexArray(vaods[GROUND]);
  glDrawElements(GL_TRIANGLES, sizeof(ground_indices) / sizeof(unsigned short), GL_UNSIGNED_SHORT, (void*)0);

  particle_shader->useProgram();

  glUniform3fv(particle_u.light_position, 1, &light[0]);
  glUniform3fv(particle_u.light_La, 1, &light_color[0]);
  glUniform3fv(particle_u.light_Ld, 1, &light_color[0]);
  glUniform3fv(particle_u.light_Ls, 1, &light_color[0]);
  glUniform3fv(particle_u.mat_Ka, 1, &particle_mat_a[0]);
  glUniform3fv(particle_u.mat_Ks, 1, &particle_mat_s[0]);
  glUniform3fv(particle_u.mat_Kd, 1, &particle_mat_d[0]);
  glUniform1f(particle_u.mat_shiny, particle_mat_sh);

  for (int i = 0; i < PARTICLES; i += 1) {
    m_mat = glm::mat4(
      glm::vec4(1, 0, 0, 0),
      glm::vec4(0, 1, 0, 0),
      glm::vec4(0, 0, 1, 0),
      glm::vec4(
        particle_locs[i].x,
        particle_locs[i].y,
        particle_locs[i].z,
      1)
    );
    mv_mat = v_mat * m_mat;
    mvp_mat = p_mat * mv_mat;
    normal_mat = glm::transpose(glm::inverse(glm::mat3(mv_mat)));

    glUniformMatrix4fv(particle_u.mv_mat, 1, GL_FALSE, &mv_mat[0][0]);
    glUniformMatrix4fv(particle_u.mvp_mat, 1, GL_FALSE, &mvp_mat[0][0]);
    glUniformMatrix3fv(particle_u.normal_mat, 1, GL_FALSE, &normal_mat[0][0]);

    glBindVertexArray(vaods[PARTICLE]);
    glDrawElements(
      GL_TRIANGLES,
      6 * SPHERE_THETA_STEPS * SPHERE_PHI_STEPS,
      GL_UNSIGNED_SHORT,
      (void*)0
    );
  }

  glutSwapBuffers();
}

void setup_GLUT(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitContextVersion(4, 3);
  glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEBUG);
  glutInitWindowPosition(50, 50);
  glutInitWindowSize(window_width, window_height);
  glutCreateWindow("SPH Fluid Simulation");

  glutKeyboardFunc(normal_keys);
  glutDisplayFunc(render);
  glutReshapeFunc(resize);
  glutMouseFunc(mouse_act);
  glutMotionFunc(mouse_move);
  glutTimerFunc((unsigned int)(1000.0 / 60.0), render_timer, 0);
}

void setup_OGL() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glFrontFace(GL_CW);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0, 0, 0, 1);

  glewExperimental = GL_TRUE;
  GLenum glewResult = glewInit();

  if (glewResult != GLEW_OK) {
    printf("Error initializing GLEW.\n");
    exit(1);
  }

  CSCI441::OpenGLUtils::printOpenGLInfo();
}

void setup_shaders() {
  ground_shader = new CSCI441::ShaderProgram(
    "shaders/ground.v.glsl",
    "shaders/ground.f.glsl"
  );
  particle_shader = new CSCI441::ShaderProgram(
    "shaders/particle.v.glsl",
    "",
    "shaders/particle.f.glsl",
    ""
  );

  ground_u.mv_mat = ground_shader->getUniformLocation("mv_mat");
  ground_u.mvp_mat = ground_shader->getUniformLocation("mvp_mat");
  ground_u.normal_mat = ground_shader->getUniformLocation("normal_mat");
  ground_u.light_position = ground_shader->getUniformLocation("light.position");
  ground_u.light_La = ground_shader->getUniformLocation("light.La");
  ground_u.light_Ld = ground_shader->getUniformLocation("light.Ld");
  ground_u.light_Ls = ground_shader->getUniformLocation("light.Ls");
  ground_u.mat_Ka = ground_shader->getUniformLocation("mat.Ka");
  ground_u.mat_Kd = ground_shader->getUniformLocation("mat.Kd");
  ground_u.mat_Ks = ground_shader->getUniformLocation("mat.Ks");
  ground_u.mat_shiny = ground_shader->getUniformLocation("mat.shiny");

  ground_a.position = ground_shader->getAttributeLocation("position");
  ground_a.normal = ground_shader->getAttributeLocation("normal");

  particle_u.mv_mat = particle_shader->getUniformLocation("mv_mat");
  particle_u.mvp_mat = particle_shader->getUniformLocation("mvp_mat");
  particle_u.normal_mat = particle_shader->getUniformLocation("normal_mat");
  particle_u.light_position = particle_shader->getUniformLocation("light.position");
  particle_u.light_La = particle_shader->getUniformLocation("light.La");
  particle_u.light_Ld = particle_shader->getUniformLocation("light.Ld");
  particle_u.light_Ls = particle_shader->getUniformLocation("light.Ls");
  particle_u.mat_Ka = particle_shader->getUniformLocation("mat.Ka");
  particle_u.mat_Kd = particle_shader->getUniformLocation("mat.Kd");
  particle_u.mat_Ks = particle_shader->getUniformLocation("mat.Ks");
  particle_u.mat_shiny = particle_shader->getUniformLocation("mat.shiny");

  particle_a.position = particle_shader->getAttributeLocation("position");
  particle_a.normal = particle_shader->getAttributeLocation("normal");
  
  compute_shader = glCreateProgram();
  glAttachShader(
    compute_shader, 
    CSCI441::ShaderUtils::compileShader("shaders/compute.glsl", GL_COMPUTE_SHADER)
  );
  glLinkProgram(compute_shader);

  compute_u.curr_time = glGetUniformLocation(compute_shader, "curr_time");
}

void setup_buffers() {
  glGenVertexArrays(2, vaods);
  GLuint vbods[2];

  // The ground.
  glBindVertexArray(vaods[GROUND]);
  glGenBuffers(2, vbods);
  glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ground_verts), ground_verts, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ground_indices), ground_indices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(ground_a.position);
  glVertexAttribPointer(ground_a.position, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(ground_a.normal);
  glVertexAttribPointer(ground_a.normal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

  // A particle.
  int quads = SPHERE_THETA_STEPS * SPHERE_PHI_STEPS;
  vertex* particle_verts = new vertex[4 * quads];
  unsigned short* particle_indices = new unsigned short[6 * quads];

  for (int i = 0; i < SPHERE_THETA_STEPS; i += 1) {
    float left = ((float)i / (float)SPHERE_THETA_STEPS) * 2 * 3.14159;
    float right = ((float)(i + 1) / (float)SPHERE_THETA_STEPS) * 2 * 3.14159;
    for (int j = 0; j < SPHERE_PHI_STEPS; j += 1) {
      float top = ((float)j / (float)SPHERE_PHI_STEPS) * 3.14159;
      float bottom = ((float)(j + 1) / (float)SPHERE_PHI_STEPS) * 3.14159;
      int k = i * SPHERE_PHI_STEPS + j;

      particle_verts[k * 4 + 0].x = cosf(left) * sinf(top);
      particle_verts[k * 4 + 0].y = cosf(top);
      particle_verts[k * 4 + 0].z = sinf(left) * sinf(top);
      particle_verts[k * 4 + 0].nx = particle_verts[k * 4 + 0].x;
      particle_verts[k * 4 + 0].ny = particle_verts[k * 4 + 0].y;
      particle_verts[k * 4 + 0].nz = particle_verts[k * 4 + 0].z;

      particle_verts[k * 4 + 1].x = cosf(right) * sinf(top);
      particle_verts[k * 4 + 1].y = cosf(top);
      particle_verts[k * 4 + 1].z = sinf(right) * sinf(top);
      particle_verts[k * 4 + 1].nx = particle_verts[k * 4 + 1].x;
      particle_verts[k * 4 + 1].ny = particle_verts[k * 4 + 1].y;
      particle_verts[k * 4 + 1].nz = particle_verts[k * 4 + 1].z;

      particle_verts[k * 4 + 2].x = cosf(right) * sinf(bottom);
      particle_verts[k * 4 + 2].y = cosf(bottom);
      particle_verts[k * 4 + 2].z = sinf(right) * sinf(bottom);
      particle_verts[k * 4 + 2].nx = particle_verts[k * 4 + 2].x;
      particle_verts[k * 4 + 2].ny = particle_verts[k * 4 + 2].y;
      particle_verts[k * 4 + 2].nz = particle_verts[k * 4 + 2].z;

      particle_verts[k * 4 + 3].x = cosf(left) * sinf(bottom);
      particle_verts[k * 4 + 3].y = cosf(bottom);
      particle_verts[k * 4 + 3].z = sinf(left) * sinf(bottom);
      particle_verts[k * 4 + 3].nx = particle_verts[k * 4 + 3].x;
      particle_verts[k * 4 + 3].ny = particle_verts[k * 4 + 3].y;
      particle_verts[k * 4 + 3].nz = particle_verts[k * 4 + 3].z;

      particle_indices[k * 6 + 0] = k * 4 + 0;
      particle_indices[k * 6 + 1] = k * 4 + 2;
      particle_indices[k * 6 + 2] = k * 4 + 1;
      particle_indices[k * 6 + 3] = k * 4 + 0;
      particle_indices[k * 6 + 4] = k * 4 + 3;
      particle_indices[k * 6 + 5] = k * 4 + 2;
    }
  }

  glBindVertexArray(vaods[PARTICLE]);
  glGenBuffers(2, vbods);
  glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
  glBufferData(GL_ARRAY_BUFFER, 4 * quads * sizeof(vertex), particle_verts, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * quads * sizeof(unsigned int), particle_indices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(particle_a.position);
  glVertexAttribPointer(particle_a.position, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(particle_a.normal);
  glVertexAttribPointer(particle_a.normal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

  // Buffer for the compute shader.
  glGenBuffers(1, &position_buffer);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, position_buffer);
  glBufferData(
    GL_SHADER_STORAGE_BUFFER,
    PARTICLES * sizeof(position),
    &particle_locs[0],
    GL_DYNAMIC_DRAW
  );

  glGenVertexArrays(1, compute_vaods);
  glBindVertexArray(compute_vaods[0]);

  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void setup_particles() {
  particle_locs = new position[PARTICLES];
  int sp = sqrt(PARTICLES);
  float offset = -15;
  float width = 30;
  float step = width / sp;

  for (int i = 0; i < sp; i += 1) {
    for (int j = 0; j < sp; j += 1) {
      particle_locs[i * sp + j].x = i * step + offset;
      particle_locs[i * sp + j].y = 2;
      particle_locs[i * sp + j].z = j * step + offset;
      particle_locs[i * sp + j].w = 1;
    }
  }
}

int main(int argc, char** argv) {
  curr_time = 0;
  step = 0;
  prev = 0;

  setup_GLUT(argc, argv);
  setup_OGL();
  setup_shaders();
  setup_particles();
  setup_buffers();

  calculate_camera_pos();

  glutMainLoop();

  delete ground_shader;
  delete particle_shader;

  return 0;
}
