#include "NvInfer.h"
#include "NvOnnxParser.h"
#if 0
#include <cuda_profiler_api.h>
#include <cuda_runtime_api.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>

#include <sys/stat.h>
#endif
#include <sstream>

#include <vector>

using namespace std;
using namespace nvinfer1;

namespace bert{


class OnnxParser
{
public:
     OnnxParser(){};
	 //OnnxParser(std::string onnxFilePath);
	 ~OnnxParser(){}; 
	 //virtual void init(string weightsPath){};

     int getEngine(const std::string& modelFile, // name of the onnx model
                    IBuilderConfig* config,    // batch size - NB must be at least as large as the batch we want to run with
                    ICudaEngine** pEngine)
	 {
		 
		IBuilder* builder = createInferBuilder(gLogger.getTRTLogger());
		assert(builder != nullptr);
		nvinfer1::INetworkDefinition* network = builder->createNetwork();

		auto parser = nvonnxparser::createParser(*network, gLogger.getTRTLogger());

		//Optional - uncomment below lines to view network layer information
		//config->setPrintLayerInfo(true);
		//parser->reportParsingInfo();

		if ( !parser->parseFromFile( modelFile.c_str(), static_cast<int>(gLogger.getReportableSeverity()) ) )
		{
			gLogError << "Failure while parsing ONNX file" << std::endl;
			return -1;
		}
#if 0
		// Build the engine
		builder->setMaxBatchSize(maxBatchSize);
		builder->setMaxWorkspaceSize(1 << 25);
		builder->setFp16Mode(gArgs.runInFp16);
		builder->setInt8Mode(gArgs.runInInt8);

		if (gArgs.runInInt8)
		{
		samplesCommon::setAllTensorScales(network, 127.0f, 127.0f);
		}
#endif
		//samplesCommon::enableDLA(builder, gArgs.useDLACore);

		//*pEngine = builder->buildCudaEngine(*network);
		*pEngine = builder->buildEngineWithConfig(*network, *config);
		assert(pEngine);

		// we can destroy the parser
		parser->destroy();

		// serialize the engine, then close everything down
		//engine->destroy();
		network->destroy();
		builder->destroy();

		return 0;
		 
	 }
	
};


}
	

