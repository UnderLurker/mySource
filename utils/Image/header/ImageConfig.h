#ifndef _IMAGE_CONFIG_
#define _IMAGE_CONFIG_

#include "Util.h"
#include <cmath>
#include <memory.h>

NAME_SPACE_START(myUtil)

//获取Two-dimensional Gaussian distribution
double** getTwoDimGaussianDistrbute(int GaussianRadius,int GaussianTemplateRadius){
    int n=2*GaussianTemplateRadius+1;
    double** res=new double*[n];
    for(int i=0;i<n;i++) res[i]=new double[n];
    double t=1.0/(2*M_PI*GaussianRadius*GaussianRadius);
    for(int i=-1*GaussianTemplateRadius;i<=GaussianTemplateRadius;i++){
        for(int j=-1*GaussianTemplateRadius;j<=GaussianTemplateRadius;j++){
            res[i+GaussianTemplateRadius][j+GaussianTemplateRadius]=t*exp((-1)*(i*i+j*j)/(2*GaussianRadius*GaussianRadius));
        }
    }
    return res;
}

NAME_SPACE_END()

#endif //!_IMAGE_CONFIG_