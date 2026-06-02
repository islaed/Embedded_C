#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>

#define SHM_NAME "/posix_shm"
#define SEM_SERVER "/sem_server"
#define SEM_CLIENT "/sem_client"
#define MSG_SIZE 256

// Структура для разделяемой памяти
typedef struct {
    char server_msg[MSG_SIZE];
    char client_msg[MSG_SIZE];
} shared_data;

int main()
{
    const char *message = "Hello!";

    // Открываем разделяемую память
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if(shm_fd == -1)
    {
        printf("Ошибка открытия разделяемой памяти!\n");
        return 1;
    }

    // Отображаем разделяемую память в адресное пространство процесса
    shared_data *shm_ptr = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_ptr == MAP_FAILED)
    {
        printf("Ошибка отображения памяти!\n");
        return 1;
    }

    // Открываем семафоры
    sem_t *sem_server = sem_open(SEM_SERVER, 0);
    sem_t *sem_client = sem_open(SEM_CLIENT, 0);

    if(sem_server == SEM_FAILED || sem_client == SEM_FAILED)
    {
        printf("Ошибка открытия семафоров!\n");
        return 1;
    }

    // Ожидаем сообщения от сервера
    sem_wait(sem_server);

    // Читаем сообщение от сервера
    printf("Получено: %s\n", shm_ptr->server_msg);

    // Записываем свой ответ
    strncpy(shm_ptr->client_msg, message, MSG_SIZE - 1);
    shm_ptr->client_msg[MSG_SIZE - 1] = '\0';

    // Сигнализируем серверу, что ответ готов
    sem_post(sem_client);

    // Очистка ресурсов
    munmap(shm_ptr, sizeof(shared_data));
    close(shm_fd);

    sem_close(sem_server);
    sem_close(sem_client);

    return 0;
}