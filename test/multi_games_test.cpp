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

#include "mcts.h"
#include "onnx.h"
#include "play.h"
#include "common.h"

#include <iostream>

int main() {
  NeuralNetwork *model = new NeuralNetwork(8);
  //torch::optim::SGD optimizer(model->module->parameters(), /*lr=*/0.01);
  SelfPlay *sp = new SelfPlay(model);
  auto train_buffer = sp->self_play_for_train(3);
  std::cout << "3 train size = " << std::get<0>(train_buffer).size() << " " <<
      std::get<1>(train_buffer).size() << " " << std::get<2>(train_buffer).size() << std::endl;
  model->train(std::get<0>(train_buffer), std::get<1>(train_buffer),std::get<2>(train_buffer));
  model->save_weights("1.pt");
  return 0;
}

