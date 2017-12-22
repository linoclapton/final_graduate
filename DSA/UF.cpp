#include "UF.h"

UF::UF(int n){
    id = new int[n];
    sz = new unsigned int[n];
    for(int i=0;i<n;i++){
        id[i]=i;sz[i]=1;
        count = n;
    }
}

void UF::uni(int p,int q){
    int pRoot = find(p);
    int qRoot = find(q);
    if(pRoot!=qRoot){
        if(sz[pRoot]<sz[qRoot]){
            id[pRoot] = qRoot;
            sz[qRoot] += sz[pRoot];
        }else{
            id[qRoot] = pRoot;
            sz[pRoot] += sz[qRoot];
        }
        count--;
    }
}

int UF::find(int p){
    while(p!=id[p]){
        id[p] = id[id[p]];
        p = id[p];
    }
    return p;
}

bool UF::connected(int p,int q){
    int pRoot = find(p); 
    int qRoot = find(q);
    return pRoot == qRoot;
}

int UF::getCount(){
    return count;
}