include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/libs)

#message(${CMAKE_SOURCE_DIR})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_definitions(-DCMAKE_PATH_OUTPUT_DIR=${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set(PATH_TO_APP_FOLDER ${CMAKE_SOURCE_DIR}/cppvolrend/)
add_definitions(-DCMAKE_PATH_TO_APP_FOLDER=${PATH_TO_APP_FOLDER})

# add the executable to be available at ide
add_executable(cppvolrend
               main.cpp                                                        defines.h
               datamanager.cpp                                                 datamanager.h
               renderingparameters.cpp                                         renderingparameters.h
               renderoutputframe.cpp                                           renderoutputframe.h
               # GPU Image Order Ray Casting
               structured/rc1pass/rc1prenderer.cpp                             structured/rc1pass/rc1prenderer.h
               )

find_package(OpenGL REQUIRED)
link_directories(${OPENGL_gl_LIBRARY})

# . Debug
target_link_libraries(cppvolrend debug ${OPENGL_gl_LIBRARY})
target_link_libraries(cppvolrend debug freeglut/freeglut)
target_link_libraries(cppvolrend debug glew/glew32s)
target_link_libraries(cppvolrend debug glew/glew32)
target_link_libraries(cppvolrend debug file_utils)
target_link_libraries(cppvolrend debug gl_utils)
target_link_libraries(cppvolrend debug volvis_utils)
# . Release
target_link_libraries(cppvolrend optimized ${OPENGL_gl_LIBRARY})
target_link_libraries(cppvolrend optimized freeglut/freeglut)
target_link_libraries(cppvolrend optimized glew/glew32s)
target_link_libraries(cppvolrend optimized glew/glew32)
target_link_libraries(cppvolrend optimized file_utils)
target_link_libraries(cppvolrend optimized gl_utils)
target_link_libraries(cppvolrend optimized volvis_utils)

# add dependency
add_dependencies(cppvolrend file_utils)
add_dependencies(cppvolrend gl_utils)
add_dependencies(cppvolrend volvis_utils)
# . Debug
file(COPY "${CMAKE_SOURCE_DIR}/lib/glew/glew32.dll" DESTINATION "${CMAKE_SOURCE_DIR}/bin/Debug")
file(COPY "${CMAKE_SOURCE_DIR}/lib/freeglut/freeglut.dll" DESTINATION "${CMAKE_SOURCE_DIR}/bin/Debug")

# . Release
file(COPY "${CMAKE_SOURCE_DIR}/lib/glew/glew32.dll" DESTINATION "${CMAKE_SOURCE_DIR}/bin/Release")
file(COPY "${CMAKE_SOURCE_DIR}/lib/freeglut/freeglut.dll" DESTINATION "${CMAKE_SOURCE_DIR}/bin/Release")
