#pragma once

#include "Matrix.h"
#ifndef _MATRIX_
#define _MATRIX_

#include <ostream>
#include <iostream>
#include <vector>
#include <type_traits>
#include <typeinfo>
#include <memory>
#include <string>
#include <cstdlib>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif

#define COUTWiDTH 5

using namespace std;

template <class T>
string type_name()
{
    typedef typename remove_reference<T>::type TR;
    unique_ptr<char, void (*)(void *)> own(
#ifndef _MSC_VER
        abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr),
#else
        nullptr,
#endif
        free);
    string r = own != nullptr ? own.get() : typeid(TR).name();
    if (is_const<TR>::value)
        r += " const";
    if (is_volatile<TR>::value)
        r += " volatile";
    if (is_lvalue_reference<T>::value)
        r += "&";
    else if (is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}

template<typename T>
class Matrix{
    using value_type = T;
public:
    value_type** matrix;
    int row;
    int col;
    Matrix(){}
    Matrix(int,int);
    Matrix(int,int,const T&);
    Matrix(int,int,const vector<T>&);
    Matrix(int,int,vector<vector<T>>&);
    Matrix(const Matrix<T>&);
    T getValue(int row,int col);
    void setValue(int row,int col,const T& value);
    Matrix<T>& operator+(const Matrix&);
    Matrix<T>& operator-(const Matrix&);
    Matrix<double> operator*(Matrix<double>& obj);
    Matrix<T>& operator=(const Matrix<T>& obj);
    friend ostream& operator<<(ostream& out, const Matrix<T>& res){
        for(int i=0;i<res.row;i++){
            for(int j=0;j<res.col;j++){
                out.width(COUTWiDTH);
                out<<res.matrix[i][j]<<" ";
            }
            out<<"\n";
        }
        return out;
    }
    Matrix<double> Pow(int n);
    //魔方矩阵
    static Matrix<double> Magic(int n);
    //单位矩阵
    static Matrix<double> Eye(int n);
    //元素全为0的矩阵
    static Matrix<double> Zeros(int n);
    static Matrix<double> Zeros(int row,int col);
    //元素全为1的矩阵
    static Matrix<double> Ones(int n);
    static Matrix<double> Ones(int row,int col);
    //帕斯卡矩阵
    static Matrix<double> Pascal(int n);
    ~Matrix();
};

template<typename T>
Matrix<T>::Matrix(int _row,int _col){
    this->row=_row;
    this->col=_col;
    matrix=new T*[row];
    for(int i=0;i<row;i++){
        matrix[i]=new T[col];
        // memset(matrix[i],0,sizeof(T)*col);
    }
}

template<typename T>
Matrix<T>::Matrix(int _row,int _col,const vector<T>& _val){
    if(_col*_row!=_val.size()) return;
    this->row=_row;
    this->col=_col;
    matrix=new T*[row];
    int count=0;
    for(int i=0;i<row;i++){
        matrix[i]=new T[col];
        for(int j=0;j<col;j++){
            matrix[i][j]=_val[count];
        }
    }
}

template<typename T>
Matrix<T>::Matrix(int _row,int _col,vector<vector<T>>& _val){
    this->col=_col;
    this->row=_row;
    matrix=new T*[row];
    for(int i=0;i<row;i++){
        matrix[i]=new T[col];
    }
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            matrix[i][j]=_val[i][j];
        }
    }
}
template<typename T>
Matrix<T>::Matrix(int _row,int _col,const T& _val){
    this->col=_col;
    this->row=_row;
    matrix=new T*[row];
    for(int i=0;i<row;i++){
        matrix[i]=new T[col];
    }
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            matrix[i][j]=_val;
        }
    }
}

template<typename T>
Matrix<T>::Matrix(const Matrix<T>& obj){
    this->row=obj.row;
    this->col=obj.col;
    matrix=new T*[row];
    for(int i=0;i<row;i++){
        matrix[i]=new T[col];
    }
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            matrix[i][j]=obj.matrix[i][j];
        }
    }
}

