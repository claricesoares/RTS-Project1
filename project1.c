#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// Definição da quantidade de trens que se direcionará ao cruzamento
#define NUM_TRENS 10

// Definição das prioridades
#define ALTA 0
#define MEDIA 1
#define BAIXA 2

// Definição das saidas
#define A2 1
#define B2 3

// Condição de saida do while
int SAIDA = 0;

// Struct para representar cada trem
typedef struct
{
    int id;
    int prioridade; // 0 - alta, 1 - média, 2 - baixa
    int origem;     // 0 - A1, 1 - B1
    int destino;    // 0 - A2, 1 - B2
} Trem;

// Filas de trens
Trem fila_a1[NUM_TRENS];
Trem fila_b1[NUM_TRENS];
int a1_index = 0;
int b1_index = 0;

// Semáforos
sem_t semaforo_a1;          // Gestão E/S dos trens na fila A1
sem_t semaforo_b1;          // Gestão E/S dos trens na fila B1
sem_t semaforo_prioridade;  // Gestão de cruzamento dos trens

// Função para imprimir prioridades como strings
const char* prioridade_string(int prioridade) {
    switch(prioridade) {
        case 0:
            return "Alta  ";
        case 1:
            return "Média ";
        case 2:
            return "Baixa ";
        default:
            return "Desconhecida ";
    }
}

// Função para a thread dos trens
void *thread_trem(void *arg)
{
    Trem *trem = (Trem *)arg;

    // Simulação da chegada do trem
    usleep(100);

    // Verifica qual fila adicionar o trem e o insere
    if (trem->origem == 0)
    {
        sem_wait(&semaforo_a1); // Aguarda até que o semáforo esteja disponível 
        fila_a1[a1_index++] = *trem;
        printf("Trem: %2d | Prioridade: %s| Status: Se aproximando| Partindo de: A1 | Destino: %s\n", trem->id, prioridade_string(trem->prioridade), trem->destino == A2 ? "A2" : "B2");
        sem_post(&semaforo_a1); // Sinaliza que o semáforo está disponível
    }
    else
    {
        sem_wait(&semaforo_b1); 
        fila_b1[b1_index++] = *trem;
        printf("Trem: %2d | Prioridade: %s| Status: Se aproximando| Partindo de: B1 | Destino: %s\n", trem->id, prioridade_string(trem->prioridade), trem->destino == A2 ? "A2" : "B2");
        sem_post(&semaforo_b1);
    }

    free(trem);
}

