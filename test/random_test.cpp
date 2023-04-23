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

#include <iostream>
#include <random>

using namespace std;

int main() {
    std::vector<int> rand_order;
    for (int i = 0; i < 10; i++) {
        rand_order.push_back(i);
    }

    std::vector<float> rand_order2;
    for (int i = 0; i < 10; i++) {
        rand_order2.push_back((float)i/100);
    }
    unsigned seed = 0;
    auto e = default_random_engine(seed);
    shuffle(rand_order.begin(), rand_order.end(), e);
    e = default_random_engine(seed);
    shuffle(rand_order2.begin(), rand_order2.end(), e);
    for (int i = 0; i < rand_order.size(); i++) {
        cout << rand_order[i] << endl;
    }
    for (int i = 0; i < rand_order2.size(); i++) {
        cout << rand_order2[i] << endl;
    }
}
