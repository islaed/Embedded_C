#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define CUSTOMER_C 3
#define LAREK_C 5

int shop[LAREK_C];
pthread_mutex_t mutex[LAREK_C];
int customer_need[CUSTOMER_C];


// Функция покупателя
void *customer_buy(void *args)
{
    int customer_id = *(int *)args; // Забираем id из переданных аргументов
    int larek_id;

    // Цикл, пока у покупателя есть потребность
    while(customer_need[customer_id] > 0)
    {
        printf("\nПокупатель %d проснулся. Текущее количество потребности = %d.\n", customer_id + 1, customer_need[customer_id]);
        larek_id = rand() % LAREK_C; // Выбираем ларёк
        
        // Заходим в критическую секцию
        pthread_mutex_lock(&mutex[larek_id]);
        
        printf("Покупатель %d зашел в ларёк %d. Количество товаров = %d.\n", customer_id + 1, larek_id + 1, shop[larek_id]);
        customer_need[customer_id] -= shop[larek_id]; // Отнимаем у потребности покупателя всё что есть в ларьке
        shop[larek_id] = 0; // Обнуляем товары в ларьке, которые купил покупатель
        printf("У покупателя %d стало %d потребности. Покупатель уснул на 2 секунды.\n", customer_id + 1, customer_need[customer_id]);

        pthread_mutex_unlock(&mutex[larek_id]);
        // Выходим из критической секции

        usleep(2000000); // Усыпляем поток на 2 секунды
    }
}

// Функция погрузчика
void *loader_work(void *args)
{
    int larek_id;

    // Цикл, пока у какого либо покупателя еще есть потребность
    while(customer_need[0] > 0 || customer_need[1] > 0 || customer_need[2] > 0)
    {
        printf("\nПогрузчик проснулся.\n");
        larek_id = rand() % LAREK_C;

        pthread_mutex_lock(&mutex[larek_id]);

        printf("Погрузчик зашел в ларёк %d.\n", larek_id + 1);
        shop[larek_id] += 200; // Пополняем ларёк на 200 товаров
        printf("Погрузчик пополнил ларёк %d на 200 товаров. Погрузчик уснул на 1 секунду.\n", larek_id + 1);

        pthread_mutex_unlock(&mutex[larek_id]);

        usleep(1000000); // Усыпляем поток на 1 секунду
    }
}

int main()
{
    srand(time(NULL));

    pthread_t customer[CUSTOMER_C];
    int customer_index[CUSTOMER_C];
    pthread_t loader;
    
    int i;
    // Задаём товар в ларьках от 900 до 1000 и инициализируем mutex
    for(i = 0; i < LAREK_C; i++)
    {
     shop[i] = (rand() % (1101 - 900)) + 900;
        pthread_mutex_init(&mutex[i], NULL);
    }

    // Задаём начальную потребность покупателям от 9900 до 10100
    for(i = 0; i < CUSTOMER_C; i++)
    {
        customer_need[i] = (rand() % (10101 - 9900)) + 9900;
    }

    // Запускаем потоки покупателей и погрузчика
    for(i = 0; i < CUSTOMER_C; i++)
    {
        customer_index[i] = i;
        pthread_create(&customer[i], NULL, customer_buy, &customer_index[i]);
    }
    pthread_create(&loader, NULL, loader_work, NULL);

    // Ждём завершения потоков
    for(i = 0; i < CUSTOMER_C; i++)
    {
        pthread_join(customer[i], NULL);
    }
    pthread_join(loader, NULL);

    return 0;
}