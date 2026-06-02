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
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

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
    const char *message_send = "Hi!";

    // Создаём ключ для разделяемой памяти
    key_t key = ftok(q_name, 0);
    if(key == -1)
    {
        printf("Ошибка создания ключа!\n");
        return 1;
    }

    // Создаём разделяемую память
    int shm_id = shmget(key, sizeof(shared_data), IPC_CREAT | 0666);
    if(shm_id == -1)
    {
        printf("Ошибка создания разделяемой памяти!\n");
        return 1;
    }

    // Подключаем разделяемую память
    shared_data *shm_ptr = (shared_data *)shmat(shm_id, NULL, 0);
    if(shm_ptr == (void *)-1)
    {
        printf("Ошибка подключения памяти!\n");
        return 1;
    }

    // Создаём семафоры (2 семафора: 0 - для сервера, 1 - для клиента)
    int sem_id = semget(key, 2, IPC_CREAT | 0666);
    if(sem_id == -1)
    {
        printf("Ошибка создания семафоров!\n");
        return 1;
    }

    // Инициализируем семафоры
    union semun arg;
    arg.val = 0;
    semctl(sem_id, 0, SETVAL, arg); // Семафор для клиента (изначально 0)
    semctl(sem_id, 1, SETVAL, arg); // Семафор для сервера (изначально 0)

    // Записываем сообщение в разделяемую память
    strncpy(shm_ptr->server_msg, message_send, MSG_SIZE - 1);
    shm_ptr->server_msg[MSG_SIZE - 1] = '\0';

    // Сигнализируем клиенту, что сообщение готово
    sem_post(sem_id, 0);

    // Ожидаем ответа от клиента
    sem_wait(sem_id, 1);

    // Читаем сообщение от клиента
    printf("Получено: %s\n", shm_ptr->client_msg);

    // Очистка ресурсов
    shmdt(shm_ptr);
    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID);

    return 0;
}