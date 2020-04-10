/*
  Copyright (c) 2019 Sogou, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

	  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  Author: Xie Han (xiehan@sogou-inc.com;63350856@qq.com)
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <thread>
#include <chrono>
#include <random>
#include <google/protobuf/text_format.h>
#include "workflow/HttpMessage.h"
#include "workflow/HttpUtil.h"
#include "workflow/WFServer.h"
#include "workflow/WFHttpServer.h"
#include "compatible_server_req_res.pb.h"
#include "BertFactory.h"
#include "json.hpp"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"


using namespace nvinfer1;
using namespace bert;
using nlohmann::json;
using namespace rapidjson;
const int tmp_batch_size = 8;
const int tmp_sentence_len = 200;
const int tmp_emb_len = 768;


using namespace chrono;


void parseDims(std::ifstream& input, const std::string& name, Dims& d)
{
    const int n_dims = d.nbDims;
    for (int it = 0; it < n_dims; it++)
    {
        input >> d.d[it];
    }
    assert(input.peek() == ' ');
    input.get();
}
template <typename T>
void loadRow(std::ifstream& input, std::string& name, T*& data, int& nbWeights, Dims& d)
{
    int32_t type;
    int32_t nbDims;
    int32_t dim;
    input >> name >> std::dec >> type >> nbDims;
    assert(nbDims > 0 && nbDims <= Dims::MAX_DIMS);
    d.nbDims = nbDims;
    parseDims(input, name, d);

    const DataType dtype = static_cast<DataType>(type);

    nbWeights = std::accumulate(d.d, d.d + d.nbDims, 1, std::multiplies<int>());
    data = new T[nbWeights]; // should be freed by caller
    char* bytes = reinterpret_cast<char*>(data);
    input.read(bytes, nbWeights * sizeof(T));

    assert(input.peek() == '\n');
    input.get();
}


void loadInputs(const std::string& weightsPath, int& Bmax, int& S, std::vector<nvinfer1::Weights>& inputIds,
    std::vector<nvinfer1::Weights>& inputMasks, std::vector<nvinfer1::Weights>& segmentIds,
    std::vector<nvinfer1::Dims>& inputDims)
{
    std::ifstream input(weightsPath, std::ios_base::binary);
    int32_t count;
    input >> count;
    //gLogInfo << "Number of buffers: " << count << std::endl;
    assert(count % 3 == 0);
    S = 0;
    Bmax = 0;

    std::vector<nvinfer1::Dims> maskDims;
    std::vector<nvinfer1::Dims> segIdDims;
    for (int it = 0; it < count; it++)
    {
        int nbWeights = 0;
        std::string name;
        int* data = nullptr;
        Dims d;
        loadRow(input, name, data, nbWeights, d);
        assert(data);
        assert(nbWeights);

        Bmax = std::max(Bmax, d.d[0]); // find the largest batch size in the dataset

        if (S == 0)
        {
            S = d.d[1];
        }
        else
        {
            assert(S == d.d[1]);
        } // all inputs should have the same sequence length

        Weights tmp;
        tmp.type = DataType::kINT32;
        tmp.values = data;
        tmp.count = nbWeights;

        if (name.find("input_id") != std::string::npos)
        {
            inputIds.push_back(tmp);
            inputDims.push_back(d);
            continue;
        }

        if (name.find("input_mask") != std::string::npos)
        {
            inputMasks.push_back(tmp);
            maskDims.push_back(d);
            continue;
        }
        segmentIds.push_back(tmp);
        segIdDims.push_back(d);
    }
    input.close();
    assert(inputIds.size() == count / 3);
    assert(inputMasks.size() == inputIds.size());
    assert(segmentIds.size() == inputIds.size());
    assert(inputDims.size() == inputIds.size());
    assert(S);
    assert(Bmax);

    for (int it = 0; it < inputIds.size(); it++)
    {
        assert(inputIds[it].count == inputMasks[it].count);
        assert(inputIds[it].count == segmentIds[it].count);
    //    assert(inputDims[it] == maskDims[it]);
     //   assert(inputDims[it] == segIdDims[it]);
    }
}


#if 1

struct tutorial_series_context
{
	std::string url;
	WFHttpTask *proxy_task;
};

struct MMInput
{
    Weights inputIds;
    Weights inputMasks;
    Weights segmentIds;
    vector<int> data_ids;
    vector<int> data_masks;
    vector<int> data_segs;
    Dims inputDims;
    Bert* pBert;
};

	
struct MMOutput
{
    std::vector<float> output;
    std::vector<float> output2;
    std::vector<float> output3;
    
    std::vector<float> output4;
    std::vector<float> output5;
};

vector<Bert*> pBertVec;
pthread_mutex_t mutex_ = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_ = PTHREAD_COND_INITIALIZER;
Bert* createMyBert(int deviceId)
{
    string dataDir = "./data_hz";
    //string dataDir = "/home/odin/shendasai2/tensorrt6_/TensorRT/demo/BERT2/data_hz";
    const std::string weightsPath(dataDir+"/weight_path/bert.weights");
   
    const int numHeads = 12;
    int S = tmp_sentence_len;
    int Bmax = tmp_batch_size;

    Bert* pBert = createBert("QA");
    pBert->setParam(numHeads,Bmax,S,true);
    pBert->setDeviceId(deviceId);
    pBert->init(weightsPath);
    return pBert;
}

class MyBertIns
{
public:
    static Bert* get_instance(int deviceId)
{
    static thread_local Bert* pBert = createMyBert(deviceId);
    return pBert;
}
};

void thread_function(int deviceId) {
  Bert* pBert= MyBertIns::get_instance(deviceId);
  pthread_mutex_lock(&mutex_);
  pBertVec.push_back(pBert);
  
  pthread_cond_signal(&cond_);
  pthread_mutex_unlock(&mutex_);
  sleep(3600);
}
void createThreadBertIns(int deviceId)
{
	
  std::thread threadObj1(thread_function, deviceId);
  //threadObj1.join();  //main thread wait this son
  threadObj1.detach();
}

typedef WFThreadTask<MMInput, MMOutput> MMTask;


void http_callback(MMTask *task)
{
    auto start = system_clock::now();

    int state = task->get_state();
    SeriesWork *series = series_of(task);
    tutorial_series_context *context =
        (tutorial_series_context *)series->get_context();
    HttpResponse *proxy_resp = context->proxy_task->get_resp();


    HttpRequest *req = context->proxy_task->get_req();

    char buf[8192];
    int len;
    len = snprintf(buf, 8192, "<p>%s %s %s</p>", req->get_method(),
            req->get_request_uri(), req->get_http_version());
    proxy_resp->append_output_body(buf, len);


    MMOutput *pOutput = task->get_output();
    MMInput *pIn = task->get_input();
    assert(task->get_state() == WFT_STATE_SUCCESS);

    int batch_size = tmp_batch_size;
    int sentence_len = tmp_sentence_len;


     
#if 0
    
    json start_logits = json::array();
    json end_logits = json::array();
    json start_prob = json::array();
    json end_prob = json::array();
    json intent_prob = json::array();
    for(int i = 0; i < batch_size; i++)
    {

    std::vector<float>::const_iterator first1 = pOutput->output.begin() + i*sentence_len;   
    std::vector<float>::const_iterator last1  = pOutput->output.begin() + (i+1)*sentence_len; 
    std::vector<float> cut1_vector(first1, last1);
    start_logits[i] = json(cut1_vector);

    std::vector<float>::const_iterator first2 = pOutput->output2.begin() + i*sentence_len;   
    std::vector<float>::const_iterator last2  = pOutput->output2.begin() + (i+1)*sentence_len; 
    std::vector<float> cut2_vector(first2, last2);
    end_logits[i] = json(cut2_vector);

        std::vector<float>::const_iterator first3 = pOutput->output3.begin() + i*sentence_len;   
    std::vector<float>::const_iterator last3  = pOutput->output3.begin() + (i+1)*sentence_len; 
    std::vector<float> cut3_vector(first3, last3);
    start_prob[i] = json(cut3_vector);

        std::vector<float>::const_iterator first4 = pOutput->output4.begin() + i*sentence_len;   
    std::vector<float>::const_iterator last4   = pOutput->output4.begin() + (i+1)*sentence_len; 
    std::vector<float> cut4_vector(first4, last4);
    end_prob[i] = json(cut4_vector);

        std::vector<float>::const_iterator first5 = pOutput->output5.begin() + i*3;   
    std::vector<float>::const_iterator last5  = pOutput->output5.begin() + (i+1)*3; 
    std::vector<float> cut5_vector(first5, last5);
    intent_prob[i] = json(cut5_vector);

    }
    
    json jResult = json({});
    jResult["outputs"]["start_logits"] = start_logits;
    jResult["outputs"]["end_logits"] = end_logits;
    jResult["outputs"]["start_prob"] = start_prob;
    jResult["outputs"]["end_prob"] = end_prob;
    jResult["outputs"]["intent_prob"] = intent_prob;
    proxy_resp->append_output_body(jResult.dump(2).c_str(), jResult.dump(2).size());
 #else

    #define writer_json_prob(wirter, name, pOut, ld) \
        writer.Key(name); \
        writer.StartArray();\
        for(int i = 0; i < batch_size; i++)\
        {\
            writer.StartArray();\
            int offset_ = i*ld;\
            for(int j = 0; j < ld; j++)   \ 
            {\
                writer.Double(pOut[j + offset_]);\
            }\
            writer.EndArray(); \
        }  \
        writer.EndArray(); 
    
 
    rapidjson::StringBuffer strBuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
    writer.StartObject();
    writer.Key("outputs");  
    
    writer.StartObject();

    writer_json_prob(writer, "start_logits", pOutput->output, sentence_len);
    writer_json_prob(writer, "end_logits", pOutput->output2, sentence_len);
    writer_json_prob(writer, "start_prob", pOutput->output3, sentence_len);
    writer_json_prob(writer, "end_prob", pOutput->output4, sentence_len);
    writer_json_prob(writer, "intent_prob", pOutput->output5, 3);

    
    writer.EndObject();   
    writer.EndObject();
    proxy_resp->append_output_body(strBuf.GetString(),  strBuf.GetSize());


 #endif

    


    auto end   = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    printf(" reply time1 is %d ms. \n", (duration)/1000);

    #if 0
    json jOut(pOutput->output);
    proxy_resp->append_output_body(jOut.dump(2).c_str(), jOut.dump(2).size());
    json jOut2(pOutput->output2);
    proxy_resp->append_output_body(jOut2.dump(2).c_str(), jOut2.dump(2).size());
    json jOut3(pOutput->output3);
    proxy_resp->append_output_body(jOut3.dump(2).c_str(), jOut3.dump(2).size());

    json jOut4(pOutput->output4);
    proxy_resp->append_output_body(jOut4.dump(2).c_str(), jOut4.dump(2).size());
    json jOut5(pOutput->output5);
    proxy_resp->append_output_body(jOut5.dump(2).c_str(), jOut5.dump(2).size());

    #endif
    pIn->pBert->unlock();
    //pthread_mutex_unlock(&mutex_);
}
void bert_forward(const MMInput *in, MMOutput *out)
{
    auto start   = system_clock::now();

    in->pBert->forward2(((MMInput *)in)->inputIds, ((MMInput *)in)->segmentIds, ((MMInput *)in)->inputMasks, ((MMInput *)in)->inputDims, 
                    out->output, out->output2, out->output3, out->output4, out->output5);

    auto end   = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    printf(" forward process time1 is %d ms. \n", (duration)/1000);
}

/*
void pwork_callback(const ParallelWork *pwork)
{
	tutorial_series_context *ctx;
	const void *body;
	size_t size;
	size_t i;

	for (i = 0; i < pwork->size(); i++)
	{
		ctx = (tutorial_series_context *)pwork->series_at(i)->get_context();
		printf("%s\n", ctx->url.c_str());
		if (ctx->state == WFT_STATE_SUCCESS)
		{
			ctx->resp.get_parsed_body(&body, &size);
			printf("%zu%s\n", size, ctx->resp.is_chunked() ? " chunked" : "");
			fwrite(body, 1, size, stdout);
			printf("\n");
		}
		else
			printf("ERROR! state = %d, error = %d\n", ctx->state, ctx->error);

		delete ctx;
	}
}*/
const int deviceCounts =1;
default_random_engine e;
Bert* getFreeBert(Bert** ppBert)
{
#if 0
    while(true)
    {
    for(int i = 0; i < deviceCounts ; i++)
    {
    	if(ppBert[i]->trylock() == 0)
    	//if(pthread_mutex_trylock(&mutex_)==0)
    		return ppBert[i];
    }


    usleep(1000);
    }
#else
    int i = e()%deviceCounts;
    ppBert[i]->lock();
    return ppBert[i];
#endif

}
void process2(WFHttpTask *proxy_task, Bert** ppBert)
{
    auto start   = system_clock::now();
    

    // 1   get req to json
    HttpRequest *req = proxy_task->get_req();
    const char* pChar = NULL;
    size_t size_ = 0;
    bool ret = req->get_parsed_body((const void **)&pChar, &size_);
    auto end1   = system_clock::now();
auto duration1 = duration_cast<microseconds>(end1 - start);
printf(" req->get_parsed_body process time1 is %d ms. \n", (duration1)/1000);


    // 2 create bert task 
    ParallelWork *pwork = Workflow::create_parallel_work(nullptr);
    typedef WFThreadTaskFactory<MMInput, MMOutput> MMFactory;
    MMTask *task = MMFactory::create_thread_task("bert_task",
                    bert_forward,
                    http_callback);
    // 3 create input & output

    int S = tmp_sentence_len;
    int Bmax = tmp_batch_size;

    MMInput *input = task->get_input();
    MMOutput *output = task->get_output();
    //3.2 choose free pBert(on different device)

    
   

    input->inputDims.nbDims = 2;
    input->inputDims.d[0] = Bmax;
    input->inputDims.d[1] = S ;
    output->output.resize(1 * Bmax * S);
    output->output2.resize(1 * Bmax * S);
    output->output3.resize(1 * Bmax * S);
    output->output4.resize(1 * Bmax * S);
    output->output5.resize(1 * Bmax * 3);
    

#if 0
    auto js = json::parse(pChar);

    auto end2   = system_clock::now();
    auto duration2 = duration_cast<microseconds>(end2 - start);
    printf("json::parse process time1 is %d ms. \n", (duration2)/1000);
        
#define set_weights(dst, name, data_) \
    dst.type = DataType::kINT32;\
    dst.count = js["inputs"][name].size();\
    for(int i = 0;i < dst.count; i++)\
        data_.push_back(js["inputs"][name][i]);\
    dst.values = data_.data();

    set_weights(input->inputIds, "input_ids", input->data_ids);
    set_weights(input->inputMasks, "input_mask", input->data_masks);
    set_weights(input->segmentIds, "segment_ids", input->data_segs);

#else
    rapidjson::Document doc;
    rapidjson::ParseResult result = doc.Parse<rapidjson::kParseStopWhenDoneFlag>(pChar);
    
    if (!result) {
    
    printf("JSON parse error: %s (%u)", rapidjson::GetParseError_En(result.Code()), result.Offset());
    return;
        }

#if 0
#define set_weights(dst, name, data_,num) \
        dst.type = DataType::kINT32;\
        const rapidjson::Value& array##num = doc["inputs"][name];\
        size_t len##num = array##num.Size();\
        dst.count = len##num;\
        for(int i = 0;i < len##num; i++)\
            {data_.push_back(array##num[i].GetInt());}\
        dst.values = data_.data();
#else
#define set_weights(dst, name, data_,num) \
        dst.type = DataType::kINT32;\
        const rapidjson::Value& array##num = doc["inputs"][name];\
        size_t len##num = array##num.Size();\
        dst.count = len##num;\
        for(int i = 0;i < len##num; i++)\
        {\
            for(auto& v: array##num[i].GetArray())\
            {\
                data_.push_back(v.GetInt());\
            }\
        }\
        dst.values = data_.data();


#endif


    set_weights(input->inputIds, "input_ids", input->data_ids,1);
    set_weights(input->inputMasks, "input_mask", input->data_masks,2);
    set_weights(input->segmentIds, "segment_ids", input->data_segs,3);
#endif




#if 0
    for(int i =0; i < input->inputIds.count; i++)
    {
       if((((int*)input->inputIds.values)[i])  > 21127)
         {(((int*)input->inputIds.values)[i]) = 21127;
           printf("index is out of border, error!\n");
        }
    }
#endif
    input->pBert = getFreeBert(ppBert);
    // 4 create series
    SeriesWork *series = series_of(proxy_task);
    // SeriesWork *series = Workflow::create_series_work(task, nullptr);

    tutorial_series_context *context = new tutorial_series_context;
    context->url = req->get_request_uri();
    context->proxy_task = proxy_task;

    series->set_context(context);
    series->set_callback([](const SeriesWork *series) {
        delete (tutorial_series_context *)series->get_context();
    });
#if 1
    *series << task;
#else
		
    pwork->add_series(series);

	
	//5 start pwork
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	bool finished = false;

	Workflow::start_series_work(pwork,
		[&mutex, &cond, &finished](const SeriesWork *)
	{
		pthread_mutex_lock(&mutex);
		finished = true;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	});
#endif
auto end   = system_clock::now();
auto duration = duration_cast<microseconds>(end - start);
printf(" request process time1 is %d ms. \n", (duration)/1000);

}

#endif
void process(WFHttpTask *server_task, Bert* pBert)
{
	HttpRequest *req = server_task->get_req();
	HttpResponse *resp = server_task->get_resp();
	long long seq = server_task->get_task_seq();
	HttpHeaderCursor cursor(req);
	std::string name;
	std::string value;
	char buf[8192];
	int len;
//        bool get_parsed_body(const void **body, size_t *size) const
//	{
//		return http_parser_get_body(body, size, this->parser) == 0;
//	}
        
        const char* pChar = NULL;
        size_t size_ = 0;
        bool ret = req->get_parsed_body((const void **)&pChar, &size_);
	/* Set response message body. */
	resp->append_output_body_nocopy("<html>", 6);
	len = snprintf(buf, 8192, "<p>%s %s %s</p>", req->get_method(),
				   req->get_request_uri(), req->get_http_version());
	resp->append_output_body(buf, len);
	resp->append_output_body(pChar, size_);
#if 0        
ProtoContent::GpuRequest gRe;
//gRe.ParseFromArray(pChar, size_);
//gRe.ParseFromString(std::string(pChar));
//std::ofstream ofs;
//  ofs.open("gpurequest.sapmle", std::ofstream::binary);
//  gRe.SerializeToString(&gReString);
  google::protobuf::TextFormat::ParseFromString(std::string(pChar), &gRe);
    std::string gReString;
    google::protobuf::TextFormat::PrintToString(gRe,&gReString);
#endif

        auto js = json::parse(pChar);
        cout << "hello :" << js << endl;
#if 0
    vector<Weights> inputIds;
    vector<Weights> inputMasks;
    vector<Weights> segmentIds;
    vector<Dims> inputDims;

    int S = 200;
    int Bmax = 10;
    string dataDir = "/home/odin/shendasai2/tensorrt6_/TensorRT/demo/BERT2/data_hz";
    const std::string inputPath( dataDir + "/out_path2/test_inputs.weights_int32");
    loadInputs(inputPath, Bmax, S, inputIds, inputMasks, segmentIds, inputDims);


    std::vector<float> output(2 * Bmax * S);
	
    pBert->forward(inputIds[0], segmentIds[0], inputMasks[0], inputDims[0], output);
#else
    Weights inputIds;
    Weights inputMasks;
    Weights segmentIds;
    Dims inputDims;
    vector<int> data_ids;
    vector<int> data_masks;
    vector<int> data_segs;

#define set_weights(dst, name, data_) \
    dst.type = DataType::kINT32;\
    dst.count = js["inputs"][name].size();\
    for(int i = 0;i < dst.count; i++)\
        data_.push_back(js["inputs"][name][i]);\
    dst.values = data_.data();

    set_weights(inputIds, "input_ids", data_ids);
    set_weights(inputMasks, "input_mask", data_masks);
    set_weights(segmentIds, "segment_ids", data_segs);
    int S = 200;
    int Bmax = 1;
    //loadInputs(inputPath, Bmax, S, inputIds, inputMasks, segmentIds, inputDims);

    inputDims.nbDims = 2;
    inputDims.d[0] = 1;
    inputDims.d[1] = 200;
    std::vector<float> output(2 * Bmax * S);
	
    pBert->forward(inputIds, segmentIds, inputMasks, inputDims, output);
    json jOut(output);
	resp->append_output_body(jOut.dump(2).c_str(), jOut.dump(2).size());
#endif

	while (cursor.next(name, value))
	{
		len = snprintf(buf, 8192, "<p>%s: %s</p>", name.c_str(), value.c_str());
		resp->append_output_body(buf, len);
	}

	resp->append_output_body_nocopy("</html>", 7);

	/* Set status line if you like. */
	resp->set_http_version("HTTP/1.1");
	resp->set_status_code("200");
	resp->set_reason_phrase("OK");

	resp->add_header_pair("Content-Type", "text/html");
	resp->add_header_pair("Server", "Sogou WFHttpServer");
	if (seq == 9) /* no more than 10 requests on the same connection. */
		resp->add_header_pair("Connection", "close");

	/* print some log */
	char addrstr[128];
	struct sockaddr_storage addr;
	socklen_t l = sizeof addr;
	unsigned short port = 0;

	server_task->get_peer_addr((struct sockaddr *)&addr, &l);
	if (addr.ss_family == AF_INET)
	{
		struct sockaddr_in *sin = (struct sockaddr_in *)&addr;
		inet_ntop(AF_INET, &sin->sin_addr, addrstr, 128);
		port = ntohs(sin->sin_port);
	}
	else if (addr.ss_family == AF_INET6)
	{
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&addr;
		inet_ntop(AF_INET6, &sin6->sin6_addr, addrstr, 128);
		port = ntohs(sin6->sin6_port);
	}
	else
		strcpy(addrstr, "Unknown");

	fprintf(stderr, "Peer address: %s:%d, seq: %lld.\n",
			addrstr, port, seq);
}
using WeightMap = std::map<std::string, Weights>;
constexpr const char* gSampleName = "TensorRT.sample_bert";
constexpr const char* kTEST_INPUT_FNAME = "test_inputs.weights_int32";
constexpr const char* kTEST_OUTPUT_FNAME = "test_outputs.weights";
constexpr const char* kBERT_WEIGHTS_FNAME = "bert.weights";
constexpr int kNUM_RUNS = 10;

void sig_handler(int signo) { }
using namespace std;
int main(int argc, char *argv[])
{

#if 0
ProtoContent::GpuRequest gRe;
gRe.set_request_id(110);
string gReString; 
//std::ofstream ofs;
//  ofs.open("gpurequest.sapmle", std::ofstream::binary);
//  gRe.SerializeToString(&gReString);
    google::protobuf::TextFormat::PrintToString(gRe,&gReString);
std::ofstream out("output.txt115");
out << gReString << std::endl;
out.flush();
out.close();
#endif
    unsigned short port;

    if (argc != 2)
    {
        fprintf(stderr, "USAGE: %s <port>\n", argv[0]);
        exit(1);
    }

    const char *root = (argc >= 3 ? argv[2] : ".");

    for(int i = 0; i < deviceCounts; i++)
    {
        createThreadBertIns(i+2);
        pthread_mutex_lock(&mutex_);
        pthread_cond_wait(&cond_, &mutex_);

        pthread_mutex_unlock(&mutex_);
    }
    Bert** ppBert = pBertVec.data();
    //ppBert[1] = createMyBert(3);
    auto&& proc = std::bind(process2, std::placeholders::_1, ppBert);

    signal(SIGINT, sig_handler);

    WFHttpServer server(proc);
    port = atoi(argv[1]);
    if (server.start(port) == 0)
    {
        pause();
        server.stop();
    }
    else
    {
        perror("Cannot start server");
        exit(1);
    }

    return 0;
}

