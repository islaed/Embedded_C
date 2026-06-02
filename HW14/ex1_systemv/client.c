#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>

#define MSG_SIZE 256

// Структура для разделяемой памяти
typedef struct {
    char server_msg[MSG_SIZE];
    char client_msg[MSG_SIZE];
} shared_data;

// Операции с семафорами
void sem_wait(int semid, int sem_num)
{
    struct sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    semop(semid, &sb, 1);
}

void sem_post(int semid, int sem_num)
{
    struct sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    semop(semid, &sb, 1);
}

int main()
{
    const char *q_name = ".";
    const char *message_send = "Hello!";

    // Получаем ключ
    key_t key = ftok(q_name, 0);
    if(key == -1)
    {
        printf("Ошибка получения ключа!\n");
        return 1;
    }

    // Подключаемся к разделяемой памяти
    int shm_id = shmget(key, sizeof(shared_data), 0);
    if(shm_id == -1)
    {
        printf("Ошибка открытия разделяемой памяти!\n");
        return 1;
    }

    // Подключаем разделяемую память
    shared_data *shm_ptr = (shared_data *)shmat(shm_id, NULL, 0);
    if(shm_ptr == (void *)-1)
    {
        printf("Ошибка подключения памяти!\n");
        return 1;
    }

    // Получаем семафоры
    int sem_id = semget(key, 2, 0);
    if(sem_id == -1)
    {
        printf("Ошибка открытия семафоров!\n");
        return 1;
    }

    // Ожидаем сообщения от сервера
    sem_wait(sem_id, 0);

    // Читаем сообщение от сервера
    printf("Получено: %s\n", shm_ptr->server_msg);

    // Записываем свой ответ
    strncpy(shm_ptr->client_msg, message_send, MSG_SIZE - 1);
    shm_ptr->client_msg[MSG_SIZE - 1] = '\0';

    // Сигнализируем серверу, что ответ готов
    sem_post(sem_id, 1);

    // Отключаемся от разделяемой памяти
    shmdt(shm_ptr);

    return 0;
}