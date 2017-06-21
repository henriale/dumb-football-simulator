#include "motion.h"
#include "../opengl.h"
#include "../shapes.h"

/**
 * Exemplo de função de movimentação: move o robô em direção à bola
 *
 * @param body
 * @param data
 *
 */
void moveRobo(cpBody *body, void *data)
{
  // Veja como obter e limitar a velocidade do robô...
  cpVect vel = cpBodyGetVelocity(body);
  // printf("vel: %f %f", vel.x,vel.y);

  // Limita o vetor em 50 unidades
  vel = cpvclamp(vel, 50);
  // E seta novamente a velocidade do corpo
  cpBodySetVelocity(body, vel);

  // Obtém a posição do robô e da bola...
  cpVect robotPos = cpBodyGetPosition(body);
  cpVect ballPos = cpBodyGetPosition(ballBody);

  // Calcula um vetor do robô à bola (DELTA = B - R)
  cpVect pos = robotPos;
  pos.x = -robotPos.x;
  pos.y = -robotPos.y;
  cpVect delta = cpvadd(ballPos, pos);

  // Limita o impulso em 20 unidades
  delta = cpvmult(cpvnormalize(delta), 20);
  // Finalmente, aplica impulso no robô
  cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
}