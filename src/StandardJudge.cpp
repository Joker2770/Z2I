/*************************************************************************
    > File Name: StandardJudge.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com
    > Created Time: Sun Apr 30 15:27:35 2023
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

#include "StandardJudge.h"
#include <iostream>

bool StandardJudge::isPosOutOfBoard(unsigned int n, int x, int y)
{
    return ((unsigned int)x > n - 1) || ((unsigned int)y > n - 1) || x < 0 || y < 0;
}

int StandardJudge::countNearStone(const board_type &board, int last_move, const std::pair<int, int> &p_drt)
{
    int i_count = 0;
    if (-1 == last_move)
        return 0;

    size_t n = board.size();
    std::pair<int, int> p_idx((int)(last_move / n), (int)(last_move % n));
    std::pair<int, int> p_drt_idx(p_idx.first + p_drt.first, p_idx.second + p_drt.second);

    while (!isPosOutOfBoard((unsigned int)n, p_drt_idx.first, p_drt_idx.second) && 0 != board[p_drt_idx.first][p_drt_idx.second])
    {
        if (board[p_drt_idx.first][p_drt_idx.second] == board[p_idx.first][p_idx.second])
            i_count++;
        else
            return i_count;

        p_drt_idx.first += p_drt.first;
        p_drt_idx.second += p_drt.second;
        if (abs(p_idx.first - p_drt_idx.first) > 5 || abs(p_idx.second - p_drt_idx.second) > 5)
            break;
    }

    return i_count;
}

bool StandardJudge::checkWin(const board_type &board, int last_move)
{
    if (last_move < 0)
    {
        if (last_move == -1)
            return true;
        else
            return false;
    }

    size_t n = board.size();

    std::pair<int, int> p_drt_up(0, -1),
        p_drt_down(0, 1),
        p_drt_left(-1, 0),
        p_drt_right(1, 0),
        p_drt_leftup(-1, -1),
        p_drt_rightdown(1, 1),
        p_drt_rightup(1, -1),
        p_drt_leftdown(-1, 1);

    int i_up = countNearStone(board, last_move, p_drt_up);
    int i_down = countNearStone(board, last_move, p_drt_down);
    int i_left = countNearStone(board, last_move, p_drt_left);
    int i_right = countNearStone(board, last_move, p_drt_right);
    int i_leftup = countNearStone(board, last_move, p_drt_leftup);
    int i_rightdown = countNearStone(board, last_move, p_drt_rightdown);
    int i_leftdown = countNearStone(board, last_move, p_drt_leftdown);
    int i_rightup = countNearStone(board, last_move, p_drt_rightup);

    if (i_up + i_down == 4 || i_left + i_right == 4 || i_leftup + i_rightdown == 4 || i_leftdown + i_rightup == 4)
        return true;

    return false;
}
