/**
MIT License

Copyright (c) 2022 Augustusmyc
Copyright (c) 2023 Joker2770

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
#include <future>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include <onnx.h>
#include <thread_pool.h>
#include <gomoku.h>
#include <common.h>

class SelfPlay
{
public:
    SelfPlay(NeuralNetwork *nn);
    //~SelfPlay();
    void play(unsigned int saved_id);

    void self_play_for_train(unsigned int game_num, unsigned int start_batch_id);

private:
    // p_buff_type *p_buffer;
    // board_buff_type *board_buffer;
    // v_buff_type *v_buffer;

    NeuralNetwork *nn;
    std::unique_ptr<ThreadPool> thread_pool;
    // std::queue<task_type> tasks;  // tasks queue
    std::mutex lock;            // lock for tasks queue
    std::condition_variable cv; // condition variable for tasks queue
};
