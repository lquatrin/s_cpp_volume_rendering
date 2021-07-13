#include "renderingmanager.h"

#include "defines.h"

#include <glm/gtc/type_ptr.hpp>

#include "structured/rc1pass/rc1prenderer.h"
#include <gl_utils/framebufferobject.h>

#include <volvis_utils/transferfunction1d.h>

#include <volvis_utils/utils.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>

//#define ALWAYS_OUTDATE_THE_CURRENT_VR_RENDERER

#define RENDERING_MANAGER_TIME_PER_FPS_COUNT_MS 5000.0

#define WHITE_BACKGROUND

#ifdef USING_FREEGLUT
double GetCurrentTimeFreeGLUT(void* data)
{
  return glutGet(GLUT_ELAPSED_TIME);
}
#else
#ifdef USING_GLFW
#endif
#endif

RenderingManager *RenderingManager::crr_instance = 0;

RenderingManager* RenderingManager::Instance ()
{
  if (!crr_instance)
    crr_instance = new RenderingManager();

  return crr_instance;
}

bool RenderingManager::Exists ()
{
  return (crr_instance != NULL);
}

void RenderingManager::DestroyInstance ()
{
  if (!RenderingManager::Exists())
    return;

  if (crr_instance)
  {
    delete crr_instance;
    crr_instance = NULL;
  }
}

// Init glew + camera + curr vol renderer
void RenderingManager::InitGL()
{
#ifdef USING_FREEGLUT
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_3D);
#else
#ifdef USING_GLFW
  // Deprecated on Opengl 4x, does not work with glfw
  //glEnable(GL_TEXTURE_2D);
  //glEnable(GL_TEXTURE_3D);
#endif
#endif
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

  int max;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
  //std::cout << max << std::endl;
}

// Init glew + camera + curr vol renderer
void RenderingManager::InitData ()
{
  // Read Datasets and Transfer Functions from Data Manager
  m_data_mgr.ReadData();
 
  // Read camera states and set first camera data
  vis::CameraData c_data;
  c_data.eye = glm::vec3(256.0, 256.0, 512.0);
  c_data.center = glm::vec3(0.0, 0.0, 0.0);
  c_data.up = glm::vec3(0.0, 1.0, 0.0);

  curr_rdr_parameters.GetCamera()->SetData(&c_data);

  curr_vol_renderer = new RayCasting1Pass();
  // Set auxiliar data parameter classes for each rendering mode
  curr_vol_renderer->SetExternalResources(&m_data_mgr, &curr_rdr_parameters);
  // Update rendering mode
  curr_vol_renderer->Init(curr_rdr_parameters.GetScreenWidth(), curr_rdr_parameters.GetScreenHeight());
 
  // Reshape
  Reshape(curr_rdr_parameters.GetScreenWidth(), curr_rdr_parameters.GetScreenHeight());
}

void RenderingManager::Display ()
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

#ifdef USING_FREEGLUT
  // Swap buffer
  f_swapbuffer(d_swapbuffer);
#else
#ifdef USING_GLFW
  f_swapbuffer(d_swapbuffer);
#endif
#endif
}

void RenderingManager::Reshape (int w, int h)
{
  glViewport(0, 0, w, h);

  curr_rdr_parameters.SetScreenSize(w, h);
  curr_rdr_parameters.GetCamera()->UpdateAspectRatio(float(w), float(h));

  if (curr_vol_renderer->IsBuilt())
  {
    curr_vol_renderer->Reshape(w,h);
    curr_vol_renderer->SetOutdated();
  }

  PostRedisplay();
}

void RenderingManager::Keyboard (unsigned char key, int x, int y)
{
  curr_rdr_parameters.GetCamera()->KeyboardDown(key, x, y);
  PostRedisplay();
}

void RenderingManager::KeyboardUp(unsigned char key, int x, int y)
{
  curr_rdr_parameters.GetCamera()->KeyboardUp(key, x, y);
  PostRedisplay();
}

