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

#ifndef TRT_SQUAD_H
#define TRT_SQUAD_H

namespace bert
{
const int tmp_batch_size = 8;
const int tmp_sentence_len = 200;
const int tmp_emb_len = 768;


const std::string SQD_W = "squad_output_weights";
const std::string SQD_B = "squad_output_bias";


inline void squadDynamic(const std::string& prefix, const BertConfig& config, WeightMap& weightMap, INetworkDefinition* network,
    ITensor* inputTensor, std::map<std::string, ILayer*>& mapLayers)
{

    assert(inputTensor);
    assert(network);

    //const Dims idims = inputTensor->getDimensions();
    //assert(idims.nbDims == 5);

    //const int S = idims.d[1];
    const int hiddenSize = config.hiddenSize; //idims.d[2];

    const Weights W_out = weightMap.at(prefix + SQD_W);
    const Weights B_out = weightMap.at(prefix + SQD_B);

    IFullyConnectedLayer* logitsLayer = network->addFullyConnected(*inputTensor, 2, W_out, B_out);
    assert(logitsLayer);

	nvinfer1::Dims starts = inputTensor->getDimensions();
	assert(starts.nbDims == 5);
	starts.d[0] = 0;
	starts.d[1] = 0;
	starts.d[2] = 0;
	starts.d[3] = 0;
	starts.d[4] = 0;

	nvinfer1::Dims strides = starts;
	strides.d[0] = 1;
	strides.d[1] = 1;
	strides.d[2] = 2;
	strides.d[3] = 1;
	strides.d[4] = 1;

	nvinfer1::Dims sizes = starts;
	sizes.d[0] = tmp_batch_size;
	sizes.d[1] = tmp_sentence_len;
	sizes.d[2] = 1;
	sizes.d[3] = 1;
	sizes.d[4] = 1;
	
	nvinfer1::ISliceLayer* slice1 = network->addSlice(*logitsLayer->getOutput(0), (starts), (sizes), (strides));

	starts.d[2] = 1;
	nvinfer1::ISliceLayer* slice2 = network->addSlice(*logitsLayer->getOutput(0), (starts), (sizes), (strides));



    setOutputName(slice1, prefix, "start_logits");
	setOutputName(slice2, prefix, "end_logits");


    mapLayers[prefix+ "start_logits"] = slice1;
	mapLayers[prefix+ "end_logits"] = slice2;
    return ;
}

inline void squadProb(const std::string& prefix,  INetworkDefinition* network, ITensor* inputTensor, std::map<std::string, ILayer*>& mapLayers)
{

    assert(inputTensor);
    assert(network);

	 
	//add softmax
    ISoftMaxLayer* sftMaxlayer = network->addSoftMax(*inputTensor);
    assert(sftMaxlayer);
	sftMaxlayer->setAxes(2);
    setOutputName(sftMaxlayer, prefix, "prob");
	mapLayers[prefix+ "prob"] = sftMaxlayer;

    return ;


	
}


inline ILayer* predeictProb(const std::string& prefix, const BertConfig& config, WeightMap& weightMap, INetworkDefinition* network,
    ITensor* inputTensor)
{

    assert(inputTensor);
    assert(network);

    //0. only choose top query word of per sentence.  example: (8,200,768)->(8, 768)

	nvinfer1::Dims starts = inputTensor->getDimensions();
	assert(starts.nbDims == 5);
	starts.d[0] = 0;
	starts.d[1] = 0;
	starts.d[2] = 0;
	starts.d[3] = 0;
	starts.d[4] = 0;

	nvinfer1::Dims strides = starts;
	strides.d[0] = 1;
	strides.d[1] = tmp_sentence_len;
	strides.d[2] = 1;
	strides.d[3] = 1;
	strides.d[4] = 1;

	nvinfer1::Dims sizes = starts;
	sizes.d[0] = tmp_batch_size;
	sizes.d[1] = 1;
	sizes.d[2] = tmp_emb_len;
	sizes.d[3] = 1;
	sizes.d[4] = 1;
	nvinfer1::ISliceLayer* slice1 = network->addSlice(*inputTensor, (starts), (sizes), (strides));


	
    //1.first fc
    const Weights W_out = weightMap.at("dense_kernel");
    const Weights B_out = weightMap.at("dense_bias");

    IFullyConnectedLayer* layer1 = network->addFullyConnected(*slice1->getOutput(0), 768, W_out, B_out);
    assert(layer1);

    //setOutputName(layer1, prefix, "squad_logits");


    //2.tanh sigmoid
    IActivationLayer* layer2 = network->addActivation(*layer1->getOutput(0),nvinfer1::ActivationType::kTANH );
    
    //3.second fc
    const Weights W_out2 = weightMap.at("dense_1_kernel");
    const Weights B_out2 = weightMap.at("dense_1_bias");


	IFullyConnectedLayer* layer3 = network->addFullyConnected(*layer2->getOutput(0), 3, W_out2, B_out2);
    assert(layer3);

    //4. add softmax
    ISoftMaxLayer* layer4 = network->addSoftMax(*layer3->getOutput(0));
	
	assert(layer4);
    setOutputName(layer4, prefix, "predict_prob");

	
	

    return layer4;
}


}
#endif // TRT_SQUAD_H
