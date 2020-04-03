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
#include <unistd.h>
#include <thread>
#include <sstream>
#include <sys/stat.h>
#include <vector>

#include "bert.h"
#include "bertUtils.h"

#include "dataUtils.h"

#include "bertEncoder.h"
#include "embLayerNormPlugin.h"
#include "squad.h"
#include "BertFactory.h"
using namespace bert;
using namespace nvinfer1;

Args gArgs;
#if 1
constexpr const char* gSampleName = "TensorRT.sample_bert";
constexpr const char* kTEST_INPUT_FNAME = "test_inputs.weights_int32";
constexpr const char* kTEST_OUTPUT_FNAME = "test_outputs.weights";
constexpr const char* kBERT_WEIGHTS_FNAME = "bert.weights";
constexpr int kNUM_RUNS = 10;
#endif
//!
//! \brief This function prints the help information for running this sample
//!
void printHelpInfo()
{
    std::cout << "Usage: ./sample_bert --nheads=<number of heads> [-h or --help] [-d or --datadir=<path to data directory>] [--fp16] [--saveEngine=<path>]\n";
    std::cout << "--help          Display help information\n";
    std::cout << "--datadir       Specify path to a data directory, overriding the default. This option can be used "
            "multiple times to add multiple directories. The given path(s) must contain the weights and test "
            "inputs/outputs.\n";
    std::cout << "--nheads        Number of attention heads.\n";
    std::cout << "--fp16          Run in FP16 mode.\n";
    std::cout << "--saveEngine    The path at which to write a serialized engine." << endl;
}

#if 0
int main(int argc, char* argv[])
{
    const bool argsOK = parseArgs(gArgs, argc, argv);
    if (gArgs.help)
    {
        printHelpInfo();
        return EXIT_SUCCESS;
    }
    if (!argsOK)
    {
        gLogError << "Invalid arguments" << endl;
        printHelpInfo();
        return EXIT_FAILURE;
    }
    if (gArgs.dataDirs.empty())
    {
        gLogError << "No datadirs given" << endl;
        printHelpInfo();
        return EXIT_FAILURE;
    }
    if (gArgs.numHeads <= 0)
    {
        gLogError << "invalid number of heads" << endl;
        printHelpInfo();
        return EXIT_FAILURE;
    }

    gLogger.setReportableSeverity(Logger::Severity::kINFO);
    auto sampleTest = gLogger.defineTest(gSampleName, argc, const_cast<const char**>(argv));
    gLogger.reportTestStart(sampleTest);

    // Load weights and golden files
    const std::string outputPath(locateFile(kTEST_OUTPUT_FNAME, gArgs.dataDirs));
    WeightMap testOutputs;
    //loadWeights(outputPath, testOutputs);

    vector<Weights> inputIds;
    vector<Weights> inputMasks;
    vector<Weights> segmentIds;
    vector<Dims> inputDims;
    const std::string inputPath(locateFile(kTEST_INPUT_FNAME, gArgs.dataDirs));

    int S = 0;
    int Bmax = 0;
    loadInputs(inputPath, Bmax, S, inputIds, inputMasks, segmentIds, inputDims);
    assert(inputIds.size() > 0 && "No inputs found in supplied golden file");

    // Create optimization profiles. In this case, we only create a single profile for the shape we care about.
    const int numHeads = gArgs.numHeads;

    const auto profile = std::make_tuple(Dims{2, Bmax, S}, Dims{2, Bmax, S}, Dims{2, Bmax, S});
    OptProfileMap optProfileMap = {std::make_pair(kMODEL_INPUT0_NAME, profile),
        std::make_pair(kMODEL_INPUT1_NAME, profile), std::make_pair(kMODEL_INPUT2_NAME, profile)};

    OptProfiles optProfiles = {optProfileMap};

    // Prepare the TRT Network
    BERTDriver bertDriver(numHeads, gArgs.runInFp16, 5000_MiB, optProfiles);

    const std::vector<size_t> inputShape(inputDims[0].d, inputDims[0].d + 2);
    const HostTensorMap inCfg{
        std::make_pair(kMODEL_INPUT0_NAME,
            make_shared<HostTensor>(const_cast<void*>(inputIds[0].values), inputIds[0].type, inputShape)),
        std::make_pair(kMODEL_INPUT1_NAME,
            make_shared<HostTensor>(const_cast<void*>(segmentIds[0].values), segmentIds[0].type, inputShape)),
        std::make_pair(kMODEL_INPUT2_NAME,
            make_shared<HostTensor>(const_cast<void*>(inputMasks[0].values), inputMasks[0].type, inputShape))};

    const int B = inputDims[0].d[0];

    WeightMap weightMap;

    const std::string weightsPath(locateFile(kBERT_WEIGHTS_FNAME, gArgs.dataDirs));
    loadWeights(weightsPath, weightMap);

    HostTensorMap params;
    for (auto& kv : weightMap)
    {
        std::vector<size_t> shape{static_cast<size_t>(kv.second.count)};
        params[kv.first] = make_shared<HostTensor>(const_cast<void*>(kv.second.values), kv.second.type, shape);
    }

    // Build the TRT Engine
    bertDriver.init(params);

    if (!gArgs.saveEngine.empty())
    {
        bertDriver.serializeEngine(gArgs.saveEngine);
    }

    // Benchmark inference
    // sds: update for hz
    const std::string outputName("prediction_module_cls_squad_logits");
    //const std::string outputName("cls_squad_logits");
    std::vector<float> output(2 * B * S);
    HostTensorMap outCfg
        = {make_pair(outputName, make_shared<HostTensor>(output.data(), DataType::kFLOAT, std::vector<size_t>{2, static_cast<size_t>(B), static_cast<size_t>(S)}))};

    cudaStream_t stream;
    cudaStreamCreate(&stream);
    std::vector<float> timesTotal(kNUM_RUNS);   // Total time
    std::vector<float> timesCompute(kNUM_RUNS); // Computation time

    bertDriver.benchmark(inCfg, outCfg, B, stream, timesTotal, timesCompute);

    cudaStreamDestroy(stream);
    transposeLogits(output, B, S);

#if 0
    const float* test = reinterpret_cast<const float*>(testOutputs["logits"].values);

    // Analyze benchmark results
    float meanAbsErr = 0;
    float maxdiff = 0;
    for (int it = 0; it < testOutputs["logits"].count; it++)
    {
        const float diff = abs(test[it] - output[it]);
        meanAbsErr += diff;
        maxdiff = max(diff, maxdiff);
    }
    meanAbsErr /= output.size();

    const float avgTotal = accumulate(timesTotal.begin(), timesTotal.end(), 0.f, plus<float>()) / timesTotal.size();
    const float avgCompute
        = accumulate(timesCompute.begin(), timesCompute.end(), 0.f, plus<float>()) / timesCompute.size();


    printf("B=%d S=%d MAE=%.12e MaxDiff=%.12e ", B, S, meanAbsErr, maxdiff);
    printf(" Runtime(total avg)=%.6fms Runtime(comp ms)=%.6f\n", avgTotal, avgCompute);

    bool pass{false};
    if (gArgs.runInFp16)
    {
        pass = meanAbsErr < 2e-2;
    }
    else
    {
        pass = meanAbsErr < 1e-5;
    }
    return gLogger.reportTest(sampleTest, pass);
#endif
    return gLogger.reportTest(sampleTest, false);
}


