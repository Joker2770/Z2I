/**
MIT License

Copyright (c) 2022 Augustusmyc
Copyright (c) 2023-2024 Joker2770

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <iostream>

// #define SMALL_BOARD_MODE
// #define USE_CUDA
// #define USE_OPENVINO
// #define USE_TENSORRT
// #define USE_ROCM

#define ORT_OLD_VISON (12)

#define CHANNEL_SIZE (3)

#ifdef SMALL_BOARD_MODE
#define BOARD_SIZE (3)
#define N_IN_ROW (3)
#define NUM_MCT_THREADS (4)
#define NUM_MCT_SIMS (54)
#define EXPLORE_STEP (3)
#define C_PUCT (3)
#define C_VIRTUAL_LOSS (1)
#define NUM_CHANNELS (64)
#define NUM_LAYERS (2)

#define BATCH_SIZE (64) // 512
#define DIRI (0.1)

#define NUM_TRAIN_THREADS (70)
#else
#define BOARD_SIZE (15)
#define N_IN_ROW (5)
#define NUM_MCT_THREADS (4)
#define NUM_MCT_SIMS (1600)
#define EXPLORE_STEP (BOARD_SIZE * BOARD_SIZE)
#define C_PUCT (5)
#define C_VIRTUAL_LOSS (3)
// #define NUM_CHANNELS (256)
// #define NUM_LAYERS (4)

#define BATCH_SIZE (256)
#define DIRI (0.01)

#define NUM_TRAIN_THREADS (10)

// 0 - free-style, 1 - standard, 2 - continuous, 4 - renju, 8 - caro
#define DEFAULT_RULE (0)

#endif

#define BLACK (1)
#define WHITE (-BLACK)

#define BUFFER_LEN (BOARD_SIZE * BOARD_SIZE + 1)
#define v_buff_type std::vector<int>
#define p_buff_type std::vector<std::vector<float>>
#define board_type std::vector<std::vector<int>>
#define board_buff_type std::vector<board_type>

// namespace customType {
//  using v_buff_type = std::vector<int>;
// using p_buff_type = std::vector<std::vector<float>>;
// using board_type = std::vector<std::vector<int>>;
// using board_buff_type = std::vector<board_type>;
//}
