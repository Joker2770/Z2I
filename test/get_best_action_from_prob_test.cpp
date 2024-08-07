/**
MIT License

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
#include "common.h"
#include "onnx.h"

#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
  Gomoku *g = new Gomoku(BOARD_SIZE, N_IN_ROW, BLACK);
  if (nullptr == g)
  {
    return -1;
  }
  
  std::shared_ptr<NeuralNetwork> module = nullptr;
  if (argc <= 1) {
      //cout << "Do not load weights. AI color = BLACK." << endl;
      
      cout << "Warning: Find No weight path and color, assume they are mymodel and 1 (AI color:Black)" << endl;
#ifdef _WIN32
  module = std::make_shared<NeuralNetwork>("E:/Projects/AlphaZero-Onnx/python/mymodel.onnx", NUM_MCT_SIMS);
#else
  module = std::make_shared<NeuralNetwork>("$HOME/data/AlphaZero-Onnx/python/mymodel.onnx", NUM_MCT_SIMS);
#endif
  }
  else {
      cout << "Load weights: "<< argv[1] << endl;
      // wchar_t wchar[128] = {0};
      // swprintf(wchar,128,L"%S",argv[1]);

      module = std::make_shared<NeuralNetwork>(argv[1], NUM_MCT_SIMS);
  }
  //module->save_weights("net.pt");
  if (nullptr == module)
  {
    return -2;
  }
  
  MCTS *m = new MCTS(module.get(), NUM_MCT_THREADS, C_PUCT, NUM_MCT_SIMS, C_VIRTUAL_LOSS, BOARD_SIZE * BOARD_SIZE);
  if (nullptr == m)
  {
    return -3;
  }

  std::cout << "Running..." << std::endl;

  g->execute_move(112);
  g->execute_move(127);
  g->execute_move(128);
  g->execute_move(144);
  g->execute_move(96);
  g->execute_move(97);
  g->render();
  std::cout << "Thinking..." << std::endl;
  std::vector<double> p = m->get_action_probs(g);
  std::cout << "Get action probs..." << std::endl;
  int action = m->get_best_action_from_prob(p);
  std::cout << "Get best action from prob: " << action << std::endl;
  m->update_with_move(action);
  g->execute_move(action);
  g->render();

  if (nullptr != m)
  {
      delete m;
      m = nullptr;
  }
  if (nullptr != g)
  {
      delete g;
      g = nullptr;
  }

  return 0;
}

