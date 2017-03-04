#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <fstream>

#include "./include/OpenGLUtils.h"
#include "./include/ShaderUtils.h"
#include "./include/ShaderProgram.h"

typedef struct {
  float x, y, z;
  float nx, ny, nz;
} vertex;

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

glm::vec3 camera_angles(1.82, 2.01, 15);
glm::vec3 eye(10, 10, 10);
glm::vec3 look_at(0, 0, 0);
glm::vec3 up(0, 1, 0);

glm::vec3 light(3, 5, 3);
glm::vec3 light_color(1, 1, 1);

CSCI441::ShaderProgram* ground_shader = NULL;
CSCI441::ShaderProgram* particle_shader = NULL;

struct GULocs {
  GLint mv_mat;
  GLint mvp_mat;
  GLint normal_mat;
} ground_u;
struct GALocs {
  GLint position;
  GLint normal;
} ground_a;

GLuint vaods[1];

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

void render_timer(int value) {
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

  glBindVertexArray(vaods[0]);
  glDrawElements(GL_TRIANGLES, sizeof(ground_indices) / sizeof(unsigned short), GL_UNSIGNED_SHORT, (void*)0);

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

  ground_u.mv_mat = ground_shader->getUniformLocation("mv_mat");
  ground_u.mvp_mat = ground_shader->getUniformLocation("mvp_mat");
  ground_u.normal_mat = ground_shader->getUniformLocation("normal_mat");

  ground_a.position = ground_shader->getAttributeLocation("position");
  ground_a.normal = ground_shader->getAttributeLocation("normal");
}

void setup_buffers() {
  glGenVertexArrays(1, vaods);
  GLuint vbods[2];

  // The container.
  glBindVertexArray(vaods[0]);
  glGenBuffers(2, vbods);
  glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ground_verts), ground_verts, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ground_indices), ground_indices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(ground_a.position);
  glVertexAttribPointer(ground_a.position, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

  glEnableVertexAttribArray(ground_a.normal);
  glVertexAttribPointer(ground_a.normal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
}

int main(int argc, char** argv) {
  setup_GLUT(argc, argv);
  setup_OGL();
  setup_shaders();
  setup_buffers();

  calculate_camera_pos();

  glutMainLoop();

  delete particle_shader;

  return 0;
}
