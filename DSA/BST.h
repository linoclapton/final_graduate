#pragma once

#include "Tree.h"

#define Balanced(x) (stature((x).lChild) == stature((x).rChild))
#define BalFac(x) (stature((x).lChild) - stature((x).rChild))
#define AvlBalanced(x) ((-2<BalFac(x))&&BalFac(x)<2)
#define tallerChild(x) ( \
    stature( (x)->lChild ) > stature( (x)->rChild ) ? (x)->lChild : ( /*左高*/ \
    stature( (x)->lChild ) < stature( (x)->rChild ) ? (x)->rChild : ( /*右高*/ \
    IsLChild( * (x) ) ? (x)->lChild : (x)->rChild /*等高：与父亲x同侧者（zIg-zIg或zAg-zAg）优先*/ \
    ) \
    ) \
    )


template <typename K,typename V>
class Entry{
private:
    K key;V value;
public:
    Entry(K k=K(),V v=V()):key(k),value(v){}
    Entry(Entry const & e):key(e.key),value(e.value){}

    bool operator<(Entry<K,V> const &e)const{return key<e.key;}
    bool operator>(Entry<K,V> const &e)const{return key>e.key;}
    bool operator==(Entry<K,V> const &e)const{return key==e.key;}
    bool operator!=(Entry<K,V> const &e)const{return key!=e.key;}
};

template <typename T>
class BST:public BinTree<T>{
public:
    virtual BinNodePosi(T) &search(T const& k);
    virtual BinNodePosi(T) insert(T const& k);
    virtual bool remove(T const& k);
    BST(T const& k):BinTree(k){}
protected:
    BinNodePosi(T) _hot;
    BinNodePosi(T) connect34(BinNodePosi(T),BinNodePosi(T),BinNodePosi(T),BinNodePosi(T),
                             BinNodePosi(T),BinNodePosi(T),BinNodePosi(T));
    BinNodePosi(T) rotateAt(BinNodePosi(T));
    //成功时返回与k相等的节点引用，失败时指向NULL，hot指向最后一个比较节点。
    BinNodePosi(T) &searchIn(BinNodePosi(T) &v,T const& k,BinNodePosi(T) &hot){
         if(!v||(k==v->data)) return v;
         hot = v;
         return searchIn(k<v->data?v->lChild:v->rChild,k,hot);
    }
    BinNodePosi(T) removeAt(BinNodePosi(T) &v,BinNodePosi(T) &hot);
};

template <typename T>
class AVL:public BST<T>{
public:
    BinNodePosi(T) insert(const T&);
    bool remove(const T&);
};

template <typename T>
class Splay:public BST<T>{
protected:
    BinNodePosi(T) splay(BinNodePosi(T) v);//将v伸展到根
public:
    BinNodePosi(T) &search(const T& k);
    BinNodePosi(T) insert(const T&);
    bool remove(const T&);

};

template <typename T>
BinNodePosi(T) &BST<T>::search(T const& k){
    return searchIn(_root,k,_hot=NULL);
}

template <typename T>
BinNodePosi(T) BST<T>::insert(T const& k){
    BinNodePosi(T) &x = search(k);
    if(!x){
        x = new BinNode<T>(k,_hot);
        _size++;updateHeightAbove(x);
    }
    return x;
}

