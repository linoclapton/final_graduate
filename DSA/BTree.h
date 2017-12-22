#pragma once

#include "Tree.h"
#include "Vector.h"
#define BTNodePosi(T) BTNode<T>*
#define uncle(x) /*����*/ \
    ( IsLChild( * ( (x)->parent ) ) ? (x)->parent->parent->rChild : (x)->parent->parent->lChild )

template <typename T>
class BTNode{
    BTNodePosi(T) parent; 
    Vector<T> key;
    Vector<BTNodePosi(T)> children;
    BTNode(){parent = NULL;children.insert(0,NULL);}
    BTNode(T e,BTNodePosi(T) lc=NULL,BTNodePosi(T) rc=NULL){
        parent = NULL;
        key.insert(0,e);
        children.insert(0,lc);children.insert(1,rc);
        if(lc) lc->parent = this;
        if(rc) rc->parent = this;
    }
};

template <typename T>
class BTree{
protected:
    int _size;int _mOrder;BTNodePosi(T) _root;
    BTNodePosi(T) _hot;//search���ʵķǿսڵ�
    void solveOverflow(BTNodePosi(T));
    void solveUnderflow(BTNodePosi(T));
public:
    BTNodePosi(T) &search(const T& k);
    bool insert(const T& k);
    bool remove(cosnt T& k);
};

template <typename T>
class RedBlack:public BST<T>{
public:
    //BST<T>::search
    BinNodePosi(T) insert(const T& k);
    bool remove(const T& k);
protected:
    void solveDoubleRed(BinNodePosi(T) x);
    void solveDoubleBlack(BinNodePosi(T) x);
    int updateHeight(BinNodePosi(T) x);//�ڸ߶�
};
template <typename T>
BTNodePosi(T) &BTree<T>::search(T const &k){
    BTNodePosi(T) v = _root; _hot = NULL;
    while(v){
        Rank r = v->key.search(k);
        if(0<=r&&k == v->key[r]) return v;
        _hot = v; v=v->children[r+1];
    }
    return NULL;
}
template <typename T>
bool BTree<T>::insert(T const &k){
    BTNodePosi(T) v = search(k);
    if(v) return false;
    Rank r = _hot->key.search(k);
    v->key.insert(r+1,k);
    v->children.insert(r+2,NULL);
    _size++; solveOverflow(_hot);
    return true;
}
template <typename T>
bool BTree<T>::remove(T const &k){
    BTNodePosi(T) v = search(v);
    if(!v) return false;
    Rank r = v->key.search(k);
    if(v->children[0]){
        BTNodePosi(T) u = v->children[r+1];
        while(u->chidren[0]) u = u->children[0];
        v->key[r] = u->key[0];v = u;r=0;
    }
    v->key.remove(r);v->children->remove(r+1);_size--;
    solveOverflow(v);return true;
}
template <typename T> 
int RedBlack<T>::updateHeight ( BinNodePosi(T) x ) { 
    x->height = max ( stature ( x->lChild ), stature ( x->rChild ) ); 
    return IsBlack ( x ) ? x->height++ : x->height; 
} 

template <typename T>
BinNodePosi(T) RedBlack<T>::insert(const T& k){
    BinNodePosi(T) &x = search(e);if(x) return x;
    x = new BinNode(e,_hot,NULL,NULL,-1);_size++;
    solveDoubleRed(x);
    return x?x:_hot->parent;
}
template <typename T>
bool RedBlack<T>::remove(const T& k){
    BinNodePosi(T) & x = search ( e ); if ( !x ) return false; 
    BinNodePosi(T) r = removeAt ( x, _hot ); if ( ! ( --_size ) ) return true; 
    if ( ! _hot ) 
    { _root->color = RB_BLACK; updateHeight ( _root ); return true; }
    if ( BlackHeightUpdated ( *_hot ) ) return true; 
    if ( IsRed ( r ) ) 
    { r->color = RB_BLACK; r->height++; return true; }
    solveDoubleBlack ( r ); return true; 
}
template <typename T>
void RedBlack<T>::solveDoubleRed(BinNodePosi(T) x){
    if(IsRoot(*x)) {_root->color = RB_BLACK; _root->height++;return;}
    BinNodePosi(T) p = x->parent;
    if(IsBlack(p)) return;
    BinNodePosi(T) g = x->parent;
    BinNodePosi(T) u = uncle(x);
    //RR-1
    if(IsBlack(u)){
        if ( IsLChild ( *x ) == IsLChild ( *p ) ) 
            p->color = RB_BLACK; 
        else 
            x->color = RB_BLACK; 
        g->color = RB_RED; 
        BinNodePosi(T) gg = g->parent; 
        BinNodePosi(T) r = FromParentTo ( *g ) = rotateAt ( x ); 
        r->parent = gg; 
    }
    else{//RR-2
        p->color = RB_BLACK; p->height++; 
        u->color = RB_BLACK; u->height++;
        if ( !IsRoot ( *g ) ) g->color = RB_RED;
        solveDoubleRed ( g ); 
    }

}

