#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

typedef struct {
  float x, y;
} Vector;

Vector vector_init(float x, float y) {
  Vector v;
  v.x = x;
  v.y = y;
  return v;
}

Vector vector_init_angle(float angle) {
  Vector v;
  v.x = sin(angle);
  v.y = cos(angle);
  return v;
}

Vector vector_sub(const Vector* a, const Vector* b) {
  Vector result;
  result.x = a->x - b->x;
  result.y = a->y - b->y;
  return result;
}

Vector vector_add(const Vector* a, const Vector* b) {
  Vector result;
  result.x = a->x + b->x;
  result.y = a->y + b->y;
  return result;
}

Vector vector_scale(const Vector* a, float s) {
  Vector result;
  result.x = a->x * s;
  result.y = a->y * s;
  return result;
}

float vector_mag(const Vector* a) {
  return sqrt(a->x * a->x + a->y * a->y);
}

Vector vector_norm(const Vector* a) {
  float s = vector_mag(a);
  return vector_scale(a, 1.0 / s);
}

float vector_dist(const Vector* a, const Vector* b) {
  float dx = a->x - b->x;
  float dy = a->y - b->y;
  return sqrt(dx * dx + dy * dy);
}

float vector_angle(const Vector* a) {
  return atan2(a->x, a->y);
}

typedef struct {
  Vector p, v;
  float m;
} Particle;

Vector particle_drag_force(const Particle* p, float c) {
  return vector_scale(&p->v, -c);
}


Vector gravity_force(const Particle* p) {
  Vector a = {0, -9.81};
  return vector_scale(&a, p->m);
}

void particle_step(Particle* p, float dt) {
  Vector fg = gravity_force(p);
  Vector fd = particle_drag_force(p, .2);
  Vector f = vector_add(&fg, &fd);

  Vector a = vector_scale(&f, 1.0 / p->m); // f = ma

  // a = dv / dt
  Vector dv = vector_scale(&a, dt);
  p->v = vector_add(&p->v, &dv);

  // v = dx / dt
  Vector dx = vector_scale(&p->v, dt);
  p->p = vector_add(&p->p, &dx);
}

typedef struct {
  unsigned int n;
  Particle particles[0];
} ParticleSystem;

/*
 * produce a random number uniformly distributed between -1 and 1
 */
float random_uniform() {
  long halfmax = RAND_MAX / 2;
  long centered = random() - halfmax;
  return (float)centered / halfmax;
}

/*
 * produce a random number uniformly distributed between 0 and 1
 */
float random_uniformm_positive() {
  return (float)random() / RAND_MAX;
}

/*
 * produce a random initial particle state
 */
void particle_init(Particle* p) {
  // initial position is center, bottom
  p->p = vector_init(0, -1);

  // initial velocity is out and up
  p->v = vector_init(random_uniform() * 1.2, random_uniformm_positive() * 6);

  // mass is random
  p->m = random_uniformm_positive();
}


/*
 * a particle is dead if it's below the bottom of the screen and going
 * down
 */
bool particle_alive(Particle* p) {
  if(p->p.y < -1 && p->v.y < 0) return false;
  return true;
}

ParticleSystem* particlesystem_new(unsigned int n) {
  size_t data_size = sizeof(ParticleSystem) + n * sizeof(Particle);
  printf("creating system with %d particles (data_size = %lu)\n", n, data_size);
  ParticleSystem* system = (ParticleSystem*)malloc(data_size);
  system->n = n;

  // initialize all particles
  for(int i = 0; i < n; ++i) {
    particle_init(&system->particles[i]);
  }

  return system;
}

void particlesystem_step(ParticleSystem* system, float dt) {
  for(int i = 0; i < system->n; ++i) {
    Particle* p = &system->particles[i];
    particle_step(p, dt);

    // if a particle dies, reinitialize
    if(!particle_alive(p)) {
      particle_init(p);
    }
  }
}

typedef struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  int w, h;

  // shaders
  GLuint filled;

  // vbo
  GLuint verts;
} Context;

typedef enum {
  ATTR_VERTEX
} ProgramAttributes;

/*
 * report any pending opengl error messages
 */
void gl_check(const char * msg) {
  GLenum error = glGetError();
  if(error == GL_NO_ERROR) return;

  const char* e_msg;
  switch(error) {
  case GL_INVALID_ENUM:
    e_msg = "GL_INVALID_ENUM";
    break;
  case GL_INVALID_VALUE:
    e_msg = "GL_INVALID_VALUE";
    break;
  case GL_INVALID_OPERATION:
    e_msg = "GL_INVALID_OPERATION";
    break;
  case GL_OUT_OF_MEMORY:
    e_msg = "GL_OUT_OF_MEMORY";
    break;
  default:
    e_msg = "unknown";
  }

  fprintf(stderr, "GL_ERROR: %s => %s\n", msg, e_msg);
}

