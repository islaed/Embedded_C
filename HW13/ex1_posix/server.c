#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>


int main()
{
    const char *q_name = "/posix_q"; // Имя очереди
    const char *message_send = "Hi!"; // Сообщение, которое будем передавать
    char message_recieve[256]; // Сообщение которое будем получать

    mqd_t mq;

    // Структура сообщений
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10; // Максимальное количество сообщений в очереди
    attr.mq_msgsize = 256; // Максимальный размер сообщения
    attr.mq_curmsgs = 0;

    // Создаём очередь
    mq = mq_open(q_name, O_CREAT | O_RDWR, 0666, &attr);
    if(mq == -1)
    {
        printf("Ошибка создания очереди!\n");
        return 1;
    }

    // Отправляем сообщение
    if(mq_send(mq, message_send, sizeof(message_send), 0) == -1)
    {
        printf("Ошибка отправки сообщения!\n");
        return 1;
    }

    // Т.к. надо успеть запустить клиента и чтобы была одна очередь,
    // пришлось сделать sleep на 10 секунд. Без него или без второй очереди
    // сервер бы читал своё же сообщение из очереди
    sleep(10);

    // Получаем сообщение
    ssize_t message_read = mq_receive(mq, message_recieve, sizeof(message_recieve), NULL);
    if(message_read == -1)
    {
        printf("Ошибка получения сообщения!\n");
    }
    printf("Получено: %s\n", message_recieve);

    // Прекращаем работу с очередью и удаляем её
    mq_close(mq);
    mq_unlink(q_name);
    return 0;
}