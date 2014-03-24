#include <stdio.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define FALSE 0
#define TRUE 1

typedef struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  int w, h;

  // shaders
  GLuint filled;

  // vbo
  GLuint buffer;
} Context;

typedef enum {
  ATTR_VERTEX
} ProgramAttributes;

typedef struct {
  float x, y;
} Vector;

Vector* vector_init(Vector* v, float x, float y) {
  v->x = x;
  v->y = y;
  return v;
}

Vector* vector_init_angle(Vector* v, float angle) {
  v->x = sin(angle);
  v->y = cos(angle);
  return v;
}

Vector* vector_sub(Vector* result, Vector* a, Vector* b) {
  result->x = a->x - b->x;
  result->y = a->y - b->y;
  return result;
}

Vector* vector_add(Vector* result, Vector* a, Vector* b) {
  result->x = a->x + b->x;
  result->y = a->y + b->y;
  return result;
}

Vector* vector_scale(Vector* result, Vector* a, float s) {
  result->x = a->x * s;
  result->y = a->y * s;
  return result;
}

float vector_mag(Vector* a) {
  return sqrt(a->x * a->x + a->y * a->y);
}

Vector* vector_norm(Vector* result, Vector* a) {
  float s = vector_mag(a);
  return vector_scale(result, a, 1.0 / s);
}

float vector_dist(Vector* a, Vector* b) {
  float dx = a->x - b->x;
  float dy = a->y - b->y;
  return sqrt(dx * dx + dy * dy);
}

float vector_angle(Vector* a) {
  return atan2(a->x, a->y);
}

typedef struct {
  Vector p, v;
  float m;
} Particle;

Particle* particle_init(Particle* p, float m, float x, float y, float dx, float dy) {
  p->m = m;
  p->p.x = x;
  p->p.y = y;
  p->v.x = dx;
  p->v.y = dy;
  return p;
}

Vector* particle_add_drag_force(Vector* f, Particle* p, float c) {
  Vector drag_force;
  vector_scale(&drag_force, &p->v, -c);
  vector_add(f, f, &drag_force);
  return f;
}

Particle* particle_step(Particle* p, Vector* f, float dt) {
  Vector a, dv, dx;
  vector_scale(&a, f, 1.0 / p->m); // f = ma

  vector_scale(&dv, &a, dt);
  vector_add(&p->v, &p->v, &dv);

  vector_scale(&dx, &p->v, dt);
  vector_add(&p->p, &p->p, &dx);

  return p;
}

Vector* gravity_add_force(Vector* f, Particle* p) {
  f->y -= 9.81 * p->m;
  return f;
}

typedef struct {
  Vector* p1;
  Vector* p2;
  float k;
  float l;
} Spring;

Spring* spring_init(Spring* s, Vector* p1, Vector* p2, float k) {
  s->p1 = p1;
  s->p2 = p2;
  s->k = k;
  s->l = vector_dist(p1, p2);
  return s;
}

Vector* spring_add_force(Vector* result, Spring* s, int negate) {
  float l = vector_dist(s->p1, s->p2);
  Vector force;
  float mf = (s->l - l) * s->k;

  vector_sub(&force, s->p2, s->p1);
  vector_norm(&force, &force);
  vector_scale(&force, &force, mf);

  // adjust for which end of the spring is experiencing the force
  if(negate) {
    vector_scale(&force, &force, -1.0f);
  }

  vector_add(result, result, &force);
  return result;
}

typedef struct {
  Vector anchor;
  Particle p1, p2;
  Spring s1, s2;
} Pendulum;

Pendulum* pendulum_init(Pendulum* p) {
  vector_init(&p->anchor, 0.0f, 1.0f);
  particle_init(&p->p1, 1.0f, 0.5f, 1.0f, 0.0f, 0.0f);
  particle_init(&p->p2, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);

  spring_init(&p->s1, &p->anchor, &p->p1.p, 100.5);
  spring_init(&p->s2, &p->p1.p, &p->p2.p, 100.5);
  return p;
}

Pendulum* pendulum_step(Pendulum* p, float dt) {
  Vector f1, f2;

  vector_init(&f1, 0.0f, 0.0f);
  vector_init(&f2, 0.0f, 0.0f);

  // add the effects of gravity
  gravity_add_force(&f1, &p->p1);
  gravity_add_force(&f2, &p->p2);

  // add the spring forces
  spring_add_force(&f1, &p->s1, 0); // spring1 on p1
  spring_add_force(&f1, &p->s2, 1); // spring2 on p1
  spring_add_force(&f2, &p->s2, 0); // spring2 on p2

  // apply drag
  particle_add_drag_force(&f1, &p->p1, 0.05f);
  particle_add_drag_force(&f2, &p->p2, 0.05f);

  // step the particles with the accumulated forces
  particle_step(&p->p1, &f1, dt);
  particle_step(&p->p2, &f2, dt);

  return p;
}

