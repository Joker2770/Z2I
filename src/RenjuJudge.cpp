/*************************************************************************
    > File Name: RenjuJudge.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: Sun Apr 30 15:27:55 2023
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

#include "RenjuJudge.h"
#include <algorithm>
#include <iostream>
using namespace std;

bool RenjuJudge::isOverLine(const board_type &board, int last_move)
{
    pair<int, int> p_drt_up(0, -1), p_drt_down(0, 1), p_drt_left(-1, 0), p_drt_right(1, 0), p_drt_leftup(-1, -1), p_drt_rightdown(1, 1), p_drt_rightup(1, -1), p_drt_leftdown(-1, 1);
    int i_up = countNearStone(board, last_move, p_drt_up);
    int i_down = countNearStone(board, last_move, p_drt_down);
    int i_left = countNearStone(board, last_move, p_drt_left);
    int i_right = countNearStone(board, last_move, p_drt_right);
    int i_leftup = countNearStone(board, last_move, p_drt_leftup);
    int i_rightdown = countNearStone(board, last_move, p_drt_rightdown);
    int i_leftdown = countNearStone(board, last_move, p_drt_leftdown);
    int i_rightup = countNearStone(board, last_move, p_drt_rightup);

    if (i_up + i_down > 4 || i_left + i_right > 4 || i_leftup + i_rightdown > 4 || i_leftdown + i_rightup > 4)
        return true;

    return false;
}

int RenjuJudge::countA4(const board_type &board, int last_move, const pair<int, int>& p_drt)
{
    int i_count = 0;

    vector<int> vColor;

    unsigned int n = board.size();
    pair<int, int> p_idx(last_move / n, last_move % n);
    pair<int, int> p_drt_idx(p_idx.first + p_drt.first, p_idx.second + p_drt.second);

    // push back current stone color
    if (1 == board[last_move / n][last_move % n])
        vColor.push_back(1);
    else if (-1 == board[last_move / n][last_move % n])
        vColor.push_back(2);
    else
        return 0;

    while (!this->isPosOutOfBoard(n, p_drt_idx.first, p_drt_idx.second))
    {
        if (1 == board[p_drt_idx.first][p_drt_idx.second])
            vColor.push_back(1);
        else if (-1 == board[p_drt_idx.first][p_drt_idx.second])
            vColor.push_back(2);
        else
            vColor.push_back(0);

        p_drt_idx.first += p_drt.first;
        p_drt_idx.second += p_drt.second;
        if (abs(p_idx.first - p_drt_idx.first) > 4 || abs(p_idx.second - p_drt_idx.second) > 4)
            break;
    }

    reverse(vColor.begin(), vColor.end());
    p_drt_idx.first = p_idx.first - p_drt.first;
    p_drt_idx.second = p_idx.second - p_drt.second;
    while (!this->isPosOutOfBoard(n,p_drt_idx.first,p_drt_idx.second))
    {
        if (1 == board[p_drt_idx.first][p_drt_idx.second])
            vColor.push_back(1);
        else if (-1 == board[p_drt_idx.first][p_drt_idx.second])
            vColor.push_back(2);
        else
            vColor.push_back(0);

        p_drt_idx.first -= p_drt.first;
        p_drt_idx.second -= p_drt.second;
        if (abs(p_idx.first - p_drt_idx.first) > 4 || abs(p_idx.second - p_drt_idx.second) > 4)
            break;
    }

    // for (size_t i = 0; i < vColor.size(); i++)
    // {
    //     cout << vColor[i] ;
    // }
    // cout <<endl;

    if (vColor.size() >= 5)
    {
        int i_flag = 0;
        bool b_1_before_3 = false;
        for (size_t j = 0; j <= vColor.size() - 5; ++j)
        {
            for (size_t i = 0; i < 5; ++i)
            {
                if (((A4_SHAPES[i][0]) == (vColor[j])) &&
                    ((A4_SHAPES[i][1]) == (vColor[j + 1])) &&
                    ((A4_SHAPES[i][2]) == (vColor[j + 2])) &&
                    ((A4_SHAPES[i][3]) == (vColor[j + 3])) &&
                    ((A4_SHAPES[i][4]) == (vColor[j + 4])))
                {
                    if ((i == 1 || i == 3)) //'1011101' at left3 to right3, '10111101' at left4 and right4, ‘101111101’ at mid, '111010111' at mid, '11110111', '11101111', '111011101', '101110111'
                    {
                        if (i == 1)
                            i_flag |= 0x01;
                        else if (i == 3)
                        {
                            // if already have 1
                            if ((i_flag & 0x01) == 0x01)
                            {
                                b_1_before_3 = true;
                            }
                            i_flag |= 0x02;
                        }

                        break;
                    }
                    else if (i == 2) //'11011011' at left4 and right4, '11110111', '11101111', '111011101', '101110111'
                    {
                        if ((i_flag & 0x04) != 0x04)
                            i_flag |= 0x04;
                        else if ((i_flag & 0x08) != 0x08)
                            i_flag |= 0x08;

                        break;
                    }
                    else //'11110011', '11001111', '10110111', '11101101'
                        i_count = 1;
                }
            }
        }

        if ((i_flag & 0x0F) == 0x0F) // '110111011'
            i_count = 2;
        else if ((i_flag & 0x07) == 0x07) // '11011101', '10111011', '111011101', '101110111', '11101111' or '11110111'
        {
            if (b_1_before_3)
                i_count = 2;
            else
                i_count = 1;
        }
        else if ((i_flag & 0x03) == 0x03) //'1011101', '10111101'
            i_count = 2;
        else if ((i_flag & (0x04 | 0x08)) == (0x04 | 0x08)) //'11011011'
            i_count = 2;
        else if (((i_flag & 0x03) == 0x01) || ((i_flag & 0x03) == 0x02))
            i_count = 1;
        else if (((i_flag & 0x04) == 0x04) || ((i_flag & 0x08) == 0x08))
            i_count = 1;
    }

    return i_count;
}

bool RenjuJudge::isDoubleFour(const board_type &board, int last_move)
{
    pair<int, int> p_drt_up(0, -1), p_drt_left(-1, 0), p_drt_leftup(-1, -1), p_drt_leftdown(-1, 1);
    int i_up = countA4(board, last_move, p_drt_up);
    int i_left = countA4(board, last_move, p_drt_left);
    int i_leftup = countA4(board, last_move, p_drt_leftup);
    int i_leftdown = countA4(board, last_move, p_drt_leftdown);

    if (i_up + i_left + i_leftup + i_leftdown >= 2)
        return true;

    return false;
}

int RenjuJudge::countA3(const board_type &board, int last_move, const pair<int, int> &p_drt)
{
    vector<int> vColor;

    unsigned int n = board.size();
    pair<int, int> p_idx(last_move / n, last_move % n);
    pair<int, int> p_drt_idx(p_idx.first + p_drt.first, p_idx.second + p_drt.second);

    // push back current stone color
    if (1 == board[last_move / n][last_move % n])
        vColor.push_back(1);
    else if (-1 == board[last_move / n][last_move % n])
        vColor.push_back(2);
    else
        return 0;

    while (!this->isPosOutOfBoard(n, p_drt_idx.first, p_drt_idx.second))
    {
        if (1 == board[p_drt_idx.first][p_drt_idx.second])
            vColor.push_back(1);
        else if (-1 == board[p_drt_idx.first][p_drt_idx.second])
            vColor.push_back(2);
        else
            vColor.push_back(0);

        p_drt_idx.first += p_drt.first;
        p_drt_idx.second += p_drt.second;
        if (abs(p_idx.first - p_drt_idx.first) > 4 || abs(p_idx.second - p_drt_idx.second) > 4)
            break;
    }

    reverse(vColor.begin(), vColor.end());
    p_drt_idx.first = p_idx.first - p_drt.first;
    p_drt_idx.second = p_idx.second - p_drt.second;
    while (!this->isPosOutOfBoard(n, p_drt_idx.first, p_drt_idx.second))
    {
        if (1 == board[p_drt_idx.first][p_drt_idx.second])
            vColor.push_back(1);
        else if (-1 == board[p_drt_idx.first][p_drt_idx.second])
            vColor.push_back(2);
        else
            vColor.push_back(0);

        p_drt_idx.first -= p_drt.first;
        p_drt_idx.second -= p_drt.second;
        if (abs(p_idx.first - p_drt_idx.first) > 4 || abs(p_idx.second - p_drt_idx.second) > 4)
            break;
    }

    // for (size_t i = 0; i < vColor.size(); i++)
    // {
    //     cout << vColor[i] ;
    // }
    // cout <<endl;

    if (vColor.size() >= 6)
    {
        for (size_t j = 0; j <= vColor.size() - 6; ++j)
        {
            for (size_t i = 0; i < 4; ++i)
            {
                // cout << "A3_element: " << A3_SHAPES[i][0] << A3_SHAPES[i][1] << A3_SHAPES[i][2] << A3_SHAPES[i][3] << A3_SHAPES[i][4] << A3_SHAPES[i][5];
                // cout << "vColor_element: " << vColor[j] << vColor[j + 1] << vColor[j + 2] << vColor[j + 3] << vColor[j + 4] << vColor[j + 5];
                if (((A3_SHAPES[i][0]) == (vColor[j])) &&
                    ((A3_SHAPES[i][1]) == (vColor[j + 1])) &&
                    ((A3_SHAPES[i][2]) == (vColor[j + 2])) &&
                    ((A3_SHAPES[i][3]) == (vColor[j + 3])) &&
                    ((A3_SHAPES[i][4]) == (vColor[j + 4])) &&
                    ((A3_SHAPES[i][5]) == (vColor[j + 5])))
                {
                    // Never could be a double-three(alive) in one line. ('101101' is not alive)

                    /**
                     * @brief 9.3 A black double-three is allowed if at least one of the following conditions  a)   or  b) is/are fulfilled:
                     * a) Not more than one of the three's can be made to a straight four when adding another stone in just any intersection,
                     *  without at the same time an overline or double-four is attained in this intersection. To find out which double-three's,
                     *  which are allowed, you must make the move, which causes the double-three, in your mind, and then continue trying to
                     *  make straight fours, which are allowed, in your mind.
                     * b) Not more than one of the three's can be made to a straight four when adding another stone in just any intersection,
                     *  without at the same time at least two three's meet in this intersection and make a forbidden double-three.
                     *  To find out if this last double-three is forbidden or not, you must at first examine if the double-three is
                     *  allowed according to a) above, and then in your mind continue trying to make straight fours of the three's in your mind.
                     *  If, when making a straight four in your mind, another double-three would be attained also these double-three's must be
                     *  examined in the same way as it is described in this point 9.3, etc.
                     *
                     */
                    return 1;
                }
            }
        }
    }

    return 0;
}

