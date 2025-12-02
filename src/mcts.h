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

#include "gomoku.h"
#include "onnx.h"
#include "thread_pool.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <random>

class TreeNode
{
public:
  // friend class can access private variables
  friend class MCTS;

  TreeNode();
  TreeNode(const TreeNode &node) = delete; // disable shallow copy to avoid double-free
  TreeNode(TreeNode *parent, double p_sa, unsigned action_size);

  TreeNode &operator=(const TreeNode &p) = delete; // disable assignment

  unsigned int select(double c_puct, double c_virtual_loss);
  void expand(const std::vector<double> &action_priors);
  // void expand(const std::vector<double>& action_priors, std::vector<int>& legal_moves);
  void backup(double leaf_value);

  double get_value(double c_puct, double c_virtual_loss,
                   unsigned int sum_n_visited) const;
  inline bool get_is_leaf() const { return this->is_leaf; }

private:
  // store tree
  TreeNode *parent;
  std::vector<TreeNode *> children;
  bool is_leaf;
  std::mutex lock;

  std::atomic<unsigned int> n_visited;
  double p_sa;
  double q_sa;
  std::atomic<int> virtual_loss;
};

class MCTS
{
public:
  MCTS(NeuralNetwork *neural_network, unsigned int thread_num, double c_puct,
       unsigned int num_mcts_sims, double c_virtual_loss,
       unsigned int action_size);
  std::vector<double> get_action_probs(Gomoku *gomoku, double temp = 1e-3);
  int get_action_by_sample(std::vector<double> &action_probs);
  int get_best_action(Gomoku *gomoku);
  int get_best_action_from_prob(std::vector<double> &action_probs);
  void update_with_move(int last_move);
  unsigned int action_size;

private:
  std::default_random_engine rnd_eng;
  std::uniform_real_distribution<double> rnd_dis;
  void simulate(std::shared_ptr<Gomoku> game);
  static void tree_deleter(TreeNode *t);

  // variables
  std::unique_ptr<TreeNode, decltype(MCTS::tree_deleter) *> root;
  std::unique_ptr<ThreadPool> thread_pool;
  NeuralNetwork *neural_network;

  unsigned int num_mcts_sims;
  double c_puct;
  double c_virtual_loss;
};
