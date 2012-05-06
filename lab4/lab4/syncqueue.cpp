#include "syncqueue.h"

SyncQueue::SyncQueue(int size)
{
    queue_ = new int [size];     
    count_ = 0;
    size_ = size;
    head_ = 0;
    tail_ = 0;

	hSemaphore = CreateSemaphore(NULL,0,size,NULL);
    if (!hSemaphore)
    {
        printf ("Error on semaphore creation\n");
    }
    InitializeCriticalSection (&cs_);

}

int SyncQueue::remove()
{
	WaitForSingleObject(hSemaphore,INFINITE);
	int res = queue_[head_];
    head_ = (size_+head_+1)%size_;
    count_--;     
	LeaveCriticalSection (&cs_);
    return res;
   

}

void SyncQueue::insert(int x)
{
	while(!ReleaseSemaphore(hSemaphore,1,NULL));	
	EnterCriticalSection (&cs_);
	queue_[tail_] = x;
    tail_ = (size_+tail_+1)%size_;
    count_++;     
    LeaveCriticalSection (&cs_);
}

SyncQueue::~SyncQueue()
{
    delete [] queue_;
	CloseHandle (hSemaphore);
    DeleteCriticalSection (&cs_);
}