bool RenjuJudge::isFourThree(const board_type &board, int last_move)
{
    pair<int, int> p_drt_up(0, -1), p_drt_left(-1, 0), p_drt_leftup(-1, -1), p_drt_leftdown(-1, 1);
    int i_up_4 = countA4(board, last_move, p_drt_up);
    int i_left_4 = countA4(board, last_move, p_drt_left);
    int i_leftup_4 = countA4(board, last_move, p_drt_leftup);
    int i_leftdown_4 = countA4(board, last_move, p_drt_leftdown);
    int i_up_3 = countA3(board, last_move, p_drt_up);
    int i_left_3 = countA3(board, last_move, p_drt_left);
    int i_leftup_3 = countA3(board, last_move, p_drt_leftup);
    int i_leftdown_3 = countA3(board, last_move, p_drt_leftdown);

    if (((i_up_4 + i_left_4 + i_leftup_4 + i_leftdown_4) == 1) && (i_up_3 + i_left_3 + i_leftup_3 + i_leftdown_3 >= 1))
    {
        if (i_up_4 == 1)
        {
            if (i_left_3 + i_leftup_3 + i_leftdown_3 == 0) // only four
                return false;
            else if (i_left_3 + i_leftup_3 + i_leftdown_3 > 1) // 433*
                return false;
            else if (i_left_3 + i_leftup_3 + i_leftdown_3 == 1)
                return true;
        }
        else if (i_left_4 == 1)
        {
            if (i_up_3 + i_leftup_3 + i_leftdown_3 == 0) // only four
                return false;
            else if (i_up_3 + i_leftup_3 + i_leftdown_3 > 1) // 433*
                return false;
            else if (i_up_3 + i_leftup_3 + i_leftdown_3 == 1)
                return true;
        }
        else if (i_leftup_4 == 1)
        {
            if (i_up_3 + i_left_3 + i_leftdown_3 == 0) // only four
                return false;
            else if (i_up_3 + i_left_3 + i_leftdown_3 > 1) // 433*
                return false;
            else if (i_up_3 + i_left_3 + i_leftdown_3 == 1)
                return true;
        }
        else if (i_leftdown_4 == 1)
        {
            if (i_left_3 + i_leftup_3 + i_up_3 == 0) // only four
                return false;
            else if (i_left_3 + i_leftup_3 + i_up_3 > 1) //433*
                return false;
            else if (i_left_3 + i_leftup_3 + i_up_3 == 1)
                return true;
        }
    }

    return false;
}

