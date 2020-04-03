(1)convert ckpt to simple weight file 
 python helpers/convert_weights.py -m ./data/squad_output_path/model.ckpt-5474 -o ./data/weight_path/bert.weights

(2)get input data. s is sentcen_len, b is batch_size
python helpers/generate_dbg.py -o ./data/out_path2 -s 200 -b 10

(2.2) build
create dir build  & cd build
cmake ../
make -j4

(3)run  . -d must have   bert.weights/bert_config.json/ test_inputs.weights_int32/test_outputs.weights
./sample_bert   -d ../data_hz/weight_path -d ../data_hz/out_path2 --fp16 --nheads 12
./sample_bert   -d ../data_hz/weight_path -d ../data_hz/out_path2 --nheads 12



(5) attention:
##some layer name is different with gov sample, so you need read hz's model and change them in the code.
