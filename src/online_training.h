#ifndef SGD_SRC_ONLINE_TRAINING_H_
#define SGD_SRC_ONLINE_TRAINING_H_
#include "isgd.h"
#include "cpp-common/cpp_common.h"
using namespace CppCommonFunction;

bool GeneratePartition(std::vector<std::vector<unsigned int> >& partitions, unsigned int num);

void OnlineTraining(const std::vector<std::vector<unsigned int> >& partitions,
					float bias,unsigned int num_feature,const std::string& data_path,
					ISGD& sgd,const std::string& predict_output,const std::string& batch_para_file);

#endif
