#!/bin/bash

input=part-00199
output_prefix=/data/ad-impression/2014-06-25/hours/
awk -F'\001' -v prefix="${output_prefix}" '{
	hour=-1;
	for(i=3;i<=NF;++i){
		if("67|"==substr($i,1,3)){
			hour=substr($i,4,2);
			break;
		}
	}
	print $0 > prefix""hour;
	if((FNR-1)%5000000==0) print "progress:"FNR" time:"strftime();
}' - ###${input}
