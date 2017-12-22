#pragma once
#include <iostream>
using namespace std;
class Serializable{
public:
    friend ofstream& operator<<(ofstream& out,Serializable& data);
    friend ifstream& operator>>(ifstream& in ,Serializable& data);
};