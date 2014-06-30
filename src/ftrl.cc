#include <cmath>
#include <iostream>
#include <cstdlib>
#include "ftrl.h"
#include "cpp-common/cpp_common.h"
using namespace CppCommonFunction;

bool FTRL::InitFromStr(const std::string& para){
    std::vector<std::string> line_vec;
    std::vector<std::string> item_vec;
    unsigned int count=0;
    unsigned int n=StringFunction::split(para,',',line_vec);
    for(size_t i=0;i<n;++i){
        unsigned int m=StringFunction::split(line_vec[i],':',item_vec);
        if(item_vec[0]==std::string("alpha")){
            ++count;
            alpha_=atof(item_vec[1].c_str());
        }else if(item_vec[0]==std::string("beta")){
            ++count;
            beta_=atof(item_vec[1].c_str());
        }else if(item_vec[0]==std::string("lambda1")){
            ++count;
            lambda1_=atof(item_vec[1].c_str());
        }else if(item_vec[0]==std::string("lambda2")){
            ++count;
            lambda2_=atof(item_vec[1].c_str());
        }
    }
    if(4==count)    return true;
    std::cerr<<"Error, not enough parameter for ftrl algo:"<<para<<std::endl;
    return false;
}
float FTRL::PredictProb(const float* w,const float* z,const float* n,const char* indicator,const std::vector<unsigned int>& x)const{
    double v=0.0;
    for(size_t i=0;i<x.size();++i){
        if(!indicator[x[i]]){  // has not update the coordinate yet
            v+=w[x[i]];
        }else{
            if(std::abs(z[x[i]])>lambda1_){
                float sgn=1;
                if(z[x[i]]<0)   sgn=-1;
                float wi=(sgn*lambda1_-z[x[i]])/(lambda2_+(beta_+std::sqrt(n[x[i]]))/alpha_);
                v+=wi;
            }
        }
    }
    return 1.0/(1.0+std::exp(-v));
}

void FTRL::Update(float* w,float* z,float* n,char* indicator,float p,unsigned int y,const std::vector<unsigned int>& x){
    for(size_t i=0;i<x.size();++i){
        float wi=0.0;
        if(!indicator[x[i]]){  // has not update the coordinate yet
            wi=w[x[i]];
        }else{
            if(std::abs(z[x[i]])>lambda1_){
                float sgn=1;
                if(z[x[i]]<0)   sgn=-1;
                wi=(sgn*lambda1_-z[x[i]])/(lambda2_+(beta_+std::sqrt(n[x[i]]))/alpha_);
            }
        }
        float gi=(p-y);
        float delta=(std::sqrt(n[x[i]]+gi*gi)-std::sqrt(n[x[i]]))/alpha_;
        z[x[i]]+=gi-delta*wi;
        n[x[i]]+=gi*gi;
        indicator[x[i]]=(char)1;
    }
}
