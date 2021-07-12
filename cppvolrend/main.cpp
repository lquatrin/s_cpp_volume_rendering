/**
 * C++ Volume Rendering Application
 *
 * Leonardo Quatrin Campagnolo
 * . campagnolo.lq@gmail.com
**/
#include "defines.h"
#include "renderingmanager.h"

#include <cstdio>
#include <fstream>
#include <iostream>

#include <glm/glm.hpp>

#include <glm/gtc/type_ptr.hpp>

//-------------------------------------------------------
// 1-pass - Ray Casting - GLSL
#include "structured/rc1pass/rc1prenderer.h"
//-------------------------------------------------------

#ifdef USING_FREEGLUT
#include "app_freeglut.h"
ApplicationFreeGLUT app;
#else
#ifdef USING_GLFW
#include "app_glfw.h"
ApplicationGLFW app;
#endif
#endif

int main (int argc, char **argv)
{
  if (!app.Init(argc, argv)) return 1;

  RenderingManager::Instance()->InitGL();
  RenderingManager::Instance()->SetVolumeRenderer(new RayCasting1Pass());
  
  RenderingManager::Instance()->InitData();

  app.MainLoop();

  app.Destroy();

  return 0;
}