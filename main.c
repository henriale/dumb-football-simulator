#include <chipmunk.h>
#include "opengl.h"

typedef unsigned int fieldZone;

fieldZone bodyZone(cpBody *body);

void moveAttackerToTheBall(cpBody *playerBody, void *data);
void moveAttackerToOrigin(cpBody *body, void *data);
void moveDefenderToTheBall(cpBody *body, void *data);
void moveDefenderToOrigin(cpBody *body, void *data);
void moveGoalkepper(cpBody *body, void *data);
void waitForBallToApproach(cpBody *body, void *data);
void ballMotion(cpBody *ballBody, void *data);

bool ballIsInDefendingZone(cpBody *playerBody, cpBody *ballBody);
bool ballIsInMidfield(cpBody *playerBody, cpBody *ballBody);
bool playerIsInAttackingZone(cpBody *playerBody);
bool playerIsInMidfield(cpBody *playerBody);
bool playerIsInDefendingZone(cpBody *playerBody);

int distanceToTheBall(cpBody *body);
int distanceToOrigin(cpBody *pBody);

// Prototipos
void initCM();
void freeCM();
void restartCM();

cpBody *newPlayer(playerTeam team, cpVect pos, cpFloat radius, cpFloat mass, bodyMotionFunc motion, cpFloat fric, cpFloat elast);
cpShape *newLine(cpVect inicio, cpVect fim, cpFloat fric, cpFloat elast);
cpBody *newCircle(cpVect pos, cpFloat radius, cpFloat mass, char *img, bodyMotionFunc motion, cpFloat fric, cpFloat elast);
int distanceToOrigin(cpBody *pBody);
const static int LEFT_ZONE = 1;
const static int MID_ZONE = 2;
const static int RIGHT_ZONE = 3;

const static int SLOW_PACE = 40;
const static int NORMAL_PACE = 75;
const static int FAST_PACE = 110;

const static double ELAST_DEFAULT = 1.5;

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

cpBody *playersBody[12];

/**
 * Clock.
 * Cada passo de simulação é 1/60 seg.
 */
