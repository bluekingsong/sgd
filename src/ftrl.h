#ifndef SGD_SRC_FTRL_H
#define SGD_SRC_FTRL_H
#include "isgd.h"

class FTRL: public ISGD{
  private:
    float alpha_;
    float beta_;
    float lambda1_;
    float lambda2_;
  public:
    virtual bool InitFromStr(const std::string& para);
    virtual float PredictProb(const float* w,const float* z,const float* n,const char* indicator,const std::vector<unsigned int>& x)const;
    virtual void Update(float* w,float* z,float* n,char* indicator,float p,unsigned int y,const std::vector<unsigned int>& x);
    virtual ~FTRL(){}
};

#endif
