#include <cstdio>
#include <iostream>
#include <Windows.h>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include "Util.h"
#include "video.h"
using namespace std;
using namespace myUtil;

class Spherical;
class Point3D{
public:
    double x;
    double y;
    double z;
    operator Spherical();
    Point3D() = default;
    Point3D(double _x,double _y,double _z):x(_x),y(_y),z(_z){}
};

class Spherical{
public:
    double ro;
    double thta;
    double beta;
    operator Point3D(){
        struct Point3D res;
        res.x=ro*sin(beta)*cos(thta);
        res.y=ro*sin(beta)*sin(thta);
        res.z=ro*cos(beta);
        return res;
    }
};

#define GOLD_RADIO (sqrt(5)+1.0)/2.0

vector<Point3D> RegularIcosahedron{
    {0,1,GOLD_RADIO},
    {0,1,-GOLD_RADIO},
    {0,-1,GOLD_RADIO},
    {0,-1,-GOLD_RADIO},
    {1,GOLD_RADIO,0},
    {1,-GOLD_RADIO,0},
    {-1,GOLD_RADIO,0},
    {-1,-GOLD_RADIO,0},
    {GOLD_RADIO,0,1},
    {GOLD_RADIO,0,-1},
    {-GOLD_RADIO,0,1},
    {-GOLD_RADIO,0,-1},
};

int main(){
    Spherical a;
    a.ro=1;
    a.beta=M_PI/4;
    a.thta=M_PI/4;
    Point3D b=static_cast<Point3D>(a);
    cout<<b.x<<endl<<b.y<<endl<<b.z<<endl;
    return 0;
}