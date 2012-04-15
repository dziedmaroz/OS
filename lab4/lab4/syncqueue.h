#ifndef SYNCQUEUE_H
#define SYNCQUEUE_H

#include <windows.h>
#include <stdio.h>

struct QueueIsFullException{};
struct QueueIsEmptyExcetption{};
class SyncQueue
{
    int* queue_;
    int size_;
    int count_;  
    int head_;
    int tail_;
    CRITICAL_SECTION cs_;
    HANDLE notFull;
    HANDLE notEmpty;
public:
    SyncQueue(int size_);
    ~SyncQueue();
    void insert(int x); // добавить элемент в очередь
    int remove (); // удалить элемент из очереди
    int capacity() {return size_;}
    int size() {return count_;}
    bool isFull () {return count_==size_;}
    bool isEmpty () {return !count_;}
    void print ()
    {
        for (int i =0;i<count_;i++)
        {
            printf ("%4d",queue_[(size_+head_+i)%size_]);
        }
        printf ("\n");
    }
};

#endif // SYNCQUEUE_H