bool RenjuJudge::isFour(const board_type &board, int last_move)
{
    pair<int, int> p_drt_up(0, -1), p_drt_left(-1, 0), p_drt_leftup(-1, -1), p_drt_leftdown(-1, 1);
    int i_up_4 = countA4(board, last_move, p_drt_up);
    int i_left_4 = countA4(board, last_move, p_drt_left);
    int i_leftup_4 = countA4(board, last_move, p_drt_leftup);
    int i_leftdown_4 = countA4(board, last_move, p_drt_leftdown);
    int i_up_3 = countA3(board, last_move, p_drt_up);
    int i_left_3 = countA3(board, last_move, p_drt_left);
    int i_leftup_3 = countA3(board, last_move, p_drt_leftup);
    int i_leftdown_3 = countA3(board, last_move, p_drt_leftdown);

    if (((i_up_4 + i_left_4 + i_leftup_4 + i_leftdown_4) == 1) && (i_up_3 + i_left_3 + i_leftup_3 + i_leftdown_3 < 2))
    {
        if (i_up_4 == 1)
        {
            if (i_left_3 + i_leftup_3 + i_leftdown_3 == 0)
                return true;
        }
        else if (i_left_4 == 1)
        {
            if (i_up_3 + i_leftup_3 + i_leftdown_3 == 0)
                return true;
        }
        else if (i_leftup_4 == 1)
        {
            if (i_up_3 + i_left_3 + i_leftdown_3 == 0)
                return true;
        }
        else if (i_leftdown_4 == 1)
        {
            if (i_up_3 + i_left_3 + i_leftup_3 == 0)
                return true;
        }
    }

    return false;
}

