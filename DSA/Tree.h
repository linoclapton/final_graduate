#pragma once
#include "Stack.h"
#include "Queue.h"

template <typename T> void release ( T x ) { Cleaner<T>::clean ( x ); }
typedef enum { RB_RED, RB_BLACK} RBColor; //�ڵ���ɫ

#define BinNodePosi(T) BinNode<T>*
#define stature(p) ((p) ? (p)->height : -1) //�ڵ�߶ȣ��롰�����߶�Ϊ-1����Լ����ͳһ��
#define IsRoot(x) ( ! ( (x).parent ) )
#define IsLChild(x) ( ! IsRoot(x) && ( & (x) == (x).parent->lChild ) )
#define IsRChild(x) ( ! IsRoot(x) && ( & (x) == (x).parent->rChild ) )
#define HasParent(x) ( ! IsRoot(x) )
#define HasLChild(x) ( (x).lChild )
#define HasRChild(x) ( (x).lChild )
#define HasChild(x) ( HasLChild(x) || HasRChild(x) ) //����ӵ��һ������
#define HasBothChild(x) ( HasLChild(x) && HasRChild(x) ) //ͬʱӵ����������
#define IsLeaf(x) ( ! HasChild(x) )
#define IsBlack(p) ( ! (p) || ( RB_BLACK == (p)->color ) ) //�ⲿ�ڵ�Ҳ�����ڽڵ�
#define IsRed(p) ( ! IsBlack(p) ) //�Ǻڼ���
#define BlackHeightUpdated(x) ( /*RedBlack�߶ȸ�������*/ \
    ( stature( (x).lc ) == stature( (x).rc ) ) && \
    ( (x).height == ( IsRed(& x) ? stature( (x).lc ) : stature( (x).lc ) + 1 ) ) \
    )

template <typename T>
class BinNode{
public:
    T data;
    BinNodePosi(T) parent;
    BinNodePosi(T) lChild;
    BinNodePosi(T) rChild;
    int height;
    RBColor color;
public:
    BinNode(T const & data,BinNodePosi(T) parent=NULL,BinNodePosi(T) lChild=NULL,BinNodePosi(T) rChild=NULL,int height = 0,RBColor color=RB_RED)
        :data(data),parent(parent),lChild(lChild),rChild(rChild),height(height),color(color)
    {}
    //include self descendent number 
    int size();
    BinNodePosi(T) insertAsLC(T const &);
    BinNodePosi(T) insertAsRC(T const &);
    BinNodePosi(T) succ();
    template <typename VST> void travLevel(VST &);
    template <typename VST> void travPre(VST &);
    template <typename VST> void travIn(VST &);
    template <typename VST> void travPost(VST &);
};

template <typename T>
class BinTree{
protected:
    int _size;
    BinNodePosi(T) _root;
    virtual int updateHeight(BinNodePosi(T) x);
    void updateHeightAbove(BinNodePosi(T) x);
public:
    BinTree(T const& data){_root = new BinNode<T>(data);_size=1;}
    int size() const{return _size;}
    bool empty() const{return !_root;}
    BinNodePosi(T) root() {return _root;}
    /*  �������룬ɾ���ͷ���ӿ�  */
    BinNodePosi(T) insertAsLC(BinNodePosi(T) x,T const& data);
    BinNodePosi(T) insertAsRC(BinNodePosi(T) x,T const& data);
    BinNodePosi(T) attachAsLC ( BinNodePosi(T) x, BinTree<T>* &T ); //T��Ϊx����������
    BinNodePosi(T) attachAsRC ( BinNodePosi(T) x, BinTree<T>* &T ); //T��Ϊx����������
    /*  �����ӿ�  */
};


template <typename T>
BinNodePosi(T) BinNode<T>::insertAsLC(T const &data){
    return lChild = new BinNode(data,this);
}

template <typename T>
BinNodePosi(T) BinNode<T>::insertAsRC(T const &data){
    return rChild = new BinNode(data,this);
}
template <typename T> //���������������㷨����S�����ڵ�x�����������룬S�����ÿ�
BinNodePosi(T) BinTree<T>::attachAsLC ( BinNodePosi(T) x, BinTree<T>* &S ) { //x->lc == NULL
    if ( x->lChild = S->_root ) x->lChild->parent = x; //����
    _size += S->_size; updateHeightAbove ( x ); //����ȫ����ģ��x�������ȵĸ߶�
    S->_root = NULL; S->_size = 0; release ( S ); S = NULL; return x; //�ͷ�ԭ�������ؽ���λ��
}
template <typename T> //���������������㷨����S�����ڵ�x�����������룬S�����ÿ�
BinNodePosi(T) BinTree<T>::attachAsRC ( BinNodePosi(T) x, BinTree<T>* &S ) { //x->rc == NULL
    if ( x->rChild = S->_root ) x->rChild->parent = x; //����
    _size += S->_size; updateHeightAbove ( x ); //����ȫ����ģ��x�������ȵĸ߶�
    S->_root = NULL; S->_size = 0; release ( S ); S = NULL; return x; //�ͷ�ԭ�������ؽ���λ��
}
template <typename T>
int BinNode<T>::size(){
    int s = 1;
    if(lChild) s += lChild->size();
    if(rChild) s += rChild->size();
    return s;
}

