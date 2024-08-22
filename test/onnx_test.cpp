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

#include "../src/gomoku.h"
#include "../src/common.h"
#include "../src/onnx.h"

#include <iostream>
#include <onnxruntime_cxx_api.h>
#include <algorithm>

int main()
{
  Gomoku gomoku(BOARD_SIZE, N_IN_ROW, BLACK);

  // test execute_move
  gomoku.execute_move(0);
  gomoku.execute_move(1);
  gomoku.execute_move(49);
  // gomoku.execute_move(4);
  // gomoku.execute_move(8);
  // gomoku.execute_move(9);

  // test render
  // gomoku.render();

  std::cout << gomoku.get_last_move() << std::endl;
  std::cout << gomoku.get_current_color() << std::endl;

  // NeuralNetwork nn(1);
#ifdef _WIN32
  string path = "E:/Projects/AlphaZero-Onnx/python/mymodel.onnx";
#else
  string path = "/data/AlphaZero-Onnx/python/mymodel.onnx";
#endif

  NeuralNetwork nn(path, 4);

  // 1
  auto res = nn.commit(&gomoku).get();
  auto p = res[0];
  auto v = res[1];

  std::for_each(p.begin(), p.end(), [](double x)
                { std::cout << x << ","; });
  std::cout << std::endl;

  std::cout << "V =" << v[0] << std::endl;

  // 2
  // gomoku.execute_move(2);
  // std::cout << gomoku.get_last_move() << std::endl;
  // std::cout << gomoku.get_current_color() << std::endl;

  // res = nn.commit(&gomoku).get();
  // p = res[0];
  // v = res[1];

  // std::for_each(p.begin(), p.end(), [](double x) { std::cout << x << ","; });
  // std::cout << std::endl;

  // std::cout << v << std::endl;

  // stress testing
  // std::cout << "stress testing" << std::endl;
  // auto start = std::chrono::system_clock::now();

  // for (unsigned i = 0; i < 100; i++) {
  //   nn.commit(&gomoku);
  // }

  // auto res = nn.commit(&gomoku).get();
  // auto end = std::chrono::system_clock::now();

  // std::cout <<  "cost time:" << double(std::chrono::duration_cast<std::chrono::microseconds>(end -
  //                                                                    start)
  //                      .count()) *
  //                  std::chrono::microseconds::period::num /
  //                  std::chrono::microseconds::period::den
  //           << std::endl;

  // auto p = res[0];
  // auto v = res[1];

  // std::for_each(p.begin(), p.end(), [](double x) { std::cout << x << ","; });
  // std::cout << std::endl;

  // std::cout << v[0] << std::endl;
}
