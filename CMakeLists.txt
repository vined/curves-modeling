cmake_minimum_required(VERSION 3.9)
project(curves_modeling)

set(CMAKE_CXX_STANDARD 11)

Find_Package (OpenGL REQUIRED)
Find_Package (GLUT REQUIRED)

if (NOT OPENGL_FOUND)
    message (FATAL_ERROR "OpenGL not found!")
endif (NOT OPENGL_FOUND)
if (NOT GLUT_FOUND)
    message (FATAL_ERROR "GLUT not found!")
endif (NOT GLUT_FOUND)

link_libraries (${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

add_executable(curves_modeling
        src/Spline.cpp
        src/Spline.h)
