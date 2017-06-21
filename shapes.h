#ifndef DUMB_FOOTBALL_SIMULATOR_SHAPES_H
#define DUMB_FOOTBALL_SIMULATOR_SHAPES_H

#include "opengl.h"

void loadBall();

cpBody *ballBody;
cpSpace *space;

cpShape *newLine(cpVect inicio, cpVect fim, cpFloat fric, cpFloat elast);
cpBody *newCircle(cpVect pos, cpFloat radius, cpFloat mass, char *img, bodyMotionFunc motion, cpFloat fric, cpFloat elast);

#endif //DUMB_FOOTBALL_SIMULATOR_SHAPES_H
