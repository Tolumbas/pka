#ifndef MYVECTOR_H
#define MYVECTOR_H
#include <utility>
#include<iostream>
using namespace std;

template<class T>
class myvector{
    public:
    int length;
    int capasity;
    T* container;
    myvector<T>(){
        container = new T[1];
        capasity = 1;
        length = 0;
    }
    myvector<T>(const myvector<T>& cop){
        this->capasity = cop.capasity;
        this->length = cop.length;
        delete[] this->container;
        this->container = new T[cop.capasity];
        for (int a=0;a<cop.length;a++){
            this->container[a] = cop.container[a];
        }

    }
    myvector<T>& operator=(const myvector<T>& cop){
        this->capasity = cop.capasity;
        this->length = cop.length;
        delete[] this->container;
        this->container = new T[cop.capasity];
        for (int a=0;a<cop.length;a++){
            this->container[a] = cop.container[a];
        }
    }
    void push(T v){
        container[length] = v;
        length++;
        if (length == capasity){
            T* cop = new T[capasity*2];
            for (int a=0;a<length;a++){
                cop[a]=container[a];
            }
            capasity=capasity*2;
            delete[] container;
            container = cop;
        }
    }
    T& operator[](int a){
        if (a >= length){
            throw "sorry";
        }
        return container[a];
    }
    ~myvector(){
        delete[] container;
    }


};

#endif
