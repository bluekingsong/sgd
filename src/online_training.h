#ifndef SGD_SRC_ONLINE_TRAINING_H_
#define SGD_SRC_ONLINE_TRAINING_H_
#include "isgd.h"
#include "cpp-common/cpp_common.h"
using namespace CppCommonFunction;

void DetermineFilename(unsigned int partition_size,const std::string& data_path,unsigned int hour,std::string& input,std::string& output,std::string& id_output);

float PredictProb(const float* w,const std::vector<unsigned int>& instance);

void OnlineTraining(const std::vector<std::vector<unsigned int> >& partitions,const IndexAdapter& index_adapter,const std::string& data_path,ISGD& sgd);

bool GeneratePartition(std::vector<std::vector<unsigned int> >& partitions, unsigned int num);

#endif
