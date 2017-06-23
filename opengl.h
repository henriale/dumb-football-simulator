#ifndef MYOPENGL_H
#define MYOPENGL_H
#ifdef _WIN32
#include <windows.h>    // includes only in MSWindows not in UNIX
#include "gl/glut.h"
#endif
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <chipmunk.h>
#define TEAM_A 1
#define TEAM_B 2
#define LARGURA_JAN 1024
#define ALTURA_JAN 712
#define ZONE_SIZE LARGURA_JAN/3
// Definição dos parâmetros das funções de movimento
// (tipo do ponteiro de função)
typedef void (*bodyMotionFunc)(cpBody *body, void *data);
typedef unsigned int playerTeam;

typedef struct {
    GLuint texture;
    cpFloat radius;
    cpShape *shape;
    bodyMotionFunc motionFunction;
    playerTeam team;
    cpVect defaultPosition;
} UserData;
// Funções da interface gráfica e OpenGL
void init(int argc, char **argv);
GLuint loadImage(char *img);
#endif // MYOPENGL_H
