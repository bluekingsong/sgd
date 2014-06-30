#ifndef SGD_SRC_ISGD_H
#define SGD_SRC_ISGD_H
#include <vector>
#include <string>

class ISGD{
  public:
    virtual bool InitFromStr(const std::string& para)=0;
    virtual float PredictProb(const float* w,const float* z,const float* n,const char* indicator,const std::vector<unsigned int>& x)const=0;
    virtual void Update(float* w,float* z,float* n,char* indicator,float p,unsigned int y,const std::vector<unsigned int>& x)=0;
    virtual ~ISGD(){};
};
#endif