template <typename T> void RedBlack<T>::solveDoubleBlack ( BinNodePosi(T) r ) {
    BinNodePosi(T) p = r ? r->parent : _hot; if ( !p ) return; //r�ĸ���
    BinNodePosi(T) s = ( r == p->lChild ) ? p->rChild : p->lChild; //r���ֵ�
    if ( IsBlack ( s ) ) { //�ֵ�sΪ��
        BinNodePosi(T) t = NULL; //s�ĺ캢�ӣ������Һ��ӽԺ죬�������ȣ��Ժ�ʱΪNULL��
        if ( IsRed ( s->rChild ) ) t = s->rChild; //����
        if ( IsRed ( s->lChild ) ) t = s->lChild; //����
        if ( t ) { //��s�к캢�ӣ�BB-1
            //*DSA*/printf("  case BB-1: Child ("); print(s->lChild); printf(") of BLACK sibling ("); print(s); printf(") is RED\n");
            RBColor oldColor = p->color; //����ԭ�������ڵ�p��ɫ������t���丸�ס��游
            // ���£�ͨ����ת��ƽ�⣬���������������Һ���Ⱦ��
            BinNodePosi(T) b = FromParentTo ( *p ) = rotateAt ( t ); //��ת
            if ( HasLChild ( *b ) ) { b->lChild->color = RB_BLACK; updateHeight ( b->lChild ); } //����
            if ( HasRChild ( *b ) ) { b->rChild->color = RB_BLACK; updateHeight ( b->rChild ); } //����
            b->color = oldColor; updateHeight ( b ); //���������ڵ�̳�ԭ���ڵ����ɫ
            //*DSA*/printBinTree(b, 0, 0);
        } else { //��s�޺캢��
            s->color = RB_RED; s->height--; //sת��
            if ( IsRed ( p ) ) { //BB-2R
                //*DSA*/printf("  case BB-2R: Both children ("); print(s->lChild); printf(") and ("); print(s->rChild); printf(") of BLACK sibling ("); print(s); printf(") are BLACK, and parent ("); print(p); printf(") is RED\n"); //s���Ӿ��ڣ�p��
                p->color = RB_BLACK; //pת�ڣ����ڸ߶Ȳ���
                //*DSA*/printBinTree(p, 0, 0);
            } else { //BB-2B
                //*DSA*/printf("  case BB-2R: Both children ("); print(s->lChild); printf(") and ("); print(s->rChild); printf(") of BLACK sibling ("); print(s); printf(") are BLACK, and parent ("); print(p); printf(") is BLACK\n"); //s���Ӿ��ڣ�p��
                p->height--; //p���ֺڣ����ڸ߶��½�
                //*DSA*/printBinTree(p, 0, 0);
                solveDoubleBlack ( p ); //�ݹ�����
            }
        }
    } else { //�ֵ�sΪ�죺BB-3
        //*DSA*/printf("  case BB-3: sibling ("); print(s); printf(" is RED\n"); //s�죨˫�Ӿ�ڣ�
        s->color = RB_BLACK; p->color = RB_RED; //sת�ڣ�pת��
        BinNodePosi(T) t = IsLChild ( *s ) ? s->lChild : s->rChild; //ȡt���丸sͬ��
        _hot = p; FromParentTo ( *p ) = rotateAt ( t ); //��t���丸�ס��游��ƽ�����
        //*DSA*/printBinTree<T>(s, 0, 0);
        solveDoubleBlack ( r ); //��������r��˫�ڡ�����ʱ��p��ת�죬�ʺ���ֻ����BB-1��BB-2R
    }
}
