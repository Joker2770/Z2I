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

#include "gomoku.h"
#include "common.h"

#include <onnxruntime_cxx_api.h>
#include <future>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include <iostream>

// using namespace customType;

class NeuralNetwork
{
public:
  std::shared_ptr<Ort::Session> shared_session;
  // Ort::Session* sess;
  // Ort::Session session;
  using return_type = std::vector<std::vector<double>>;

  NeuralNetwork(const std::string &model_path, unsigned int batch_size);
  // void save_weights(std::string model_path);
  ~NeuralNetwork();

  std::future<return_type> commit(Gomoku *gomoku); // commit task to queue
  // std::shared_ptr<torch::jit::script::Module> module;  // torch module    origin:private
  static std::vector<float> transorm_gomoku_to_Tensor(Gomoku *gomoku);
  static std::vector<float> transorm_board_to_Tensor(const board_type &board, int last_move, int cur_player);

  bool set_batch_size(unsigned int u_batch_size);

  inline unsigned int get_batch_size() const { return this->batch_size; }

private:
  Ort::Env env;
  Ort::MemoryInfo memory_info;
  Ort::AllocatorWithDefaultOptions allocator;
  Ort::SessionOptions session_options;
  using task_type = std::pair<std::vector<float>, std::promise<return_type>>;
  // pair: input board state(float list), output P and V

#if ORT_API_VERSION < ORT_OLD_VISON
  char *_inputName, *_output_name0; // , * _output_name1;
#else
  std::shared_ptr<char> _inputName, _output_name0; // , _output_name1;
#endif

  std::vector<const char *> input_node_names;
  std::vector<const char *> output_node_names;
  // std::vector<float> input_tensor_values;
  //  std::shared_ptr<torch::jit::script::Module> module;

  void infer(); // infer

  std::unique_ptr<std::thread> loop; // call infer in loop
  bool running;                      // is running

  std::queue<task_type> tasks; // tasks queue
  std::mutex lock;             // lock for tasks queue
  std::condition_variable cv;  // condition variable for tasks queue
  std::vector<int64_t> input_node_dims;

  unsigned int batch_size; // batch size
};
