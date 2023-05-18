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

#pragma once

#include "common.h"
#include "FreeStyleJudge.h"
#include "StandardJudge.h"
#include "RenjuJudge.h"
#include "CaroJudge.h"

#include <tuple>
#include <vector>
#include <utility>
#include <memory>

// using namespace customType;

class Gomoku
{
public:
  using move_type = int;

  Gomoku(const unsigned int n, const unsigned int n_in_row, int first_color);

  bool set_rule(const unsigned int i_rule_flag);
  bool has_legal_moves();
  std::vector<int> get_legal_moves();
  void execute_move(move_type move);
  void take_back_move();
  std::pair<int, int> get_game_status();
  void display() const;
  void render();
  // bool is_illegal(move_type move);
  bool is_illegal(unsigned int x, unsigned int y);

  inline unsigned int get_action_size() const { return this->n * this->n; }
  inline board_type get_board() const { return this->board; }
  inline move_type get_last_move() const { return this->last_move; }
  inline int get_current_color() const { return this->cur_color; }
  inline unsigned int get_n() const { return this->n; }
  inline std::vector<move_type> get_record_list() const { return this->record_list; }
  inline unsigned int get_rule() const { return this->rule_flag; };

private:
  board_type board;                   // game board
  std::vector<move_type> record_list; // record moves in order
  const unsigned int n;               // board size
  const unsigned int n_in_row;        // 5 in row or else
  unsigned int rule_flag;

  std::shared_ptr<FreeStyleJudge> free_style;
  std::shared_ptr<StandardJudge> standard;
  std::shared_ptr<RenjuJudge> renju;
  std::shared_ptr<CaroJudge> caro;

  int cur_color;       // current player's color
  move_type last_move; // last move
};
