/**
MIT License

Copyright (c) 2022 Augustusmyc
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

#include "../onnx.h"
#include "../play.h"
#include "../common.h"
#include "../mcts.h"

#include <iostream>
#include <fstream>
#include <sstream>

void generate_data_for_train(int current_weight, int start_batch_id)
{
    std::string path = "./weights/" + std::to_string(current_weight) + ".onnx";

    std::shared_ptr<NeuralNetwork> model = std::make_shared<NeuralNetwork>(path, NUM_MCT_THREADS * NUM_MCT_SIMS);
    SelfPlay *sp = new SelfPlay(model.get());

    if (nullptr != sp)
    {
        sp->self_play_for_train(NUM_TRAIN_THREADS, start_batch_id);
        delete sp;
        sp = nullptr;
    }
}

void play_for_eval(NeuralNetwork *a, NeuralNetwork *b, bool a_first, int *win_table, bool do_render, const unsigned int a_mct_sims, const unsigned int b_mct_sims)
{
    MCTS *ma = new MCTS(a, NUM_MCT_THREADS, C_PUCT, a_mct_sims, C_VIRTUAL_LOSS, BOARD_SIZE * BOARD_SIZE);
    MCTS *mb = new MCTS(b, NUM_MCT_THREADS, C_PUCT, b_mct_sims, C_VIRTUAL_LOSS, BOARD_SIZE * BOARD_SIZE);
    int step = 0;
    auto g = std::make_shared<Gomoku>(BOARD_SIZE, N_IN_ROW, BLACK);
    std::pair<int, int> game_state = g->get_game_status();
    // std::cout << episode << " th game!!" << std::endl;
    while (game_state.first == 0)
    {
        int res = ((step + a_first) % 2) ? ma->get_best_action(g.get()) : mb->get_best_action(g.get());
        ma->update_with_move(res);
        mb->update_with_move(res);
        g->execute_move(res);
        if (do_render)
            g->render();
        game_state = g->get_game_status();
        step++;
    }
    std::cout << "eval: total step num = " << step << std::endl;

    if ((game_state.second == BLACK && a_first) || (game_state.second == WHITE && !a_first))
    {
        std::cout << "winner = a" << std::endl;
        win_table[0]++;
    }
    else if ((game_state.second == BLACK && !a_first) || (game_state.second == WHITE && a_first))
    {
        std::cout << "winner = b" << std::endl;
        win_table[1]++;
    }
    else if (game_state.second == 0)
        win_table[2]++;

    if (nullptr != ma) {
      delete ma;
      ma = nullptr;
    }
    if (nullptr != mb) {
      delete mb;
      mb = nullptr;
    }
}

std::vector<int> eval(int weight_a, int weight_b, unsigned int game_num, unsigned int a_sims, unsigned int b_sims)
{
    int win_table[3] = {0, 0, 0};
    std::unique_ptr<ThreadPool> thread_pool(new ThreadPool(game_num));
    std::shared_ptr<NeuralNetwork> nn_a = nullptr;
    std::shared_ptr<NeuralNetwork> nn_b = nullptr;

    if (weight_a >= 0)
    {
        std::string path = "./weights/" + std::to_string(weight_a) + ".onnx";
        nn_a = std::make_shared<NeuralNetwork>(path, game_num * a_sims);
        std::cout << "NeuralNetwork A load: " << weight_a << std::endl;
    }
    else
    {
        std::cout << "NeuralNetwork A applies random policy!" << std::endl;
    }

    if (weight_b >= 0)
    {
        std::string path = "./weights/" + std::to_string(weight_b) + ".onnx";
        nn_b = std::make_shared<NeuralNetwork>(path, game_num * b_sims);
        std::cout << "NeuralNetwork B load: " << weight_b << std::endl;
    }
    else
    {
        std::cout << "NeuralNetwork B applies random policy!" << std::endl;
    }

    std::vector<std::future<void>> futures;
    // NeuralNetwork* a = new NeuralNetwork(NUM_MCT_THREADS * NUM_MCT_SIMS);
    for (unsigned int i = 0; i < game_num; i++)
    {
        auto future = thread_pool->commit(std::bind(play_for_eval, nn_a.get(), nn_b.get(), rand() % 2 == 0 ? false : true, win_table, false, a_sims, b_sims));
        futures.emplace_back(std::move(future));
    }
    for (unsigned int i = 0; i < futures.size(); i++)
    {
        futures[i].wait();
        if (nn_a != nullptr)
        {
            nn_a->set_batch_size(std::max((unsigned)1, (game_num - i) * NUM_MCT_THREADS));
        }
        if (nn_b != nullptr)
        {
            nn_b->set_batch_size(std::max((unsigned)1, (game_num - i) * NUM_MCT_THREADS));
        }
    }
    // std::cout << "win_table = " << win_table[0] << win_table[1] << win_table [2] << std::endl;

    return {win_table[0], win_table[1], win_table[2]};
}

int main(int argc, char *argv[])
{
    if (strcmp(argv[1], "prepare") == 0)
    {
        std::cout << "Prepare for training." << std::endl;
        // system("mkdir weights");
#ifdef _WIN32
        system("mkdir .\\weights");
        system("mkdir .\\data");
#elif __linux__
        auto ret = system("mkdir ./weights");
        // std::cout <<ret << std::endl;
        ret = system("mkdir ./data");
#endif
        std::ofstream weight_logger_writer("current_and_best_weight.txt");
        weight_logger_writer << 0 << " " << 0;
        weight_logger_writer.close();

        std::ofstream random_mcts_logger_writer("random_mcts_number.txt");
        random_mcts_logger_writer << NUM_MCT_SIMS;
        random_mcts_logger_writer.close();

        std::cout << "Next: Generate initial weight by python." << std::endl;
        // system("python ..\\python\\learner.py");
    }
    else if (strcmp(argv[1], "generate") == 0)
    {
        std::cout << "generate " << atoi(argv[2]) << "-th batch." << std::endl;
        int current_weight;

        std::ifstream logger_reader("current_and_best_weight.txt");
        logger_reader >> current_weight;
        // logger_reader >> best_weight;
        if (current_weight < 0)
        {
            std::cout << "LOAD error,check current_and_best_weight.txt" << std::endl;
            return -1;
        }
        // logger_reader >> temp[1];
        logger_reader.close();
        std::cout << "Generating... current_weight = " << current_weight << " start batch id: " << argv[2] << std::endl;
        generate_data_for_train(current_weight, atoi(argv[2]) * NUM_TRAIN_THREADS);
    }
    else if (strcmp(argv[1], "eval_with_winner") == 0)
    {
        int current_weight;
        int best_weight;

        std::ifstream weight_logger_reader("current_and_best_weight.txt");
        weight_logger_reader >> current_weight;
        weight_logger_reader >> best_weight;
        std::cout << "Evaluating... current_weight = " << current_weight << " and best_weight = " << best_weight << std::endl;

        int game_num = atoi(argv[2]);

#ifdef USE_CUDA
        auto result = eval(current_weight, best_weight, game_num, (unsigned int)(NUM_MCT_SIMS / 4 + 1), (unsigned int)(NUM_MCT_SIMS / 4 + 1));
#else
        auto result = eval(current_weight, best_weight, game_num, (unsigned int)(NUM_MCT_SIMS / 16 + 1), (unsigned int)(NUM_MCT_SIMS / 16 + 1));
#endif
        std::string result_log_info = std::to_string(current_weight) +
                                 "-th weight win: " +
                                 std::to_string(result[0]) +
                                 "  " +
                                 std::to_string(best_weight) +
                                 "-th weight win: " +
                                 std::to_string(result[1]) +
                                 "  tie: " +
                                 std::to_string(result[2]) +
                                 "\n";

        double win_ratio = result[0] / (result[1] + 0.01);
        if (win_ratio > 1.2)
        {
            result_log_info += "new best weight: " + std::to_string(current_weight) + " generated!!!!\n";
            std::ofstream weight_logger_writer("current_and_best_weight.txt");
            weight_logger_writer << current_weight << " " << current_weight;
            weight_logger_writer.close();
        }
        std::cout << result_log_info;

        std::ofstream detail_logger_writer("logger.txt", std::ios::app);
        // detail_logger_writer << result_log_info << result_log_info2;
        detail_logger_writer << result_log_info;
        detail_logger_writer.close();
    }
    else if (strcmp(argv[1], "eval_with_random") == 0)
    {
        int current_weight;

        std::ifstream weight_logger_reader("current_and_best_weight.txt");
        weight_logger_reader >> current_weight;
        // weight_logger_reader >> best_weight;

        int game_num = atoi(argv[2]);

        unsigned int random_mcts_simulation;
        std::ifstream random_mcts_logger_reader("random_mcts_number.txt");
        random_mcts_logger_reader >> random_mcts_simulation;

        unsigned int nn_mcts_simulation = NUM_MCT_SIMS / 16 + 1; // can not be too small !!

        std::vector<int> result_random_mcts = eval(current_weight, -1, game_num, nn_mcts_simulation, random_mcts_simulation);

        std::string result_log_info2 = std::to_string(current_weight) +
                                  "-th weight with mcts [" +
                                  std::to_string(nn_mcts_simulation) +
                                  "] win: " +
                                  std::to_string(result_random_mcts[0]) +
                                  "  Random mcts [" +
                                  std::to_string(random_mcts_simulation) +
                                  "] win: " +
                                  std::to_string(result_random_mcts[1]) +
                                  "  tie: " +
                                  std::to_string(result_random_mcts[2]) +
                                  "\n";
        if (result_random_mcts[0] == game_num)
        {
            if (random_mcts_simulation < 8000)
            {
                random_mcts_simulation += 100;
                result_log_info2 += "add random mcts number to: " + std::to_string(random_mcts_simulation) + "\n";
                std::ofstream random_mcts_logger_writer("random_mcts_number.txt");
                random_mcts_logger_writer << random_mcts_simulation;
                random_mcts_logger_writer.close();
            }

            ///////////////
            result_log_info2 += "new best weight: " + std::to_string(current_weight) + " generated!!!!\n";
            std::ofstream weight_logger_writer("current_and_best_weight.txt");
            weight_logger_writer << current_weight << " " << current_weight;
            weight_logger_writer.close();
            //////////////
        }
        std::cout << result_log_info2;

        std::ofstream detail_logger_writer("logger.txt", std::ios::app);
        detail_logger_writer << result_log_info2;
        detail_logger_writer.close();
    }
    else
    {
        std::cout << "Do nothing...check your input!!" << std::endl;
    }
}
