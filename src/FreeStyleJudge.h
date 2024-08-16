/*************************************************************************
    > File Name: FreeStyleJudge.h
    > Author: Jintao Yang
    > Mail: 18608842770@163.com
    > Created Time: Sun Apr 30 15:27:01 2023
 ************************************************************************/

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

#ifndef FREESTYLEJUDGE_H
#define FREESTYLEJUDGE_H

#include "rule.h"
#include <iostream>
using namespace std;

class FreeStyleJudge final : public rule
{
public:
    bool checkWin(const board_type &board, int last_move) override;

private:
    bool isPosOutOfBoard(unsigned int n, int x, int y);
    int countNearStone(const board_type &board, int last_move, const pair<int, int> &p_drt);
};

#endif
