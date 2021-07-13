#pragma once

//#define UPDATE_WINDOW_TITLE

// Using suggestion on using MAKE_STR macro at C++ code from "naoyam"
// . https://github.com/LLNL/lbann/issues/117
#define MAKE_STR(x) _MAKE_STR(x)
#define _MAKE_STR(x) #x

#define CPPVOLREND_DIR MAKE_STR(CMAKE_PATH_TO_APP_FOLDER)
#define CPPVOLREND_SHADER_DIR MAKE_STR(CMAKE_PATH_TO_APP_FOLDER)"shader/"
#define CPPVOLREND_INCLUDE_DIR MAKE_STR(CMAKE_PATH_TO_INCLUDE)
#define CPPVOLREND_DATA_DIR MAKE_STR(CMAKE_PATH_TO_DATA_FOLDER)

// -> DEFINED IN CMAKE
//----------------------------------------------------------------------
// must link "x64/glfw/[release|debug]/glfw3.lib"
#define USING_GLFW 
// --> COMMENT USING_FREEGLUT TO USE GLFW
// http://freeglut.sourceforge.net/
#define USING_FREEGLUT
//----------------------------------------------------------------------

// undefine glfw if freeglut is defined
#ifdef USING_FREEGLUT
#undef USING_GLFW
#endif

#include <GL/glew.h>

#ifdef USING_GLFW
//#define USING_VULKAN
#endif

#ifdef USING_VULKAN
#include <C:/VulkanSDK/1.1.114.0/Include/vulkan/vulkan.h>
#endif

#ifdef USING_FREEGLUT
#include <GL/freeglut.h>
#else
#ifdef USING_GLFW
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif
#endif