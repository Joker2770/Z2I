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

#include "common.h"
#include "onnx.h"

#include <onnxruntime_cxx_api.h>
#include <iostream>
#include <random>
#include <future>
#include <memory>
#include <queue>
#include <assert.h>
#include <algorithm>

#include <codecvt>

// using namespace customType;

bool CheckStatus(const OrtApi *g_ort, OrtStatus *status)
{
  if (status != nullptr)
  {
    const char *msg = g_ort->GetErrorMessage(status);
    std::cerr << msg << std::endl;
    g_ort->ReleaseStatus(status);
    throw Ort::Exception(msg, OrtErrorCode::ORT_EP_FAIL);
  }
  return true;
}

NeuralNetwork::NeuralNetwork(const std::string &model_path, const unsigned int batch_size)
    : /* module(std::make_shared<torch::jit::script::Module>(torch::jit::load(model_path.c_str()))),*/
      env(nullptr),
      shared_session(nullptr),
      batch_size(batch_size),
      running(true),
      loop(nullptr),
      memory_info(nullptr)
{
  this->memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
  this->env = Ort::Env(ORT_LOGGING_LEVEL_WARNING, "alphaZero");
  // const auto& api = Ort::GetApi();
  // OrtTensorRTProviderOptionsV2* tensorrt_options;
  // auto share_session_options = std::make_shared<Ort::SessionOptions>(session_options);
  // session_options->SetIntraOpNumThreads(1); // TODO:study the parameter
  // session_options->SetInterOpNumThreads(1); // TODO:study the parameter

  this->session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

#if (defined(USE_CUDA) || defined(USE_OPENVINO) || defined(USE_TENSORRT) || defined(USE_ROCM))
  const OrtApiBase *ptr_api_base = OrtGetApiBase();
  const OrtApi *g_ort = ptr_api_base->GetApi(ORT_API_VERSION);
#endif // (USE_CUDA || USE_OPENVINO || USE_TENSORRT || USE_ROCM)

// #define USE_CUDA
#ifdef USE_CUDA
  // CUDA option set
  OrtCUDAProviderOptions cuda_option;
  cuda_option.device_id = 0;
  cuda_option.arena_extend_strategy = 0;
  cuda_option.cudnn_conv_algo_search = OrtCudnnConvAlgoSearchExhaustive;
  cuda_option.gpu_mem_limit = SIZE_MAX;
  cuda_option.do_copy_in_default_stream = 1;
  CheckStatus(g_ort, g_ort->SessionOptionsAppendExecutionProvider_CUDA(session_options, &cuda_option));
  // OrtSessionOptionsAppendExecutionProvider_CUDA(session_options, 0);
  // std::cout << "DEBUG ......Enable CUDA......" << std::endl;
#endif

// #define USE_OPENVINO
#ifdef USE_OPENVINO
  // OpenVINO options set
  OrtOpenVINOProviderOptions OpenVINO_Options;
  OpenVINO_Options.device_type = "CPU_FP32";
  OpenVINO_Options.device_id = "";
  CheckStatus(g_ort, g_ort->SessionOptionsAppendExecutionProvider_OpenVINO(session_options, &OpenVINO_Options));
#endif

#ifdef USE_TENSORRT
  OrtTensorRTProviderOptions TensorRT_Options;
  TensorRT_Options.device_id = 0;
  CheckStatus(g_ort, g_ort->SessionOptionsAppendExecutionProvider_TensorRT(session_options, &TensorRT_Options));
#endif

#ifdef USE_ROCM
  OrtROCMProviderOptions ROCM_Options;
  ROCM_Options.device_id = 0;
  CheckStatus(g_ort, g_ort->SessionOptionsAppendExecutionProvider_ROCM(session_options, &ROCM_Options));
#endif

#ifdef _WIN32
  // std::wstring widestr = std::wstring(model_path.begin(), model_path.end());
  std::wstring wstr(model_path.length(), L' ');
  std::copy(model_path.begin(), model_path.end(), wstr.begin());
  // std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  // const wchar_t *model_path_w = converter.from_bytes(model_path).c_str();
  // No CUDA
  this->shared_session = std::make_shared<Ort::Session>(Ort::Session(env, wstr.c_str(), session_options));
#else
  // Ort::Session session = Ort::Session(env, model_path.c_str(), *session_options);
  this->shared_session = std::make_shared<Ort::Session>(Ort::Session(env, model_path.c_str(), session_options));
#endif
  // sess = &session;

  size_t input_tensor_size = CHANNEL_SIZE * BOARD_SIZE * BOARD_SIZE;
  // simplify ... using known dim values to calculate size
  // use OrtGetTensorShapeElementCount() to get official size!

  // this->input_tensor_values = std::vector<float> (input_tensor_size);
  this->output_node_names = std::vector<const char *>{"V", "P"};

  // print number of model input nodes
  size_t num_input_nodes = shared_session->GetInputCount();
  this->input_node_names = std::vector<const char *>(num_input_nodes);
  // simplify... this model has only 1 input node {?, 3, 15, 15}.
  // Otherwise need vector<vector<>>

  // printf("Number of inputs = %zu\n", num_input_nodes);

  // iterate over all input nodes
  for (size_t i = 0; i < num_input_nodes; i++)
  {
    // print input node names
    char *input_name = shared_session->GetInputName(i, allocator);
    // printf("Input %d : name = %s\n", i, input_name);
    this->input_node_names[i] = input_name;

    // print input node types
    Ort::TypeInfo type_info = shared_session->GetInputTypeInfo(i);
    auto tensor_info = type_info.GetTensorTypeAndShapeInfo();

    ONNXTensorElementDataType type = tensor_info.GetElementType();
    // printf("Input %d : type = %d\n", i, type);

    // print input shapes/dims
    this->input_node_dims = tensor_info.GetShape();
    // printf("Input %d : num_dims = %zu\n", i, input_node_dims.size());
    // for (size_t j = 0; j < input_node_dims.size(); j++)
    //   printf("Input %d : dim %zu = %jd\n", i, j, input_node_dims[j]);
  }

  /////////////

  // input_node_dims[0] = 1;

  // std::vector<float> input_tensor_values(input_tensor_size);

  // for (unsigned int i = 0; i < input_tensor_size; i++)
  //   input_tensor_values[i] = (float)i / (input_tensor_size + 1);
  // auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
  // input_node_dims[0] = 1;
  // Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_size, input_node_dims.data(), 4);
  // assert(input_tensor.IsTensor());

  // // score model & input tensor, get back output tensor
  // auto output_tensors = session.Run(Ort::RunOptions{nullptr}, input_node_names.data(), &input_tensor, 1, output_node_names.data(), 2);
  // assert(output_tensors.size() == 2 && output_tensors[1].IsTensor());
  // std::cout<<"ok!!"<<std::endl;
  /////////////

  // run infer thread
  this->loop = std::make_unique<std::thread>([this]
                                             {
	  while (this->running) {
		  this->infer();
	  } });
}

