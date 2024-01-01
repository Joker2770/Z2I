/**
MIT License

Copyright (c) 2022 Augustusmyc
Copyright (c) 2023 - 2024 Joker2770

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

#include "gomoku.h"

#include <iostream>
#include <sstream>

Gomoku::Gomoku(const unsigned int n, const unsigned int n_in_row, int first_color)
    : n(n), n_in_row(n_in_row), cur_color(first_color), last_move(-1), rule_flag(DEFAULT_RULE), free_style(new FreeStyleJudge()), standard(new StandardJudge()), renju(new RenjuJudge()), caro(new CaroJudge())
{
  this->board = std::vector<std::vector<int>>(n, std::vector<int>(n, 0));
}

bool Gomoku::set_rule(unsigned int rule_flag)
{
  if (-1 == this->last_move)
  {
    this->rule_flag = rule_flag;
    return true;
  }

  return false;
}

bool Gomoku::is_illegal(unsigned int x, unsigned int y)
{
  return x > this->n - 1 || y > this->n - 1 || this->board[x][y] != 0;
}

std::vector<int> Gomoku::get_legal_moves()
{
  auto n = this->n;
  std::vector<int> legal_moves(this->get_action_size(), 0);

  for (unsigned int i = 0; i < n; i++)
  {
    for (unsigned int j = 0; j < n; j++)
    {
      if (this->board[i][j] == 0)
      {
        legal_moves[i * n + j] = 1;
      }
    }
  }

  return legal_moves;
}

bool Gomoku::has_legal_moves()
{
  auto n = this->n;

  for (unsigned int i = 0; i < n; i++)
  {
    for (unsigned int j = 0; j < n; j++)
    {
      if (this->board[i][j] == 0)
      {
        return true;
      }
    }
  }
  return false;
}

void Gomoku::execute_move(move_type move)
{
  auto i = move / this->n;
  auto j = move % this->n;

  if (this->board[i][j] != 0)
  {
    throw std::runtime_error("execute_move board[i][j] != 0.");
  }

  this->board[i][j] = this->cur_color;
  this->last_move = move;
  // change player
  this->cur_color = -(this->cur_color);
}

std::pair<int, int> Gomoku::get_game_status()
{
  // return (is ended, winner)
  auto n = this->n;
  auto n_in_row = this->n_in_row;

  if (5 == n_in_row && n > 8 && n < 25)
  {
    if (-1 == this->last_move)
      return {0, 0};

    bool isWin = false;
    int i_win = 0;
    if (nullptr != free_style)
      isWin = free_style->checkWin(this->board, this->last_move);
    if (0x01 == (this->rule_flag & 0x01) && (this->get_action_size() - this->get_legal_moves().size() >= 9) && nullptr != standard)
    {
      if (standard->checkWin(this->board, this->last_move))
        i_win |= 1;
      else
        isWin = false;
    }
    if (0x04 == (this->rule_flag & 0x04) && (this->get_action_size() - this->get_legal_moves().size() >= 6) && nullptr != renju)
    {
      if (renju->checkWin(this->board, this->last_move))
        i_win |= 4;
      else
        isWin = false;
    }
    if (0x08 == (this->rule_flag & 0x08) && (this->get_action_size() - this->get_legal_moves().size() >= 9) && nullptr != caro)
    {
      if (caro->checkWin(this->board, this->last_move))
        i_win |= 8;
      else
        isWin = false;
    }

    if (0 != i_win)
    {
      if ((this->rule_flag & i_win) == this->rule_flag)
        isWin = true;
      else
        isWin = false;
    }

    if (isWin)
      return {1, this->board[this->last_move / n][this->last_move % n]};
    else if (4 == (this->rule_flag & 4) && (this->get_action_size() - this->get_legal_moves().size() >= 6) && nullptr != renju)
    {
      if ((1 == this->board[this->last_move / n][this->last_move % n]) && !(renju->isLegal(this->board, this->last_move)))
        return {1, -1};
    }
  }
  else
  {
    for (unsigned int i = 0; i < n; i++)
    {
      for (unsigned int j = 0; j < n; j++)
      {
        if (this->board[i][j] == 0)
        {
          continue;
        }

        if (j <= n - n_in_row)
        {
          auto sum = 0;
          for (unsigned int k = 0; k < n_in_row; k++)
          {
            sum += this->board[i][j + k];
          }
          if (abs(sum) == n_in_row)
          {
            return {1, this->board[i][j]};
          }
        }

        if (i <= n - n_in_row)
        {
          auto sum = 0;
          for (unsigned int k = 0; k < n_in_row; k++)
          {
            sum += this->board[i + k][j];
          }
          if (abs(sum) == n_in_row)
          {
            return {1, this->board[i][j]};
          }
        }

        if (i <= n - n_in_row && j <= n - n_in_row)
        {
          auto sum = 0;
          for (unsigned int k = 0; k < n_in_row; k++)
          {
            sum += this->board[i + k][j + k];
          }
          if (abs(sum) == n_in_row)
          {
            return {1, this->board[i][j]};
          }
        }

        if (i <= n - n_in_row && j >= n_in_row - 1)
        {
          auto sum = 0;
          for (unsigned int k = 0; k < n_in_row; k++)
          {
            sum += this->board[i + k][j - k];
          }
          if (abs(sum) == n_in_row)
          {
            return {1, this->board[i][j]};
          }
        }
      }
    }
  }

  if (this->has_legal_moves())
  {
    return {0, 0};
  }
  else
  {
    return {1, 0};
  }
}

void Gomoku::display() const
{
  auto n = this->board.size();

  for (unsigned int i = 0; i < n; i++)
  {
    for (unsigned int j = 0; j < n; j++)
    {
      std::cout << this->board[i][j] << ", ";
    }
    std::cout << std::endl;
  }
}

void Gomoku::render()
{
  std::ostringstream out;
  const size_t n = this->board.size();
  // out << "step ?" << std::endl;

  const size_t cell_size = 3;
  const size_t row_size = (cell_size + 1) * n + 1;
  // std::vector<char> line[row_size + 1];
  char *line = new char[row_size + 1];
  char *line2 = new char[row_size + 1];
  for (size_t i = 0; i < row_size; i++)
  {
    if (i % (cell_size + 1) == 0)
      line[i] = '+';
    else
      line[i] = '-';

    line2[i] = ' ';
  }
  line[row_size] = 0;
  line2[row_size] = 0;
  out << line << std::endl;
  unsigned int this_i = this->last_move / this->n;
  unsigned int this_j = this->last_move % this->n;
  for (unsigned int i = 0; i < n; i++)
  {
    for (unsigned int j = 0; j < n; j++)
    {
      line2[j * (cell_size + 1)] = '|';
      if (this_i == i)
      {
        if (this_j == j)
        {
          line2[j * (cell_size + 1)] = '[';
        }
        else if (this_j == j - 1)
        {
          line2[j * (cell_size + 1)] = ']';
        }
      }

      int st = j * (cell_size + 1) + cell_size / 2 + 1;
      if (board[i][j] == 1)
        line2[st] = '#';
      else if (board[i][j] == -1)
        line2[st] = 'O';
      else
        line2[st] = ' ';
    }
    line2[row_size - 1] = '|';
    if (this_i == i && this_j == this->n - 1)
    {
      line2[row_size - 1] = ']';
    }

    out << line2 << " " << (char)('A' + i) << std::endl;
    out << line << std::endl;
  }
  // char column_no[row_size] = {0};
  char *column_no = new char[row_size]{0};
  unsigned int offset = 0;
  for (unsigned int i = 1; i <= n; i++)
  {
    offset += snprintf(column_no + offset, row_size - offset, "  %-2u", i); // snprintf -> sprintf_s
  }
  if (nullptr != column_no)
    out << column_no;
  puts(out.str().c_str());

  if (nullptr != line)
  {
      delete[] line;
      line = nullptr;
  }
  if (nullptr != line2)
  {
      delete[] line2;
      line2 = nullptr;
  }
  if (nullptr != column_no)
  {
      delete[] column_no;
      column_no = nullptr;
  }
}
