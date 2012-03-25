#include <iostream>
#include <windows.h>
#include <cstdio>
#include <time.h>
using namespace std;


struct MarkerArgs
{
    int id;
    int arrSize;
    int* arrP;
    CRITICAL_SECTION* cs;
    HANDLE startEvent;
    HANDLE stoppedEvent;
    HANDLE resumeEvent;
    HANDLE killEvent;

};

DWORD WINAPI marker (LPVOID argument)
{
    //Начать работу по сигналу от потока main.
    MarkerArgs args = *((MarkerArgs*)argument);
  //  EnterCriticalSection (args.cs);
    printf ("Thread #%d waiting for start signal\n",args.id);
  //  LeaveCriticalSection (args.cs);
    WaitForSingleObject (args.startEvent,INFINITE);
    //Инициализировать генерацию последовательности случайных чисел. Для этого
    //использовать функцию srand, которой передать в качестве аргумента свой
    //порядковый номер.
    srand (args.id);
    //Работать циклически, выполняя на каждом цикле следующие действия:
    while (true)
    {
        //Генерировать случайное число, используя функцию rand.
        //Разделить это число по модулю на размерность массива.
        int randomNum = rand()%args.arrSize;


        //Если элемент массива, индекс которого равен результату деления, равен нулю, то
        //выполнить следующие действия:

        if (args.arrP[randomNum]==0)
        {

            //Поспать 5 миллисекунд.
            Sleep (5);

            //Занести в элемент, индекс которого вычислен, свой порядковый номер.
            args.arrP[randomNum]=args.id;

            //Поспать 5 миллисекунд.
            Sleep(5);

            //Продолжить исполнение цикла 3.
        }
        //В противном случае:
        else
        {
            //Вывести на консоль свой порядковый номер.
            //Вывести на консоль индекс элемента массива, который невозможно
            //пометить.

            //printf ("Thread id: %d stoped at %d\n",&rand,&rand);
            cout<<"Thread id:"<<args.id<<" stoped at "<<rand<<endl;
            //Дать сигнал потоку main на невозможность продолжения своей работы.

            SetEvent (args.stoppedEvent);
            //Ждать ответный сигнал на продолжение или завершение работы от потока
            //main.
            HANDLE* events = new HANDLE [2];
            events[0] = args.resumeEvent;
            events[1] = args.killEvent;
            //Если получен сигнал на завершение работы, то выполнить следующие действия:
            if (WaitForMultipleObjects (2,events,FALSE, INFINITE)-WAIT_OBJECT_0 == 1)
            {
                //Заполнить нулями в массиве все элементы, которые он пометил.
                //Завершить работу.
                printf ("%d received KILL_SIGNAL...\n",args.id);
                for (int i=0;i<args.arrSize;i++)
                {
                    if (args.arrP[i]==args.id)
                    {
                        args.arrP[i]=0;
                    }
                }
                SetEvent(args.stoppedEvent);
                ExitThread(0);
            }
            //Если получен сигнал на продолжение работы, то продолжить исполнение цикла
            printf ("Thread %d resumed...\n",args.id);
            ResetEvent (args.stoppedEvent);
        }
    }


}

void removeThread (HANDLE* hThreads, DWORD* threadIDs, MarkerArgs* args,HANDLE* hEvents,int pos,int &sz)
{

}

void clearEvents (HANDLE* hEvents, int sz)
{

}

void printArr (int* arr, int sz)
{
    for (int i=0;i<sz;i++)
    {
        cout<<arr[i]<<"  ";
    }
    cout<<endl;
}

