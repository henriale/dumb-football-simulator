#include "shapes.h"

void loadBall()
{
  ballBody = newCircle(cpv(512, 350), 8, 1, "../images/ball.png", NULL, 0.2, 1);
}

/**
 * Cria e adiciona uma nova linha estática (segmento) ao ambiente
 *
 * @param inicio
 * @param fim
 * @param fric
 * @param elast
 * @return
 */
cpShape *newLine(cpVect inicio, cpVect fim, cpFloat fric, cpFloat elast)
{
  cpShape *aux = cpSegmentShapeNew(cpSpaceGetStaticBody(space), inicio, fim, 0);
  cpShapeSetFriction(aux, fric);
  cpShapeSetElasticity(aux, elast);
  cpSpaceAddShape(space, aux);

  return aux;
}

/**
 * Cria e adiciona um novo corpo dinâmico, com formato circular
 *
 * @param pos
 * @param radius
 * @param mass
 * @param img
 * @param motion
 * @param fric
 * @param elast
 *
 * @return cpBody
 */
cpBody *newCircle(cpVect pos, cpFloat radius, cpFloat mass, char *img, bodyMotionFunc motion, cpFloat fric, cpFloat elast)
{
  // Primeiro criamos um cpBody para armazenar as propriedades fisicas do objeto
  // Estas incluem: massa, posicao, velocidade, angulo, etc do objeto
  // A seguir, adicionamos formas de colisao ao cpBody para informar o seu formato e tamanho

  // O momento de inercia e' como a massa, mas para rotacao
  // Use as funcoes cpMomentFor*() para calcular a aproximacao dele
  cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

  // As funcoes cpSpaceAdd*() retornam o que voce esta' adicionando
  // E' conveniente criar e adicionar um objeto na mesma linha
  cpBody *newBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));

  // Por fim, ajustamos a posicao inicial do objeto
  cpBodySetPosition(newBody, pos);

  // Agora criamos a forma de colisao do objeto
  // Voce pode criar multiplas formas de colisao, que apontam ao mesmo objeto (mas nao e' necessario para o trabalho)
  // Todas serao conectadas a ele, e se moverao juntamente com ele
  cpShape *newShape = cpSpaceAddShape(space, cpCircleShapeNew(newBody, radius, cpvzero));
  cpShapeSetFriction(newShape, fric);
  cpShapeSetElasticity(newShape, elast);
  UserData *newUserData = malloc(sizeof(UserData));
  newUserData->tex = loadImage(img);
  newUserData->radius = radius;
  newUserData->shape = newShape;
  newUserData->func = motion;
  cpBodySetUserData(newBody, newUserData);
  printf("newCircle: loaded img %s\n", img);

  return newBody;
}
