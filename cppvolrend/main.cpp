/**
 * C++ Volume Rendering Application
 *
 * Leonardo Quatrin Campagnolo
 * . campagnolo.lq@gmail.com
**/
#include "defines.h"

#include <cstdio>
#include <fstream>
#include <iostream>

#include <glm/glm.hpp>

#include <glm/gtc/type_ptr.hpp>

//-------------------------------------------------------
// 1-pass - Ray Casting - GLSL
#include "structured/rc1pass/rc1prenderer.h"
//-------------------------------------------------------

//#define ALWAYS_OUTDATE_THE_CURRENT_VR_RENDERER
#define RENDERING_MANAGER_TIME_PER_FPS_COUNT_MS 5000.0

#define WHITE_BACKGROUND

std::unique_ptr<RayCasting1Pass> curr_vol_renderer;
vis::RenderingParameters curr_rdr_parameters;
vis::DataManager m_data_mgr;

bool   s_idle_rendering  = true;
double s_ts_current_time = 0.0;
double s_ts_last_time;
int    s_ts_n_frames     = 0;
double s_ts_window_fps   = 0.0;
double s_ts_window_ms    = -1;

void UpdateFrameRate ()
{
  // Measure speed
  s_ts_current_time = glutGet(GLUT_ELAPSED_TIME);
  s_ts_n_frames++;
  // After 1 second, compute frames per second...
  if ((s_ts_current_time - s_ts_last_time) > RENDERING_MANAGER_TIME_PER_FPS_COUNT_MS)
  {
    s_ts_window_fps = double(s_ts_n_frames) * 1000.0 / (s_ts_current_time - s_ts_last_time);
    s_ts_window_ms = 1000.0 / s_ts_window_fps;

    s_ts_last_time = s_ts_current_time;
    s_ts_n_frames = 0;
    printf("%.2lf frames per second\n", s_ts_window_fps);
  }
}

void PostRedisplay ()
{
  glutPostRedisplay();
}

static void s_Display (void)
{
  // Get the current render mode
  UpdateFrameRate();

  if (curr_rdr_parameters.GetCamera()->UpdatePositionAndRotations())
    curr_vol_renderer->SetOutdated();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Render Function
  if (curr_vol_renderer && curr_vol_renderer->IsBuilt())
  {
    curr_vol_renderer->PrepareRender(curr_rdr_parameters.GetCamera());
    curr_vol_renderer->Redraw();
  }

  glutSwapBuffers();
}

static void s_Reshape (int w, int h)
{
  glViewport(0, 0, w, h);

  curr_rdr_parameters.SetScreenSize(w, h);
  curr_rdr_parameters.GetCamera()->UpdateAspectRatio(float(w), float(h));

  if (curr_vol_renderer->IsBuilt())
  {
    curr_vol_renderer->Reshape(w, h);
    curr_vol_renderer->SetOutdated();
  }

  PostRedisplay();
}

static void s_Keyboard (unsigned char key, int x, int y)
{
  switch (key)
  {
  case 27:
    exit(EXIT_FAILURE);
    return;
  default:
    break;
  }
  curr_rdr_parameters.GetCamera()->KeyboardDown(key, x, y);
  PostRedisplay();
}

static void s_KeyboardUp (unsigned char key, int x, int y)
{
  curr_rdr_parameters.GetCamera()->KeyboardUp(key, x, y);
  switch (key)
  {
  case 'l':
    curr_rdr_parameters.SetBlinnPhongLightingPosition(curr_rdr_parameters.GetCamera()->GetEye());
    curr_rdr_parameters.SetBlinnPhongLightSourceCameraVectors(
      curr_rdr_parameters.GetCamera()->GetZAxis(),
      curr_rdr_parameters.GetCamera()->GetYAxis(),
      curr_rdr_parameters.GetCamera()->GetXAxis()
    );
    curr_vol_renderer->SetOutdated();
    break;
  }
  PostRedisplay();
}

static void s_OnMouse (int glut_button, int state, int x, int y)
{
  curr_rdr_parameters.GetCamera()->MouseButton(glut_button, state, x, y);
  PostRedisplay();
}

static void s_OnMotion (int x, int y)
{
  if (curr_rdr_parameters.GetCamera()->MouseMotion(x, y) == 1)
    curr_vol_renderer->SetOutdated();
  PostRedisplay();
}

static void s_CloseFunc ()
{
  gl::PipelineShader::Unbind();
  gl::ArrayObject::Unbind();
}

static void s_IdleFunc ()
{
  if (s_idle_rendering)
  {
#ifdef ALWAYS_OUTDATE_THE_CURRENT_VR_RENDERER
    curr_vol_renderer->SetOutdated();
#endif
    PostRedisplay();
  }
}

void InitGL ()
{
  // Init glew + camera + curr vol renderer
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_3D);

  glEnable(GL_DEPTH_TEST);
  gl::ExitOnGLError("RenderingManager: Could not enable depth test...");

  glEnable(GL_CULL_FACE);
  gl::ExitOnGLError("RenderingManager: Could not enable cull face...");

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
  gl::ExitOnGLError("RenderingManager: Could not enable blend...");

#ifdef WHITE_BACKGROUND
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
#else
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
#endif
  s_ts_last_time = glutGet(GLUT_ELAPSED_TIME);
}

// Init glew + camera + curr vol renderer
void InitData ()
{
  // Read Dataset and Transfer Function
  // . check datamanager.cpp for defines 
  m_data_mgr.ReadData();

  // Set first camera
  vis::CameraData c_data;
  c_data.eye = glm::vec3(256.0, 256.0, 512.0);
  c_data.center = glm::vec3(0.0, 0.0, 0.0);
  c_data.up = glm::vec3(0.0, 1.0, 0.0);

  curr_rdr_parameters.GetCamera()->SetData(&c_data);

  curr_vol_renderer = std::make_unique<RayCasting1Pass>();
  curr_vol_renderer->SetExternalResources(&m_data_mgr, &curr_rdr_parameters);
  curr_vol_renderer->Init(curr_rdr_parameters.GetScreenWidth(), curr_rdr_parameters.GetScreenHeight());

  // Reshape
  s_Reshape(curr_rdr_parameters.GetScreenWidth(), curr_rdr_parameters.GetScreenHeight());
}

int main (int argc, char **argv)
{
  glutInit(&argc, argv);
#ifdef __FREEGLUT_EXT_H__
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL | GLUT_ALPHA);

  glutInitWindowSize(curr_rdr_parameters.GetScreenWidth(), curr_rdr_parameters.GetScreenHeight());
  glutCreateWindow("CppVolRend [FreeGLUT]");

  if (glewInit() != GLEW_OK)
  {
    printf("Glew didn't initialized!\n");
    exit(EXIT_FAILURE);
  }
  printf("Running OpenGL %s\n\n", glGetString(GL_VERSION));

  // Setup GLUT display function
  glutDisplayFunc(s_Display);
  glutReshapeFunc(s_Reshape);
  glutKeyboardFunc(s_Keyboard);
  glutKeyboardUpFunc(s_KeyboardUp);
  glutMouseFunc(s_OnMouse);
  glutMotionFunc(s_OnMotion);

  glutCloseFunc(s_CloseFunc);
  glutIdleFunc(s_IdleFunc);

  // VSYNC
  if (wglGetSwapIntervalEXT() > 0)
    wglSwapIntervalEXT(1);

  InitGL();
  InitData();

  glutMainLoop();

  return 0;
}