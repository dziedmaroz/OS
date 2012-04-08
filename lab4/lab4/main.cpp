#include <stdio.h>
#include "syncqueue.h"


struct Arguments
{
    HANDLE startEvent;
    SyncQueue* queueP;
    int count;
    int ID;
};

DWORD WINAPI producer (LPVOID arg)
{
    Arguments args = *((Arguments*) arg);
    // ждать сигнал на начало работы;
    SyncQueue queue = *args.queueP;
    WaitForSingleObject (args.startEvent,INFINITE);
    //циклически выполнять следующие действия (количество циклов задается в параметре):
    for (int i=0;i<args.count;i++)
    {
         //добавить в кольцевую очередь целое число, равное своему порядковому
         //номеру;
         queue.insert(args.ID);

         //вывести на консоль сообщение: "Произведено число: N ", где N - номер числа,
         //помещенного в очередь.
         printf ("%d is produced\n", args.ID);

         //поспать 7 мс.
         Sleep (7);
    }
    SetEvent (args.startEvent);
}

DWORD WINAPI consumer (LPVOID arg)
{
    Arguments args = *((Arguments*) arg);
    SyncQueue queue = *args.queueP;
    // ждать сигнал на начало работы;
    WaitForSingleObject (args.startEvent,INFINITE);

    //циклически извлекать из кольцевой очереди целые числа с интервалом в 7 мс
    //(количество циклов задается в параметре);
    for (int i=0;i<args.count;i++)
    {
        printf ("\t%d is consumed", queue.remove());
        //при извлечении числа из кольцевой очереди, выводить на консоль сообщение:
        //"\tУпотреблено число N ", где N - номер числа, извлеченного из очереди
        Sleep (7);
    }
    SetEvent (args.startEvent);
}

int main(int argc, char *argv[])
{
    //создать объект кольцевой очереди, размер очереди вводится пользователем с
    //клавиатуры;
    int queueSz = 0;
    printf ("Queue size: ");
    scanf("%d",&queueSz);
    SyncQueue queue = SyncQueue (queueSz);

    //ввести с клавиатуры количество потоков producer и количество потоков consumer,
    //которые он должен запустить;
    int prodCount = 0;
    int consCount = 0;

    printf ("Producer count: ");
    scanf ("%d",&prodCount);
    printf ("Consumer count: ");
    scanf ("%d",&consCount);

    int* prodPortions  = new int [prodCount];
    int* consPortions  = new int [consCount];

    //запросить для каждого из потоков producer и cosumer количество целых чисел,
    //которые эти потоки должны соответственно произвести и потребить;
    for (int i=0;i<prodCount;i++)
    {
        printf ("Produser %d items to produce: ", i+1);
        scanf ("%d",&prodPortions[i]);
    }

    for (int i=0;i<consCount;i++)
    {
        printf ("Consumer %d items to consume: ", i+1);
        scanf ("%d",&consPortions[i]);
    }

    //создать требуемое количество потоков producer, каждому потоку передать его
    //порядковый номер и количество целых чисел, которые он должен произвести;
    HANDLE* hProducers = new HANDLE [prodCount];
    DWORD* prodID = new DWORD [prodCount];
    Arguments* prodArgs = new Arguments [prodCount];
    HANDLE* endItAll = new HANDLE [prodCount+consCount];
    for (int i=0;i<prodCount;i++)
    {
        prodArgs[i].count = prodPortions[i];
        prodArgs[i].queueP = &queue;
        prodArgs[i].startEvent = CreateEvent (NULL,TRUE,FALSE,NULL);
        endItAll[i] = prodArgs[i].startEvent;
        prodArgs[i].ID = i+1;
        if (!prodArgs[i].startEvent)
        {
            printf ("Error on creating startEvent for producer %d\n", i+1);
            return 200;
        }
        hProducers[i] = (HANDLE) CreateThread (NULL,0, producer, (void*) &prodArgs[i], 0, &prodID[i]);
        if (hProducers[i]==NULL)
        {
            printf ("Can't start producer thread %d\n",i+1);
            return 100;
        }
    }


    //создать требуемое количество потоков consumer, каждому потоку передать
    //количество целых чисел, которые он должен потребить;

    HANDLE* hConsumers = new HANDLE [consCount];
    DWORD* consID = new DWORD [consCount];
    Arguments* consArgs = new Arguments[consCount];

    for (int i=0;i<consCount;i++)
    {
        consArgs[i].count = consPortions[i];
        consArgs[i].queueP = &queue;
        consArgs[i].startEvent = CreateEvent (NULL,TRUE,FALSE,NULL);
        endItAll[prodCount+i]=consArgs[i].startEvent;
        consArgs[i].ID = i+1;
        if (!consArgs[i].startEvent)
        {
            printf ("Error on creating startEvent for consumer %d\n", i+1);
            return 200;
        }
        hConsumers[i] = (HANDLE) CreateThread (NULL,0,consumer, (void*) &consArgs[i],0 ,&consID[i]);
        if (hProducers[i]==NULL)
        {
            printf ("Can't start consumer thread %d\n",i+1);
            return 100;
        }
    }



    //подать сигнал на начало работы потоков producer и consumer;
    for (int i=0;i<consCount+prodCount;i++)
    {
        SetEvent(endItAll[i]);
    }

    //завершить свою работу после окончания работы всех потоков producer и consumer.
    WaitForMultipleObjects (prodCount+consCount, endItAll, true, INFINITE);

    delete[] prodPortions;
    delete[] consPortions;

    delete [] hProducers;
    delete [] prodID;
    delete [] prodArgs;

    delete [] endItAll;

    delete [] hConsumers;
    delete [] consID;
    delete [] consArgs;
    return 0;
}