void RenderingManager::MouseButton (int bt, int st, int x, int y)
{
  curr_rdr_parameters.GetCamera()->MouseButton(bt, st, x, y);

  PostRedisplay();
}

void RenderingManager::MouseMotion (int x, int y)
{
  if (curr_rdr_parameters.GetCamera()->MouseMotion(x, y) == 1)
    curr_vol_renderer->SetOutdated();
  PostRedisplay();
}

void RenderingManager::CloseFunc ()
{
  gl::PipelineShader::Unbind();
  gl::ArrayObject::Unbind();

  if(curr_vol_renderer) delete curr_vol_renderer;
  curr_vol_renderer = nullptr;
}

void RenderingManager::IdleFunc ()
{
  if (m_idle_rendering)
  {
#ifdef ALWAYS_OUTDATE_THE_CURRENT_VR_RENDERER
    curr_vol_renderer->SetOutdated();
#endif
    PostRedisplay();
  }
}

void RenderingManager::PostRedisplay ()
{
#ifdef USING_FREEGLUT
  glutPostRedisplay();
#else
#ifdef USING_GLFW
  Display();
#endif
#endif
}

void RenderingManager::ResetGLStateConfig ()
{
#ifdef USING_FREEGLUT
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
#endif
}

GLubyte* RenderingManager::GetFrontBufferPixelData (bool alpha)
{
  GLubyte *gl_img_data = new GLubyte[(alpha ? 4 : 3) * curr_rdr_parameters.GetScreenWidth() * curr_rdr_parameters.GetScreenHeight()];
  
  glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
  glPushAttrib(GL_PIXEL_MODE_BIT);
  glReadBuffer(GL_BACK);
  glFlush();
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  
  glReadPixels(0, 0, curr_rdr_parameters.GetScreenWidth(), curr_rdr_parameters.GetScreenHeight(), (alpha ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, gl_img_data);
  
  glPopAttrib();
  glPopClientAttrib();

  return gl_img_data;
}

void RenderingManager::UpdateFrameRate ()
{
  // Measure speed
#ifdef USING_FREEGLUT
  m_ts_current_time = glutGet(GLUT_ELAPSED_TIME);
  m_ts_n_frames++;
  // After 1 second, compute frames per second...
  if ((m_ts_current_time - m_ts_last_time) > RENDERING_MANAGER_TIME_PER_FPS_COUNT_MS)
  {
    m_ts_window_fps = double(m_ts_n_frames) * 1000.0 / (m_ts_current_time - m_ts_last_time);
    m_ts_window_ms = 1000.0 / m_ts_window_fps;

    m_ts_last_time = m_ts_current_time;
    m_ts_n_frames = 0;
    printf("%.2lf frames per second\n", m_ts_window_fps);
  }
#else
#ifdef USING_GLFW
  currentTime = glfwGetTime();
  nbFrames++;
  if ((currentTime - lastTime) > 1.0)
  {
    double milisec = (currentTime - lastTime) * 1000.0;
    window_fps = double(nbFrames) * 1000.0 / (currentTime - lastTime);
    window_ms = 1000.0 / window_fps;

    lastTime = currentTime;
    nbFrames = 0;
  }
#endif
#endif
}

RenderingManager::RenderingManager ()
  : curr_vol_renderer(nullptr)
{
  f_swapbuffer = nullptr;
  d_swapbuffer = nullptr;

  curr_vol_renderer = nullptr;

  m_idle_rendering        = true;
  m_ts_current_time = 0.0;
#ifdef USING_FREEGLUT
  m_ts_last_time = glutGet(GLUT_ELAPSED_TIME);
#else
#ifdef USING_GLFW
  m_ts_last_time = glfwGetTime();
#endif
#endif
  m_ts_n_frames = 0;
  m_ts_window_fps = 0.0;
  m_ts_window_ms = -1;
}

RenderingManager::~RenderingManager ()
{
  CloseFunc();
}