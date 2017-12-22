#pragma once

#include "Vector.h"


typedef enum{UNDISCOVERED,DISCOVERED,VISITED} VStatus;
typedef enum{UNDETERMINED,TREE,CROSS,FORWARD,BACKWARD} EStatus;

template <typename Tv>
class Vertex{
public:
    Tv data;
    int inDegree,outDegree;
    VStatus status;
    int dTime,fTime;
    int parent;//遍历树中的父节点
    int priority;//在遍历树中的优先级(最短通路，极短跨边)
    Vertex(Tv const& d):data(d),inDegree(0),outDegree(0),status(UNDISCOVERED)
                        dTime(-1),fTime(-1),parent(-1),priority(INT_MAX){}
};

template <typename Te>
class Edge{
public:
    Te data:
    int weight:
    EStatus status;
    Edge(T const& d,int w):data(d),weight(w),status(UNDETERMINED){}
};

template <typename Tv,typename Te>
class Graph{
private:
    void reset(){
        for(int i=0;i<n;i++){
            status(i) = UNDISCOVERED;
        }
    }
public:
    /* ... 顶点操作，边操作，图算法 ...   */
};

template <typename Tv,typename Te>
class GraphMatrix:public Graph<Tv,Te>{
private:
    Vector< Vertex<Tv> > V;
    Vector< Vector< Edge<Te>* > > E;
    int n,e;
public:
    /* 顶点相关，边相关 */  
    GraphMatrix(){n = e = 0;}
    ~GraphMatrix(){
        for(int i=0;i<n;i++) 
        for(int j=0;j<n;j++) 
            delete E[i][j];
    }
    /* 顶点操作 */
    Tv& vertex(int i){return V[i].data;}
    int inDegree(int i){return V[i].inDegree;}
    int outDegree(int i){return V[i].outDegree;}
    VStatus &status(int i){return V[i].status;}
    int &dTime(int i){return V[i].dTime;}
    int &fTime(int i){return V[i].fTime;}
    int &parent(int i){return V[i].parent;}
    int &priority(int i){return V[i].priority;}
    int nextNbr(int i,int j){while(-1<j&&!exists(i,--j));return j;}
    int firstNbr(int i){return nextNbr(i,n);}
    bool exisits(int i,int j){ return (0<=i&&i<n)&&(0<=j&&j<n)&&E[i][j]!=NULL;}
    Te &edge(int i,int j){return E[i][j]->data;}
    EStatus &status(int i,int j){return E[i][j]->status;}
    int &weight(int i,int j){return E[i][j]->weight;}
    void insert(Te const &data,int weight,int i,int j){
        if(exisits(i,j)) return;
        E[i][j] = new Edge(data,weight);
        e++;
        V[i].outDegree++;
        V[j].inDegree++;
    }
    int insert(Tv const &vertex){
        for(int i=0;i<n;i++) E[i].insert(NULL);n++;
        E.insert(Vector< Edge<Te> >(n,n,NULL));
        V.insert(Vertex<Tv>(vertex));
    }
    Te remove(int i,int j){
        Te eBak = edge(i,j);
        delete E[i][j];E[i][j] = NULL;
        e--;
        V[i].outDegree--;
        V[j].inDegree--;
        return eBak;
    }
    Tv remove(int i){
        for(int j=0;j<n;j++)
            if(exists(i,j)){
                delete E[i][j];V[j].inDegree--;
            }
        E.remove(i);n--;
        for(int j=0;j<n;j++){
            if(exist(j,i)){
                delete E[j][i];V[j].outDegree--;
            }
        }
        Tv vBak = vertex(i);
        V.remove(i);
        return vBak;
    }
    void BFS(int v,int &clock);
    void bfs(int s);
    void DFS(int v,int &clock);
    void dfs(int s);
};


template<typename Tv,typename Te>
void Graph<Tv,Te>::BFS(int v,int &clock){
    Queue<int> Q;status(v) = DISCOVERED;
    Q.enqueue(v);
    while(!Q.empty()){
        int v = Q.dequeue();
        dTime(v) = ++clock;
        for(int u=firstNbr(v);-1<u;u=nextNbr(v,u))
            if(status(u)==UNDISCOVERED){
                status(u) = DISCOVERED;Q.enqueue(u);
                status(v,u) = TREE;parent(u) = v;
            }else if(status(v)==DISCOVERED){
                status(v,u) = CROSS;
            }else{
                status(v,u) = CROSS;//BACKWARD;
            }
        status(v) = VISITED;
    }
}
template <typename Tv,typename Te>
void bfs(int s){
    reset();int clock = 0;int v=s;
    do{
        if(status(v)==UNDISCOVERED)
            BFS(v,clock);
    }while(s!=(v=(++v%n)));
}
template <typename Tv,typename Te>
void DFS(int v,int &clock){
    status(v) = DISCOVERED; dTime(v) = ++clock;
    for(u = firstNbr(v);-1<u;u = nextNbr(v)){
        switch(status(u)){
        case UNDISCOVERED:
            status(v,u)=TREE;parent(u)=v;DFS(u,clock);break;
        case DISCOVERED:
            status(v,u)=BACKWARD;break;
        default:
            status(u) = (dTime(v)<dTime(u))?FORWARD:CROSS;break;
        }
    }
    fTime(v) = ++clock; status(v) = VISITED;
}
template <typename Tv,typename Te>
void dfs(int s){
    reset();int clock = 0;int v=s;
    do{
        if(status(v)==UNDISCOVERED)
            DFS(v,clock);
    }while(s!=(v=(++v%n)));
}