cpFloat timeStep = 1.0 / 60.0;
bool ballIsInsideGoal = false;
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
  ballBody = newCircle(cpv(512, 350), 8, 1, "../images/ball.png", ballMotion, 0.2, 1);

  playersBody[0] = newPlayer(TEAM_A, cpv(ZONE_SIZE*0.2, 350+rand()%100-50), 20, 5, moveGoalkepper, 0.2, ELAST_DEFAULT+rand()%1-0.3);
  playersBody[1] = newPlayer(TEAM_A, cpv(ZONE_SIZE*0.7+rand()%30-15, 180+rand()%30-15), 20, 5, waitForBallToApproach, 0.2, ELAST_DEFAULT+rand()%1-0.3);
  playersBody[2] = newPlayer(TEAM_A, cpv(ZONE_SIZE*0.6+rand()%30-15, 350+rand()%30-15), 20, 5, waitForBallToApproach, 0.2, ELAST_DEFAULT+rand()%1-0.3);
  playersBody[3] = newPlayer(TEAM_A, cpv(ZONE_SIZE*0.7+rand()%30-15, 520+rand()%30-15), 20, 5, waitForBallToApproach, 0.2, ELAST_DEFAULT+rand()%1-0.3);
  playersBody[4] = newPlayer(TEAM_A, cpv(350, 250), 20, 5, moveAttackerToTheBall, 0.2, ELAST_DEFAULT+rand()%1-0.3);
  playersBody[5] = newPlayer(TEAM_A, cpv(350, 450), 20, 5, moveAttackerToTheBall, 0.2, ELAST_DEFAULT+rand()%1-0.3);

  playersBody[6] = newPlayer(TEAM_B, cpv(ZONE_SIZE*2+ZONE_SIZE*0.8, 350+rand()%100-50), 20, 5, moveGoalkepper, 0.2, ELAST_DEFAULT+rand()%1-0.3);
  playersBody[7] = newPlayer(TEAM_B, cpv(ZONE_SIZE*2+ZONE_SIZE*0.3+rand()%30-15, 180+rand()%30-15), 20, 5, waitForBallToApproach, 0.2, ELAST_DEFAULT+rand()%1-0.3);
  playersBody[8] = newPlayer(TEAM_B, cpv(ZONE_SIZE*2+ZONE_SIZE*0.4+rand()%30-15, 350+rand()%30-15), 20, 5, waitForBallToApproach, 0.2, ELAST_DEFAULT+rand()%1-0.3);
  playersBody[9] = newPlayer(TEAM_B, cpv(ZONE_SIZE*2+ZONE_SIZE*0.3+rand()%30-15, 520+rand()%30-15), 20, 5, waitForBallToApproach, 0.2, ELAST_DEFAULT+rand()%1-0.3);
  playersBody[10] = newPlayer(TEAM_B, cpv(650+rand()%100-50, 200+rand()%60-30), 20, 5, moveAttackerToTheBall, 0.2, ELAST_DEFAULT+rand()%1-0.3);
  playersBody[11] = newPlayer(TEAM_B, cpv(650+rand()%100-50, 500+rand()%60-30), 20, 5, moveAttackerToTheBall, 0.2, ELAST_DEFAULT+rand()%1-0.3);
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

  for (int i = 0; i < 12; ++i) {
      ud = cpBodyGetUserData(playersBody[i]);
      cpShapeFree(ud->shape);
      cpBodyFree(playersBody[i]);
  }

  cpShapeFree(leftWall);
  cpShapeFree(rightWall);
  cpShapeFree(bottomWall);
  cpShapeFree(topWall);
  cpSpaceFree(space);
}
// Função chamada para reiniciar a simulação
void restartCM()
{
    UserData *ud;
    cpBody *bod;
    cpVect pos;
    for (int i = 0; i < 12; ++i) {
        bod = playersBody[i];
        ud = cpBodyGetUserData(bod);
        pos = cpBodyGetPosition(bod);
        pos.x = ud->defaultPosition.x;
        pos.y = ud->defaultPosition.y;
        cpBodySetPosition(bod, pos);
    }

    pos.x = 512;
    pos.y = 350;
    cpBodySetPosition(ballBody, pos);

  // Escreva o código para reposicionar os jogadores, ressetar o score, etc.
  score1 = score2 = 0;
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
 *
 * @param body
 *
 * @return
 */
fieldZone bodyZone(cpBody *body)
{
  cpVect position = cpBodyGetPosition(body);

  if (position.x < ZONE_SIZE) {
      return LEFT_ZONE;
    }

  if (position.x >= ZONE_SIZE && position.x <= ZONE_SIZE*2) {
      return MID_ZONE;
    }

  if (position.x > ZONE_SIZE*2) {
      return RIGHT_ZONE;
    }
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
  newUserData->texture = loadImage(img);
  newUserData->radius = radius;
  newUserData->shape = newShape;
  newUserData->motionFunction = motion;
  cpBodySetUserData(newBody, newUserData);
  printf("newCircle: loaded img %s\n", img);

  return newBody;
}

/**
 * Create new circle body and set a team to its user data
 *
 * @param team
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
cpBody * newPlayer(playerTeam team, cpVect pos, cpFloat radius, cpFloat mass, bodyMotionFunc motion, cpFloat fric, cpFloat elast)
{
  char *img;
  if (team == TEAM_A) {
      img = "../images/player1.png";
    } else {
      img = "../images/player2.png";
    }

  cpBody *body = newCircle(pos, radius, mass, img, motion, fric, elast);

  UserData *userData = cpBodyGetUserData(body);
  userData->team = team;
  userData->defaultPosition = pos;

  return body;
}

/**
 * Exemplo de função de movimentação: move o robô em direção à bola
 *
 * @param playerBody
 * @param data
 *
 * @todo: extract
 */
void moveAttackerToTheBall(cpBody *playerBody, void *data)
{
  UserData *userData = data;

  if (! playerIsInMidfield(playerBody) || distanceToTheBall(playerBody) > 500 || distanceToOrigin(playerBody) > 200) {
      userData->motionFunction = moveAttackerToOrigin;
  }

  cpVect robotPos = cpBodyGetPosition(playerBody);
  cpVect ballPos = cpBodyGetPosition(ballBody);

  cpVect vel = cpBodyGetVelocity(playerBody);
  int pace = FAST_PACE;

  if (distanceToTheBall(playerBody) > 350) {
      pace = NORMAL_PACE;
    }

  if (distanceToTheBall(playerBody) > 450) {
      pace = SLOW_PACE;
    }

  vel = cpvclamp(vel, pace + rand()%30);
  cpBodySetVelocity(playerBody, vel);

  // Calcula um vetor do robô à bola (DELTA = B - R)
  cpVect pos = robotPos;
  pos.x = -robotPos.x;
  pos.y = -robotPos.y;
  cpVect delta = cpvadd(ballPos, pos);

  // Limita o impulso em 20 unidades
  delta = cpvmult(cpvnormalize(delta), 20);
  // Finalmente, aplica impulso no robô
  cpBodyApplyImpulseAtWorldPoint(playerBody, delta, robotPos);
}

int distanceToTheBall(cpBody *body)
{
  cpVect robotPos = cpBodyGetPosition(body);
  cpVect ballPos = cpBodyGetPosition(ballBody);

  int x = abs((int) (ballPos.x - robotPos.x));
  int y = abs((int) (ballPos.y - robotPos.y));

  return (int) sqrt(pow(x, 2) + pow(y, 2));
}

/**
 *
 * @param body
 * @param data
 */
void moveDefenderToTheBall(cpBody *body, void *data)
{
  UserData *userData = data;

  if (! ballIsInDefendingZone(body, ballBody) || distanceToTheBall(body) > 500 || distanceToOrigin(body) > 200) {
      userData->motionFunction = moveDefenderToOrigin;
      return;
  }

  cpVect robotPos = cpBodyGetPosition(body);
  cpVect ballPos = cpBodyGetPosition(ballBody);

  cpVect vel = cpBodyGetVelocity(body);
  int pace = FAST_PACE;

  if (distanceToTheBall(body) > 350) {
    pace = NORMAL_PACE;
  }

  if (distanceToTheBall(body) > 450) {
      pace = SLOW_PACE;
  }

  vel = cpvclamp(vel, pace + rand()%30);
  cpBodySetVelocity(body, vel);

  cpVect pos = robotPos;
  pos.x = -robotPos.x;
  pos.y = -robotPos.y;
  cpVect delta = cpvadd(ballPos, pos);
  delta = cpvmult(cpvnormalize(delta), 20);

  cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
}
/**
 *
 * @param body
 * @param data
 */
void moveGoalkepper(cpBody *body, void *data)
{
  cpVect vel = cpBodyGetVelocity(body);
  vel = cpvclamp(vel, SLOW_PACE);
  cpBodySetVelocity(body, vel);

  UserData *userData = data;

  cpVect playerPosition = cpBodyGetPosition(body);
  cpVect ballPosition = cpBodyGetPosition(ballBody);

  cpVect invPlayerPosition = playerPosition;
  invPlayerPosition.x = -playerPosition.x;
  invPlayerPosition.y = -playerPosition.y;


  cpVect direction = ballPosition;
  direction.x = userData->defaultPosition.x;
  direction.y = playerPosition.y < 400.0 && playerPosition.y > 300.0 ? ballPosition.y : userData->defaultPosition.y;

  cpVect delta = cpvadd(direction, invPlayerPosition);
  delta = cpvmult(cpvnormalize(delta), 20);

  cpBodyApplyImpulseAtWorldPoint(body, delta, playerPosition);
}

/**
 *
 * @param body
 * @param data
 */
void moveDefenderToOrigin(cpBody *body, void *data)
{
  cpVect vel = cpBodyGetVelocity(body);
  vel = cpvclamp(vel, NORMAL_PACE + rand()%60-20);
  cpBodySetVelocity(body, vel);
  UserData *userData = data;

  if (ballIsInDefendingZone(body, ballBody)) {
      userData->motionFunction = moveDefenderToTheBall;
      return;
  }

  cpVect playerPosition = cpBodyGetPosition(body);
  cpVect pos = playerPosition;
  pos.x = -playerPosition.x;
  pos.y = -playerPosition.y;
  cpVect delta = cpvadd(userData->defaultPosition, pos);
  delta = cpvmult(cpvnormalize(delta), 20);

  cpBodyApplyImpulseAtWorldPoint(body, delta, playerPosition);
}

/**
 *
 * @param body
 * @param data
 */
void moveAttackerToOrigin(cpBody *body, void *data)
{
  cpVect vel = cpBodyGetVelocity(body);
  vel = cpvclamp(vel, SLOW_PACE + rand()%30);
  cpBodySetVelocity(body, vel);
  UserData *userData = data;

  if (ballIsInMidfield(body, ballBody)) {
      userData->motionFunction = moveAttackerToTheBall;
      return;
  }

  cpVect playerPosition = cpBodyGetPosition(body);
  cpVect pos = playerPosition;
  pos.x = -playerPosition.x;
  pos.y = -playerPosition.y;
  cpVect delta = cpvadd(userData->defaultPosition, pos);
  delta = cpvmult(cpvnormalize(delta), 20);

  cpBodyApplyImpulseAtWorldPoint(body, delta, playerPosition);
}

/**
 *
 * @param body
 * @param data
 */
void waitForBallToApproach(cpBody *body, void *data)
{
  cpVect vel = cpBodyGetVelocity(body);
  vel = cpvclamp(vel, 0);
  cpBodySetVelocity(body, vel);
  UserData *userData = data;

  if (ballIsInDefendingZone(body, ballBody)) {
      userData->motionFunction = moveDefenderToTheBall;
  }
}

/**
 *
 * @param body
 * @param data
 */
void ballMotion(cpBody *body, void *data)
{
  cpVect ballPosition = cpBodyGetPosition(body);

  if (ballPosition.x < 100 && ballPosition.x > 20 && ballPosition.y > 350 && ballPosition.y < 450) {
      if (ballIsInsideGoal) {
          score1++;
      }
  } else {
      ballIsInsideGoal = false;
  }

  if (ballPosition.x < LARGURA_JAN-20 && ballPosition.x > LARGURA_JAN-100 && ballPosition.y > 350 && ballPosition.y < 450) {
      if (ballIsInsideGoal) {
          score2++;
        }
    } else {
      ballIsInsideGoal = false;
    }
}

/**
 *
 * @param playerBody
 * @param ballBody
 *
 * @return
 */
bool ballIsInDefendingZone(cpBody *playerBody, cpBody *ballBody)
{
  UserData *userData = cpBodyGetUserData(playerBody);
  playerTeam team = userData->team;

  fieldZone zone = bodyZone(ballBody);

  if ((team == TEAM_A && zone == LEFT_ZONE) || (team == TEAM_B && zone == RIGHT_ZONE)) {
      return true;
  }

  return false;
}

/**
 *
 * @param playerBody
 * @param ballBody
 *
 * @return
 */
bool ballIsInMidfield(cpBody *playerBody, cpBody *ballBody)
{
  UserData *userData = cpBodyGetUserData(playerBody);
  playerTeam team = userData->team;

  fieldZone zone = bodyZone(ballBody);

  // TODO: refactor
  if ((team == TEAM_A && zone == MID_ZONE) || (team == TEAM_B && zone == MID_ZONE)) {
      return true;
  }

  return false;
}

/**
 *
 * @param playerBody
 *
 * @return
 */
bool playerIsInDefendingZone(cpBody *playerBody)
{
  UserData *userData = cpBodyGetUserData(playerBody);
  playerTeam team = userData->team;
  fieldZone zone = bodyZone(playerBody);

  if ((team == TEAM_A && zone == LEFT_ZONE) || (team == TEAM_B && zone == RIGHT_ZONE)) {
      return true;
  }

  return false;
}

/**
 *
 * @param playerBody
 *
 * @return
 */
bool playerIsInAttackingZone(cpBody *playerBody)
{
  UserData *userData = cpBodyGetUserData(playerBody);
  playerTeam team = userData->team;
  fieldZone zone = bodyZone(playerBody);

  if ((team == TEAM_A && zone == RIGHT_ZONE) || (team == TEAM_B && zone == LEFT_ZONE)) {
      return true;
  }

  return false;
}

/**
 *
 * @param playerBody
 *
 * @return
 */
bool playerIsInMidfield(cpBody *playerBody)
{
  fieldZone zone = bodyZone(playerBody);

  if (zone == MID_ZONE) {
      return true;
  }

  return false;
}

int distanceToOrigin(cpBody *pBody)
{
  cpVect robotPos = cpBodyGetPosition(pBody);
  UserData *userData = cpBodyGetUserData(pBody);
  cpVect originPos = userData->defaultPosition;

  int x = abs((int) (originPos.x - robotPos.x));
  int y = abs((int) (originPos.y - robotPos.y));

  return (int) sqrt(pow(x, 2) + pow(y, 2));
}
