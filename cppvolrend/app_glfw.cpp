#include "app_glfw.h"
#ifdef USING_GLFW

#include "renderingmanager.h"
#include <GL/wglew.h>

#include <cstdio> // fprintf
#include <cstdlib> // exit

void ApplicationGLFW::glfw_error_callback (int error, const char* description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void ApplicationGLFW::glfwSwapBuffer (void* data)
{
  glfwSwapBuffers((GLFWwindow*)data);
}

void ApplicationGLFW::s_MouseButtonCallback (GLFWwindow* window, int button, int action, int mods)
{
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  RenderingManager::Instance()->MouseButton(button, (action + 1) % 2, xpos, ypos);
}

void ApplicationGLFW::s_ScrollCallback (GLFWwindow* window, double xoffset, double yoffset)
{
}

void ApplicationGLFW::s_KeyCallback (GLFWwindow* window, int key, int scancode, int action, int mods)
{
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  if (action == 1) //  Key Down
  {
    RenderingManager::Instance()->Keyboard(key, xpos, ypos);
  }
  else if (action == 0) //  Key Up
  {
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
      RenderingManager::Instance()->DestroyInstance();
      exit(EXIT_FAILURE);
      return;
    default:
      break;
    }

    RenderingManager::Instance()->KeyboardUp(key, xpos, ypos);
  }
}

void ApplicationGLFW::s_CharCallback (GLFWwindow* window, unsigned int codepoint)
{
}

void ApplicationGLFW::s_MouseMotionCallback (GLFWwindow* window, double xpos, double ypos)
{
  RenderingManager::Instance()->MouseMotion(xpos, ypos);
}

void ApplicationGLFW::s_WindowSizeCallback (GLFWwindow* window, int w, int h)
{
  RenderingManager::Instance()->Reshape(w, h);
}

ApplicationGLFW::ApplicationGLFW ()
{
  window = nullptr;
  use_vsync = true;
}

ApplicationGLFW::~ApplicationGLFW ()
{


}

bool ApplicationGLFW::Init (int argc, char** argv)
{
  // Setup window
  glfwSetErrorCallback(ApplicationGLFW::glfw_error_callback);

  //glewExperimental = GL_TRUE;
  if (!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW\n");
    exit(EXIT_FAILURE);
  }

  // Decide GL+GLSL versions
#if __APPLE__
  printf("Meh, no apple!"); exit(1);
#else
  // GL 3.0 + GLSL 130
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
#endif

  // Create window with graphics context
  window = glfwCreateWindow(RenderingManager::Instance()->GetScreenWidth(),
                            RenderingManager::Instance()->GetScreenHeight(), "CppVolRend [GLFW]", NULL, NULL);
  if (window == NULL) return 1;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(use_vsync ? 1 : 0); // Enable vsync

  // Initialize OpenGL
  bool err = glewInit() != GLEW_OK;
  if (err)
  {
    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    return 1;
  }
  printf("Running OpenGL %s\n\n", glGetString(GL_VERSION));

  // Set callbacks
  glfwSetMouseButtonCallback(window, ApplicationGLFW::s_MouseButtonCallback);
  glfwSetScrollCallback(window, ApplicationGLFW::s_ScrollCallback);
  glfwSetKeyCallback(window, ApplicationGLFW::s_KeyCallback);
  glfwSetCharCallback(window, ApplicationGLFW::s_CharCallback);

  glfwSetCursorPosCallback(window, ApplicationGLFW::s_MouseMotionCallback);
  glfwSetWindowSizeCallback(window, ApplicationGLFW::s_WindowSizeCallback);

  RenderingManager::Instance()->f_swapbuffer = ApplicationGLFW::glfwSwapBuffer;
  RenderingManager::Instance()->d_swapbuffer = window;

#ifdef USING_FREEGLUT
  if (wglGetSwapIntervalEXT() > 0)
    wglSwapIntervalEXT(1);
#endif

  if (glfwVulkanSupported())
  {
    std::cout << "It seems like vulkan is available" << std::endl;
  }

  return true;
}

void ApplicationGLFW::MainLoop()
{
  while (!glfwWindowShouldClose(GetWindow()))
  {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    if (RenderingManager::Instance()->IdleRendering()) glfwPollEvents();
    else                                               glfwWaitEventsTimeout(1.0);

    RenderingManager::Instance()->Display();
  }
}

void ApplicationGLFW::Destroy ()
{
  glfwDestroyWindow(window);
  glfwTerminate();
}

#endif