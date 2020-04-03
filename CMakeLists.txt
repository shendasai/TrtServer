# Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
set(CMAKE_CUDA_COMPILER "/usr/local/cuda/bin/nvcc")
set(CMAKE_BUILD_TYPE Debug CACHE STRING "build type")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

project(cmake_and_cuda LANGUAGES CXX CUDA)

find_package(OpenSSL REQUIRED)
#find_package(workflow REQUIRED CONFIG HINTS ..)
include_directories(${OPENSSL_INCLUDE_DIR} ${WORKFLOW_INCLUDE_DIR} /usr/local/cuda-10.0/include ./bert_inc /home/odin/shendasai/TensorRT6/TensorRT-6.0.1.5/include/)
link_directories(${WORKFLOW_LIB_DIR} /home/odin/shendasai/lib/protobuf/protobuf-3.8.0/src/.libs/ /usr/local/cuda-10.0/lib64 /home/odin/shendasai/TensorRT6/TensorRT-6.0.1.5/lib ./bert_lib /home/odin/shendasai/CUDNN_7.6.3/cuda/lib64)



set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS}  \
--expt-relaxed-constexpr \
--expt-extended-lambda \
-std=c++11 \
-g \
-gencode arch=compute_70,code=sm_70 \
-gencode arch=compute_75,code=sm_75 \
-Wno-deprecated-declarations")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wno-deprecated-declarations")

set(BERT_LIBS
    cudart
    cublas
    nvinfer
    nvinfer_plugin
    pthread
    z
)

include_directories(
    ./
    ./bert
    ./layers
    ./plugins
    ./util
    ../../include/
    ../../third_party/cub/
    /usr/include/x86_64-linux-gnu
    /usr/local/cuda-10.0/include
    /workspace/tensorrt/include
    /workspace/tensorrt/samples/common
    /workspace/cub/
    /workspace/cutlass/
    ./plugins/common
    ./include/tensorrt/common
    ./include/tensorrt/cub
    ./include/tensorrt
    ./include
)

link_directories(
    /usr/lib/x86_64-linux-gnu
    /usr/local/cuda-10.0/lib64
    /workspace/tensorrt/lib
    ./lib
    /home/odin/shendasai/TensorRT6/TensorRT-6.0.1.5/lib
)

add_library(common SHARED
    util/logger.cpp
)

add_library(bert_plugins SHARED
    plugins/embLayerNormPlugin.cu
    plugins/geluPlugin.cu
    plugins/qkvToContextPlugin.cu
    plugins/skipLayerNormPlugin.cu
)

target_link_libraries(bert_plugins
    common
    ${BERT_LIBS}
)

target_link_libraries(common
    ${BERT_LIBS}
)
include_directories(proto)
aux_source_directory(proto PROTO)

#add_library(sample_bert 
add_library(bert SHARED
#add_executable(sample_bert
    bert/bert.cpp
    bert/driver.cpp
    util/dataUtils.cpp
    bert/BertQA.cpp
    bert/BertFactory.cpp
)

add_executable(sample_bert
    sampleBERT.cpp
)


target_compile_features(sample_bert PUBLIC cxx_std_11)

target_link_libraries(sample_bert
    common
    bert_plugins
    bert
)
add_executable(http_gpu_server
    server/tutorial-13-http_gpu_server.cc ${PROTO}
)
target_link_libraries(http_gpu_server workflow protobuf common bert bert_plugins pthread)
