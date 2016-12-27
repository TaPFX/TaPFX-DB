#ifndef _TAPWOLFSERVER_H_
#define _TAPWOLFSERVER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "UDPServer.h"
#include "ledString.h"

#define DEFAULTPORT 5555
#define LEDANZ 36

#define SLEEPMIN  2000000  // Minimale Zeit zwischen Blinzeln
#define SLEEPMAX  3000000  // Maximale Zeit zwischen Blinzeln
#define BLINKTIME  100000  // Blinzelzeit Augen geschlossen

int debug;
char* Mode;


//pthread_mutex_t mutexWolfRun,mutexStarRun;
pthread_mutex_t mutexThreadRun;
int WolfThreadRun;
int StarThreadRun;

int parseCommand(char command[BUFSIZE]);

#endif
