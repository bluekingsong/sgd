#include <iostream>
#include <cstdlib>
#include "cpp-common/cpp_common.h"
#include "ftrl.h"
#include "online_training.h"
using namespace CppCommonFunction;

int main(int argc,char **argv){
	std::string data_path("/data/ad-impression/2014-06-25/hours/");
	std::string batch_para_file("model/ctr_coef.txt");
	if(argc!=5){
		std::cerr<<"usage:"<<argv[0]<<" num_partition algo algo_para_str predict_output"<<std::endl;
		return -1;
	}
	unsigned int num_partition=std::atoi(argv[1]);
	std::string algo(argv[2]);
	std::string algo_para(argv[3]);
	std::string predict_output(argv[4]);
	std::vector<std::vector<unsigned int> > partitions;
	/*IndexAdapter index_adapter;
	std::cout<<"start build IndexAdapter.time:"<<TimeFunction::now()<<std::endl;
	if(!index_adapter.InitFromFile(index_file_name,0,'\001',1000000)){
		std::cerr<<"Error, init index adapter faild."<<std::endl;
		return -1;
	}
	std::cout<<"end of  build IndexAdapter.time:"<<TimeFunction::now()<<std::endl;
	*/
	unsigned int num_feature=18395569;
	//num_feature=5; // DEBUG
	//data_path=std::string("debug_sample.txt");
	float bias=-1.80113;
	if(!GeneratePartition(partitions,num_partition))    return -1;
	ISGD *sgd=0;
	std::cout<<"algorithm supplied:"<<algo<<std::endl;
	if(algo==std::string("ftrl")){
		sgd=new FTRL();
		if(!sgd->InitFromStr(algo_para))	return -1;
	}
	if(sgd!=0){
		OnlineTraining(partitions,bias,num_feature,data_path,*sgd,predict_output,batch_para_file);
	}else{
		std::cerr<<"Error, not algo supplied."<<std::endl;
	}
	if(sgd!=0)	delete sgd;
}
