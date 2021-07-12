add_library(gl_utils STATIC arrayobject.cpp       arrayobject.h
                            bufferobject.cpp      bufferobject.h
                            computeshader.cpp     computeshader.h
                            framebufferobject.cpp framebufferobject.h
                            pipelineshader.cpp    pipelineshader.h
                            texture1d.cpp         texture1d.h
                            texture2d.cpp         texture2d.h
                            texture3d.cpp         texture3d.h
                            shader.cpp            shader.h
                            utils.cpp             utils.h
                            )

include_directories(${CMAKE_SOURCE_DIR}/include)
add_definitions(-DEXPMODULE)
include_directories(${CMAKE_SOURCE_DIR}/libs)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
link_directories(${CMAKE_SOURCE_DIR}/lib)

# link with math_utils and glew
target_link_libraries(gl_utils debug glew/glew32)
target_link_libraries(gl_utils debug glew/glew32s)

target_link_libraries(gl_utils optimized glew/glew32)
target_link_libraries(gl_utils optimized glew/glew32s)

# add dependency
