#!/usr/bin/env python3

# coding=utf-8
# Copyright 2018 The Google AI Language Team Authors.
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

import argparse
import os
from os.path import join as jp
import numpy as np
import re

parser = argparse.ArgumentParser(description='Generate debug input/output pairs')

TEST_INPUT_FN = 'test_inputs.weights_int32'
TEST_OUTPUT_FN = 'test_outputs.weights'

parser.add_argument('-o', '--output', required=True, help='The directory to dump the data to. The script will create an input file, {}, and a correponding reference output file, {}.'.format(TEST_INPUT_FN, TEST_OUTPUT_FN))
parser.add_argument('-s', '--seqlen', type=int, required=True, help='The sequence length used to generate the tf record dataset')
parser.add_argument('-b', '--batchsize',type=int, required=True, help='The sequence length used to generate the tf record dataset')
parser.add_argument('-r', '--randomseed',type=int, required=False, default=12345, help='Seed for PRNG')

opt = parser.parse_args()

slen = opt.seqlen
B = opt.batchsize
outputbase = opt.output

def del_flags_by_key(FLAGS, keys_list):
    for keys in keys_list:
        FLAGS.__delattr__(keys)


# we need to remove all default flags set by importing the Example code below
# we save the original keys to diff against

np.random.seed(opt.randomseed)






test_word_ids = np.random.randint(0, 30522, (B, slen), dtype=np.int32)
test_input_mask = np.ones((B,slen), dtype=np.int32)
test_segment_ids = np.random.randint(0, 2, (B, slen), dtype=np.int32)

fd = {'input_ids:0' : test_word_ids,
        'input_mask:0':test_input_mask,
        'segment_ids:0':test_segment_ids}


if not os.path.exists(outputbase):
    print("Output path does not exist. Creating.")
    os.makedirs(outputbase)

out_fn = jp(outputbase,  TEST_INPUT_FN)

with open(out_fn, 'wb') as output_file:
    count = 3
    output_file.write("{}\n".format(count).encode('ASCII'))
    idx = 0
    for k,v in fd.items():
        outname = '{}_{}'.format(k[:-2], idx)
        shape = v.shape
        shape_str = '{} '.format(len(shape)) + ' '.join([str(d) for d in shape])
        output_file.write("{} 3 {} ".format(outname, shape_str).encode('ASCII'))
        output_file.write(v.tobytes())
        output_file.write("\n".encode('ASCII'))

