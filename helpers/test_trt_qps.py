#coding:utf-8
import threading
import sys
import requests
import json
import time
import pdb
#http://10.144.120.27:8543/v1/models/yesno_model:predict
#def get_model_outputs(url='http://10.144.120.27:8543/v1/models/yesno_model:predict',max_seq_len=40):
def get_model_outputs(url='http://10.144.44.35:20020',max_seq_len=40):
    input_id = [101, 6821, 3221, 671, 702, 3844, 6407, 4638, 1368, 2094, 102, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    input_mask = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    segment_id = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    if len(input_id) < max_seq_len:
        input_id.extend([0] * (max_seq_len - len(input_id)))
        input_mask.extend([0] * (max_seq_len - len(input_mask)))
        segment_id.extend([0] * (max_seq_len - len(segment_id)))

    batch_size =8
    input_ids = []
    input_masks = []
    segment_ids = []
    for i in range(batch_size):
        input_ids.append(input_id)
        input_masks.append(input_mask)
        segment_ids.append(segment_id)
    #    input_ids.extend(input_id)
     #   input_masks.extend(input_mask)
      #  segment_ids.extend(segment_id)

    data = {
        "signature_name": "serving_default",
        "inputs": {
            "input_ids": input_ids,
            "input_mask": input_masks,
            "segment_ids": segment_ids,
            "training": False,
        }
    }
    #pdb.set_trace()
    start = time.time()#print(get_model_outputs())
    response = requests.post(url=url, data=json.dumps(data))
    print("rest cost :"+str((time.time()-start)*float(1000)))
#    print(response.text)
    #outputs = json.loads(response.text)['outputs']
    #print(outputs)
    #return outputs
    return

def music():
    for i in range(100):get_model_outputs(max_seq_len=200)
threads = []
t1 = threading.Thread(target = music)
threads.append(t1)
t2 = threading.Thread(target = music)
threads.append(t2)
t3 = threading.Thread(target = music)
threads.append(t3)
t4 = threading.Thread(target = music)
threads.append(t4)
t5 = threading.Thread(target = music)
threads.append(t5)
t6 = threading.Thread(target = music)
threads.append(t6)
t7 = threading.Thread(target = music)
threads.append(t7)
t8 = threading.Thread(target = music)
threads.append(t8)
t9 = threading.Thread(target = music)
threads.append(t9)
ta = threading.Thread(target = music)
threads.append(ta)
if __name__=='__main__':
    start = time.time()#print(get_model_outputs())
    # for i in range(100):run('10.144.120.27',port=8530,model='intent_model',signature_name='serving_default',max_seq_len=300)
    # print("grpc cost :"+str((time.time()-start)/float(100)))#print(get_model_outputs())
    # start = time.time()#print(get_model_outputs())
#    for i in range(1):get_model_outputs(max_seq_len=200)
    #print("rest cost :"+str((time.time()-start)*float(1000)))#print(get_model_outputs())
    # print(" max grpc time : "+str(max_grpc_time))
    # print("min grpc time "+str(min_grpc_time))
    for t in threads:
        t.setDaemon(True)
        t.start()
    t.join()
    print("-----------> final cost :"+str((time.time()-start)*float(1000))) 
