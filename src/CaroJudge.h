/*************************************************************************
    > File Name: CaroJudge.h
    > Author: Jintao Yang
    > Mail: 18608842770@163.com
    > Created Time: Mon May  1 16:44:08 2023
 ************************************************************************/

/**
MIT License

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

#ifndef CAROJUDGE_H
#define CAROJUDGE_H

#include "rule.h"
#include <iostream>
using namespace std;

const int WIN_SHAPES[][7] = {
    {1, 1, 1, 1, 1, 0, 0},
    {1, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 1, 1, 0, 2},
    {1, 1, 1, 1, 1, 0, 3},
    {0, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 1},
    {1, 0, 1, 1, 1, 1, 1},
    {2, 0, 1, 1, 1, 1, 1},
    {3, 0, 1, 1, 1, 1, 1},
    {2, 1, 1, 1, 1, 1, 0},
    {2, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 2},
    {1, 1, 1, 1, 1, 1, 2},
    {1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 0},
    {3, 1, 1, 1, 1, 1, 0},
    {3, 1, 1, 1, 1, 1, 1},
    {3, 1, 1, 1, 1, 1, 2},
    {3, 1, 1, 1, 1, 1, 3},
    {0, 1, 1, 1, 1, 1, 3},
    {1, 1, 1, 1, 1, 1, 3},
    {2, 1, 1, 1, 1, 1, 3},
    {2, 2, 2, 2, 2, 0, 0},
    {2, 2, 2, 2, 2, 0, 1},
    {2, 2, 2, 2, 2, 0, 2},
    {2, 2, 2, 2, 2, 0, 3},
    {0, 2, 2, 2, 2, 2, 0},
    {0, 0, 2, 2, 2, 2, 2},
    {1, 0, 2, 2, 2, 2, 2},
    {2, 0, 2, 2, 2, 2, 2},
    {3, 0, 2, 2, 2, 2, 2},
    {1, 2, 2, 2, 2, 2, 0},
    {1, 2, 2, 2, 2, 2, 2},
    {0, 2, 2, 2, 2, 2, 1},
    {2, 2, 2, 2, 2, 2, 1},
    {2, 2, 2, 2, 2, 2, 2},
    {0, 2, 2, 2, 2, 2, 2},
    {2, 2, 2, 2, 2, 2, 0},
    {3, 2, 2, 2, 2, 2, 0},
    {3, 2, 2, 2, 2, 2, 1},
    {3, 2, 2, 2, 2, 2, 2},
    {3, 2, 2, 2, 2, 2, 3},
    {0, 2, 2, 2, 2, 2, 3},
    {1, 2, 2, 2, 2, 2, 3},
    {2, 2, 2, 2, 2, 2, 3}};

class CaroJudge final : public rule
{
public:
    bool checkWin(const board_type &board, int last_move) override;

private:
    bool isPosOutOfBoard(unsigned int n, int x, int y);
    bool findShap(const board_type &board, int last_move, const pair<int, int> &p_drt);
};

#endif
