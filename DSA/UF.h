#pragma once

class UF{
private:
    int *id;
    unsigned int* sz;
    int count;
public:
    UF(int n);
    int find(int p);
    void uni(int p,int q);
    bool connected(int p,int q);
    int getCount();
};