int main ()
{
    //Захватить память под массив целых чисел, размерность которого вводится с консоли.
    int arrSz = 0;
    printf ("Set array size: ");
    scanf("%d",&arrSz);
    int* array = new int [arrSz];

    //Инициализировать элементы массива нулями.
    for (int i=0;i<arrSz;i++)
    {
        array[i]=0;
    }

    //Запросить количество потоков marker, которые требуется запустить.
    int markerCount = 0;
    printf("Set amount of instances of \'maker\': ");
    scanf("%d",&markerCount);

    //Запустить заданное количество экземпляров потока marker. В качестве параметра
    //каждому экземпляру потока marker передать его порядковый номер в запуске.

    HANDLE* hThreads = new HANDLE [markerCount];
    DWORD* threadIDs = new DWORD [markerCount];

    HANDLE* resumeEvents = new HANDLE [markerCount];
    HANDLE* startEvents = new HANDLE [markerCount];
    HANDLE* stopedEvents = new HANDLE [markerCount];
    HANDLE* killEvnets = new HANDLE [markerCount];
    bool* killMask = new bool [markerCount];
    for (int i=0;i<markerCount;i++) killMask[i] = false;

    CRITICAL_SECTION* cs;

    MarkerArgs* args = new MarkerArgs [markerCount];
    //InitializeCriticalSection(cs);

    for (int i=0;i<markerCount;i++)
    {
        //Инициализация аргументов
        args[i].id = i+1;
        args[i].arrP = array;
        args[i].arrSize = arrSz;
        args[i].cs =cs;
       // stopedEvents[i] = CreateEvent (NULL,FALSE,FALSE,NULL);
       // startEvents[i] = CreateEvent (NULL,FALSE,FALSE,NULL);
        args[i].stoppedEvent =  CreateEvent (NULL,FALSE,FALSE,NULL);
        stopedEvents[i] = args[i].stoppedEvent;
        args[i].startEvent =  CreateEvent (NULL,FALSE,FALSE,NULL);
        startEvents[i] = args[i].startEvent;
        args[i].resumeEvent = CreateEvent (NULL,FALSE,FALSE,NULL);
        resumeEvents[i] = args[i].resumeEvent;
        args[i].killEvent = CreateEvent (NULL,FALSE,FALSE,NULL);
        killEvnets [i] = args[i].killEvent;

        printf ("Creating thread %d    ",i+1);
        hThreads[i] = CreateThread(NULL, 0, marker, &args[i], 0, &threadIDs[i]);
        if (hThreads[i]==NULL)
        {
            printf ("\n[ERR] Error on creation thread #%d",i+1);
        }
        else
        {
            printf ("[OK]\n");
        }
    }
    //Дать сигнал на начало работы всех потоков marker.

    for (int i=0;i<markerCount;i++)
    {
        printf ("Starting thread %d...\n",i+1);
        SetEvent (args[i].startEvent);
    }
    int threadCount = markerCount;
    while (threadCount!=0)
    {

        // Ждать, пока все потоки marker не подадут сигналы о невозможности
        //продолжения своей работы.
        printf ("Waiting for threads ...\n");
        for (int i=0;i<markerCount;i++)
        {
            if (killMask[i])
            {
                SetEvent(stopedEvents[i]);
            }

        }
        WaitForMultipleObjects (markerCount,stopedEvents,TRUE, INFINITE);
        for (int i =0;i<markerCount;i++)
        {
            if (!killMask[i])
            {
                ResetEvent(args[i].stoppedEvent);
            }
            stopedEvents[i] = args[i].stoppedEvent;
        }
        //Вывести содержимое массива на консоль
        for (int i=0;i<arrSz;i++)
        {
            printf ("%3d ",array[i]);
        }
        printf ("\n");
        //Запросить с консоли порядковый номер потока marker, которому будет подан
        //сигнал на завершение своей работы.
        printf ("Witch of  threads do you want to kill [");
        for (int i=0;i<markerCount;i++)
        {
            if (!killMask[i]) printf (" %d ",i+1);
        }
        printf ("]: ");
        int kill = 0;
        scanf ("%d",&kill);
        //Подать потоку marker, номер которого получен в пункте 6.3, сигнал на
        //завершение работы.
        printf ("Sending  killEvent to %d thread...\n",kill);
        SetEvent(args[kill-1].killEvent);
        //Ждать завершение работы потока marker, которому был подан сигнал на
        //завершение работы

        WaitForSingleObject (args[kill-1].stoppedEvent,INFINITE);
        //Вывести содержимое массива на консоль.
        for (int i=0;i<arrSz;i++)
        {
            printf ("%3d ",array[i]);
        }
        printf ("\n");
        //Подать сигнал на продолжение работы, оставшимся потокам marker.
        for (int i=0;i<markerCount;i++)
        {
            if (i!=kill-1)
            {
                SetEvent(args[i].resumeEvent);
            }
            else
            {
                CloseHandle (hThreads[i]);
                threadCount--;
                killMask[kill-1]=true;
                SetEvent (args[kill-1].stoppedEvent);
            }
        }
    }



    //Высвобождение памяти
    delete[] array;
    delete[]hThreads;
    delete[] args;

    delete [] startEvents;
    delete [] stopedEvents;
    delete [] resumeEvents;
    delete [] killEvnets;

    return 0;
}