NeuralNetwork::~NeuralNetwork()
{
  this->running = false;
  this->loop->join();
  // release buffers allocated by ORT alloctor
  for (const char *node_name : input_node_names)
  {
    if (node_name != nullptr)
    {
      try
      {
        allocator.Free(const_cast<void *>(reinterpret_cast<const void *>(node_name)));
      }
      catch (...)
      {
        // swallow any exception here; double-free detection will be caught by sanitizer/rt
      }
    }
  }
  input_node_names.clear();

  if (nullptr != this->shared_session)
  {
    this->shared_session.reset();
  }
}

std::future<NeuralNetwork::return_type> NeuralNetwork::commit(const Gomoku *gomoku)
{
  std::vector<float> state = transorm_gomoku_to_Tensor(gomoku);

  // emplace task
  std::promise<return_type> promise;
  auto ret = promise.get_future();

  {
    std::lock_guard<std::mutex> lock(this->lock);
    tasks.emplace(std::make_pair(std::move(state), std::move(promise)));
  }

  this->cv.notify_all();
  // std::cout<< "return promise !" << std::endl;
  return ret;
}

std::vector<float> NeuralNetwork::transorm_board_to_Tensor(const board_type &board, int last_move, int cur_player)
{
  auto input_tensor_values = std::vector<float>(CHANNEL_SIZE * BOARD_SIZE * BOARD_SIZE);
  int first = 0;
  int second = 0;
  if (cur_player == BLACK)
  {
    second = 1; // Black currently play = All black positions occupy the 0-th dimension in board
  }
  else
  {
    first = 1;
  }
  for (int r = 0; r < BOARD_SIZE; r++)
  {
    for (int c = 0; c < BOARD_SIZE; c++)
    {
      switch (board[r][c])
      {
      case 1:
        input_tensor_values[first * BOARD_SIZE * BOARD_SIZE + r * BOARD_SIZE + c] = 1;
        break;
      case -1:
        input_tensor_values[second * BOARD_SIZE * BOARD_SIZE + r * BOARD_SIZE + c] = 1;
        break;
      default:
        break;
      }
    }
    if (last_move >= 0)
    {
      input_tensor_values[2 * BOARD_SIZE * BOARD_SIZE + last_move] = 1;
    }
  }
  return input_tensor_values;

  // std::vector<int> board0;
  // std::vector<int> state0;
  // std::vector<int> state1;
  // for (unsigned int i = 0; i < BOARD_SIZE; i++) {
  //     board0.insert(board0.end(), board[i].begin(), board[i].end());
  // }

  // torch::Tensor temp =
  //     torch::from_blob(&board0[0], { 1, 1, BOARD_SIZE, BOARD_SIZE }, torch::dtype(torch::kInt32));

  // torch::Tensor state0 = temp.gt(0).toType(torch::kFloat32);
  // torch::Tensor state1 = temp.lt(0).toType(torch::kFloat32);

  // if (cur_player == -1) {
  //     std::swap(state0, state1);
  // }

  // torch::Tensor state2 =
  //     torch::zeros({ 1, 1, BOARD_SIZE, BOARD_SIZE }, torch::dtype(torch::kFloat32));

  // if (last_move != -1) {
  //     state2[0][0][last_move / BOARD_SIZE][last_move % BOARD_SIZE] = 1;
  // }

  // torch::Tensor states = torch::cat({ state0, state1 }, 1);
  //  return cat({ state0, state1, state2 }, 1);
}

