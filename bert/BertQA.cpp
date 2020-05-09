#include "BertFactory.h"
#include "BertQA.h"

using namespace nvinfer1;


namespace bert{



void BertQA::init(string weightsPath)
{
    //1. load weight
    cudaSetDevice(getDeviceId());
    cudaStreamCreate(&stream_);
    
    WeightMap weightMap;
    //const std::string weightsPath(locateFile(kBERT_WEIGHTS_FNAME, dataDirs_));
    loadWeights(weightsPath, weightMap);

    //2. Prepare the TRT Network
    //2.1 Create optimization profiles. In this case, we only create a single profile for the shape we care about.
    //sds: the input is fixed , 20200318.
    const auto profile = std::make_tuple(Dims{2, getBMax(), getS()}, Dims{2, getBMax(), getS()}, Dims{2, getBMax(), getS()});
    OptProfileMap optProfileMap = {std::make_pair(kMODEL_INPUT0_NAME, profile),
        std::make_pair(kMODEL_INPUT1_NAME, profile), std::make_pair(kMODEL_INPUT2_NAME, profile)};
    OptProfiles optProfiles = {optProfileMap};

    //2.2 create driver
    pBertDriver= new BERTDriver(getNumHeads(), getRunInFp16(), 5000_MiB, optProfiles);


    //2.3 init weight
    HostTensorMap params;
    for (auto& kv : weightMap)
    {
        std::vector<size_t> shape{static_cast<size_t>(kv.second.count)};
        params[kv.first] = make_shared<HostTensor>(const_cast<void*>(kv.second.values), kv.second.type, shape);
    }

    //2.4 Build the TRT Engine
    pBertDriver->init(params);

    //3. if serial engine
    //if (!gArgs.saveEngine.empty())
    //{
      //  bertDriver.serializeEngine(gArgs.saveEngine);
    //}



    // 4.set output layer
    // sds: update for hz .
    //outputName_= "prediction_module_cls_squad_logits";//const std::string outputName(" ");
//    setOutputName("start_prob");
//    setOutputName("embeddings_output");
    //setOutputName("prediction_module_cls_squad_logits");
    addOutputName("cls_start_logits");
    addOutputName("cls_end_logits");
    addOutputName("start_prob");
    addOutputName("end_prob");
    addOutputName("predict_prob");
     cudaError_t cudaerr = cudaDeviceSynchronize();
    if (cudaerr != cudaSuccess)
        printf("kernel launch failed with error \"%s\".\n",cudaGetErrorString(cudaerr));  

}
void BertQA::initByOnnx(string modelFile)
{
    //1. load weight
    cudaSetDevice(getDeviceId());
    cudaStreamCreate(&stream_);
    

    //2. Prepare the TRT Network
    //2.1 Create optimization profiles. In this case, we only create a single profile for the shape we care about.
    //sds: the input is fixed , 20200318.
    const auto profile = std::make_tuple(Dims{2, getBMax(), getS()}, Dims{2, getBMax(), getS()}, Dims{2, getBMax(), getS()});
    OptProfileMap optProfileMap = {std::make_pair(kMODEL_INPUT0_NAME, profile),
        std::make_pair(kMODEL_INPUT1_NAME, profile), std::make_pair(kMODEL_INPUT2_NAME, profile)};
    OptProfiles optProfiles = {optProfileMap};

    //2.2 create driver
    pBertDriver= new BERTDriver(getNumHeads(), getRunInFp16(), 5000_MiB, optProfiles);

    //2.3 Build the TRT Engine
    pBertDriver->initByOnnx(modelFile);


    // 4.set output layer
    #if 0
    addOutputName("cls_start_logits");
    addOutputName("cls_end_logits");
    addOutputName("start_prob");
    addOutputName("end_prob");
    addOutputName("predict_prob");
    #else

    #endif
    
    cudaError_t cudaerr = cudaDeviceSynchronize();
    if (cudaerr != cudaSuccess)
    printf("kernel launch failed with error \"%s\".\n",cudaGetErrorString(cudaerr));  

}





void BertQA::forward(Weights& inputIds, Weights& segmentIds, Weights& inputMasks, Dims& inputDims, std::vector<float>& output)
{
    cudaSetDevice(getDeviceId());

    std::vector<float> timesTotal(1);   // Total time
    std::vector<float> timesCompute(1); // Computation time



    const std::vector<size_t> inputShape(inputDims.d, inputDims.d + 2);
    const HostTensorMap inCfg{
    std::make_pair(kMODEL_INPUT0_NAME,
    make_shared<HostTensor>(const_cast<void*>(inputIds.values), inputIds.type, inputShape)),
    std::make_pair(kMODEL_INPUT1_NAME,
    make_shared<HostTensor>(const_cast<void*>(segmentIds.values), segmentIds.type, inputShape)),
    std::make_pair(kMODEL_INPUT2_NAME,
    make_shared<HostTensor>(const_cast<void*>(inputMasks.values), inputMasks.type, inputShape))};

    const int B = inputDims.d[0];
    const int S = inputDims.d[1];



    HostTensorMap outCfg
    = {make_pair(getOutputName(0), make_shared<HostTensor>(output.data(), DataType::kFLOAT, std::vector<size_t>{2, static_cast<size_t>(B), static_cast<size_t>(S)}))};


    
    pBertDriver->benchmark(inCfg, outCfg, B, stream_, timesTotal, timesCompute);
    //pBertDriver->benchmark(inCfg, outCfg, B, stream_, timesTotal, timesCompute, true);


    //transposeLogits(output, B, S);
    return ;
}

void BertQA::forward2(Weights& inputIds, Weights& segmentIds, Weights& inputMasks, Dims& inputDims, 
    std::vector<float>& output, std::vector<float>& output2, std::vector<float>& output3, std::vector<float>& output4, std::vector<float>& output5)
{
    cudaSetDevice(getDeviceId());

    std::vector<float> timesTotal(1);   // Total time
    std::vector<float> timesCompute(1); // Computation time



    const std::vector<size_t> inputShape(inputDims.d, inputDims.d + 2);
    const HostTensorMap inCfg{
    std::make_pair(kMODEL_INPUT0_NAME,
    make_shared<HostTensor>(const_cast<void*>(inputIds.values), inputIds.type, inputShape)),
    std::make_pair(kMODEL_INPUT1_NAME,
    make_shared<HostTensor>(const_cast<void*>(segmentIds.values), segmentIds.type, inputShape)),
    std::make_pair(kMODEL_INPUT2_NAME,
    make_shared<HostTensor>(const_cast<void*>(inputMasks.values), inputMasks.type, inputShape))};

    const int B = inputDims.d[0];
    const int S = inputDims.d[1];



    HostTensorMap outCfg
    = {make_pair(getOutputName(0), make_shared<HostTensor>(output.data(), DataType::kFLOAT, std::vector<size_t>{1, static_cast<size_t>(B), static_cast<size_t>(S)})),
    make_pair(getOutputName(1), make_shared<HostTensor>(output2.data(), DataType::kFLOAT, std::vector<size_t>{1, static_cast<size_t>(B), static_cast<size_t>(S)})),
    make_pair(getOutputName(2), make_shared<HostTensor>(output3.data(), DataType::kFLOAT, std::vector<size_t>{1, static_cast<size_t>(B), static_cast<size_t>(S)})),
    make_pair(getOutputName(3), make_shared<HostTensor>(output4.data(), DataType::kFLOAT, std::vector<size_t>{1, static_cast<size_t>(B), static_cast<size_t>(S)})),
    make_pair(getOutputName(4), make_shared<HostTensor>(output5.data(), DataType::kFLOAT, std::vector<size_t>{1, static_cast<size_t>(B), 3}))};


    
    //pBertDriver->benchmark(inCfg, outCfg, B, stream_, timesTotal, timesCompute, false);
    pBertDriver->benchmark(inCfg, outCfg, B, stream_, timesTotal, timesCompute, true);


    //transposeLogits(output, B, S);
    return ;
}

BertQA::BertQA(int numHeads, int Bmax, int S, bool runInFp16):Bert(numHeads, Bmax, S, runInFp16)
{

//    cudaStreamCreate(&stream_);

}
BertQA::BertQA()
{

//cudaStreamCreate(&stream_);

}

BertQA::~BertQA()
{
    cudaStreamDestroy(stream_);
    delete(pBertDriver);

}




}