template <typename T>
int BinTree<T>::updateHeight(BinNodePosi(T) x){
    return x->height = max(stature(x->lChild),
                           stature(x->rChild))+1;
}

template <typename T>
void BinTree<T>::updateHeightAbove(BinNodePosi(T) x){
    while(x){
        updateHeight(x);x=x->parent;
    }
}

template <typename T> 
BinNodePosi(T) BinTree<T>::insertAsRC(BinNodePosi(T) x,T const& data){
    _size++; x->insertAsRC(data);
    updateHeightAbove(x);
    return x->rChild;
}
template <typename T> 
BinNodePosi(T) BinTree<T>::insertAsLC(BinNodePosi(T) x,T const& data){
    _size++; x->insertAsLC(data);
    updateHeightAbove(x);
    return x->lChild;
}

template <typename T,typename VST>
void traversePre(BinNodePosi(T) x,VST& visit){
    if(!x) return;
    visit(x->data);
    traversePre(x->lChild,visit);
    traversePre(x->rChild,visit);
}

template <typename T,typename VST>
void travPre_I1(BinNodePosi(T) x,VST& visit){
    Stack<T> s;
    if(!x) s.push(x);
    while(!s.empty()){
        x = s.pop();
        visit(x);
        if(x->rChild) s.push(x->rChild);
        if(x->lChild) s.push(x->lChild);
    }
}
template <typename T,typename VST>
void visitAlongLeftBranch(BinNodePosi(T) x,VST& visit,Stack<BinNodePosi(T)> &s){
    while(x){
        visit(x->data);
        if(x->rChild) s.push(x->rChild);
        x = x->lChild;
    }
}
template <typename T,typename VST>
void travPre_I2(BinNodePosi(T) x,VST& visit){
    Stack<BinNodePosi(T)> s;
    do{
        visitAlongLeftBranch(x,visit,s);
    }while(!s.empty()&&(x=s.pop()));
}

template <typename T,typename VST>
void traverseIn(BinNodePosi(T) x,VST& visit){
    if(!x) return;
    traverseIn(x->lChild,visit);
    visit(x);
    traverseIn(x->rChild,visit);
}

template <typename T>
void goAlongLeftBranch(BinNodePosi(T) x,Stack<BinNodePosi(T)> &s){
    while(x){
        s.push(x);x = x->lChild;
    }
}
template <typename T,typename VST>
void travIn_I1(BinNodePosi(T) x,VST& visit){
    Stack<BinNodePosi(T)> s;
    do{
        goAlongLeftBranch(x,s);
        x = s.pop();
        visit(x->data);
        x = x->rChild;
    }while(x||!s.empty());
}
template <typename T,typename VST>
void traversePost(BinNodePosi(T) x,VST& visit){
    if(!x) return;
    traversePost(x->lChild);
    traversePost(x->rChild);
    visit(x->data);
}
template <typename T,typename S>
void goAlongLeftRightBranch(BinNodePosi(T) x,Stack<BinNodePosi(T)> &s,Stack<S> &s2){
    while(x){
        s.push(x);s2.push(false);
        if(x->rChild){s.push(x->rChild);s2.push(true);}
        x = x->lChild;
    }
}
template <typename T,typename VST>
void travPost_I1(BinNodePosi(T) x,VST& visit){
    Stack<BinNodePosi(T)> s;Stack<bool> s2;
    bool flag = true;
    do{
        if(flag)
        {
            goAlongLeftRightBranch(x,s,s2);
            x = s.pop(); flag = s2.pop();
        }
        visit(x->data);
        if(s.empty()) break;
        x = s.pop(); flag = s2.pop();
    }while(x||!s.empty());

}
template <typename T> template <typename VST>
void BinNode<T>::travLevel(VST &visit){
    Queue<BinNodePosi(T)> Q;
    Q.enqueue(this);
    BinNodePosi(T) n;
    while(!Q.empty()){
        n = Q.dequeue();
        visit(n->data);
        if(n->lChild) Q.enqueue(n->lChild);
        if(n->rChild) Q.enqueue(n->rChild);
    }
}

template <typename T> 
BinNodePosi(T) BinNode<T>::succ() { //��λ�ڵ�v��ֱ�Ӻ��
    BinNodePosi(T) s = this; //��¼��̵���ʱ����
    if ( rChild ) { //�����Һ��ӣ���ֱ�Ӻ�̱����������У�����ؾ���
        s = rChild; //��������
        while ( HasLChild ( *s ) ) s = s->lChild; //�����С���Ľڵ�
    } else { //����ֱ�Ӻ��Ӧ�ǡ�����ǰ�ڵ���������������е�������ȡ�������ؾ���
        while ( IsRChild ( *s ) ) s = s->parent; //������������֧�����ϳ����Ϸ��ƶ�
        s = s->parent; //����ٳ����Ϸ��ƶ�һ�������ִ�ֱ�Ӻ�̣�������ڣ�
    }
    return s;
}

