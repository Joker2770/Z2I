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

#include "play.h"
#include "mcts.h"
#include "gomoku.h"
#include "common.h"
#include "onnx.h"

#include <iostream>
#include <fstream>
#include <sstream>

SelfPlay::SelfPlay(NeuralNetwork *nn) : /* p_buffer(new p_buff_type()),*/
                                        /* board_buffer(new board_buff_type()),*/
                                        /* v_buffer(new v_buff_type()),*/
                                        nn(nn),
                                        thread_pool(new ThreadPool(NUM_TRAIN_THREADS))
{
}

void SelfPlay::play(unsigned int saved_id)
{
    auto g = std::make_shared<Gomoku>(BOARD_SIZE, N_IN_ROW, BLACK);
    MCTS *mcts = new MCTS(nn, NUM_MCT_THREADS, C_PUCT, NUM_MCT_SIMS, C_VIRTUAL_LOSS, BOARD_SIZE * BOARD_SIZE);
    std::pair<int, int> game_state;
    game_state = g->get_game_status();
    std::cout << "game rule: " << g->get_rule() << std::endl;
    // std::cout << "begin !!" << std::endl;
    int step = 0;
    board_buff_type board_buffer(BUFFER_LEN, std::vector<std::vector<int>>(BOARD_SIZE, std::vector<int>(BOARD_SIZE)));
    v_buff_type v_buffer(BUFFER_LEN);
    p_buff_type p_buffer(BUFFER_LEN, std::vector<float>(BOARD_SIZE * BOARD_SIZE)); /* = new p_buff_type();*/
    std::vector<int> col_buffer(BUFFER_LEN);
    std::vector<int> last_move_buffer(BUFFER_LEN);
    // diri noise
    static std::gamma_distribution<float> gamma(0.3f, 1.0f);
    static std::default_random_engine rng(static_cast<unsigned int>(std::time(nullptr)));

    while (game_state.first == 0)
    {
        // std::cout << "game id: " << saved_id << std::endl;
        double temp = (step < EXPLORE_STEP) ? 1.0 : 1e-3;
        auto action_probs = mcts->get_action_probs(g.get(), temp);
        // auto action_probs = m->get_action_probs(g.get(), 1);
        // int best_action = m->get_best_action_from_prob(action_probs);

        board_type board = g->get_board();
        for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
        {
            p_buffer[step][i] = (float)action_probs[i];
        }
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                board_buffer[step][i][j] = board[i][j];
            }
        }
        col_buffer[step] = g->get_current_color();
        last_move_buffer[step] = g->get_last_move();

        std::vector<int> lm = g->get_legal_moves();
        double sum = 0;
        for (int i = 0; i < lm.size(); i++)
        {
            if (lm[i])
            {
                double noi = DIRI * gamma(rng);
                // if (is1){
                //     std::cout << "noi = " << noi << std::endl;
                //     is1 = false;
                // }
                action_probs[i] += noi;
                //    if (step < EXPLORE_STEP) {
                //        action_probs[i] += DIRI * gamma(rng);
                //    }
                //    else {
                //        action_probs[i] = (i== best_action) + DIRI * gamma(rng);
                //    }
                sum += action_probs[i];
            }
        }
        for (int i = 0; i < lm.size(); i++)
        {
            if (lm[i])
            {
                action_probs[i] /= sum;
            }
        }

        // int res = mcts->get_action_by_sample(action_probs);
        int res = mcts->get_best_action_from_prob(action_probs);
        mcts->update_with_move(res);
        g->execute_move(res);
        game_state = g->get_game_status();
        g->render();
        step++;
    }
    std::cout << "Self play: total step num = " << step << " winner = " << game_state.second << std::endl;

    size_t hash_value = std::hash<std::shared_ptr<Gomoku>>{}(g);
    std::ostringstream oss;
    oss << hash_value;
    std::ofstream bestand;
    bestand.open("./data/data_" + std::to_string(saved_id) + "_" + oss.str(), std::ios::out | std::ios::binary);
    bestand.write(reinterpret_cast<char *>(&step), sizeof(int));

    for (int i = 0; i < step; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            bestand.write(reinterpret_cast<char *>(&board_buffer[i][j][0]), BOARD_SIZE * sizeof(int));
        }
    }

    for (int i = 0; i < step; i++)
    {
        bestand.write(reinterpret_cast<char *>(&p_buffer[i][0]), BOARD_SIZE * BOARD_SIZE * sizeof(float));
        v_buffer[i] = col_buffer[i] * game_state.second;
    }

    bestand.write(reinterpret_cast<char *>(&v_buffer[0]), step * sizeof(int));
    bestand.write(reinterpret_cast<char *>(&col_buffer[0]), step * sizeof(int));
    bestand.write(reinterpret_cast<char *>(&last_move_buffer[0]), step * sizeof(int));

    bestand.close();

    if (nullptr != mcts)
    {
        delete mcts;
        mcts = nullptr;
    }

    // just validation
    // ifstream inlezen;
    // int new_step;
    // inlezen.open("./data/data_"+str(id), std::ios::in | std::ios::binary);
    // inlezen.read(reinterpret_cast<char*>(&new_step), sizeof(int));

    // board_buff_type new_board_buffer(new_step, std::vector<std::vector<int>>(BOARD_SIZE, std::vector<int>(BOARD_SIZE)));
    // p_buff_type new_p_buffer(new_step, std::vector<float>(BOARD_SIZE * BOARD_SIZE));
    // v_buff_type new_v_buffer(new_step);

    // for (int i = 0; i < step; i++) {
    //     for (int j = 0; j < BOARD_SIZE; j++) {
    //         inlezen.read(reinterpret_cast<char*>(&new_board_buffer[i][j][0]), BOARD_SIZE * sizeof(int));
    //     }
    // }

    // for (int i = 0; i < step; i++) {
    //     inlezen.read(reinterpret_cast<char*>(&new_p_buffer[i][0]), BOARD_SIZE * BOARD_SIZE * sizeof(float));
    // }

    // inlezen.read(reinterpret_cast<char*>(&new_v_buffer[0]), step * sizeof(int));
}

