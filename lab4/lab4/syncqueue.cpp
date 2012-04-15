#include "syncqueue.h"

SyncQueue::SyncQueue(int size)
{
    queue_ = new int [size];     
    count_ = 0;
    size_ = size;
    head_ = 0;
    tail_ = 0;

    notFull = CreateEvent (NULL, FALSE, TRUE, NULL);
    notEmpty = CreateEvent (NULL,FALSE,FALSE,NULL);
    if (!notFull || !notEmpty)
    {
        printf ("Error on event creation\n");
    }
    InitializeCriticalSection (&cs_);

}

int SyncQueue::remove()
{
    EnterCriticalSection (&cs_);
    if (isEmpty())
    {       
       ResetEvent (notEmpty);
	   LeaveCriticalSection (&cs_);
       WaitForSingleObject (notEmpty,INFINITE); 
	}
	EnterCriticalSection (&cs_);
	int res = queue_[head_];
    head_ = (size_+head_+1)%size_;
    count_--;
    SetEvent (notFull);
	LeaveCriticalSection (&cs_);
    return res;
   

}

void SyncQueue::insert(int x)
{
    EnterCriticalSection (&cs_);
    if (isFull())
    {
		ResetEvent (notFull);
		LeaveCriticalSection (&cs_);		
        WaitForSingleObject (notFull,INFINITE);        
    }   
	EnterCriticalSection (&cs_);
	queue_[tail_] = x;
    tail_ = (size_+tail_+1)%size_;
    count_++;
    SetEvent (notEmpty);
    LeaveCriticalSection (&cs_);
}

SyncQueue::~SyncQueue()
{
    delete [] queue_;
    CloseHandle (notFull);
    CloseHandle (notEmpty);
    DeleteCriticalSection (&cs_);
}


