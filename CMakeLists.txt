cmake_minimum_required(VERSION 2.8)

project(dumb-football-simulator)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

set(SOURCES
        main.c
        opengl.c
        include/SOIL/SOIL.h
        SOIL/SOIL.c
        SOIL/image_DXT.c
        SOIL/image_helper.c
        SOIL/stb_image_aug.c
        )

include_directories("include/chipmunk" "include/SOIL")
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    message(STATUS "Compilando no Mac OSX...")
    add_definitions(-Wno-deprecated-declarations)
    link_directories(${CMAKE_SOURCE_DIR}/lib-Mac)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    message(STATUS "Compilando no Linux...")
    link_directories(${CMAKE_SOURCE_DIR}/lib-Linux)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    message(STATUS "Compilando no Linux...")
    link_directories(${CMAKE_SOURCE_DIR}/lib-Windows)
endif ()

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} -lm)
target_link_libraries(${PROJECT_NAME} Chipmunk)
