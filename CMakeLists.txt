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


add_executable(casteljau src/Casteljau.cpp
        src/utils/Colors.cc src/utils/Colors.h src/utils/Renderer.cc src/utils/Renderer.h src/utils/Points.cc src/utils/Points.h src/utils/Draw.cc src/utils/Draw.h src/utils/SplineUtils.cc src/utils/SplineUtils.h)

add_executable(approximatingSpline src/ApproxSpline.cc
        src/utils/Colors.cc src/utils/Colors.h src/utils/Renderer.cc src/utils/Renderer.h src/utils/Points.cc src/utils/Points.h src/utils/Draw.cc src/utils/Draw.h src/utils/SplineUtils.cc src/utils/SplineUtils.h)

add_executable(interpolatingSpline src/InterpolatingSpline.cc
        src/utils/Colors.cc src/utils/Colors.h src/utils/Renderer.cc src/utils/Renderer.h src/utils/Points.cc src/utils/Points.h src/utils/Draw.cc src/utils/Draw.h src/utils/SplineUtils.cc src/utils/SplineUtils.h)

add_executable(curveCutting src/CurveCutting.cc
        src/utils/Colors.cc src/utils/Colors.h src/utils/Renderer.cc src/utils/Renderer.h src/utils/Points.cc src/utils/Points.h src/utils/Draw.cc src/utils/Draw.h src/utils/SplineUtils.cc src/utils/SplineUtils.h)

add_executable(thirdDegCurveCutting src/ThirdDegCurveCutting.cc
        src/utils/Colors.cc src/utils/Colors.h src/utils/Renderer.cc src/utils/Renderer.h src/utils/Points.cc src/utils/Points.h src/utils/Draw.cc src/utils/Draw.h src/utils/SplineUtils.cc src/utils/SplineUtils.h)

add_executable(fourPointsScheme src/FourPointsScheme.cc
        src/utils/Colors.cc src/utils/Colors.h src/utils/Renderer.cc src/utils/Renderer.h src/utils/Points.cc src/utils/Points.h src/utils/Draw.cc src/utils/Draw.h src/utils/SplineUtils.cc src/utils/SplineUtils.h)

add_executable(approxCubicSpline src/ApproxCubicSpline.cc
        src/utils/Colors.cc src/utils/Colors.h src/utils/Renderer.cc src/utils/Renderer.h src/utils/Points.cc src/utils/Points.h src/utils/Draw.cc src/utils/Draw.h src/utils/SplineUtils.cc src/utils/SplineUtils.h)
