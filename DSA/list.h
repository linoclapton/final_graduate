#pragma once

template <typename T>
class ListNode;
typedef int Rank;

#define Posi(T) ListNode<T>*

template <typename T>
class ListNode{
public:
    T data;
    Posi(T) pred;
    Posi(T) succ;
    ListNode(){};
    ListNode(T e,Posi(T) p = NULL,Posi(T) s=NULL):data(e),pred(p),succ(s){}
    Posi(T) insertAsPred(T const & e);
    Posi(T) insertAsSucc(T const & e);
};

template <typename T>
class List{

private: 
    int _size;
    Posi(T) header;
    Posi(T) trailer;
protected:
    /*内部函数*/
    void init();
    int clear();
    Posi(T) selectMax(Posi(T) p,int n);
public:
    /*构造函数，析构函数，只读接口，可写接口，遍历接口*/
    //pred succ data insertAsPred(e) insertAsSucc(e)
    List(){init();}
    Posi(T) first() const{return header->succ;}
    Posi(T) last() const{return trailer->pred;}
    T       operator[](Rank r) const;
    Posi(T) find(T const &e,int n,Posi(T) p)const;
    Posi(T) insertBefore(Posi(T) p,T const& e);
    Posi(T) insertAsLast(T const & data);
    Posi(T) insertAfter(Posi(T) p,T const& e);
    void    copyNodes(Posi(T) p,int n);
    T       remove(Posi(T) p); 
    int     deduplicate();
    int     uniquify();
    Posi(T) search(T const& e,int n,Posi(T) p);
    void    selectionSort(Posi(T) p,int n);
    void    insertionSort(Posi(T) p,int n);
    bool    empty(){return _size == 0;}
    int     size(){return _size;}
    ~List();
};

template <typename T>
void List<T>::init(){
    header  = new ListNode<T>;
    trailer = new ListNode<T>;
    header->succ = trailer; header->pred = NULL;
    trailer->pred = header; trailer->succ = NULL;
    _size=0;
}

template <typename T>
T List<T>::operator[](Rank r)const{
    Posi(T) p = first();
    while(0<r--) p = p->succ;
    return p->data;
}
template <typename T>
Posi(T) List<T>::find(T const &e,int n,Posi(T) p) const{
    while(0<n--)
        if(e == (p=p->pred)->data) return p;
    return NULL;
}
template <typename T>
Posi(T) List<T>::insertBefore(Posi(T) p,T const& e){
    _size++;return p->insertAsPred(e);
}

template <typename T>
Posi(T) ListNode<T>::insertAsPred(T const & e){
    Posi(T) tmp = new ListNode<T>(e,pred,this);
    pred->succ = tmp; pred = tmp; return tmp;
}

template <typename T>
void List<T>::copyNodes(Posi(T) p,int n){
    init();
    while(0<n--)
    {insertAsLast(p->data);p=p->succ;}
}

template <typename T>
Posi(T) List<T>::insertAsLast(T const& data){
    return insertBefore(trailer,data);
}
template <typename T>
Posi(T) List<T>::insertAfter(Posi(T) p,T const& data){
    _size++;return p->insertAsSucc(data);
}
template <typename T>
Posi(T) ListNode<T>::insertAsSucc(T const & e){
    Posi(T) tmp = new ListNode<T>(e,this,succ);
    succ->pred = tmp; succ = tmp; 
    return tmp;
}
template <typename T>
T   List<T>::remove(Posi(T) p){
    T e = p->data;
    p->succ->pred = p->pred;
    p->pred->succ = p->succ;
    delete p;
    _size--;
    return e;
}
template <typename T>
int List<T>::clear(){
    int oldSize = _size;
    while(_size>0)
        remove(header->succ);
    return _size;
}
template <typename T>
List<T>::~List(){
    clear();
    delete header;
    delete trailer;
}
template <typename T>
int List<T>::deduplicate(){
    if(_size<2) return 0;
    int oldSize = _size;
    Rank r = 1;
    Posi(T) p = first();
    while(trailer != (p=p->succ)){
        Posi(T) q = find(p->data,r,p);
        q?remove(q):r++;
    }
    return oldSize - _size;
}
template <typename T>
int List<T>::uniquify(){
    if(_size<2) return 0;
    int oldSize = _size;
    Posi(T) p = first();
    Posi(T) q ;
    while(trailer!=(q = p->succ)){
        p->data == q->data?remove(q):p = q;
    }
    return oldSize - _size;
}
template <typename T>
Posi(T) List<T>::search(T const& e,int n,Posi(T) p){
    while(0<=n--){
        if((p=p->pred)->data<=e) break;
    }
    return p;
}
template <typename T>
void List<T>::selectionSort(Posi(T) p,int n){
    Posi(T) head = p->pred;Posi(T) tail = p;
    for(int i=0;i<n;i++) tail = tail->succ;
    while(1<n){
        insertBefore(tail,remove(selectMax(head->succ,n)));
        n--;tail=tail->pred;
    }
}
template <typename T>
void List<T>::insertionSort(Posi(T) p,int n){
    for(Rank r=0;r<n;r++){
        insertAfter(search(p->data,r,p),p->data);
        p = p->succ;
        remove(p->pred);
    }
}
template <typename T>
Posi(T) List<T>::selectMax(Posi(T) p,int n){
    Posi(T) max = p;
    while(0<n--){
        if(max->data<p->data)
            max = p;
        p = p->succ;
    }
    return max;
}