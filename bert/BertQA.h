/*
 * Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "NvInfer.h"
#include "common.h"
#include "logger.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <ctime>
#include <cuda_profiler_api.h>
#include <cuda_runtime_api.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <sys/stat.h>
#include <vector>

#include "bert.h"
#include "bertUtils.h"

#include "dataUtils.h"

#include "bertEncoder.h"
#include "embLayerNormPlugin.h"
#include "squad.h"
//#include "BertFactory.h"

using namespace nvinfer1;


namespace bert{
class BertQA: public Bert
{
public:
     BertQA();
	 BertQA(int numHeads, int Bmax, int S, bool runInFp16);
	 ~BertQA(); 
	 void init(string weightsPath);
	 void forward(Weights& inputIds, Weights& segmentIds, Weights& inputMasks, Dims& inputDims, std::vector<float>& output);
private:
	#if 0
	string dataDirs_;
	int numHeads_;
	int Bmax_;  //batchsize
	int S_;     //sentence len
	bool runInFp16_;
	string saveEngine_; 
	const HostTensorMap inCfg_;
	
	string outputName;
	#endif
	cudaStream_t stream_;
	BERTDriver* pBertDriver;
};
}

