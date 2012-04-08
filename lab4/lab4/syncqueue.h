#ifndef SYNCQUEUE_H
#define SYNCQUEUE_H

#include <windows.h>

class SyncQueue
{
    int* queue_;
    int size_;
    int count_;
    HANDLE hHasMoreElements;
    HANDLE hNotFull;
    int head_;
    int tail_;
public:
    SyncQueue(int size_);
    ~SyncQueue();
    void insert(int x); // добавить элемент в очередь
    int remove (); // удалить элемент из очереди
};

#endif // SYNCQUEUE_H
