set(V_LIB_VIS_UTILS_SHADER_DIR ${CMAKE_SOURCE_DIR}/libs/vis_utils/shader/)
add_definitions(-DCMAKE_VIS_UTILS_PATH_TO_SHADER=${V_LIB_VIS_UTILS_SHADER_DIR})

add_library(vis_utils STATIC  camera.cpp                          camera.h
                              defines.cpp                         defines.h
                              renderoutputframe.cpp               renderoutputframe.h
                             )

include_directories(${CMAKE_SOURCE_DIR}/include)
add_definitions(-DEXPMODULE)
include_directories(${CMAKE_SOURCE_DIR}/libs)
include_directories(${CMAKE_SOURCE_DIR}/lib)


link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# only link with math_utils
target_link_libraries(vis_utils debug file_utils)
target_link_libraries(vis_utils debug gl_utils)
target_link_libraries(vis_utils debug glew/glew32)
target_link_libraries(vis_utils debug glew/glew32s)

target_link_libraries(vis_utils optimized file_utils)
target_link_libraries(vis_utils optimized gl_utils)
target_link_libraries(vis_utils optimized glew/glew32)
target_link_libraries(vis_utils optimized glew/glew32s)
                      
# add dependency
add_dependencies(vis_utils file_utils)
add_dependencies(vis_utils gl_utils)