#include <iostream>
#include <cstdlib>
#include "cpp-common/cpp_common.h"
#include "ftrl.h"
#include "online_training.h"
using namespace CppCommonFunction;

int main(int argc,char **argv){
    std::string data_path("/data/ad-impression/2014-06-25/hours/");
    std::string index_file_name("model/ctr_coef.txt");
    IndexAdapter index_adapter;
    if(!index_adapter.InitFromFile(index_file_name,0,'\001')){
        std::cerr<<"Error, init index adapter faild."<<std::endl;
        return -1;
    }
    if(argc!=4){
        std::cerr<<"usage:"<<argv[0]<<" num_partition algo algo_para_str"<<std::endl;
        return -1;
    }
    unsigned int num_partition=atoi(argv[1]);
    string algo(argv[2]);
    string algo_para(argv[3]);
    std::vector<std::vector<unsigned int> > partitions;
    if(GeneratePartition(partitions,num_partition))    return -1;
    ISGD *sgd=0;
    if(algo==std::string("ftrl")){
        sgd=new FTRL();
        if(!sgd->InitFromStr(algo_para))    return -1;
    }
    if(sgd!=0){
        OnlineTraining(partitions,index_adapter,data_path,*sgd);
    }else{
        std::cerr<<"Error, not algo supplied."<<std::endl;
    }
    if(sgd!=0)    delete sgd;
}
