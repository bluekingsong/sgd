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

void DetermineFilename(unsigned int partition_size,const std::string& data_path,
					   unsigned int hour,std::string& input,std::string& output,std::string& id_output){
	std::stringstream strstream;
	strstream<<data_path<<hour<<".id";
	input=strstream.str();
	//strstream<<".predict";
	//output=strstream.str();
	//strstream.str("");
	//strstream<<input<<".id";
	//id_output=strstream.str();
}
double PredictProb(const float* w,const std::vector<unsigned int>& instance,float bias){
	double v=bias;
	for(size_t i=0;i<instance.size();++i)   v+=w[instance[i]];
	return 1.0/(1.0+std::exp(-v));
}
int ParseFeature(const std::string& line,std::vector<unsigned int>& instance,
				const IndexAdapter& index_adapter,unsigned int bias_index){
	instance.clear();
	instance.push_back(bias_index);
	std::vector<std::string> line_vec;
	unsigned int n=StringFunction::split(line,'\001',line_vec);
	for(unsigned int i=2;i<n;++i){
		unsigned int index;
		if(index_adapter.GetIndex(line_vec[i],index))    instance.push_back(index);
	}
	return std::atoi(line_vec[1].c_str());
}
int ParseFeature(const std::string& line,std::vector<unsigned int>& instance,
				 unsigned int num_feature){
	instance.clear();
	std::vector<std::string> line_vec;
	unsigned int n=StringFunction::split(line,'\t',line_vec);
	for(unsigned int i=1;i<n;++i){
		unsigned int index=std::atoi(line_vec[i].c_str());
		//if(index_adapter.GetIndex(line_vec[i],index))    instance.push_back(index);
		if(index<num_feature)    instance.push_back(index);
	}
	return std::atoi(line_vec[0].c_str());
}
bool InitParaFromFile(float *new_w,float *w,const std::string& filename,unsigned int num_feature){
	ifstream fin(filename.c_str());
	if(!fin.is_open()){
		std::cerr<<"Error, open file "<<filename<<" faild."<<std::endl;
		return false;
	}
	std::string line;
	std::vector<std::string> line_vec;
	unsigned int cnt=0;
	while((std::getline(fin,line)) && cnt<num_feature){
		StringFunction::split(line,'\003',line_vec);
		new_w[cnt]=w[cnt]=std::atof(line_vec[1].c_str());
		++cnt;
	}
	return true;
}
//void OnlineTraining(const std::vector<std::vector<unsigned int> >& partitions,const IndexAdapter& index_adapter,const std::string& data_path,ISGD& sgd,const std::string& predict_output){
void OnlineTraining(const std::vector<std::vector<unsigned int> >& partitions,
					float bias,unsigned int num_feature,const std::string& data_path,
					ISGD& sgd,const std::string& predict_output,
					const std::string& batch_para_file){
	//unsigned int num_feature=index_adapter.GetSize();
	std::cout<<"the number of features:"<<num_feature<<std::endl;
	unsigned int mem_size=5*num_feature;
	float *mem=new float[mem_size];
	float *w=mem;  // LR parameter
	float *z=w+num_feature;  // parameter in FTRL
	float *n=z+num_feature;  // parameter in FTRL
	float *new_w=n+num_feature; // updated paramter by FTRL
	float *w2=new_w+num_feature; // Last Round Updated paramter by FTRL
	if(!InitParaFromFile(new_w,w,batch_para_file,num_feature))    return;
	std::vector<unsigned int> instance;
	ofstream fout(predict_output.c_str());
	unsigned int line_cnt=0;
	int zero_cnt=0;
	double logloss=0.0;
	double new_logloss=0.0;
	std::cout<<"start training model. time:"<<TimeFunction::now()<<std::endl;
	for(unsigned int P=0;P<partitions.size();++P){
		//std::memset(z,0,sizeof(float)*num_feature);
		//std::memset(n,0,sizeof(float)*num_feature);
		 // start of a round training
		std::memcpy(w2,new_w,sizeof(float)*num_feature);
		// training agian
		int iter=1;
		while(iter--){
		for(unsigned int Q=0;Q<partitions[P].size();++Q){
			unsigned int hour=partitions[P][Q];
			std::string input,output;  //stream_output;
			std::string id_output;
			DetermineFilename(partitions.size(),data_path,hour,input,output,id_output);
			ifstream fin(input.c_str());
			//ofstream fout(output.c_str());
			//ofstream idfout(id_output.c_str());
			if(!fin.is_open()){
				std::cerr<<"open file("<<input<<") faild."<<std::endl;
				return;
			}
			//fin.close();
			//fin.open("debug_sample.txt");
			std::string line;
			while(std::getline(fin,line)){
				//int y=ParseFeature(line,instance,index_adapter,bias_index);
				int y=ParseFeature(line,instance,num_feature);
/*
				std::cout<<"DEBUG: ";
				for(unsigned int j=0;j<5;++j)    std::cout<<"new["<<j<<"]="<<new_w[j]<<" w["<<j<<"]="<<w[j]<<"---";
				std::cout<<std::endl;
*/
				double q=PredictProb(w,instance,bias);
				double p2=PredictProb(w2,instance,bias);
				double p=PredictProb(new_w,instance,bias);
				//TODO: fix p, it seems ftrl predicted probability is too aggressive
				//if(p2<1e-12)    p2=1e-12;
				//else if(p2>1-1e-12)    p2=1-1e-12;
				sgd.Update(new_w,w,z,n,p,y,instance,zero_cnt);
				if(0==iter){
				fout<<y<<"\t"<<q<<"\t"<<p2<<"\t"<<p<<std::endl;
				++line_cnt;
				if(y)    logloss-=std::log(q),  new_logloss-=std::log(p2);
				else    logloss-=std::log(1-q), new_logloss-=std::log(1-p2);
				if(line_cnt%1000000==0)    std::cout<<"Hint: progress "<<line_cnt<<" average logloss="<<logloss/line_cnt<<
										" new_logloss="<<new_logloss/line_cnt<<" gap="<<(new_logloss-logloss)/line_cnt<<
										" zero_rate="<<float(zero_cnt)/num_feature<<" time:"<<TimeFunction::now()<<std::endl<<std::flush;
				//logloss-=y?std::log(q):std::log(1-q);
				//new_logloss-=y?std::log(p):std::log(1-p);
				}
			}
			fin.close();
			std::cout<<"Hint: file "<<input<<" processed. time:"<<TimeFunction::now()<<std::endl;
		}
		}
		std::cout<<"Hint: Partion "<<P<<" processed."<<std::endl;
	}
	delete []mem;
}
bool GeneratePartition(std::vector<std::vector<unsigned int> >& partitions, unsigned int num){
	if(num>24){
		std::cerr<<"partition num given is too large:"<<num<<std::endl;
		return false;
	}
	unsigned int gap=24/num;
	partitions.clear();
	unsigned int hour=0;
	for(unsigned int i=0;i<num;++i){
		partitions.push_back(std::vector<unsigned int>());
		for(unsigned int j=0;j<gap;++j)    partitions.back().push_back(hour++);
		if(hour>23)	break;
	}
	return true;
}