#endif
void thread_function(int deviceId) {
    //if(deviceId ==2) sleep(3600);
    gLogger.setReportableSeverity(Logger::Severity::kINFO);
    cudaSetDevice(deviceId);
    // Load weights and golden files
    //const std::string outputPath(locateFile(kTEST_OUTPUT_FNAME, gArgs.dataDirs));
    WeightMap testOutputs;
    //loadWeights(outputPath, testOutputs);

    vector<Weights> inputIds;
    vector<Weights> inputMasks;
    vector<Weights> segmentIds;
    vector<Dims> inputDims;
    const std::string inputPath(locateFile(kTEST_INPUT_FNAME, gArgs.dataDirs));

    int S = 0;
    int Bmax = 0;
    loadInputs(inputPath, Bmax, S, inputIds, inputMasks, segmentIds, inputDims);
    assert(inputIds.size() > 0 && "No inputs found in supplied golden file");

    // Create optimization profiles. In this case, we only create a single profile for the shape we care about.
    const int numHeads = gArgs.numHeads;

    const auto profile = std::make_tuple(Dims{2, Bmax, S}, Dims{2, Bmax, S}, Dims{2, Bmax, S});
    OptProfileMap optProfileMap = {std::make_pair(kMODEL_INPUT0_NAME, profile),
        std::make_pair(kMODEL_INPUT1_NAME, profile), std::make_pair(kMODEL_INPUT2_NAME, profile)};

    OptProfiles optProfiles = {optProfileMap};

    const std::vector<size_t> inputShape(inputDims[0].d, inputDims[0].d + 2);
    #if 0
    const HostTensorMap inCfg{
        std::make_pair(kMODEL_INPUT0_NAME,
            make_shared<HostTensor>(const_cast<void*>(inputIds[0].values), inputIds[0].type, inputShape)),
        std::make_pair(kMODEL_INPUT1_NAME,
            make_shared<HostTensor>(const_cast<void*>(segmentIds[0].values), segmentIds[0].type, inputShape)),
        std::make_pair(kMODEL_INPUT2_NAME,
            make_shared<HostTensor>(const_cast<void*>(inputMasks[0].values), inputMasks[0].type, inputShape))};
    #endif
    const int B = inputDims[0].d[0];

    WeightMap weightMap;

    const std::string weightsPath(locateFile("bert.weights", gArgs.dataDirs));
 for(int i =0; i < inputIds[0].count; i++)
    {
       if((((int*)inputIds[0].values)[i])  > 21120)
         (((int*)inputIds[0].values)[i]) = 20000;
    }

   
    std::vector<float> output(2 * B * S);

    Bert* pBert2 = createBert("QA");
    pBert2->setParam(numHeads,Bmax,S,gArgs.runInFp16);
    pBert2->setDeviceId(deviceId);
    pBert2->init(weightsPath);
    pBert2->forward(inputIds[0], segmentIds[0], inputMasks[0], inputDims[0], output);
    sleep(3600);
}
void createThreadBertIns(int deviceId)
{
	
  std::thread threadObj1(thread_function, deviceId);
  //threadObj1.join();  //main thread wait this son
  threadObj1.detach();
}


