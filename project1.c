#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


#define N_TREMS 7

sem_t semCru;

struct Trem
{
    int id;
    char dir_origem;
    char dir_destino;
    int prioridade;
};