#pragma once 
#include <iostream>
typedef int Rank;
#define DEFAULT_CAPACITY 3
//overflow上溢 underflow下溢 load factor装载因子
//无序向量 判等 == != 比对
//有序向量 比较 < >
template <typename T>
class Vector{
private:
    Rank _size;
    int _capacity;
    T* _elem;
protected:
    /*内部函数*/
    void expand();
    Rank binarySearch(T const &e,Rank lo,Rank hi);
    Rank bubble(Rank lo,Rank hi);
    void bubbleSort(Rank lo,Rank hi);
    void mergeSort(Rank lo,Rank hi);
    void merge(Rank lo,Rank mid,Rank hi);
public:
    void copyFrom(const T * A,Rank lo,Rank hi);
    /*构造函数*/
    Vector(int c=DEFAULT_CAPACITY){
        _elem = new T[_capacity=c];_size = 0;
    }
    Vector(const T * A,Rank lo,Rank hi){
        copyFrom(A,lo,hi);
    }
    Vector(const Vector<T> &v,Rank lo,Rank hi){
        copyFrom(v._elem,lo,hi);
    }
    Vector(const Vector<T> &v){
        copyFrom(v._elem,0,v._size);
    }
    /*析构函数*/
    ~Vector(){
        delete[] _elem;
    }
    /*只读接口*/
    T&   operator[](Rank r);
    int  disordered() const;
    int  size()const{return _size;}
    bool empty()const{return _size==0;}
    //[lo,hi) input-sensible 
    Rank find(const T &e,Rank lo,Rank hi);
    Rank search(const T &e,Rank lo,Rank hi);
    /*可写接口*/
    Rank insert(Rank r,const T & e);
    Rank insert(const T &e);
    int  remove(Rank lo,Rank hi);
    T    remove(Rank r);
    int  deduplicate();
    int  uniquify();
    /*遍历接口*/
    template<typename VST>
    void traverse(VST& visit);
    /*排序*/
    void sort(Rank lo,Rank hi);
};

template <typename T>
T& Vector<T>::operator[](Rank r){
    //保证r在范围内
    return _elem[r];
}


template <typename T>
void Vector<T>::copyFrom(const T * A,Rank lo,Rank hi){
    _elem = new T[_capacity=2*(hi-lo)];
    _size = 0;
    while(lo<hi){
        _elem[_size++]=A[lo++];
    }
}
template <typename T>
void Vector<T>::expand(){
    if(_size<_capacity)
        return;
    _capacity = std::max(_capacity,DEFAULT_CAPACITY);
    T* oldelem = _elem;_elem = new T[_capacity<<1];
    for(int i=0;i<_size;i++)
        _elem[i] = oldelem[i];
    delete[] oldelem;
}
//循秩访问
template <typename T>
Rank Vector<T>::insert(Rank r,const T & e){
    expand();
    for(int i=_size;i>r;i--)
        _elem[i]=_elem[i-1];
    _elem[r] = e;
    ++_size;
    return r;
}
template <typename T>
Rank Vector<T>::insert(const T & e){
    insert(_size,e);
}
template <typename T>
int Vector<T>::remove(Rank lo,Rank hi){
    if(lo==hi) return 0;
    while(hi<_size) _elem[lo++] = _elem[hi++];
    _size = lo;
    //shrink();
    return hi-lo;
}

template <typename T>
T Vector<T>::remove(Rank r){
    T e = _elem[r];
    remove(r,r+1);
    return e;
}
template <typename T>
Rank Vector<T>::find(T const &e,Rank lo,Rank hi){
    while(lo<hi--&&_elem[hi]!=e);
    return hi;
}

template <typename T>
int Vector<T>::deduplicate(){
    int oldSize = _size;
    Rank i = 1;
    while(i<_size){
        (find(_elem[i],0,i)<0)?i++:remove(i);
    }
    return oldSize - _size;
}
template<typename T> template<typename VST>
void Vector<T>::traverse(VST& visit){
    for(int i=0;i<_size;i++) 
        visit(_elem[i]);
}

template<typename T>
int Vector<T>::disordered() const{
    int num = 0;
    for(int i=1;i<_size;i++)
        num +=_elem[i]<_elem[i-1]; 
    return num;
}
template<typename T>
int Vector<T>::uniquify(){
    int i=0,j=0;
    while(++j<_size){
        if(_elem[i]!=_elem[j]){
            _elem[++i] = _elem[j];
        }
    }
    _size = i+1;
    //shrink();
    return  j - i;
}
template<typename T>
Rank Vector<T>::binarySearch(const T &e,Rank lo,Rank hi){
    //A
    /*
    while(lo<hi){
        Rank mid = (lo+hi)>>1;
        if(e<_elem[mid])        hi = mid;
        else if(_elem[mid]<e)   lo = mid+1;
        else                    return mid;
    }
    return -1;
    */
    //B
    /*
    while(1<lo-hi){
        Rank mid = (lo+hi)>>1:
        (e<_elem[mid])?hi=mid:lo=mid;
    }
    return e==_elem
    */
    //C
    while(lo<hi){
        Rank mid = (lo+hi)>>1; 
        (e<_elem[mid])?hi=mid:lo=mid+1;
    }
    return --lo;
}

template<typename T>
Rank Vector<T>::search(const T &e,Rank lo,Rank hi){
    return binarySearch(e,lo,hi);
}

template<typename T>
Rank Vector<T>::bubble(Rank lo,Rank hi){
    Rank last = lo;
    while(++lo<hi){
        if(_elem[lo]<_elem[lo-1]){
            last = lo;
            swap(_elem[lo],_elem[lo-1]);
        }
    }
    return last;
}
template<typename T>
void Vector<T>::bubbleSort(Rank lo,Rank hi){
    while(lo<(hi=bubble(lo,hi)));
}
template<typename T>
void Vector<T>::mergeSort(Rank lo,Rank hi){
    if(hi-lo<2) return;
    Rank mid = (hi+lo)>>1;
    mergeSort(lo,mid);
    mergeSort(mid,hi);
    merge(lo,mid,hi);
}
template<typename T>
void Vector<T>::merge(Rank lo,Rank mid, Rank hi){
    T* A = _elem + lo;
    int lb = mid - lo; T* B = new T[lb];
    for(int i=0;i<lb;B[i]=A[i++]);
    int lc = hi - mid;
    T* C = _elem + mid;
    for(int i=0,j=0,k=0;j<lb;){
        if(k<lc&&C[k]<B[j])  A[i++] = C[j++];
        if(lc<=k||B[j]<=C[k]) A[i++] = B[j++]; 
    }
    //其实lb<j时已经不必在重排后面的元素了
    /*
    for(int i=0,j=0,k=0;(j<lb||k<lc);){
        if(j<lb&&(lc<=k||B[j]<=C[k])) A[i++] = B[j++]; 
        if(k<lc&&(lb<=j||C[k]<B[j]))  A[i++] = C[j++];
    }
    */
    delete[] B;
}
template<typename T>
void Vector<T>::sort(Rank lo,Rank hi){
    //bubbleSort(lo,hi);
    mergeSort(lo,hi);
}
