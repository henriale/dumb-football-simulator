#include <chipmunk.h>
#include "opengl.h"

/**
 * Funções para movimentação de objetos
 * @todo: extract
 */
void moveRobo(cpBody *body, void *data);
void moveDefenderToTheBall(cpBody *body, void *data);
void waitForBallToApproach(cpBody *body, void *data);

// Prototipos
void initCM();
void freeCM();
void restartCM();
cpShape *newLine(cpVect inicio, cpVect fim, cpFloat fric, cpFloat elast);
cpBody *newCircle(cpVect pos, cpFloat radius, cpFloat mass, char *img, bodyMotionFunc motion, cpFloat fric, cpFloat elast);
void moveDefenderToOrigin(cpBody *body, void *data)
;
/**
 * Score do jogo
 */
int score1 = 0;
int score2 = 0;

/**
 * Flag de controle (0 ou 1)
 */
int gameIsOver = 0;

/**
 * cpVect é um vetor 2D e cpv() é uma forma rápida de inicializar ele.
 */
cpVect gravity;

/**
 * O ambiente
 */
cpSpace *space;

/**
 * Paredes "invisíveis" do ambiente
 */
cpShape *leftWall, *rightWall, *topWall, *bottomWall;

/**
 * bola e jogadores
 * @todo: extract
 */
cpBody *ballBody;
cpBody *robotBody;

typedef struct {
  int from, to;
} xLimits;

cpBody *playersBody[];

/**
 * Clock.
 * Cada passo de simulação é 1/60 seg.
 */
cpFloat timeStep = 1.0 / 60.0;

/**
 * Inicializa campo e seus componentes
 */
void initCM()
{
  gravity = cpv(0, 100);

  // Cria o universo
  space = cpSpaceNew();

  // Seta o fator de damping, isto é, de atrito do ar
  cpSpaceSetDamping(space, 0.8);

  // Descomente a linha abaixo se quiser ver o efeito da gravidade!
  //cpSpaceSetGravity(space, gravity);

  // Adiciona 4 linhas estáticas para formarem as "paredes" do ambiente
  leftWall = newLine(cpv(0, 0), cpv(0, ALTURA_JAN), 0, 1.0);
  rightWall = newLine(cpv(LARGURA_JAN, 0), cpv(LARGURA_JAN, ALTURA_JAN), 0, 1.0);
  bottomWall = newLine(cpv(0, 0), cpv(LARGURA_JAN, 0), 0, 1.0);
  topWall = newLine(cpv(0, ALTURA_JAN), cpv(LARGURA_JAN, ALTURA_JAN), 0, 1.0);

  // todo: extract
  ballBody = newCircle(cpv(512, 350), 8, 1, "../images/ball.png", NULL, 0.2, 1);

  // team red
  robotBody = newCircle(cpv(50, 350), 20, 5, "../images/player1.png", moveRobo, 0.2, 0.5);

  // team blue
  playersBody[0] = newCircle(cpv(900, 350), 20, 5, "../images/player2.png", waitForBallToApproach, 0.2, 0.5);
}

/**
 * Exemplo de função de movimentação: move o robô em direção à bola
 *
 * @param body
 * @param data
 *
 * @todo: extract
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

/**
 * Libera memória ocupada por cada corpo, forma e ambiente
 * Acrescente mais linhas caso necessário
 */
void freeCM()
{
  printf("Cleaning up!\n");
  UserData *ud = cpBodyGetUserData(ballBody);
  cpShapeFree(ud->shape);
  cpBodyFree(ballBody);
  ud = cpBodyGetUserData(robotBody);
  cpShapeFree(ud->shape);
  cpBodyFree(robotBody);
  cpShapeFree(leftWall);
  cpShapeFree(rightWall);
  cpShapeFree(bottomWall);
  cpShapeFree(topWall);
  cpSpaceFree(space);
}
// Função chamada para reiniciar a simulação
void restartCM()
{
  // Escreva o código para reposicionar os jogadores, ressetar o score, etc.

  // Não esqueça de ressetar a variável gameIsOver!
  gameIsOver = 0;
}

// ************************************************************
//
// A PARTIR DESTE PONTO, O PROGRAMA NÃO DEVE SER ALTERADO
//
// A NÃO SER QUE VOCÊ SAIBA ***EXATAMENTE*** O QUE ESTÁ FAZENDO
//
// ************************************************************

/**
 * Interação via teclado e mouse apenas, na janela gráfica
 *
 * @param argc
 * @param argv
 *
 * @return
 */
int main(int argc, char **argv)
{
  init(argc, argv);
  glutMainLoop();

  return 0;
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
void moveDefenderToTheBall(cpBody *body, void *data)
{
  cpVect vel = cpBodyGetVelocity(body);
  vel = cpvclamp(vel, 35);
  cpBodySetVelocity(body, vel);

  cpVect robotPos = cpBodyGetPosition(body);
  cpVect ballPos = cpBodyGetPosition(ballBody);

  UserData *userData = data;

  if (robotPos.x < 700) {
      printf("changing defender motion to its position\n");
      userData->func = moveDefenderToOrigin;
      return;
  }

  cpVect pos = robotPos;
  pos.x = -robotPos.x;
  pos.y = -robotPos.y;
  cpVect delta = cpvadd(ballPos, pos);
  delta = cpvmult(cpvnormalize(delta), 20);

  cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
}

void moveDefenderToOrigin(cpBody *body, void *data)
{
  cpVect vel = cpBodyGetVelocity(body);
  vel = cpvclamp(vel, 90);
  cpBodySetVelocity(body, vel);

  cpVect robotPos = cpBodyGetPosition(body);
  cpVect ballPos = cpBodyGetPosition(ballBody);

  UserData *userData = data;

  // ball inside defender zone
  if (ballPos.x < 700 && robotPos.x > 700) {
      printf("changing defender motion to the ball\n");
      userData->func = moveDefenderToTheBall;
      return;
  }
  cpVect pos = robotPos;
  pos.x = -robotPos.x;
  pos.y = -robotPos.y;
  cpVect delta = cpvadd(ballPos, pos);
  delta = cpvmult(cpvnormalize(delta), 20);

  cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
}
void waitForBallToApproach(cpBody *body, void *data)
{
  printf("player is waiting for the ball\n");

  cpVect vel = cpBodyGetVelocity(body);
  vel = cpvclamp(vel, 0);
  cpBodySetVelocity(body, vel);

  cpVect robotPos = cpBodyGetPosition(body);
  cpVect ballPos = cpBodyGetPosition(ballBody);

  UserData *userData = data;

  // ball inside defender zone
  if (ballPos.x > 700) {
      userData->func = moveDefenderToTheBall;
  }
}
