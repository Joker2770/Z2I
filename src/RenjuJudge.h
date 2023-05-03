/*************************************************************************
    > File Name: RenjuJudge.h
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: Sun Apr 30 15:27:49 2023
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

#ifndef RENJUJUDGE_H
#define RENJUJUDGE_H

#include "rule.h"
#include <iostream>
using namespace std;

typedef enum pattern
{
    ROW,
    UNBROKEN_ROW,
    OVERLINE,
    FIVE_IN_A_ROW,
    FOUR,
    STRAIGHT_FOUR,
    THREE,
    DOUBLE_FOUR,
    DOUBLE_THREE
} PATTERN;

// 0 - pos empty, 1 - pos black, 2 - pos white
const int A4_SHAPES[][5] = {
    {0, 1, 1, 1, 1},
    {1, 0, 1, 1, 1},
    {1, 1, 0, 1, 1},
    {1, 1, 1, 0, 1},
    {1, 1, 1, 1, 0}};
// A3 is that add one stone could be A4, especially '2011102', '|01110|', '|011102' and '201110|' are Dead-Three. (| - boarder)
const int A3_SHAPES[][6] = {
    {0, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 0},
    {0, 1, 0, 1, 1, 0},
    {0, 1, 1, 0, 1, 0}};

class RenjuJudge final : public rule
{
public:
    std::pair<bool, int> checkWin(const board_type &board, int last_move) override;
    //after checkWin
    bool isLegal(const board_type &board, int last_move);
    int getRenjuState();
private:
    bool isOverLine(const board_type &board, int last_move);
    //after overline, include 44*
    bool isDoubleFour(const board_type &board, int last_move);
    //after double-four, only 43
    bool isFourThree(const board_type &board, int last_move);
    //after four-three, only 4 and no 3
    bool isFour(const board_type &board, int last_move);
    //after four, include 433*, 33*
    bool isDoubleThree(const board_type &board, int last_move);
    //after double-three, only 3
    bool isThree(const board_type &board, int last_move);
    int countA4(const board_type &board, int last_move, const pair<int, int>& p_drt);
    int countA3(const board_type &board, int last_move, const pair<int, int>& p_drt);
    bool isPosOutOfBoard(unsigned int n, int x, int y);
    int countNearStone(const board_type &board, int last_move, const pair<int, int> &p_drt);

    int m_renju_state;
};

#endif