// Função para controlar a saída dos trens (controlador)
void *controlar_saida(void *arg)
{
    // Enquanto SAIDA = 0 (valor inicial) a thread do controlador é executada
    while (!SAIDA)
    {
        sem_wait(&semaforo_prioridade); // Aguarda até que o semáforo esteja disponível  

        if (a1_index > 0 && b1_index == 0) // Só existe trem na fila A1
        {
            // Obtém o primeiro trem de cada fila
            sem_wait(&semaforo_a1);
            Trem trem_a1 = fila_a1[0];
            Trem trem_b1 = fila_b1[0];
            printf("Trem: %2d | Prioridade: %s| Status: Cruzando      | Partindo de: A1 | Destino: %s\n", trem_a1.id, prioridade_string(trem_a1.prioridade), trem_a1.destino == A2 ? "A2" : "B2");
            usleep(100);
            printf("Trem: %2d | Prioridade: %s| Status: Cruzou        | Partindo de: A1 | Destino: %s\n", trem_a1.id, prioridade_string(trem_a1.prioridade), trem_a1.destino == A2 ? "A2" : "B2");
            a1_index--;
            for (int i = 0; i < a1_index; i++)
                fila_a1[i] = fila_a1[i + 1];
            sem_post(&semaforo_a1);
        }
        else if (a1_index == 0 && b1_index > 0) // Só existe trem na fila B1
        {
            // Obtém o primeiro trem de cada fila
            sem_wait(&semaforo_b1);
            Trem trem_a1 = fila_a1[0];
            Trem trem_b1 = fila_b1[0];
            printf("Trem: %2d | Prioridade: %s| Status: Cruzando      | Partindo de: B1 | Destino: %s\n", trem_b1.id, prioridade_string(trem_b1.prioridade), trem_b1.destino == A2 ? "A2" : "B2");
            usleep(100);
            printf("Trem: %2d | Prioridade: %s| Status: Cruzou        | Partindo de: B1 | Destino: %s\n", trem_b1.id, prioridade_string(trem_b1.prioridade), trem_b1.destino == A2 ? "A2" : "B2");
            b1_index--;
            for (int i = 0; i < b1_index; i++)
                fila_b1[i] = fila_b1[i + 1];
            sem_post(&semaforo_b1);
        }

        // Verifica se há trens em ambas as filas A1 e B1
        else if (a1_index > 0 && b1_index > 0) 
        {
            // Obtém o primeiro trem de cada fila
            Trem trem_a1 = fila_a1[0];
            Trem trem_b1 = fila_b1[0];

            // Escolhe o trem com a prioridade mais alta
            if (trem_a1.prioridade < trem_b1.prioridade) // Prioridade do A1 prevalece
            {
                sem_wait(&semaforo_a1);
                printf("Trem: %2d | Prioridade: %s| Status: Cruzando      | Partindo de: A1 | Destino: %s\n", trem_a1.id, prioridade_string(trem_a1.prioridade), trem_a1.destino == A2 ? "A2" : "B2");
                printf("Trem: %2d | Prioridade: %s| Status: Aguardando    | Partindo de: B1 | Destino: %s\n", trem_b1.id, prioridade_string(trem_b1.prioridade), trem_b1.destino == A2 ? "A2" : "B2");
                usleep(100);
                printf("Trem: %2d | Prioridade: %s| Status: Cruzou        | Partindo de: A1 | Destino: %s\n", trem_a1.id, prioridade_string(trem_a1.prioridade), trem_a1.destino == A2 ? "A2" : "B2");
                a1_index--;
                for (int i = 0; i < a1_index; i++)
                    fila_a1[i] = fila_a1[i + 1];
                sem_post(&semaforo_a1);
            }
            else if (trem_b1.prioridade < trem_a1.prioridade) // Prioridade do B1 prevalece
            {
                sem_wait(&semaforo_b1);
                printf("Trem: %2d | Prioridade: %s| Status: Cruzando      | Partindo de: B1 | Destino: %s\n", trem_b1.id, prioridade_string(trem_b1.prioridade), trem_b1.destino == A2 ? "A2" : "B2");
                printf("Trem: %2d | Prioridade: %s| Status: Aguardando    | Partindo de: A1 | Destino: %s\n", trem_a1.id, prioridade_string(trem_a1.prioridade), trem_a1.destino == A2 ? "A2" : "B2");
                usleep(100);
                printf("Trem: %2d | Prioridade: %s| Status: Cruzou        | Partindo de: B1 | Destino: %s\n", trem_b1.id, prioridade_string(trem_b1.prioridade), trem_b1.destino == A2 ? "A2" : "B2");
                b1_index--;
                for (int i = 0; i < b1_index; i++)
                    fila_b1[i] = fila_b1[i + 1];
                sem_post(&semaforo_b1);
            }
            else
            {
                // Caso os trens tenham mesma prioridade, escolhe o trem com menor valor de ID
                if (trem_a1.id < trem_b1.id) // A1 com menor ID
                {
                    sem_wait(&semaforo_a1);
                    printf("Trem: %2d | Prioridade: %s| Status: Cruzando      | Partindo de: A1 | Destino: %s\n", trem_a1.id, prioridade_string(trem_a1.prioridade), trem_a1.destino == A2 ? "A2" : "B2");
                    printf("Trem: %2d | Prioridade: %s| Status: Aguardando    | Partindo de: B1 | Destino: %s\n", trem_b1.id, prioridade_string(trem_b1.prioridade), trem_b1.destino == A2 ? "A2" : "B2");
                    usleep(100);
                    printf("Trem: %2d | Prioridade: %s| Status: Cruzou        | Partindo de: A1 | Destino: %s\n", trem_a1.id, prioridade_string(trem_a1.prioridade), trem_a1.destino == A2 ? "A2" : "B2");
                    a1_index--;
                    for (int i = 0; i < a1_index; i++)
                        fila_a1[i] = fila_a1[i + 1];
                    sem_post(&semaforo_a1);
                }
                else // B1 com menor ID
                {
                    sem_wait(&semaforo_b1);
                    printf("Trem: %2d | Prioridade: %s| Status: Cruzando      | Partindo de: B1 | Destino: %s\n", trem_b1.id, prioridade_string(trem_b1.prioridade), trem_b1.destino == A2 ? "A2" : "B2");
                    printf("Trem: %2d | Prioridade: %s| Status: Aguardando    | Partindo de: A1 | Destino: %s\n", trem_a1.id, prioridade_string(trem_a1.prioridade), trem_a1.destino == A2 ? "A2" : "B2");
                    usleep(100);
                    printf("Trem: %2d | Prioridade: %s| Status: Cruzou        | Partindo de: B1 | Destino: %s\n", trem_b1.id, prioridade_string(trem_b1.prioridade), trem_b1.destino == A2 ? "A2" : "B2");
                    b1_index--;
                    for (int i = 0; i < b1_index; i++)
                        fila_b1[i] = fila_b1[i + 1];
                    sem_post(&semaforo_b1);
                }
            }
        }
        
        sem_post(&semaforo_prioridade); // Sinaliza que o semáforo está disponível

        usleep(rand() % 100);
    }
}

int main()
{
    int tempo_estimado = NUM_TRENS/3;

    srand(time(NULL));

    // Inicialização dos semáforos
    sem_init(&semaforo_a1, 0, 1);
    sem_init(&semaforo_b1, 0, 1);
    sem_init(&semaforo_prioridade, 0, 1);

    // Criação da thread de controle de saída
    pthread_t thread_controle;
    pthread_create(&thread_controle, NULL, controlar_saida, NULL);

    // Criação das threads dos trens
    pthread_t threads_trens[NUM_TRENS];
    for (int i = 0; i < NUM_TRENS; i++)
    {
        Trem *trem = (Trem *)malloc(sizeof(Trem));
        trem->id = i + 1;
        trem->prioridade = rand() % 3; // Prioridade aleatória
        trem->origem = rand() % 2;     // Origem aleatória
        trem->destino = rand() % 2;    // Destino aleatório
        pthread_create(&threads_trens[i], NULL, thread_trem, trem);
    }

    // Espera as threads terminarem execução
    for (int i = 0; i < NUM_TRENS; i++)
        pthread_join(threads_trens[i], NULL);

    // Tempo estimado para todos os trens cruzarem
    sleep(tempo_estimado);
    SAIDA = 1; // Sai do laço do while (controlar_saida) e encerra a thread de controle de saída
    
    // Espera a thread de controle terminar
    pthread_join(thread_controle, NULL); 

    // Destruição dos semáforos criados
    sem_destroy(&semaforo_a1);
    sem_destroy(&semaforo_b1);
    sem_destroy(&semaforo_prioridade);

    return 0;
}
