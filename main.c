#include <chipmunk.h>
#include "opengl.h"
#include "shapes.h"
#include "players/body.h"
/**
 * Funções para movimentação de objetos
 * @todo: extract
 */
void moveRobo(cpBody *body, void *data);

// Prototipos
void initCM();
void freeCM();
void restartCM();

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
 * Paredes "invisíveis" do ambiente
 */
cpShape *leftWall, *rightWall, *topWall, *bottomWall;

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

  loadBall();
  loadPlayers();
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
  ud = cpBodyGetUserData(playerBody);
  cpShapeFree(ud->shape);
  cpBodyFree(playerBody);
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