bool RenjuJudge::isDoubleThree(const board_type &board, int last_move)
{
    pair<int, int> p_drt_up(0, -1), p_drt_left(-1, 0), p_drt_leftup(-1, -1), p_drt_leftdown(-1, 1);
    int i_up_4 = countA4(board, last_move, p_drt_up);
    int i_left_4 = countA4(board, last_move, p_drt_left);
    int i_leftup_4 = countA4(board, last_move, p_drt_leftup);
    int i_leftdown_4 = countA4(board, last_move, p_drt_leftdown);
    int i_up_3 = countA3(board, last_move, p_drt_up);
    int i_left_3 = countA3(board, last_move, p_drt_left);
    int i_leftup_3 = countA3(board, last_move, p_drt_leftup);
    int i_leftdown_3 = countA3(board, last_move, p_drt_leftdown);

    if ((i_up_4 + i_left_4 + i_leftup_4 + i_leftdown_4 < 2) && (i_up_3 + i_left_3 + i_leftup_3 + i_leftdown_3 >= 2))
    {
        if (i_up_4 + i_left_4 + i_leftup_4 + i_leftdown_4 == 0)
        {
            if (i_up_3 + i_left_3 + i_leftup_3 + i_leftdown_3 >= 2)
                return true;
        }
        else
        {
            if (i_up_4 == 1)
            {
                if (i_left_3 + i_leftup_3 + i_leftdown_3 >= 2) // 433*
                    return true;
                else if (i_left_3 + i_leftup_3 + i_leftdown_3 == 1) // 43
                    return false;
                else // only 4
                    return false;
            }
            else if (i_left_4 == 1)
            {
                if (i_up_3 + i_leftup_3 + i_leftdown_3 >= 2) // 433*
                    return true;
                else if (i_up_3 + i_leftup_3 + i_leftdown_3 == 1) // 43
                    return false;
                else // only 4
                    return false;
            }
            else if (i_leftup_4 == 1)
            {
                if (i_up_3 + i_left_3 + i_leftdown_3 >= 2) // 433*
                    return true;
                else if (i_up_3 + i_left_3 + i_leftdown_3 == 1) // 43
                    return false;
                else // only 4
                    return false;
            }
            else if (i_leftdown_4 == 1)
            {
                if (i_up_3 + i_leftup_3 + i_left_3 >= 2) // 433*
                    return true;
                else if (i_up_3 + i_leftup_3 + i_left_3 == 1) // 43
                    return false;
                else // only 4
                    return false;
            }
        }
    }

    return false;
}

