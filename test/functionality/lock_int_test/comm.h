#ifndef DOZERG_TEST_COMM_H
#define DOZERG_TEST_COMM_H

template<typename T>
struct IsEven
{
    T c_;
    explicit IsEven(T c):c_(c){}
    bool operator ()(T c) const{return (0 == ((c + c_) & 1));}
};

template<typename T>
struct AddSquare
{
    T c_;
    explicit AddSquare(T c):c_(c){}
    void operator ()(T & v) const{v += c_ * c_;}
};

#endif
