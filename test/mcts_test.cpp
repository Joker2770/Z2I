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

#include "../src/mcts.h"
#include "../src/common.h"
#include "../src/onnx.h"

#include <iostream>

int main(int argc, char *argv[])
{
  auto g = std::make_shared<Gomoku>(BOARD_SIZE, N_IN_ROW, BLACK);
  // Gomoku g(15, 5, 1);
  // g.execute_move(12);
  // g->execute_move(12);
  // g->execute_move(13);
  // g->execute_move(14);
  // g->execute_move(15);
  // g->execute_move(16);
  //  g->execute_move(17);
  //  g->execute_move(18);
  //  g->execute_move(19);
  //  g->render();

  // Deserialize the ScriptModule from a file using torch::jit::load().
  // std::shared_ptr<torch::jit::script::Module> module = torch::jit::load("../test/models/checkpoint.pt");
  // torch::jit::script::Module module = torch::jit::load("../test/models/checkpoint.pt");

  std::shared_ptr<NeuralNetwork> module = nullptr;
  bool ai_black = true;
  if (argc <= 1)
  {
    // std::cout << "Do not load weights. AI color = BLACK." << std::endl;

    std::cout << "Warning: Find No weight path and color, assume they are mymodel and 1 (AI color:Black)" << std::endl;
#ifdef _WIN32
    module = std::make_shared<NeuralNetwork>("E:/Projects/AlphaZero-Onnx/python/mymodel.onnx", NUM_MCT_SIMS);
#else
    module = std::make_shared<NeuralNetwork>("$HOME/data/AlphaZero-Onnx/python/mymodel.onnx", NUM_MCT_SIMS);
#endif
  }
  else
  {
    ai_black = (strcmp(argv[2], "1") == 0) ? true : false;
    std::string color = ai_black ? "BLACK" : "WHITE";
    std::cout << "Load weights: " << argv[1] << "  AI color: " << color << std::endl;
    // wchar_t wchar[128] = {0};
    // swprintf(wchar,128,L"%S",argv[1]);

    module = std::make_shared<NeuralNetwork>(argv[1], NUM_MCT_SIMS);
  }
  // module->save_weights("net.pt");

  MCTS m(module.get(), NUM_MCT_THREADS, C_PUCT, NUM_MCT_SIMS, C_VIRTUAL_LOSS, BOARD_SIZE * BOARD_SIZE);

  std::cout << "Running..." << std::endl;

  char move_ic;
  int move_j;
  bool is_illlegal = true;
  std::pair<int, int> game_state;
  if (ai_black)
  {
    int res = m.get_best_action(g.get());
    m.update_with_move(res);
    g->execute_move(res);
  }

  while (true)
  {
    g->render();
    game_state = g->get_game_status();
    if (game_state.first != 0)
      break;
    int x, y;
    printf("your move: \n");
    std::cin >> move_ic >> move_j;
    x = move_ic - 'A';
    y = move_j - 1;
    is_illlegal = g->is_illegal(x, y);
    while (is_illlegal)
    {
      printf("Illegal move ! Please input \"character\" and \"number\" such as A 1 and ensure the position is empty !\n");
      printf("move again: \n");
      std::cin >> move_ic >> move_j;
      x = move_ic - 'A';
      y = move_j - 1;
      is_illlegal = g->is_illegal(x, y);
    }
    int my_move = x * BOARD_SIZE + y;
    m.update_with_move(my_move);
    g->execute_move(my_move);
    game_state = g->get_game_status();
    if (game_state.first != 0)
    {
      g->render();
      break;
    }

    int res = m.get_best_action(g.get());
    m.update_with_move(res);
    g->execute_move(res);

    // std::for_each(res.begin(), res.end(),
    //               [](double x) { std::cout << x << ","; });
    // std::cout << std::endl;
    // m.update_with_move(-1);
  }
  std::cout << "winner num = " << game_state.second << std::endl;

  return 0;
}