// macro to run a command followed by a call to gl_check
#define GL_CHECK(x) x; gl_check(#x)

GLuint shader_compile(const char* shader_src, GLenum kind) {
  GLuint shader = glCreateShader(kind);
  GL_CHECK(glShaderSource(shader, 1, &shader_src, NULL));
  GL_CHECK(glCompileShader(shader));

  int status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if(status == GL_FALSE) {
    char buffer[1024];
    int length;
    glGetShaderInfoLog(shader, sizeof(buffer), &length, buffer);
    fprintf(stderr, "compile error: %s\n", buffer);
    exit(1);
  }

  return shader;
}

GLuint shader_link(const char* vertex_src, const char* fragment_src) {
  GLuint program = glCreateProgram();
  GLuint vertex = shader_compile(vertex_src, GL_VERTEX_SHADER);
  GLuint fragment = shader_compile(fragment_src, GL_FRAGMENT_SHADER);

  GL_CHECK(glAttachShader(program, vertex));
  GL_CHECK(glAttachShader(program, fragment));

  GL_CHECK(glLinkProgram(program));

  int status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if(status == GL_FALSE) {
    char buffer[1024];
    int length;
    glGetProgramInfoLog(program, sizeof(buffer), &length, buffer);
    fprintf(stderr, "link error: %s\n", buffer);
    exit(1);
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  return program;
}

bool graphics_init(Context* ctx) {
  SDL_RendererInfo info;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(ctx->w, ctx->h, SDL_WINDOW_OPENGL, &ctx->window, &ctx->renderer);

  // verify opengl support
  SDL_GetRendererInfo(ctx->renderer, &info);
  if(!(info.flags & SDL_RENDERER_ACCELERATED) ||
     !(info.flags & SDL_RENDERER_TARGETTEXTURE)) {
    return false;
  }
  SDL_GL_CreateContext(ctx->window);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // some standard opengl choices
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SPRITE);
  glPointSize(4);

  glViewport(0, 0, ctx->w, ctx->h);

  // compile some shaders
  char solid_vs[] =
    "attribute vec2 vertex;"
    "void main(void) {"
    "  gl_Position = vec4(vertex, 0, 1);"
    "}";

  char solid_fs[] =
    "void main(void) {"
    "  gl_FragColor = vec4(1, 1, 1, 1);"
    "}";

  ctx->filled = shader_link(solid_vs, solid_fs);
  GL_CHECK(glBindAttribLocation(ctx->filled, ATTR_VERTEX, "vertex"));

  // initialize VBOs
  glGenBuffers(1, &ctx->verts);

  return true;
}

void graphics_prerender(Context* ctx) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void graphics_postrender(Context* ctx) {
  SDL_GL_SwapWindow(ctx->window);
}

void particlesystem_render(Context* ctx, const ParticleSystem* sys) {
  size_t data_size = sizeof(GLfloat) * sys->n * 2;
  GLfloat* points = (GLfloat*)malloc(data_size);
  int idx = 0;
  for(int i = 0; i < sys->n; ++i) {
    points[idx++] = sys->particles[i].p.x;
    points[idx++] = sys->particles[i].p.y;
  }

  glUseProgram(ctx->filled);

  glEnableVertexAttribArray(ATTR_VERTEX);
  glBindBuffer(GL_ARRAY_BUFFER, ctx->verts);
  glBufferData(GL_ARRAY_BUFFER, data_size, points, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(ATTR_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_POINTS, 0, sys->n);
  glDisableVertexAttribArray(ATTR_VERTEX);
  free(points);
}

int main(int argc, char *argv[]) {
  Context ctx;
  bool finished = false;
  int nextn;

  ParticleSystem *sys = particlesystem_new(10);

  ctx.w = 800;
  ctx.h = 800;
  graphics_init(&ctx);

  unsigned int last_tick = SDL_GetTicks();

  while(!finished) {
    // drain event queue
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
      case SDL_KEYDOWN:
        nextn = sys->n * 2;
        free(sys);
        sys = particlesystem_new(nextn);

        break;
      case SDL_QUIT:
        finished = true;
        break;
      default:
        break;
      }
    }

    graphics_prerender(&ctx);
    particlesystem_render(&ctx, sys);
    graphics_postrender(&ctx);

    unsigned int now_tick = SDL_GetTicks();
    unsigned int dticks = now_tick - last_tick;
    float dt = dticks * 1e-3;
    particlesystem_step(sys, dt);

    last_tick = now_tick;
  }

  return 0;
}
