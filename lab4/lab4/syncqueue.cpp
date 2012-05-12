#include "syncqueue.h"

SyncQueue::SyncQueue(int size)
{
    queue_ = new int [size];     
    count_ = 0;
    size_ = size;
    head_ = 0;
    tail_ = 0;

	hSemaphoreFill = CreateSemaphore (NULL,size,size,NULL);
	hSemaphoreEmpty = CreateSemaphore(NULL,0,size,NULL);	 
    InitializeCriticalSection (&cs_);

}

int SyncQueue::remove()
{	 
		 
	WaitForSingleObject(hSemaphoreEmpty,INFINITE);
	EnterCriticalSection(&cs_);	
	int res = queue_[head_];
    head_ = (size_+head_+1)%size_;
    count_--;     	 
	ReleaseSemaphore(hSemaphoreFill,1,NULL);	
	LeaveCriticalSection (&cs_);	
    return res;
   

}

void SyncQueue::insert(int x)
{
	WaitForSingleObject(hSemaphoreFill,INFINITE);
	EnterCriticalSection (&cs_);
	queue_[tail_] = x;
    tail_ = (size_+tail_+1)%size_;
    count_++;     	 
	ReleaseSemaphore(hSemaphoreEmpty,1,NULL);
    LeaveCriticalSection (&cs_);	
}

SyncQueue::~SyncQueue()
{
    delete [] queue_;
	CloseHandle (hSemaphoreEmpty);
	CloseHandle(hSemaphoreFill);	 
    DeleteCriticalSection (&cs_);
}


