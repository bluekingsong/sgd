#include <iostream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include "cpp-common/cpp_common.h"
#include "online_training.h"
#include "isgd.h"
#include "ftrl.h"

void DetermineFilename(unsigned int partition_size,const std::string& data_path,unsigned int hour,std::string& input,std::string& output,std::string& id_output){
    std::stringstream strstream(data_path);
    strstream<<hour<<"-"<<partition_size;
    input=strstream.str();
    strstream<<".predict";
    output=strstream.str();
    strstream.str(input);
    strstream<<".id";
    id_output=strstream.str();
}
float PredictProb(const float* w,const std::vector<unsigned int>& instance){
    float v=0.0;
    for(size_t i=0;i<instance.size();++i)   v+=w[instance[i]];
    return 1.0/(1.0+std::exp(-v));
}
int ParseFeature(const std::string& line,std::vector<unsigned int>& instance,const IndexAdapter& index_adapter){
    unsigned int bias_index;
    index_adapter.GetIndex(std::string("INTERCEPT"),bias_index);
    instance.clear();
    instance.push_back(bias_index);
    std::vector<std::string> line_vec;
    unsigned int n=StringFunction::split(line,'\001',line_vec);
    for(unsigned int i=2;i<n;++i){
        unsigned int index;
        if(index_adapter.GetIndex(line_vec[i],index))   instance.push_back(index);
    }
    return std::atoi(line_vec[1].c_str());
}
void OnlineTraining(const std::vector<std::vector<unsigned int> >& partitions,const IndexAdapter& index_adapter,const std::string& data_path,ISGD& sgd){
    unsigned int num_feature=index_adapter.GetSize();
    unsigned int mem_size=3*sizeof(float)*num_feature+sizeof(char)*num_feature;
    char *mem=new char[mem_size];
    char *indicator=mem;  // to help restart of FTRL traning
    float *w=(float*)(indicator+num_feature);  // LR parameter
    float *z=w+num_feature;  // parameter in FTRL
    float *n=z+num_feature;  // parameter in FTRL
    std::vector<unsigned int> instance;
    for(unsigned int P=0;P<partitions.size();++P){
         // start of a new stream training
        std::memset(indicator,0,sizeof(indicator)*num_feature);
        std::memset(z,0,sizeof(z)*num_feature*2);
        for(unsigned int Q=0;Q<partitions[P].size();++Q){
           unsigned int hour=partitions[P][Q];
           std::string input,output;  //stream_output;
           std::string id_output;
           DetermineFilename(partitions.size(),data_path,hour,input,output,id_output);
           ifstream fin(input.c_str());
           ofstream fout(output.c_str());
           ofstream idfout(id_output.c_str());
           if(!fin.is_open() || !fout.is_open()){
               std::cerr<<"open file("<<input<<","<<output<<","<<id_output<<") faild."<<std::endl;
               return;
           }
           string line;
           while(std::getline(fin,line)){
               int y=ParseFeature(line,instance,index_adapter);

               idfout<<y;
               for(size_t i=0;i<instance.size();++i) idfout<<" "<<instance[i];
               idfout<<std::endl;
               //
               float q=PredictProb(w,instance);
               float p=sgd.PredictProb(w,z,n,indicator,instance);
               sgd.Update(w,z,n,indicator,p,y,instance);
               fout<<y<<"\t"<<q<<"\t"<<p<<std::endl;
           }
       }
    }
}
bool GeneratePartition(std::vector<std::vector<unsigned int> >& partitions, unsigned int num){
    if(num>=24) return false;
    unsigned int gap=24/num;
    partitions.clear();
    unsigned int hour=0;
    for(unsigned int i=0;i<num;++i){
        partitions.push_back(std::vector<unsigned int>());
        for(unsigned int j=0;j<gap;++j)    partitions.back().push_back(hour);
        ++hour;
        if(hour>23)    break;
    }
    return true;
}
