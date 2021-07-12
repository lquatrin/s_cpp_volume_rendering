#include "app_freeglut.h"
#ifdef USING_FREEGLUT

#include "renderingmanager.h"
#include <GL/wglew.h>

void ApplicationFreeGLUT::glutSwapBuffer (void* data)
{
  glutSwapBuffers();
}

void ApplicationFreeGLUT::Display (void)
{
  RenderingManager::Instance()->Display();
}

void ApplicationFreeGLUT::s_Reshape (int w, int h)
{
  RenderingManager::Instance()->Reshape(w, h);
}

void ApplicationFreeGLUT::s_Keyboard (unsigned char key, int x, int y)
{
  switch (key)
  {
  case 27:
    RenderingManager::Instance()->DestroyInstance();
    exit(EXIT_FAILURE);
    return;
  default:
    break;
  }
  RenderingManager::Instance()->Keyboard(key, x, y);
}

void ApplicationFreeGLUT::s_KeyboardUp (unsigned char c, int x, int y)
{
  RenderingManager::Instance()->KeyboardUp(c, x, y);
}

void ApplicationFreeGLUT::s_OnMouse (int glut_button, int state, int x, int y)
{
  RenderingManager::Instance()->MouseButton(glut_button, state, x, y);
}

void ApplicationFreeGLUT::s_OnMotion (int x, int y)
{
  RenderingManager::Instance()->MouseMotion(x, y);
}

void ApplicationFreeGLUT::s_MouseWheel (int wheel, int direction, int x, int y)
{
}

void ApplicationFreeGLUT::s_SpecialFunc (int key, int x, int y)
{
}

void ApplicationFreeGLUT::s_SpecialUpFunc (int key, int x, int y)
{
}

void ApplicationFreeGLUT::s_PassiveMotionFunc (int x, int y)
{
}

void ApplicationFreeGLUT::s_CloseFunc ()
{
  RenderingManager::Instance()->DestroyInstance();
}

void ApplicationFreeGLUT::s_IdleFunc ()
{
  RenderingManager::Instance()->IdleFunc();
}

ApplicationFreeGLUT::ApplicationFreeGLUT ()
{
}

ApplicationFreeGLUT::~ApplicationFreeGLUT ()
{
}

bool ApplicationFreeGLUT::Init (int argc, char** argv)
{
  glutInit(&argc, argv);
#ifdef __FREEGLUT_EXT_H__
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL | GLUT_ALPHA);

  glutInitWindowSize(RenderingManager::Instance()->GetScreenWidth(),
                     RenderingManager::Instance()->GetScreenHeight());
  glutCreateWindow("CppVolRend [FreeGLUT]");

  if (glewInit() != GLEW_OK)
  {
    printf("Glew didn't initialized!\n");
    exit(EXIT_FAILURE);
  }
  printf("Running OpenGL %s\n\n", glGetString(GL_VERSION));

  // Setup GLUT display function
  glutDisplayFunc(ApplicationFreeGLUT::Display);
  glutReshapeFunc(ApplicationFreeGLUT::s_Reshape);
  glutKeyboardFunc(ApplicationFreeGLUT::s_Keyboard);
  glutKeyboardUpFunc(ApplicationFreeGLUT::s_KeyboardUp);
  glutMouseFunc(ApplicationFreeGLUT::s_OnMouse);
  glutMotionFunc(ApplicationFreeGLUT::s_OnMotion);
#ifdef __FREEGLUT_EXT_H__
  glutMouseWheelFunc(ApplicationFreeGLUT::s_MouseWheel);
#endif
  glutSpecialFunc(ApplicationFreeGLUT::s_SpecialFunc);
  glutSpecialUpFunc(ApplicationFreeGLUT::s_SpecialUpFunc);
  glutPassiveMotionFunc(ApplicationFreeGLUT::s_PassiveMotionFunc);

  glutCloseFunc(ApplicationFreeGLUT::s_CloseFunc);
  glutIdleFunc(ApplicationFreeGLUT::s_IdleFunc);

  RenderingManager::Instance()->f_swapbuffer = ApplicationFreeGLUT::glutSwapBuffer;
  
  // VSYNC
  if (wglGetSwapIntervalEXT() > 0)
    wglSwapIntervalEXT(1);

  return true;
}

void ApplicationFreeGLUT::MainLoop ()
{
  glutMainLoop();
}

void ApplicationFreeGLUT::Destroy ()
{}

#endif