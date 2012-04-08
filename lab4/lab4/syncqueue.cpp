#include "syncqueue.h"

SyncQueue::SyncQueue(int size)
{
    queue_ = new int [size];
    hHasMoreElements =  CreateEvent (NULL,FALSE,FALSE,NULL);
    count_ = 0;
    size_ = size;
    head_ = 0;
    tail_ = 0;
    hNotFull = CreateEvent (NULL,FALSE,TRUE,NULL);
}

int SyncQueue::remove()
{
    WaitForSingleObject (hHasMoreElements,INFINITE);
    int res = queue_[head_];
    head_ = (size_+head_+1)%size_;
    count_--;
    if (count_!=0)
    {
        SetEvent (hHasMoreElements);
    }
    else
    {
        ResetEvent (hHasMoreElements);
    }
    return res;
}

void SyncQueue::insert(int x)
{    
    WaitForSingleObject (hNotFull, INFINITE);
    queue_[tail_] = x;
    tail_ = (size_+tail_+1)%size_;
    count_++;
    if (count!=size_)
    {
        SetEvent (hNotFull);
    }
    else
    {
        ResetEvent (hNotFull);
    }
}