int main(int argc, char* argv[])
{
    const bool argsOK = parseArgs(gArgs, argc, argv);
    if (gArgs.help)
    {
        printHelpInfo();
        return EXIT_SUCCESS;
    }
    if (!argsOK)
    {
        gLogError << "Invalid arguments" << endl;
        printHelpInfo();
        return EXIT_FAILURE;
    }
    if (gArgs.dataDirs.empty())
    {
        gLogError << "No datadirs given" << endl;
        printHelpInfo();
        return EXIT_FAILURE;
    }
    if (gArgs.numHeads <= 0)
    {
        gLogError << "invalid number of heads" << endl;
        printHelpInfo();
        return EXIT_FAILURE;
    }
    //cudaSetDevice(2);
    //gLogger.setReportableSeverity(Logger::Severity::kINFO);
    //auto sampleTest = gLogger.defineTest(gSampleName, argc, const_cast<const char**>(argv));
    //gLogger.reportTestStart(sampleTest);
    createThreadBertIns(2);
    createThreadBertIns(3);
#if 0
    // Load weights and golden files
    //const std::string outputPath(locateFile(kTEST_OUTPUT_FNAME, gArgs.dataDirs));
    WeightMap testOutputs;
    //loadWeights(outputPath, testOutputs);

    vector<Weights> inputIds;
    vector<Weights> inputMasks;
    vector<Weights> segmentIds;
    vector<Dims> inputDims;
    const std::string inputPath(locateFile(kTEST_INPUT_FNAME, gArgs.dataDirs));

    int S = 0;
    int Bmax = 0;
    loadInputs(inputPath, Bmax, S, inputIds, inputMasks, segmentIds, inputDims);
    assert(inputIds.size() > 0 && "No inputs found in supplied golden file");

    // Create optimization profiles. In this case, we only create a single profile for the shape we care about.
    const int numHeads = gArgs.numHeads;

    const auto profile = std::make_tuple(Dims{2, Bmax, S}, Dims{2, Bmax, S}, Dims{2, Bmax, S});
    OptProfileMap optProfileMap = {std::make_pair(kMODEL_INPUT0_NAME, profile),
        std::make_pair(kMODEL_INPUT1_NAME, profile), std::make_pair(kMODEL_INPUT2_NAME, profile)};

    OptProfiles optProfiles = {optProfileMap};

    const std::vector<size_t> inputShape(inputDims[0].d, inputDims[0].d + 2);
    #if 0
    const HostTensorMap inCfg{
        std::make_pair(kMODEL_INPUT0_NAME,
            make_shared<HostTensor>(const_cast<void*>(inputIds[0].values), inputIds[0].type, inputShape)),
        std::make_pair(kMODEL_INPUT1_NAME,
            make_shared<HostTensor>(const_cast<void*>(segmentIds[0].values), segmentIds[0].type, inputShape)),
        std::make_pair(kMODEL_INPUT2_NAME,
            make_shared<HostTensor>(const_cast<void*>(inputMasks[0].values), inputMasks[0].type, inputShape))};
    #endif
    const int B = inputDims[0].d[0];

    WeightMap weightMap;

    const std::string weightsPath(locateFile("bert.weights", gArgs.dataDirs));

   
    std::vector<float> output(2 * B * S);

    Bert* pBert = createBert("QA");
    pBert->setParam(numHeads,Bmax,S,gArgs.runInFp16);
    pBert->setDeviceId(2);
    pBert->init(weightsPath);
    Bert* pBert2 = createBert("QA");
    pBert2->setParam(numHeads,Bmax,S,gArgs.runInFp16);
    pBert2->setDeviceId(3);
    pBert2->init(weightsPath);
    pBert2->forward(inputIds[0], segmentIds[0], inputMasks[0], inputDims[0], output);
 #endif 
sleep(3600); 
    //return gLogger.reportTest(sampleTest, false);
    return 0;
}
