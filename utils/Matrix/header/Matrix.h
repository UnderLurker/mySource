#pragma once

#ifndef _MATRIX_
#define _MATRIX_

#include <ostream>
#include <vector>
using namespace std;
template<typename T>
class Matrix{
    using value_type = T;
public:
    value_type** matrix;
    int row;
    int col;
    Matrix(){}
    Matrix(int,int,T);
    Matrix(int,int,vector<vector<T>>);
    Matrix(const Matrix&) = default;
    Matrix<T>& operator+(const Matrix&);
    Matrix<T>& operator-(const Matrix&);
    friend ostream& operator<<(ostream& out, const Matrix<T>& res){
        for(int i=0;i<res.row;i++){
            for(int j=0;j<res.col;j++){
                out<<res.matrix[i][j]<<" ";
            }
            out<<"\n";
        }
        return out;
    }

    ~Matrix();
};

template<typename T>
Matrix<T>::Matrix(int _row,int _col,vector<vector<T>> _val){
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
Matrix<T>::Matrix(int _row,int _col,T _val){
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
Matrix<T>::~Matrix<T>(){
    for(int i=0;i<row;i++){
        delete [] matrix[i];
    }
    delete [] matrix;
}

#endif