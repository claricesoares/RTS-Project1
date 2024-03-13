#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_TRAINS 10

// Definições das prioridades
#define ALTA 0
#define MEDIA 1
#define BAIXA 2

// Define as saidas
#define A2 1
#define B2 3

// Estrutura para representar um trem
typedef struct
{
    int id;
    int prioridade; // 0 - alta, 1 - média, 2 - baixa
    int origem;     // 0 - A1, 1 - B1
    int destino;    // 0 - A2, 1 - B2
} Trem;

// Filas de trens
Trem fila_a1[NUM_TRAINS];
Trem fila_b1[NUM_TRAINS];
int a1_index = 0;
int b1_index = 0;

// Semáforos
sem_t semaforo_a1;
sem_t semaforo_b1;
sem_t semaforo_prioridade;

// Função para a thread dos trens
void *thread_trem(void *arg)
{
    Trem *trem = (Trem *)arg;

    // Simulação da chegada do trem
    sleep(2);

    // Verifica qual fila adicionar o trem e insere
    if (trem->origem == 0)
    {
        sem_wait(&semaforo_a1);
        fila_a1[a1_index++] = *trem;
        printf("Trem %d chegou em A1 com prioridade %d e vai p/ direção %s\n", trem->id, trem->prioridade, trem->destino == A2 ? "A2" : "B2");
    }
    else
    {
        sem_wait(&semaforo_b1);
        fila_b1[b1_index++] = *trem;
        printf("Trem %d chegou em B1 com prioridade %d e vai p/ direção %s\n", trem->id, trem->prioridade, trem->destino == A2 ? "A2" : "B2");
    }

    free(trem);
    pthread_exit(NULL);
}

// Função para controlar a saída dos trens
void *controlar_saida(void *arg)
{
    while (1)
    {
        sem_wait(&semaforo_prioridade);

        // Verifica se há trens nas filas A1 e B1
        if (a1_index > 0 && b1_index > 0)
        {
            // Obtém o primeiro trem de cada fila
            Trem trem_a1 = fila_a1[0];
            Trem trem_b1 = fila_b1[0];

            // Escolhe o trem com a prioridade mais alta
            if (trem_a1.prioridade < trem_b1.prioridade)
            {
                printf("Trem %d esta saindo de A1 com prioridade %d e vai p/ direção %s\n", trem_a1.id, trem_a1.prioridade, trem_a1.destino == A2 ? "A2" : "B2");
                sleep(2);
                printf("Trem %d saiu de A1 com prioridade %d e vai p/ direção %s\n", trem_a1.id, trem_a1.prioridade, trem_a1.destino == A2 ? "A2" : "B2");
                a1_index--;
                for (int i = 0; i < a1_index; i++)
                    fila_a1[i] = fila_a1[i + 1];
                sem_post(&semaforo_a1);
            }
            else if (trem_b1.prioridade < trem_a1.prioridade)
            {
                printf("Trem %d esta saindo de B1 com prioridade %d e vai p/ direção %s\n", trem_b1.id, trem_b1.prioridade, trem_b1.destino == A2 ? "A2" : "B2");
                sleep(2);
                printf("Trem %d saiu de B1 com prioridade %d e vai p/ direção %s\n", trem_b1.id, trem_b1.prioridade, trem_b1.destino == A2 ? "A2" : "B2");
                b1_index--;
                for (int i = 0; i < b1_index; i++)
                    fila_b1[i] = fila_b1[i + 1];
                sem_post(&semaforo_b1);
            }
            else
            {
                // Trens têm mesma prioridade, escolhe o próximo na fila, verificando o trem que chegou primeiro
                if (trem_a1.id < trem_b1.id)
                {
                    /* code */
                    printf("Trem %d esta saindo de A1 com prioridade %d e vai p/ direção %s\n", trem_a1.id, trem_a1.prioridade, trem_a1.destino == A2 ? "A2" : "B2");
                    sleep(2);
                    printf("Trem %d saiu de A1 com prioridade %d e vai p/ direção %s\n", trem_a1.id, trem_a1.prioridade, trem_a1.destino == A2 ? "A2" : "B2");
                    a1_index--;
                    for (int i = 0; i < a1_index; i++)
                        fila_a1[i] = fila_a1[i + 1];
                    sem_post(&semaforo_a1);
                }
                else
                {
                    printf("Trem %d esta saindo de B1 com prioridade %d e vai p/ direção %s\n", trem_b1.id, trem_b1.prioridade, trem_b1.destino == A2 ? "A2" : "B2");
                    sleep(2);
                    printf("Trem %d saiu de B1 com prioridade %d e vai p/ direção %s\n", trem_b1.id, trem_b1.prioridade, trem_b1.destino == A2 ? "A2" : "B2");
                    b1_index--;
                    for (int i = 0; i < b1_index; i++)
                        fila_b1[i] = fila_b1[i + 1];
                    sem_post(&semaforo_b1);
                }
            }
        }
        sem_post(&semaforo_prioridade);

        usleep(rand() % 1000);
    }
}

int main()
{
    srand(time(NULL));

    // Inicialização dos semáforos
    sem_init(&semaforo_a1, 0, 1);
    sem_init(&semaforo_b1, 0, 1);
    sem_init(&semaforo_prioridade, 0, 1);

    // Criação da thread de controle de saída
    pthread_t thread_controle;
    pthread_create(&thread_controle, NULL, controlar_saida, NULL);

    // Criação das threads dos trens
    pthread_t threads_trens[NUM_TRAINS];
    for (int i = 0; i < NUM_TRAINS; i++)
    {
        Trem *trem = (Trem *)malloc(sizeof(Trem));
        trem->id = i + 1;
        trem->prioridade = rand() % 3; // Prioridade aleatória
        trem->origem = rand() % 2;     // Origem aleatória
        trem->destino = rand() % 2;    // Destino aleatório
        pthread_create(&threads_trens[i], NULL, thread_trem, trem);
    }

    // Espera as threads terminarem
    for (int i = 0; i < NUM_TRAINS; i++)
        pthread_join(threads_trens[i], NULL);

    // Destruir semáforos
    sem_destroy(&semaforo_a1);
    sem_destroy(&semaforo_b1);
    sem_destroy(&semaforo_prioridade);

    return 0;
}