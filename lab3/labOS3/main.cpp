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
    MarkerArgs arg = *((MarkerArgs*) argument);
    srand( arg.id);
    bool slp = false;
    cout<<"Hello, thread #"<<arg.id<<endl;
    while(true)
    {
        cout<<"Sleeping "<<slp<<endl;
        MSG msg;
        BOOL msgReturn = GetMessage (&msg, NULL, RUN_AGAIN, KILL_ME_PLS);
        cout<<"Some message?  ";
        msgReturn?(cout<<"[YES]"):(cout<<"[NO]");
        cout<<endl;
        if (msgReturn)
        {
            cout<<"BANG!\n";
            switch (msg.message)
            {
            case RUN_AGAIN:
                slp = false;
                cout<<"RESUME thread #"<<arg.id;
                break;
            case KILL_ME_PLS:
                cout<<"Thread "<<arg.id<<" is dying...\n";
                for (int i=0;i<arg.arrSize;i++)
                {
                    if ( arg.arrP[i]==arg.id)
                    {
                        arg.arrP[i]=0;
                    }
                }
                arg.hEvent = CreateEvent (NULL, false, false, "KILLED");
                SetEvent (arg.hEvent);
                ExitThread(0);
                break;

            }

        }
        if (!slp)
        {
            cout<<"Thread #"<<arg.id<<" not sleeping\n";
            int tmp = rand()%arg.arrSize;
            if (arg.arrP[tmp]==0)
            {
                sleep(5);
                arg.arrP[tmp]= arg.id;
                cout<<"Thread #"<<arg.id<<" just generated some number\n";
                sleep(5);
            }
            else
            {
                printf ("Thread .id: %d STOPED at  %d\n", arg.id, tmp);
                arg.hEvent = CreateEvent (NULL, false, false, "STOPED");
                SetEvent (arg.hEvent);
                slp = true;
            }
        }
    }
}

void removeThread (HANDLE* hThreads, DWORD* threadIDs, MarkerArgs* args,HANDLE* hEvents,int pos,int &sz)
{
    if (pos<sz && pos >= 0)
    {
        CloseHandle ( hEvents[pos] );
        CloseHandle ( hThreads[pos]);
        for (int i=pos;i<sz-1;i++)
        {
            hThreads[i]=hThreads[i+1];
            threadIDs[i]=threadIDs[i+1];
            args[i]=args[i+1];
            hEvents[i]=hEvents[i+1];
        }
        sz--;
    }
    else
    {
        cout<<"Wrong thread index on removeThread\n";
    }
}

void clearEvents (HANDLE* hEvents, int sz)
{
    for (int i=0;i<sz;i++)    SetEvent (hEvents[i]);
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
    int* arr = new int[arrSz];
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
        ResetEvent(hEvents[i]);
        hThreads[i] = CreateThread (NULL, 0, marker,(LPVOID)&args[i],0, &threadIDs[i]);
        cout<<"Starting thread #"<<i<<"...         ";
        if (!hThreads[i])
        {
            cout<<"[ERR]\n\t on "<<i<<"\'s \'marker\'  thread creation\n";
        }
        else
        {
            workingThreads++;
            cout<<"[OK]\n";
        }
    }

    while(threadCount!=0)
    {
        for (int i=0;i<threadCount;i++) ResetEvent (hEvents[i]);
        cout<<"================================\n\n";
        WaitForMultipleObjects (threadCount,hEvents,true,INFINITE);
        printArr(arr,arrSz);

        cout<<"Kill thread no?[0.."<<threadCount-1<<"]\n>";
        int x = 0;
        cin>>x;
        cout<<"Killing thread #"<<x<<"..."<<endl;
        for (int i=0;i<threadCount;i++) ResetEvent (hEvents[i]);
        cout<<"Clear hEvents...\n";

        PostThreadMessage (threadIDs[x], KILL_ME_PLS, 0,0);
        cout<<"Posted thread message. Waiting...\n";
        WaitForSingleObject (hEvents[x],INFINITE);
        printArr(arr,arrSz);
        cout<<"Removing thread from pool...\n";
        removeThread(hThreads,threadIDs,args,hEvents,x,threadCount);
        cout<<"Thread count:"<<threadCount<<endl;
        cout<<"Resuming threads...\n";
        for (int i=0;i<threadCount;i++)
        {
            PostThreadMessage (threadIDs[i], RUN_AGAIN, 0,0);
        }
        cout<<"\n\n";

    }


    delete [] arr;
     delete [] hThreads;
     delete [] threadIDs;
     delete [] args;
    return 0;
}
