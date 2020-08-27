# TrtServer
基于sogou workflow搭建的tensort server。(sogou workflow地址: https://github.com/sogou/workflow.git)
基于Tensorrt 7;
主要支持Bert模型。


Buffer type is hard-coded to 0 (float), i.e. 4 bytes per coefficient
203
Orig.name: bert/embeddings/LayerNorm/beta TRT name: bert_embeddings_layernorm_beta shape: 1 768
Orig.name: bert/embeddings/LayerNorm/gamma TRT name: bert_embeddings_layernorm_gamma shape: 1 768
Orig.name: bert/embeddings/position_embeddings TRT name: bert_embeddings_position_embeddings shape: 2 512 768
Orig.name: bert/embeddings/token_type_embeddings TRT name: bert_embeddings_token_type_embeddings shape: 2 2 768
Orig.name: bert/embeddings/word_embeddings TRT name: bert_embeddings_word_embeddings shape: 2 21128 768
Orig.name: bert/encoder/layer_0/attention/output/LayerNorm/beta TRT name: l0_attention_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_0/attention/output/LayerNorm/gamma TRT name: l0_attention_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_0/attention/output/dense/bias TRT name: l0_attention_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_0/attention/output/dense/kernel TRT name: l0_attention_output_dense_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_0/attention/self/key/bias TRT name: l0_attention_self_key_bias shape: 1 768
Orig.name: bert/encoder/layer_0/attention/self/key/kernel TRT name: l0_attention_self_key_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_0/attention/self/query/bias TRT name: l0_attention_self_query_bias shape: 1 768
Orig.name: bert/encoder/layer_0/attention/self/query/kernel TRT name: l0_attention_self_query_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_0/attention/self/value/bias TRT name: l0_attention_self_value_bias shape: 1 768
Orig.name: bert/encoder/layer_0/attention/self/value/kernel TRT name: l0_attention_self_value_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_0/intermediate/dense/bias TRT name: l0_intermediate_dense_bias shape: 1 3072
Orig.name: bert/encoder/layer_0/intermediate/dense/kernel TRT name: l0_intermediate_dense_kernel shape: 2 768 3072
Orig.name: bert/encoder/layer_0/output/LayerNorm/beta TRT name: l0_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_0/output/LayerNorm/gamma TRT name: l0_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_0/output/dense/bias TRT name: l0_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_0/output/dense/kernel TRT name: l0_output_dense_kernel shape: 2 3072 768
Orig.name: bert/encoder/layer_1/attention/output/LayerNorm/beta TRT name: l1_attention_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_1/attention/output/LayerNorm/gamma TRT name: l1_attention_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_1/attention/output/dense/bias TRT name: l1_attention_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_1/attention/output/dense/kernel TRT name: l1_attention_output_dense_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_1/attention/self/key/bias TRT name: l1_attention_self_key_bias shape: 1 768
Orig.name: bert/encoder/layer_1/attention/self/key/kernel TRT name: l1_attention_self_key_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_1/attention/self/query/bias TRT name: l1_attention_self_query_bias shape: 1 768
Orig.name: bert/encoder/layer_1/attention/self/query/kernel TRT name: l1_attention_self_query_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_1/attention/self/value/bias TRT name: l1_attention_self_value_bias shape: 1 768
Orig.name: bert/encoder/layer_1/attention/self/value/kernel TRT name: l1_attention_self_value_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_1/intermediate/dense/bias TRT name: l1_intermediate_dense_bias shape: 1 3072
Orig.name: bert/encoder/layer_1/intermediate/dense/kernel TRT name: l1_intermediate_dense_kernel shape: 2 768 3072
Orig.name: bert/encoder/layer_1/output/LayerNorm/beta TRT name: l1_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_1/output/LayerNorm/gamma TRT name: l1_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_1/output/dense/bias TRT name: l1_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_1/output/dense/kernel TRT name: l1_output_dense_kernel shape: 2 3072 768
Orig.name: bert/encoder/layer_10/attention/output/LayerNorm/beta TRT name: l10_attention_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_10/attention/output/LayerNorm/gamma TRT name: l10_attention_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_10/attention/output/dense/bias TRT name: l10_attention_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_10/attention/output/dense/kernel TRT name: l10_attention_output_dense_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_10/attention/self/key/bias TRT name: l10_attention_self_key_bias shape: 1 768
Orig.name: bert/encoder/layer_10/attention/self/key/kernel TRT name: l10_attention_self_key_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_10/attention/self/query/bias TRT name: l10_attention_self_query_bias shape: 1 768
Orig.name: bert/encoder/layer_10/attention/self/query/kernel TRT name: l10_attention_self_query_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_10/attention/self/value/bias TRT name: l10_attention_self_value_bias shape: 1 768
Orig.name: bert/encoder/layer_10/attention/self/value/kernel TRT name: l10_attention_self_value_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_10/intermediate/dense/bias TRT name: l10_intermediate_dense_bias shape: 1 3072
Orig.name: bert/encoder/layer_10/intermediate/dense/kernel TRT name: l10_intermediate_dense_kernel shape: 2 768 3072
Orig.name: bert/encoder/layer_10/output/LayerNorm/beta TRT name: l10_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_10/output/LayerNorm/gamma TRT name: l10_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_10/output/dense/bias TRT name: l10_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_10/output/dense/kernel TRT name: l10_output_dense_kernel shape: 2 3072 768
Orig.name: bert/encoder/layer_11/attention/output/LayerNorm/beta TRT name: l11_attention_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_11/attention/output/LayerNorm/gamma TRT name: l11_attention_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_11/attention/output/dense/bias TRT name: l11_attention_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_11/attention/output/dense/kernel TRT name: l11_attention_output_dense_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_11/attention/self/key/bias TRT name: l11_attention_self_key_bias shape: 1 768
Orig.name: bert/encoder/layer_11/attention/self/key/kernel TRT name: l11_attention_self_key_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_11/attention/self/query/bias TRT name: l11_attention_self_query_bias shape: 1 768
Orig.name: bert/encoder/layer_11/attention/self/query/kernel TRT name: l11_attention_self_query_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_11/attention/self/value/bias TRT name: l11_attention_self_value_bias shape: 1 768
Orig.name: bert/encoder/layer_11/attention/self/value/kernel TRT name: l11_attention_self_value_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_11/intermediate/dense/bias TRT name: l11_intermediate_dense_bias shape: 1 3072
Orig.name: bert/encoder/layer_11/intermediate/dense/kernel TRT name: l11_intermediate_dense_kernel shape: 2 768 3072
Orig.name: bert/encoder/layer_11/output/LayerNorm/beta TRT name: l11_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_11/output/LayerNorm/gamma TRT name: l11_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_11/output/dense/bias TRT name: l11_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_11/output/dense/kernel TRT name: l11_output_dense_kernel shape: 2 3072 768
Orig.name: bert/encoder/layer_2/attention/output/LayerNorm/beta TRT name: l2_attention_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_2/attention/output/LayerNorm/gamma TRT name: l2_attention_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_2/attention/output/dense/bias TRT name: l2_attention_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_2/attention/output/dense/kernel TRT name: l2_attention_output_dense_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_2/attention/self/key/bias TRT name: l2_attention_self_key_bias shape: 1 768
Orig.name: bert/encoder/layer_2/attention/self/key/kernel TRT name: l2_attention_self_key_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_2/attention/self/query/bias TRT name: l2_attention_self_query_bias shape: 1 768
Orig.name: bert/encoder/layer_2/attention/self/query/kernel TRT name: l2_attention_self_query_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_2/attention/self/value/bias TRT name: l2_attention_self_value_bias shape: 1 768
Orig.name: bert/encoder/layer_2/attention/self/value/kernel TRT name: l2_attention_self_value_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_2/intermediate/dense/bias TRT name: l2_intermediate_dense_bias shape: 1 3072
Orig.name: bert/encoder/layer_2/intermediate/dense/kernel TRT name: l2_intermediate_dense_kernel shape: 2 768 3072
Orig.name: bert/encoder/layer_2/output/LayerNorm/beta TRT name: l2_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_2/output/LayerNorm/gamma TRT name: l2_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_2/output/dense/bias TRT name: l2_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_2/output/dense/kernel TRT name: l2_output_dense_kernel shape: 2 3072 768
Orig.name: bert/encoder/layer_3/attention/output/LayerNorm/beta TRT name: l3_attention_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_3/attention/output/LayerNorm/gamma TRT name: l3_attention_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_3/attention/output/dense/bias TRT name: l3_attention_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_3/attention/output/dense/kernel TRT name: l3_attention_output_dense_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_3/attention/self/key/bias TRT name: l3_attention_self_key_bias shape: 1 768
Orig.name: bert/encoder/layer_3/attention/self/key/kernel TRT name: l3_attention_self_key_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_3/attention/self/query/bias TRT name: l3_attention_self_query_bias shape: 1 768
Orig.name: bert/encoder/layer_3/attention/self/query/kernel TRT name: l3_attention_self_query_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_3/attention/self/value/bias TRT name: l3_attention_self_value_bias shape: 1 768
Orig.name: bert/encoder/layer_3/attention/self/value/kernel TRT name: l3_attention_self_value_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_3/intermediate/dense/bias TRT name: l3_intermediate_dense_bias shape: 1 3072
Orig.name: bert/encoder/layer_3/intermediate/dense/kernel TRT name: l3_intermediate_dense_kernel shape: 2 768 3072
Orig.name: bert/encoder/layer_3/output/LayerNorm/beta TRT name: l3_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_3/output/LayerNorm/gamma TRT name: l3_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_3/output/dense/bias TRT name: l3_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_3/output/dense/kernel TRT name: l3_output_dense_kernel shape: 2 3072 768
Orig.name: bert/encoder/layer_4/attention/output/LayerNorm/beta TRT name: l4_attention_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_4/attention/output/LayerNorm/gamma TRT name: l4_attention_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_4/attention/output/dense/bias TRT name: l4_attention_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_4/attention/output/dense/kernel TRT name: l4_attention_output_dense_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_4/attention/self/key/bias TRT name: l4_attention_self_key_bias shape: 1 768
Orig.name: bert/encoder/layer_4/attention/self/key/kernel TRT name: l4_attention_self_key_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_4/attention/self/query/bias TRT name: l4_attention_self_query_bias shape: 1 768
Orig.name: bert/encoder/layer_4/attention/self/query/kernel TRT name: l4_attention_self_query_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_4/attention/self/value/bias TRT name: l4_attention_self_value_bias shape: 1 768
Orig.name: bert/encoder/layer_4/attention/self/value/kernel TRT name: l4_attention_self_value_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_4/intermediate/dense/bias TRT name: l4_intermediate_dense_bias shape: 1 3072
Orig.name: bert/encoder/layer_4/intermediate/dense/kernel TRT name: l4_intermediate_dense_kernel shape: 2 768 3072
Orig.name: bert/encoder/layer_4/output/LayerNorm/beta TRT name: l4_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_4/output/LayerNorm/gamma TRT name: l4_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_4/output/dense/bias TRT name: l4_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_4/output/dense/kernel TRT name: l4_output_dense_kernel shape: 2 3072 768
Orig.name: bert/encoder/layer_5/attention/output/LayerNorm/beta TRT name: l5_attention_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_5/attention/output/LayerNorm/gamma TRT name: l5_attention_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_5/attention/output/dense/bias TRT name: l5_attention_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_5/attention/output/dense/kernel TRT name: l5_attention_output_dense_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_5/attention/self/key/bias TRT name: l5_attention_self_key_bias shape: 1 768
Orig.name: bert/encoder/layer_5/attention/self/key/kernel TRT name: l5_attention_self_key_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_5/attention/self/query/bias TRT name: l5_attention_self_query_bias shape: 1 768
Orig.name: bert/encoder/layer_5/attention/self/query/kernel TRT name: l5_attention_self_query_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_5/attention/self/value/bias TRT name: l5_attention_self_value_bias shape: 1 768
Orig.name: bert/encoder/layer_5/attention/self/value/kernel TRT name: l5_attention_self_value_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_5/intermediate/dense/bias TRT name: l5_intermediate_dense_bias shape: 1 3072
Orig.name: bert/encoder/layer_5/intermediate/dense/kernel TRT name: l5_intermediate_dense_kernel shape: 2 768 3072
Orig.name: bert/encoder/layer_5/output/LayerNorm/beta TRT name: l5_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_5/output/LayerNorm/gamma TRT name: l5_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_5/output/dense/bias TRT name: l5_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_5/output/dense/kernel TRT name: l5_output_dense_kernel shape: 2 3072 768
Orig.name: bert/encoder/layer_6/attention/output/LayerNorm/beta TRT name: l6_attention_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_6/attention/output/LayerNorm/gamma TRT name: l6_attention_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_6/attention/output/dense/bias TRT name: l6_attention_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_6/attention/output/dense/kernel TRT name: l6_attention_output_dense_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_6/attention/self/key/bias TRT name: l6_attention_self_key_bias shape: 1 768
Orig.name: bert/encoder/layer_6/attention/self/key/kernel TRT name: l6_attention_self_key_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_6/attention/self/query/bias TRT name: l6_attention_self_query_bias shape: 1 768
Orig.name: bert/encoder/layer_6/attention/self/query/kernel TRT name: l6_attention_self_query_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_6/attention/self/value/bias TRT name: l6_attention_self_value_bias shape: 1 768
Orig.name: bert/encoder/layer_6/attention/self/value/kernel TRT name: l6_attention_self_value_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_6/intermediate/dense/bias TRT name: l6_intermediate_dense_bias shape: 1 3072
Orig.name: bert/encoder/layer_6/intermediate/dense/kernel TRT name: l6_intermediate_dense_kernel shape: 2 768 3072
Orig.name: bert/encoder/layer_6/output/LayerNorm/beta TRT name: l6_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_6/output/LayerNorm/gamma TRT name: l6_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_6/output/dense/bias TRT name: l6_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_6/output/dense/kernel TRT name: l6_output_dense_kernel shape: 2 3072 768
Orig.name: bert/encoder/layer_7/attention/output/LayerNorm/beta TRT name: l7_attention_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_7/attention/output/LayerNorm/gamma TRT name: l7_attention_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_7/attention/output/dense/bias TRT name: l7_attention_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_7/attention/output/dense/kernel TRT name: l7_attention_output_dense_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_7/attention/self/key/bias TRT name: l7_attention_self_key_bias shape: 1 768
Orig.name: bert/encoder/layer_7/attention/self/key/kernel TRT name: l7_attention_self_key_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_7/attention/self/query/bias TRT name: l7_attention_self_query_bias shape: 1 768
Orig.name: bert/encoder/layer_7/attention/self/query/kernel TRT name: l7_attention_self_query_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_7/attention/self/value/bias TRT name: l7_attention_self_value_bias shape: 1 768
Orig.name: bert/encoder/layer_7/attention/self/value/kernel TRT name: l7_attention_self_value_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_7/intermediate/dense/bias TRT name: l7_intermediate_dense_bias shape: 1 3072
Orig.name: bert/encoder/layer_7/intermediate/dense/kernel TRT name: l7_intermediate_dense_kernel shape: 2 768 3072
Orig.name: bert/encoder/layer_7/output/LayerNorm/beta TRT name: l7_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_7/output/LayerNorm/gamma TRT name: l7_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_7/output/dense/bias TRT name: l7_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_7/output/dense/kernel TRT name: l7_output_dense_kernel shape: 2 3072 768
Orig.name: bert/encoder/layer_8/attention/output/LayerNorm/beta TRT name: l8_attention_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_8/attention/output/LayerNorm/gamma TRT name: l8_attention_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_8/attention/output/dense/bias TRT name: l8_attention_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_8/attention/output/dense/kernel TRT name: l8_attention_output_dense_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_8/attention/self/key/bias TRT name: l8_attention_self_key_bias shape: 1 768
Orig.name: bert/encoder/layer_8/attention/self/key/kernel TRT name: l8_attention_self_key_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_8/attention/self/query/bias TRT name: l8_attention_self_query_bias shape: 1 768
Orig.name: bert/encoder/layer_8/attention/self/query/kernel TRT name: l8_attention_self_query_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_8/attention/self/value/bias TRT name: l8_attention_self_value_bias shape: 1 768
Orig.name: bert/encoder/layer_8/attention/self/value/kernel TRT name: l8_attention_self_value_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_8/intermediate/dense/bias TRT name: l8_intermediate_dense_bias shape: 1 3072
Orig.name: bert/encoder/layer_8/intermediate/dense/kernel TRT name: l8_intermediate_dense_kernel shape: 2 768 3072
Orig.name: bert/encoder/layer_8/output/LayerNorm/beta TRT name: l8_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_8/output/LayerNorm/gamma TRT name: l8_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_8/output/dense/bias TRT name: l8_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_8/output/dense/kernel TRT name: l8_output_dense_kernel shape: 2 3072 768
Orig.name: bert/encoder/layer_9/attention/output/LayerNorm/beta TRT name: l9_attention_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_9/attention/output/LayerNorm/gamma TRT name: l9_attention_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_9/attention/output/dense/bias TRT name: l9_attention_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_9/attention/output/dense/kernel TRT name: l9_attention_output_dense_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_9/attention/self/key/bias TRT name: l9_attention_self_key_bias shape: 1 768
Orig.name: bert/encoder/layer_9/attention/self/key/kernel TRT name: l9_attention_self_key_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_9/attention/self/query/bias TRT name: l9_attention_self_query_bias shape: 1 768
Orig.name: bert/encoder/layer_9/attention/self/query/kernel TRT name: l9_attention_self_query_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_9/attention/self/value/bias TRT name: l9_attention_self_value_bias shape: 1 768
Orig.name: bert/encoder/layer_9/attention/self/value/kernel TRT name: l9_attention_self_value_kernel shape: 2 768 768
Orig.name: bert/encoder/layer_9/intermediate/dense/bias TRT name: l9_intermediate_dense_bias shape: 1 3072
Orig.name: bert/encoder/layer_9/intermediate/dense/kernel TRT name: l9_intermediate_dense_kernel shape: 2 768 3072
Orig.name: bert/encoder/layer_9/output/LayerNorm/beta TRT name: l9_output_layernorm_beta shape: 1 768
Orig.name: bert/encoder/layer_9/output/LayerNorm/gamma TRT name: l9_output_layernorm_gamma shape: 1 768
Orig.name: bert/encoder/layer_9/output/dense/bias TRT name: l9_output_dense_bias shape: 1 768
Orig.name: bert/encoder/layer_9/output/dense/kernel TRT name: l9_output_dense_kernel shape: 2 3072 768
Orig.name: cls/squad/output_bias TRT name: cls_squad_output_bias shape: 1 2
Orig.name: cls/squad/output_weights TRT name: cls_squad_output_weights shape: 2 2 768
Orig.name: dense/bias TRT name: dense_bias shape: 1 768
Orig.name: dense/kernel TRT name: dense_kernel shape: 2 768 768
Orig.name: dense_1/bias TRT name: dense_1_bias shape: 1 3
Orig.name: dense_1/kernel TRT name: dense_1_kernel shape: 2 768 3