bool RenjuJudge::isThree(const board_type &board, int last_move)
{
    pair<int, int> p_drt_up(0, -1), p_drt_left(-1, 0), p_drt_leftup(-1, -1), p_drt_leftdown(-1, 1);
    int i_up_4 = countA4(board, last_move, p_drt_up);
    int i_left_4 = countA4(board, last_move, p_drt_left);
    int i_leftup_4 = countA4(board, last_move, p_drt_leftup);
    int i_leftdown_4 = countA4(board, last_move, p_drt_leftdown);
    int i_up_3 = countA3(board, last_move, p_drt_up);
    int i_left_3 = countA3(board, last_move, p_drt_left);
    int i_leftup_3 = countA3(board, last_move, p_drt_leftup);
    int i_leftdown_3 = countA3(board, last_move, p_drt_leftdown);

    if ((i_up_4 + i_left_4 + i_leftup_4 + i_leftdown_4 == 0) && (i_up_3 + i_left_3 + i_leftup_3 + i_leftdown_3 == 1))
        return true;

    return false;
}

bool RenjuJudge::isLegal(const board_type &board, int last_move)
{
    if (last_move < 0)
    {
        if (last_move == -1)
            return true;
        else
            return false;
    }

    unsigned int n = board.size();
    if (board[last_move/n][last_move%n] == 1)
    {
        if (this->isOverLine(board, last_move))
        {
            // cout << "Over-Line";
            this->m_renju_state = PATTERN::OVERLINE;
            return false;
        }
        else if (this->isDoubleFour(board, last_move))
        {
            // cout << "Double-Four";
            this->m_renju_state = PATTERN::DOUBLE_FOUR;
            return false;
        }
        else if (this->isFourThree(board, last_move))
        {
            // cout << "Four-Three";
            return true;
        }
        else if (this->isFour(board, last_move))
        {
            // cout << "Four";
            this->m_renju_state = PATTERN::FOUR;
            return true;
        }
        else if (this->isDoubleThree(board, last_move))
        {
            // cout << "Double-Three";
            this->m_renju_state = PATTERN::DOUBLE_THREE;
            return false;
        }
        else if (this->isThree(board, last_move))
        {
            // cout << "three";
            this->m_renju_state = PATTERN::THREE;
            return true;
        }
    }

    return true;
}

int RenjuJudge::getRenjuState()
{
    return this->m_renju_state;
}

bool RenjuJudge::isPosOutOfBoard(unsigned int n, int x, int y)
{
  return x > n - 1 || y > n - 1 || x < 0 || y < 0;
}

int RenjuJudge::countNearStone(const board_type &board, int last_move, const pair<int, int>& p_drt)
{
    int i_count = 0;
    if (-1 == last_move)
        return 0;

    unsigned int n = board.size();
    pair<int, int> p_idx(last_move / n, last_move % n);
    pair<int, int> p_drt_idx(p_idx.first + p_drt.first, p_idx.second + p_drt.second);

    while (!isPosOutOfBoard(n, p_drt_idx.first, p_drt_idx.second) && 0 != board[p_drt_idx.first][p_drt_idx.second])
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

bool RenjuJudge::checkWin(const board_type &board, int last_move)
{
    if (last_move < 0)
    {
        if (last_move == -1)
            return true;
        else
            return false;
    }

    int n = board.size();

    pair<int, int> p_drt_up(0, -1), 
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

    if (1 == board[last_move / n][last_move % n])
    {
        if (i_up + i_down == 4 || i_left + i_right == 4 || i_leftup + i_rightdown == 4 || i_leftdown + i_rightup == 4)
        {
            this->m_renju_state = PATTERN::FIVE_IN_A_ROW;
            return true;
        }
    }
    else
    {
        if (i_up + i_down >= 4 || i_left + i_right >= 4 || i_leftup + i_rightdown >= 4 || i_leftdown + i_rightup >= 4)
        {
            this->m_renju_state = PATTERN::FIVE_IN_A_ROW;
            return true;
        }
    }

    return false;
}