template<typename T>
T Matrix<T>::getValue(int row,int col){
    if(row<0||row>this->row||col<0||col>this->col){
        return T();
    }
    return matrix[row][col];
}

template<typename T>
void Matrix<T>::setValue(int row,int col,const T& value){
    if(row<0||row>this->row||col<0||col>this->col) return;
    matrix[row][col]=value;
}

template<typename T>
Matrix<T>& Matrix<T>::operator+(const Matrix &Addend){
    for(int r=0;r<this->row;r++){
        for(int c=0;c<this->col;c++){
            this->matrix[r][c]+=Addend.matrix[r][c];
        }
    }
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator-(const Matrix &Addend){
    for(int r=0;r<this->row;r++){
        for(int c=0;c<this->col;c++){
            this->matrix[r][c]-=Addend.matrix[r][c];
        }
    }
    return *this;
}

template<typename T>
Matrix<double> Matrix<T>::operator*(Matrix<double>& obj){
    Matrix<double> res=Matrix<double>::Zeros(row,obj.col);
    if(this->col!=obj.row) return res;
    for(int i=0;i<row;i++){
        for(int j=0;j<obj.col;j++){
            double sum=0;
            for(int k=0;k<col;k++){
                sum+=getValue(i, k)*obj.getValue(k, j);
            }
            res.setValue(i, j, sum);
        }
    }
    return res;
}

template<typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& obj){
    this->row=obj.row;
    this->col=obj.col;
    matrix=new T*[row];
    for(int i=0;i<row;i++){
        matrix[i]=new T[col];
    }
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            matrix[i][j]=obj.matrix[i][j];
        }
    }
    return *this;
}

template<typename T>
Matrix<T>::~Matrix<T>(){
    for(int i=0;i<row;i++){
        delete [] matrix[i];
    }
    delete [] matrix;
}


template<>
inline Matrix<double> Matrix<double>::Magic(int n){
    Matrix<double> res(n,n,0);
    int maxCount=n*n;
    int curNum=1,r=0,c=0;
    while(curNum<=maxCount){
        if(res.getValue(r, c)==0){
            res.setValue(r--, c++, curNum++);
        }
        else{
            r+=2;
            c--;
        }
        r=(r+n)%n;
        c=(c+n)%n;
    }
    return res;
}

template<>
inline Matrix<double> Matrix<double>::Eye(int n){
    Matrix<double> res(n,n,0);
    for(int i=0;i<n;i++){
        res.setValue(i, i, 1);
    }
    return res;
}

template<>
inline Matrix<double> Matrix<double>::Zeros(int n){
    return Matrix<double>(n,n,0);
}

template<>
inline Matrix<double> Matrix<double>::Zeros(int row,int col){
    return Matrix<double>(row,col,0);
}

template<>
inline Matrix<double> Matrix<double>::Ones(int n){
    return Matrix<double>(n,n,1);
}

template<>
inline Matrix<double> Matrix<double>::Ones(int row,int col){
    return Matrix<double>(row,col,1);
}

template<>
inline Matrix<double> Matrix<double>::Pascal(int n){
    Matrix<double> res(n,n,1);
    for(int r=1;r<n;r++){
        for(int c=1;c<n;c++){
            res.setValue(r, c, res.getValue(r-1, c)+res.getValue(r, c-1));
        }
    }
    return res;
}

template<>
inline Matrix<double> Matrix<double>::Pow(int n){
    Matrix<double> res(row,col,0);
    for(int i=0;i<row;i++){
        res.setValue(i, i, 1);
    }
    Matrix<double> temp=*this;
    while(n){
        if((n&1)!=0){
            res=res*temp;
        }
        temp=temp*temp;
        n>>=1;
    }
    return res;
}

#endif
