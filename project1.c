#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_TRAINS 15
#define A1 0
#define A2 1
#define B1 2
#define B2 3

sem_t semaphore;
int crossing = -1; // Indica o cruzamento: -1 para livre, 0 para A1-A2, 1 para B1-B2
int train_count = 0; // Contador de trens

typedef struct {
    int id;
    int priority; // 0 - alta, 1 - média, 2 - baixa
    int origin; // 0 - A1, 1 - B1
    int destination; // 0 - A2, 1 - B2
} Train;

void* train_approach(void* arg) {
    Train* train = (Train*) arg;
    
    // Simulação de tempo de aproximação
    sleep(rand() % 5);
    
    printf("Train %d approaching the crossing from direction %s with priority %d.\n", train->id, train->origin == A1 ? "A1" : "B1", train->priority);
    
    // Verifica se o cruzamento está livre ou se precisa esperar
    sem_wait(&semaphore);
    while ((crossing != -1 && crossing != train->origin) || (crossing == train->origin && train_count > 0) || (train->priority == 0 && crossing != -1 && ((train->origin == A1) || (train->origin == B1)))) {
        sem_post(&semaphore);
        sem_wait(&semaphore);
    }
    // Atribui o cruzamento ao trem atual
    crossing = train->origin;
    train_count++;
    
    printf("Train %d crossing from %s to %s with priority %d.\n", train->id, train->origin == A1 ? "A1" : "B1", train->destination == A2 ? "A2" : "B2", train->priority);
    
    // Simulação de tempo de travessia
    sleep(rand() % 5);
    
    // Libera o cruzamento
    crossing = -1;
    train_count--;
    sem_post(&semaphore);

    printf("Train %d crossed from %s to %s with priority %d.\n", train->id, train->origin == A1 ? "A1" : "B1", train->destination == A2 ? "A2" : "B2", train->priority);
    
    free(train);
    return NULL;
}

int main() {
    pthread_t threads[NUM_TRAINS];
    srand(time(NULL));
    
    // Inicialização do semáforo
    sem_init(&semaphore, 0, 1);
    
    for (int i = 0; i < NUM_TRAINS; i++) {
        Train* train = malloc(sizeof(Train));
        train->id = i + 1;
        train->priority = rand() % 3; // Prioridade aleatória
        train->origin = rand() % 2; // Origem aleatória
        train->destination = rand() % 2; // Destino aleatório
        
        pthread_create(&threads[i], NULL, train_approach, (void*) train);
        
        // Simulação de intervalo entre trens
        sleep(rand() % 3);
    }
    
    for (int i = 0; i < NUM_TRAINS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Destruir o semáforo
    sem_destroy(&semaphore);
    
    return 0;
}