void pendulum_print(FILE* output, Pendulum* p) {
  fprintf(output, "p1.p = (%f, %f)  p1.v = (%f, %f)\n", p->p1.p.x, p->p1.p.y, p->p1.v.x, p->p1.v.y);
  fprintf(output, "p2.p = (%f, %f)  p2.v = (%f, %f)\n", p->p2.p.x, p->p2.p.y, p->p2.v.x, p->p2.v.y);
}

void pendulum_spring_zigzag(GLfloat* dest, int n, Vector* p1, Vector* p2) {
  Vector p = *p1;
  Vector tangent, normal;
  float t_ang, n_ang;
  float step = vector_dist(p1, p2) / (n-1);
  int idx = 0;

  vector_sub(&tangent, p2, p1);
  vector_norm(&tangent, &tangent);
  vector_scale(&tangent, &tangent, step);

  t_ang = vector_angle(&tangent);
  n_ang = t_ang + M_PI/2;
  vector_init_angle(&normal, n_ang);
  vector_scale(&normal, &normal, 0.01);

  dest[idx++] = p.x;
  dest[idx++] = p.y;

  for(int i = 1; i < n-1; i++) {
    Vector np;

    vector_add(&p, &p, &tangent);

    if(i % 2 == 0) {
      np = normal;
    } else {
      vector_scale(&np, &normal, -1);
    }

    vector_add(&np, &np, &p);
    dest[idx++] = np.x;
    dest[idx++] = np.y;
  }

  dest[idx++] = p2->x;
  dest[idx++] = p2->y;
}

void pendulum_render(Context* ctx, Pendulum* p) {
  GLfloat points[400];

  pendulum_spring_zigzag(&points[0], 100, &p->anchor, &p->p1.p);
  pendulum_spring_zigzag(&points[200], 100, &p->p1.p, &p->p2.p);

  glUseProgram(ctx->filled);

  glEnableVertexAttribArray(ATTR_VERTEX);
  glBindBuffer(GL_ARRAY_BUFFER, ctx->buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(ATTR_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_LINE_STRIP, 0, sizeof(points) / (sizeof(GLfloat) * 2));
  glDisableVertexAttribArray(ATTR_VERTEX);
}

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

GLuint shader_compile(const char* shader_src, GLenum kind) {
  GLuint shader = glCreateShader(kind);
  glShaderSource(shader, 1, &shader_src, NULL);
  gl_check("glShaderSource");
  glCompileShader(shader);
  gl_check("glCompileShader");

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

  glAttachShader(program, vertex);
  gl_check("glAttachShader");
  glAttachShader(program, fragment);
  gl_check("glAttachShader");

  glLinkProgram(program);
  gl_check("glLinkProgram");

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

int graphics_init(Context* ctx) {
  SDL_RendererInfo info;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(ctx->w, ctx->h, SDL_WINDOW_OPENGL, &ctx->window, &ctx->renderer);

  // verify opengl support
  SDL_GetRendererInfo(ctx->renderer, &info);
  if(!(info.flags & SDL_RENDERER_ACCELERATED) ||
     !(info.flags & SDL_RENDERER_TARGETTEXTURE)) {
    return FALSE;
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
  glBindAttribLocation(ctx->filled, ATTR_VERTEX, "vertex");
  gl_check("glBindAttribLocation");

  // initialize VBOs
  glGenBuffers(1, &ctx->buffer);

  return TRUE;
}

void graphics_prerender(Context* ctx) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void graphics_postrender(Context* ctx) {
  SDL_GL_SwapWindow(ctx->window);
}



int main(int argc, char *argv[]) {
  Context ctx;
  int finished = 0;
  int step = 0;

  Pendulum p;
  pendulum_init(&p);

  ctx.w = 800;
  ctx.h = 800;
  graphics_init(&ctx);

  unsigned int last_tick = SDL_GetTicks();
  unsigned int remaining_ticks = 0;
  unsigned int min_ticks = 16;

  while(!finished) {
    // drain event queue
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
      case SDL_KEYDOWN:
        pendulum_init(&p);
        break;
      case SDL_QUIT:
        finished = 1;
        break;
      default:
        break;
      }
    }

    graphics_prerender(&ctx);
    pendulum_render(&ctx, &p);
    graphics_postrender(&ctx);

    /*
    fprintf(stderr, "Step %d\n", step++);
    pendulum_print(stderr, &p);
    */

    unsigned int now_tick = SDL_GetTicks();
    unsigned int dticks = now_tick - last_tick;
    remaining_ticks += dticks;

    while(remaining_ticks >= min_ticks) {
      const float dt = min_ticks * 1e-3;
      pendulum_step(&p, dt);
      remaining_ticks -= min_ticks;
    }
    last_tick = now_tick;
  }

  return 0;
}
