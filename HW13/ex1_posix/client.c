#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>


int main()
{
    const char *q_name = "/posix_q";
    const char *message = "Hello!";
    char buffer[256];

    mqd_t mq;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 256;
    attr.mq_curmsgs = 0;

    // Открываем очередь на чтение и запись
    mq = mq_open(q_name, O_RDWR, 0666, &attr);
    if(mq == -1)
    {
        printf("Ошибка создания очереди!\n");
        return 1;
    }

    // Читаем сообщение из очереди
    ssize_t message_read = mq_receive(mq, buffer, sizeof(buffer), NULL);
    if(message_read == -1)
    {
        printf("Ошибка получения сообщения!\n");
    }
    printf("Получено: %s\n", buffer);

    // Передаём сообщение
    if(mq_send(mq, message, sizeof(message), 0) == -1)
    {
        printf("Ошибка отправки сообщения!\n");
        return 1;
    }

    // Прекращаем работу с очередью
    mq_close(mq);
    return 0;
}