#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
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
    const char *message_send = "Hi!";

    // Создаём разделяемую память
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if(shm_fd == -1)
    {
        printf("Ошибка создания разделяемой памяти!\n");
        return 1;
    }

    // Устанавливаем размер разделяемой памяти
    if(ftruncate(shm_fd, sizeof(shared_data)) == -1)
    {
        printf("Ошибка установки размера памяти!\n");
        return 1;
    }

    // Отображаем разделяемую память в адресное пространство процесса
    shared_data *shm_ptr = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_ptr == MAP_FAILED)
    {
        printf("Ошибка отображения памяти!\n");
        return 1;
    }

    // Создаём семафоры для синхронизации
    sem_t *sem_server = sem_open(SEM_SERVER, O_CREAT, 0666, 0);
    sem_t *sem_client = sem_open(SEM_CLIENT, O_CREAT, 0666, 0);

    if(sem_server == SEM_FAILED || sem_client == SEM_FAILED)
    {
        printf("Ошибка создания семафоров!\n");
        return 1;
    }

    // Записываем сообщение в разделяемую память
    strncpy(shm_ptr->server_msg, message_send, MSG_SIZE - 1);
    shm_ptr->server_msg[MSG_SIZE - 1] = '\0';

    // Сигнализируем клиенту, что сообщение готово
    sem_post(sem_server);

    // Ожидаем ответа от клиента
    sem_wait(sem_client);

    // Читаем сообщение от клиента
    printf("Получено: %s\n", shm_ptr->client_msg);

    // Очистка ресурсов
    munmap(shm_ptr, sizeof(shared_data));
    close(shm_fd);
    shm_unlink(SHM_NAME);

    sem_close(sem_server);
    sem_close(sem_client);
    sem_unlink(SEM_SERVER);
    sem_unlink(SEM_CLIENT);

    return 0;
}