/*************************************************************************
    > File Name: CaroJudge.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com
    > Created Time: Mon May  1 16:44:21 2023
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

#include "CaroJudge.h"

#include <algorithm>
#include <iostream>
using namespace std;

bool CaroJudge::findShap(const board_type &board, int last_move, const pair<int, int> &p_drt)
{
    vector<int> vColor;

    size_t n = board.size();
    pair<int, int> p_idx((int)(last_move / n), (int)(last_move % n));
    pair<int, int> p_drt_idx(p_idx.first + p_drt.first, p_idx.second + p_drt.second);

    // push back current stone color
    if (1 == board[last_move / n][last_move % n])
        vColor.push_back(1);
    else if (-1 == board[last_move / n][last_move % n])
        vColor.push_back(2);
    else
        return false;

    while (true)
    {
        if (1 == board[p_drt_idx.first][p_drt_idx.second])
            vColor.push_back(1);
        else if (-1 == board[p_drt_idx.first][p_drt_idx.second])
            vColor.push_back(2);
        else if (this->isPosOutOfBoard((unsigned int)n, p_drt_idx.first, p_drt_idx.second))
            vColor.push_back(3);
        else
            vColor.push_back(0);

        p_drt_idx.first += p_drt.first;
        p_drt_idx.second += p_drt.second;
        if (abs(p_idx.first - p_drt_idx.first) > 5 || abs(p_idx.second - p_drt_idx.second) > 5)
            break;
    }

    reverse(vColor.begin(), vColor.end());
    p_drt_idx.first = p_idx.first - p_drt.first;
    p_drt_idx.second = p_idx.second - p_drt.second;
    while (true)
    {
        if (1 == board[p_drt_idx.first][p_drt_idx.second])
            vColor.push_back(1);
        else if (-1 == board[p_drt_idx.first][p_drt_idx.second])
            vColor.push_back(2);
        else if (this->isPosOutOfBoard((unsigned int)n, p_drt_idx.first, p_drt_idx.second))
            vColor.push_back(3);
        else
            vColor.push_back(0);

        p_drt_idx.first -= p_drt.first;
        p_drt_idx.second -= p_drt.second;
        if (abs(p_idx.first - p_drt_idx.first) > 5 || abs(p_idx.second - p_drt_idx.second) > 5)
            break;
    }

    // for (size_t i = 0; i < vColor.size(); i++)
    // {
    //     cout << vColor[i] ;
    // }
    // cout <<endl;

    if (vColor.size() >= 7)
    {
        size_t win_shape_size = sizeof(WIN_SHAPES) / sizeof(WIN_SHAPES[0]);
        for (size_t j = 0; j <= vColor.size() - 7; ++j)
        {
            for (size_t i = 0; i < win_shape_size; ++i)
            {
                if (((WIN_SHAPES[i][0]) == (vColor[j])) &&
                    ((WIN_SHAPES[i][1]) == (vColor[j + 1])) &&
                    ((WIN_SHAPES[i][2]) == (vColor[j + 2])) &&
                    ((WIN_SHAPES[i][3]) == (vColor[j + 3])) &&
                    ((WIN_SHAPES[i][4]) == (vColor[j + 4])) &&
                    ((WIN_SHAPES[i][5]) == (vColor[j + 5])) &&
                    ((WIN_SHAPES[i][6]) == (vColor[j + 6])))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool CaroJudge::isPosOutOfBoard(unsigned int n, int x, int y)
{
    return ((unsigned int)x > n - 1) || ((unsigned int)y > n - 1) || x < 0 || y < 0;
}

bool CaroJudge::checkWin(const board_type &board, int last_move)
{
    if (last_move < 0)
    {
        return false;
    }

    size_t n = board.size();
    pair<int, int> p_drt_up(0, -1), p_drt_left(-1, 0), p_drt_leftup(-1, -1), p_drt_leftdown(-1, 1);
    bool b_up = findShap(board, last_move, p_drt_up);
    bool b_left = findShap(board, last_move, p_drt_left);
    bool b_leftup = findShap(board, last_move, p_drt_leftup);
    bool b_leftdown = findShap(board, last_move, p_drt_leftdown);

    if (b_up || b_left || b_leftup || b_leftdown)
        return true;

    return false;
}
