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

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/wglew.h>