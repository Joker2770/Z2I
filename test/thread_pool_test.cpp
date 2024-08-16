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

#include "../src/thread_pool.h"

#include <iostream>

unsigned long long calc(unsigned long long n)
{

  return n == 0 ? 1 : n * calc(n - 1);
}

int main()
{

  ThreadPool thread_pool(4);

  auto r1 = thread_pool.commit(calc, 10);
  auto r2 = thread_pool.commit(calc, 50);
  auto r3 = thread_pool.commit(calc, 100);
  auto r4 = thread_pool.commit(calc, 200);

  std::cout << r1.get() << std::endl;
  std::cout << r2.get() << std::endl;
  std::cout << r3.get() << std::endl;
  std::cout << r4.get() << std::endl;

  std::cout << "END" << std::endl;

  return 0;
}