void SelfPlay::self_play_for_train(unsigned int game_num, unsigned int start_batch_id)
{
    std::vector<std::future<void>> futures;
    for (unsigned int i = 0; i < game_num; i++)
    {
        auto future = thread_pool->commit(std::bind(&SelfPlay::play, this, start_batch_id + i));
        futures.emplace_back(std::move(future));
    }
    this->nn->set_batch_size(game_num * NUM_MCT_THREADS);
    for (unsigned int i = 0; i < futures.size(); i++)
    {
        futures[i].wait();

        this->nn->set_batch_size(std::max((unsigned)1, (game_num - i) * NUM_MCT_THREADS));
        // std::cout << "end" << std::endl;
    }
    // return { *this->board_buffer , *this->p_buffer ,*this->v_buffer };
}

// pair<int,int> SelfPlay::self_play_for_eval(unsigned int game_num, NeuralNetwork* a, NeuralNetwork* b) {
//    std::vector<std::future<void>> futures;
//    //NeuralNetwork* a = new NeuralNetwork(NUM_MCT_THREADS * NUM_MCT_SIMS);
//    for (unsigned int i = 0; i < game_num; i++) {
//        auto future = thread_pool->commit(std::bind(&SelfPlay::play, this,a,b));
//        futures.emplace_back(std::move(future));
//    }
//    this->nn->batch_size = game_num * NUM_MCT_THREADS;
//    for (unsigned int i = 0; i < futures.size(); i++) {
//        futures[i].wait();
//
//        this->nn->batch_size = std::max((unsigned)1, (game_num - i) * NUM_MCT_THREADS);
//    }
//    //return { *this->board_buffer , *this->p_buffer ,*this->v_buffer };
//}
