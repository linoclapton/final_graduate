#pragma once

#include "list.h"

template <typename T>
class Queue : public List<T>{
//empty,enqueue,dequeue,size,front,rear
public:
T& front(){return first()->data;}
T& rear(){return trailer->pred->data;}
void enqueue(const T &e){insertAsLast(e);}
T   dequeue(){return remove(first());}
};