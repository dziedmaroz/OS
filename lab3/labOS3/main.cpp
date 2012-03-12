#include <iostream>
#include <windows.h>
#include <cstdio>
using namespace std;

#define KILL_ME_PLS WM_USER + 1
#define RUN_AGAIN WM_USER + 2

struct MarkerArgs
{
    int id;
    int arrSize;
    int* arrP;
    HANDLE hEvent;
};

DWORD WINAPI marker (LPVOID argument)
{
    MarkerArgs arg = (MarkerArgs*) argument;
    srand((MarkerArgs*) arg->ID);
    bool firstRun = true;
    while(true)
    {
        MSG msg=NULL;
        BOOL msgReturn = GetMessage (&msg, NULL, RUN_AGAIN, KILL_ME_PLS);
        if (msgReturn || firstRun)
        {
            if (msg!=NULL)
            {
                switch (msg.message)
                {
                    case RUN_AGAIN:
                        int tmp = rand()%(MarkerArgs*) arg->arrSize;
                        if ((MarkerArgs*)arg->arrP[tmp]==0)
                        {
                            sleep(5);
                            (MarkerArgs*)arg->arrP[tmp]=(MarkerArgs*) arg->ID;
                            sleep(5);
                        }
                        else
                        {
                            printf ("Thread ID: %d  INDEX: %d\n", (MarkerArgs*)arg->ID, tmp);
                            (MarkerArgs*)arg->hEvent = CreateEvent (NULL, false, false, "STOPED");
                        }
                        break;
                   case KILL_ME_PLS:
                        for (int i=0;i<(MarkerArgs*) arg->arrSize;i++)
                        {
                            if ((MarkerArgs*) arg->arrP[i]==(MarkerArgs*)arg->id)
                            {
                              (MarkerArgs*) arg->arrP[i]=0;
                            }
                        }
                        (MarkerArgs*)arg->hEvent = CreateEvent (NULL, false, false, "KILLED");
                        ExitThread(0);
                        break;
                }
            }
            else
            {
                int tmp = rand()%(MarkerArgs*) arg->arrSize;
                if ((MarkerArgs*)arg->arrP[tmp]==0)
                {
                    sleep(5);
                    (MarkerArgs*)arg->arrP[tmp]=(MarkerArgs*) arg->ID;
                    sleep(5);
                }
                else
                {
                    printf ("Thread ID: %d  INDEX: %d\n", (MarkerArgs*)arg->ID, tmp);
                    (MarkerArgs*)arg->hEvent = CreateEvent (NULL, false, false, "STOPED");
                }
            }
        }
    }
}

void removeThread (HANDLE* hThreads, DWORD* threadIDs, MarkerArgs* args,HANDLE* hEvents,int pos,int &sz)
{
    for (int i=pos;i<sz-1;i++)
    {
        hThreads[i]=hThreads[i+1];
        threadIDs[i]=threadIDs[i+1];
        args[i]=args[i+1];
        hEvents[i]=hEvents[i+1];
    }
    sz--;
    delete hThreads[sz];
    delete threadIDs[sz];
    delete args[sz];
    delete hEvents[sz];
}

void clearEvents (HANDLE* hEvents, int sz)
{
    for (int i=0;i<sz;i++) hEvents[i]=NULL;
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
    int arrSz=0;
    cout<<"Array size: ";
    cin>>arrSz;
    int* arr = new int[n];
    int threadCount = 0;
    cout<<"# instances of \'marker\': ";
    cin>>threadCount;
    for (int i=0;i<arrSz;i++) arr[i]=0;

    HANDLE* hThreads = new HANDLE[threadCount];
    DWORD* threadIDs = new DWORD [threadCount];
    MarkerArgs* args = new MarkerArgs [threadCount];
    HANDLE* hEvents = new HANDLE [threadCount];
    clearEvents(hEvents,threadCount);
    int workingThreads = 0;

    for (int i=0;i<threadCount;i++)
    {
        args[i].arrP = arr;
        args[i].arrSize = arrSz;
        args[i].id =  i;
        args[i].hEvent = hEvents[i];
        hThreads[i] = CreateThread (NULL, 0, marker,(LPVOID)&args[i], &threadIDs[i]);
        if (!hThreads[i])
        {
            cout<<"Error on "<<i<<"\'s \'marker\'  thread creation\n";
        }
        else workingThreads++;
    }

    while(threadCount!=0)
    {
        cout<<"================================\n\n";
        WaitForMultipleObjects (threadCount,hEvents,true,INFINITE);
        printArr(arr,arrSz);

        cout<<"Kill thread no?\n>";
        int x = 0;
        cin>>x;

        PostThreadMessage (threadIDs[x], KILL_ME_PLS, 0,0);
        clearEvents(hEvents,threadCount);
        WaitForSingleObject (hEvents[x],INFINITE);
        printArr(arr,arrSz);
        removeThread(hThreads,threadIDs,args,hEvents,x,&threadCount);


        for (int i=0;i<threadCount;i++)
        {
            PostThreadMessage (threadIDs[i], RUN_AGAIN, 0,0);
        }
        cout<<"\n\n";

    }


    delete [] arr;
    //delete [] hThreads;
    //delete [] threadIDs;
    //delete [] args;
    return 0;
}
