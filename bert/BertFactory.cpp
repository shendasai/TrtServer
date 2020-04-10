#include "BertFactory.h"
#include "BertQA.h"
namespace bert
{
//class BertQA;

#define create_bert(type)  new Bert##type()

Bert::Bert(int numHeads, int Bmax, int S, bool runInFp16):numHeads_(numHeads),Bmax_(Bmax), S_(S), runInFp16_(runInFp16)
{

    //cudaStreamCreate(&stream_);

}


Bert::~Bert()
{
    //cudaStreamDestroy(stream_);

}

void Bert::setParam(int numHeads, int Bmax, int S, bool runInFp16)
{
    
    numHeads_ =numHeads;
    Bmax_ = Bmax;
    S_ = S;
    runInFp16_ = runInFp16;

}

int Bert::getNumHeads()
{
    return numHeads_;
}
int Bert::getBMax()
{
    return Bmax_;
}
int Bert::getS()
{
    return S_;
}

bool Bert::getRunInFp16()
{
    return runInFp16_;
}

string Bert::getOutputName(int index)
{
    return outputName_[index];
}
void Bert::addOutputName(string outputName)
{
    outputName_.push_back(outputName);
}


int Bert::getDeviceId()
{
    return deviceId_;
}

void Bert::setDeviceId(int deviceId)
{
    deviceId_= deviceId;
}


Bert* createBert(string type)
{

    Bert* pBert = create_bert(QA);
    //Bert* pBert = create_bert(type);
    return pBert;

}

}