template <typename T>
bool BST<T>::remove(T const &k){
    BinNodePosi(T) x = search(k); if(!x) return false;
    removeAt(x,_hot); _size--;updateHeightAbove(_hot);
    return true;
}
template <typename T>
BinNodePosi(T) BST<T>::removeAt(BinNodePosi(T) &v,BinNodePosi(T) &hot){
     BinNodePosi(T) succ;
     BinNodePosi(T) w = v;
     if(v->lChild==NULL){
        succ = v = v->rChild;        
     }else if(v->rChild==NULL){
        succ = v = v->lChild;        
     }else{
        w = w->succ(); swap(v->data,w->data);
        BinNodePosi(T) u = w->parent;
        (u==v?u->rChild:u->lChild)=succ=w->rChild;
     }
     if(succ) succ->parent = hot;
     delete w;
     return succ;
}
template <typename T>
BinNodePosi(T) AVL<T>::insert(T const &k){
    BinNodePosi(T) &x = search(k); if(x) return x;
    x = new BinNode<T>(k,_hot);_size++;BinNodePosi(T) xx=x;
    for(BinNodePosi(T) g=x->parent;g;g=g->parent){
        if(!AvlBalanced(*g)){
            FromParentTo(*g) = rotateAt(tallerChild(tallerChild(g)));
            break;
        }else{
            updateHeight(g);
        }
    }
    return xx;
}
template <typename T>
bool AVL<T>::remove(T const &k){
    BinNodePosi(T) &x = search(k); if(x) return false;
    removeAt(x,_hot);_size--;
    for(BinNodePosi(T) g= _hot;g;g=g->parent){
        if(!AvlBalanced(*g)){
            g = FromParentTo(*g) = rotateAt(tallerChild(tallerChild(g)));
        }
            updateHeight(g);
    }
    return true;
}
template <typename T>
BinNodePosi(T) BST<T>::connect34(BinNodePosi(T) a,BinNodePosi(T) b,BinNodePosi(T) c,
                                 BinNodePosi(T) T0,BinNodePosi(T) T1,BinNodePosi(T) T2,BinNodePosi(T) T3){
    a->lChild = T0;if(T0) T0->parent = a;
    a->rChild = T1;if(T1) T1->parent = a; updateHeight(a);
    c->lChild = T2;if(T2) T2->parent = c; 
    c->rChild = T3;if(T3) T3->parent = c; updateHeight(c);
    b->lChild = a; a->parent = b;
    b->rChild = c; c->parent = b; updateHeight(b);
    return b;
}
template <typename T>
BinNodePosi(T) BST<T>::rotateAt(BinNodePosi(T) v){
    BinNodePosi(T) p = v->parent; BinNodePosi(T) g = p->parent;
    if(IsLChild(*p)){
        if(IsLChild(*v)){//zig-zig
            p->parent = g->parent;
            return connect34(v,p,g,v->lChild,v->rChild,p->rChild,g->rChild);
        }else{//zig-zag
            v->parent = g->parent;
            return connect34(p,v,g,p->lChild,v->lChild,v->rChild,g->rChild);
        }
    }else{
        if(IsRChild(*v)){//zag-zag
            p->parent = g->parent;
            return connect34(g,p,v,g->lChild,p->lChild,v->lChild,v->rChild);
        }else{//zag-zig
            v->parent = g->parent;
            return connect34(g,v,p,g->lChild,v->lChild,v->rChild,p->rChild);
        }

    }
}

template <typename T>
BinNodePosi(T) Splay<T>::splay(BinNodePosi(T) v){
    if(!v) return NULL;
    BinNodePosi(T) p,g;
    while((p=v->parent)&&(g=p->parent)){
        BinNodePosi(T) gg = g->parent;
        if(IsLChild(*v)){
            if(IsLChild(*p)){
                attachAsLC(g,p->rChild);
                attachAsLC(p,v->rChild);
                attachAsRC(p,g);
                attachAsRC(v,p);
            }else{
                //zig-zag
            }
        }else{
            if(IsRChild(*p)){
                //zag-zag
            }else{
                //zag-zig
            }
        }
        if(!gg) v->parent = NULL;
        else{
            g == gg->lChild?attachAsLC(gg,v):attachAsRC(gg,v);
        }
        updateHeight(g);updateHeight(p);updateHeight(v);
    }
    if(p = v-parent){
        //p为根额外单旋一次
    }
        v->parent = NULL;
    return v;
}

template <typename T>
BinNodePosi(T) &Splay<T>::search(const T& k){
    BinNodePosi(T) x = searchIn(_root,k,_hot=NULL);
    _root = splay(x?x:_hot);
    return _root;
}
template <typename T>
BinNodePosi(T) Splay<T>::insert(const T& k){
    search(k);
    BinNodePosi(T) x = new BinNode<T>(k,NULL);
    _size++;
    x->rChild = _root->rChild;
    x->lChild = _root;
    _root->parent = x;
    _root = x;
    updateHeightAbove(_root->lChild);
    return _root;
}
template <typename T>
bool Splay<T>::remove(const T& k){
}