std::vector<float> NeuralNetwork::transorm_gomoku_to_Tensor(const Gomoku *gomoku)
{
  return NeuralNetwork::transorm_board_to_Tensor(gomoku->get_board(), gomoku->get_last_move(), gomoku->get_current_color());
}

void NeuralNetwork::infer()
{
  //{
  //  this->module->eval();
  // torch::NoGradGuard no_grad;
  // torch::AutoGradMode enable_grad(false);
  // get inputs
  // std::vector<torch::Tensor> states;
  std::vector<std::vector<float>> states; // TODO class
  std::vector<std::promise<return_type>> promises;
  bool timeout = false;

  while (states.size() < this->batch_size && !timeout)
  {
    // pop task
    {
      std::unique_lock<std::mutex> lock(this->lock);
      if (this->cv.wait_for(lock, std::chrono::microseconds(1),
                            [this]
                            { return this->tasks.size() > 0; }))
      {
        auto task = std::move(this->tasks.front());
        states.emplace_back(std::move(task.first));
        promises.emplace_back(std::move(task.second));

        this->tasks.pop();
      }
      else
      {
        // timeout
        // std::cout << "timeout" << std::endl;
        timeout = true;
      }
    }
  }
  // inputs empty
  size_t size = states.size();
  if (size == 0)
  {
    return;
  }

  // set promise value
  this->input_node_dims[0] = promises.size();

  // std::cout<<"promises size  = "<<promises.size()<<std::endl;

  size_t input_tensor_size = input_node_dims[0] * CHANNEL_SIZE * BOARD_SIZE * BOARD_SIZE;
  std::vector<float> state_all(0);
  for (auto &item : states)
  {
    state_all.insert(state_all.end(), item.begin(), item.end());
  }

  // std::for_each(state_all.begin(), state_all.end(), [](double x) { std::cout << x << ","; });
  // std::cout << std::endl;

  Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, state_all.data(), input_tensor_size, this->input_node_dims.data(), 4);
  assert(input_tensor.IsTensor());

  auto output_tensors = shared_session->Run(Ort::RunOptions{nullptr}, input_node_names.data(), &input_tensor, 1, output_node_names.data(), 2);

  assert(output_tensors.size() == 2 && output_tensors[0].IsTensor() && output_tensors[1].IsTensor());

  float *V = output_tensors[0].GetTensorMutableData<float>();
  float *P = output_tensors[1].GetTensorMutableData<float>();

  for (unsigned int i = 0; i < promises.size(); i++)
  {

    // Get pointer to output tensor float values

    std::vector<double> prob(P + i * BOARD_SIZE * BOARD_SIZE, P + (i + 1) * BOARD_SIZE * BOARD_SIZE);
    std::vector<double> value{V[i]};
    // assert(prob.size() == BOARD_SIZE * BOARD_SIZE);

    for (int j = 0; j < BOARD_SIZE * BOARD_SIZE; j++)
    {
      prob[j] = std::exp(prob[j]);
      // printf("prob [%d] =  %f\n", j, prob[j]);
    }
    // printf("value [%d] =  %f\n", 0, V[0]);
    //  printf("value [%d] =  %f\n", 1, V[1]);

    // printf("prob [%d] =  %f\n", 0, P[0]);
    // printf("prob [%d] =  %f\n", 1, P[1]);
    // printf("prob [%d] =  %f\n", 2, P[2]);
    // printf("prob [%d] =  %f\n", 3, P[3]);

    return_type temp{std::move(prob), std::move(value)};

    promises[i].set_value(std::move(temp));
  }

  // #ifdef USE_GPU
  //     TS inputs{ cat(states, 0).to(at::kCUDA) };
  // #else
  //   TS inputs{ cat(states, 0) };
  // #endif

  // #ifdef JIT_MODE
  //     auto result = this->module->forward(inputs).toTuple();
  //     torch::Tensor p_batch = result->elements()[0]
  //         .toTensor()
  //         .exp()
  //         .toType(torch::kFloat32)
  //         .to(at::kCPU);
  //     torch::Tensor v_batch =
  //         result->elements()[1].toTensor().toType(torch::kFloat32).to(at::kCPU);
  // #else
  //     auto result = this->module->forward(inputs);
  //     //std::cout << y.requires_grad() << std::endl; // prints `false`

  //     Tensor p_batch = result.first.exp().toType(kFloat32).to(at::kCPU);
  //     Tensor v_batch = result.second.toType(kFloat32).to(at::kCPU);
  // #endif

  //   // set promise value
  //   for (unsigned int i = 0; i < promises.size(); i++) {
  //     torch::Tensor p = p_batch[i];
  //     torch::Tensor v = v_batch[i];

  //     std::vector<double> prob(static_cast<float*>(p.data_ptr()),
  //                              static_cast<float*>(p.data_ptr()) + p.size(0));
  //     std::vector<double> value{v.item<float>()};

  //     return_type temp{std::move(prob), std::move(value)};

  //     promises[i].set_value(std::move(temp));
  //   }
}

bool NeuralNetwork::set_batch_size(unsigned int u_batch_size)
{
  this->batch_size = u_batch_size;
  return true;
}
