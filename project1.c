#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


#define N_TRENS 7
#define BUFFER_SIZE 1 //So pode 1 trem de cada vez
#define N_A1_TRENS 3
#define N_B1_TRENS 4

sem_t empty, full, mutex;
typedef enum{Velocidade, Pessoas, Carga}Prioridade;

//sem_t semCru;

typedef struct{
    int id;
    /*char dir_origem;
    char dir_destino;
    int prioridade;*/
    Prioridade prioridade;
}Trem;

void *cruzamento(void *arg){
    Trem *trem = (Trem *)arg;

    printf("Trem %d (Prioridade %d) esta se aproximando do cruzamento.",trem->id, trem->prioridade);
    sleep(rand()%3);
}