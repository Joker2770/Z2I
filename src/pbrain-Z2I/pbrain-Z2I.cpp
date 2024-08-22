/*************************************************************************
    > File Name: pbrain-Z2I.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com
    > Created Time: Mon Apr 24 12:39:45 2023
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

#include "../mcts.h"
#include "../common.h"
#include "../onnx.h"
#include "toml.hpp"

#include <string>
#include <cstring>
#include <vector>
#include <filesystem>

bool isNumericString(const char *str, size_t i_len)
{
    for (size_t i = 0; i < i_len; i++)
    {
        if (!isdigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

std::vector<std::string> split(const std::string &str, const std::string &pattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    std::string strs = str + pattern;
    size_t size = strs.size();
    for (size_t i = 0; i < size; ++i)
    {
        pos = strs.find(pattern, i);
        if (pos < size)
        {
            std::string s = strs.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

void toupper(std::string &str)
{
    for (size_t i = 0; i < str.size(); i++)
    {
        char &c = str[i];
        if (c >= 'a' && c <= 'z')
        {
            c += 'A' - 'a';
        }
    }
}

int main(int argc, char *argv[])
{
    std::cout << "MESSAGE ................................................................................." << std::endl;
    std::cout << "MESSAGE MIT License" << std::endl;
    std::cout << "MESSAGE " << std::endl;
    std::cout << "MESSAGE Copyright (c) 2023-2024 Joker2770" << std::endl;
    std::cout << "MESSAGE " << std::endl;
    std::cout << "MESSAGE Permission is hereby granted, free of charge, to any person obtaining a copy" << std::endl;
    std::cout << "MESSAGE of this software and associated documentation files (the \"Software\"), to deal" << std::endl;
    std::cout << "MESSAGE in the Software without restriction, including without limitation the rights" << std::endl;
    std::cout << "MESSAGE to use, copy, modify, merge, publish, distribute, sublicense, and/or sell" << std::endl;
    std::cout << "MESSAGE copies of the Software, and to permit persons to whom the Software is" << std::endl;
    std::cout << "MESSAGE furnished to do so, subject to the following conditions:" << std::endl;
    std::cout << "MESSAGE MESSAGE " << std::endl;
    std::cout << "MESSAGE The above copyright notice and this permission notice shall be included in all" << std::endl;
    std::cout << "MESSAGE copies or substantial portions of the Software." << std::endl;
    std::cout << "MESSAGE " << std::endl;
    std::cout << "MESSAGE THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR" << std::endl;
    std::cout << "MESSAGE IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY," << std::endl;
    std::cout << "MESSAGE FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE" << std::endl;
    std::cout << "MESSAGE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER" << std::endl;
    std::cout << "MESSAGE LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM," << std::endl;
    std::cout << "MESSAGE OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE" << std::endl;
    std::cout << "MESSAGE SOFTWARE." << std::endl;
    std::cout << "MESSAGE " << std::endl;
    std::cout << "MESSAGE ................................................................................." << std::endl;

    std::shared_ptr<NeuralNetwork> module = nullptr;
    std::filesystem::path exe_path = std::filesystem::canonical(std::filesystem::path(argv[0])).remove_filename();
    std::string s_config_file_path = exe_path.string() + "config.toml";
    std::string s_model_path;
    unsigned int u_num_mct_sims = 1600;
    unsigned int u_num_mct_threads = 10;
    toml::value toml_data;
    if (std::filesystem::exists(s_config_file_path))
    {
        toml_data = toml::parse(s_config_file_path);
        if (toml_data["MCTS"].is_table())
            u_num_mct_sims = toml::find<unsigned int>(toml_data["MCTS"], "num_mct_sims");
        if (toml_data["MCTS"].is_table())
            u_num_mct_threads = toml::find<unsigned int>(toml_data["MCTS"], "num_mct_threads");
        std::cout << "MESSAGE num_mct_sims: " << u_num_mct_sims << std::endl;
        std::cout << "MESSAGE num_mct_threads: " << u_num_mct_threads << std::endl;
        if (toml_data["model"].is_table())
            s_model_path = exe_path.string() + toml::find<std::string>(toml_data["model"], "default_model");
        std::cout << "MESSAGE model load path: " << s_model_path << std::endl;
        if (!s_model_path.empty() && std::filesystem::exists(s_model_path))
        {
            std::cout << "MESSAGE model exists" << std::endl;
            module = std::make_shared<NeuralNetwork>(s_model_path, NUM_MCT_SIMS);
        }
        else
        {
            std::cout << "ERROR model not exists" << std::endl;
            return -1;
        }
    }
    else
    {
        std::cout << "ERROR config file not exists" << std::endl;
        return -1;
    }

    Gomoku *g = new Gomoku(BOARD_SIZE, N_IN_ROW, BLACK);
    g->set_rule(0);
    std::cout << "MESSAGE game rule: " << g->get_rule() << std::endl;

    MCTS *m;
    m = new MCTS(module.get(),
                 (unsigned int)(u_num_mct_threads != 0 ? u_num_mct_threads : NUM_MCT_THREADS),
                 C_PUCT,
                 (unsigned int)(u_num_mct_sims != 0 ? u_num_mct_sims : NUM_MCT_SIMS),
                 C_VIRTUAL_LOSS, BOARD_SIZE * BOARD_SIZE);

    std::string command;
    unsigned int size = 0;
    char dot = ',';
    bool isPlaying = false;
    for (;;)
    {
        std::cin >> command;
        toupper(command);

        if (command == "START")
        {
            char s_size[4] = "\0";
            std::cin >> s_size;
            if (isNumericString(s_size, strlen(s_size)))
            {
                size = atoi(s_size);
            }

            // reset game
            if (nullptr != g)
            {
                delete g;
                g = nullptr;
            }
            g = new Gomoku(BOARD_SIZE, N_IN_ROW, BLACK);

            if (size == 15)
            {
                std::cout << "OK" << std::endl;
            }
            else
            {
                std::cout << "ERROR unsupported board size!" << std::endl;
            }
        }
        // else if (command == "RESTART")
        // {
        // }
        // else if (command == "TAKEBACK")
        // {
        // }
        else if (command == "BEGIN")
        {
            isPlaying = true;
            int res = m->get_best_action(g);
            m->update_with_move(res);
            g->execute_move(res);
            unsigned int x = res / size;
            unsigned int y = res % size;
            std::cout << x << "," << y << std::endl;
        }
        else if (command == "TURN")
        {
            isPlaying = true;
            unsigned int x, y;
            std::cin >> x >> dot >> y;
            if (!(g->is_illegal(x, y)))
            {
                int move = x * size + y;
                m->update_with_move(move);
                g->execute_move(move);
                std::vector<double> p = m->get_action_probs(g);
                int action = m->get_best_action_from_prob(p);
                x = action / size;
                y = action % size;
                if (!g->is_illegal(x, y))
                {
                    m->update_with_move(action);
                    g->execute_move(action);
                    std::cout << x << "," << y << std::endl;
                }
                else
                {
                    std::cout << "ERROR Illegal move generated! " << std::endl;
                }
            }
            else
            {
                std::cout << "ERROR Illegal move from opponent! " << std::endl;
            }
        }
        else if (command == "BOARD")
        {
            isPlaying = true;
            unsigned int x, y, c;
            std::vector<int> move_1, move_2, move_3;

            if (nullptr != g)
            {
                delete g;
                g = nullptr;
            }
            g = new Gomoku(BOARD_SIZE, N_IN_ROW, BLACK);

            std::cin >> command;
            while (command != "DONE")
            {
                // std::cout << "DEBUG " << command << std::endl;
                std::vector<std::string> v_s = split(command, ",");
                if ((command.find_first_of(',', 0) != command.find_last_of(',', 0)) && v_s.size() == 3)
                {
                    // std::cout << "DEBUG " << v_s.at(0) << "," << v_s.at(1) << "," << v_s.at(2) << std::endl;
                    if (isNumericString(v_s.at(0).c_str(), v_s.at(0).length()) &&
                        isNumericString(v_s.at(1).c_str(), v_s.at(1).length()) &&
                        isNumericString(v_s.at(2).c_str(), v_s.at(2).length()))
                    {
                        x = atoi(v_s.at(0).c_str());
                        y = atoi(v_s.at(1).c_str());
                        c = atoi(v_s.at(2).c_str());
                        int move = x * size + y;
                        // std::cout << "DEBUG move: " << move << std::endl;
                        if (c == 1)
                        {
                            move_1.push_back(move);
                        }
                        else if (c == 2)
                        {
                            move_2.push_back(move);
                        }
                        else if (c == 3)
                        {
                            move_3.push_back(move);
                        }
                        else
                        {
                        }
                    }
                    else
                    {
                        std::cout << "ERROR Illegal pos! " << std::endl;
                        move_1.clear();
                        move_2.clear();
                        move_3.clear();
                        break;
                    }
                }
                else
                {
                    std::cout << "ERROR Illegal board! " << std::endl;
                    move_1.clear();
                    move_2.clear();
                    move_3.clear();
                    break;
                }
                std::cin >> command;
            }
            // std::cout << "DEBUG move_1 size: " << move_1.size() << std::endl;
            // std::cout << "DEBUG move_2 size: " << move_2.size() << std::endl;
            // std::cout << "DEBUG move_3 size: " << move_3.size() << std::endl;

            if (move_1.size() == 0 && move_2.size() == 0 && move_3.size() != 0)
            {
                reverse(move_3.begin(), move_3.end());
                for (size_t i = 0; i < move_3.size(); i++)
                {
                    if (!(g->is_illegal(move_3.back() / size, move_3.back() % size)))
                    {
                        m->update_with_move(move_3.back());
                        g->execute_move(move_3.back());
                        move_3.pop_back();
                    }
                    else
                    {
                        std::cout << "ERROR Illegal pos! " << std::endl;
                        move_3.clear();
                        break;
                    }
                }

                while (g->get_game_status().first != 1)
                {
                    std::vector<double> p = m->get_action_probs(g);
                    int action = m->get_best_action_from_prob(p);
                    x = action / size;
                    y = action % size;
                    if (!g->is_illegal(x, y))
                    {
                        m->update_with_move(action);
                        g->execute_move(action);
                        std::cout << x << "," << y << std::endl;
                    }
                    else
                    {
                        std::cout << "ERROR Illegal move generated! " << std::endl;
                        break;
                    }
                }
                continue;
            }

            if (move_1.size() != move_2.size())
            {
                if (move_2.size() - move_1.size() == 1)
                {
                    // move_2 first
                    reverse(move_2.begin(), move_2.end());
                    reverse(move_1.begin(), move_1.end());
                    size_t min_size = move_1.size();
                    for (size_t i = 0; i < min_size; i++)
                    {
                        if (!(g->is_illegal(move_2.back() / size, move_2.back() % size)))
                        {
                            m->update_with_move(move_2.back());
                            g->execute_move(move_2.back());
                            move_2.pop_back();
                        }
                        else
                        {
                            std::cout << "ERROR Illegal pos! " << std::endl;
                            move_1.clear();
                            move_2.clear();
                            move_3.clear();
                            break;
                        }
                        if (!(g->is_illegal(move_1.back() / size, move_1.back() % size)))
                        {
                            m->update_with_move(move_1.back());
                            g->execute_move(move_1.back());
                            move_1.pop_back();
                        }
                        else
                        {
                            std::cout << "ERROR Illegal pos! " << std::endl;
                            move_1.clear();
                            move_2.clear();
                            move_3.clear();
                            break;
                        }
                    }
                    // last move of move_2
                    if (!(g->is_illegal(move_2.back() / size, move_2.back() % size)))
                    {
                        m->update_with_move(move_2.back());
                        g->execute_move(move_2.back());
                        move_2.pop_back();
                    }
                    else
                    {
                        std::cout << "ERROR Illegal pos! " << std::endl;
                        move_1.clear();
                        move_2.clear();
                        move_3.clear();
                        break;
                    }
                }
                else
                {
                    std::cout << "ERROR Illegal quantity gap! " << std::endl;
                    move_1.clear();
                    move_2.clear();
                    move_3.clear();
                    continue;
                }
            }
            else
            {
                // move_1 first
                reverse(move_1.begin(), move_1.end());
                reverse(move_2.begin(), move_2.end());
                size_t s = move_1.size();
                for (size_t i = 0; i < s; i++)
                {
                    if (!(g->is_illegal(move_1.back() / size, move_1.back() % size)))
                    {
                        m->update_with_move(move_1.back());
                        g->execute_move(move_1.back());
                        move_1.pop_back();
                    }
                    else
                    {
                        std::cout << "ERROR Illegal pos! " << std::endl;
                        move_1.clear();
                        move_2.clear();
                        move_3.clear();
                        break;
                    }
                    if (!(g->is_illegal(move_2.back() / size, move_2.back() % size)))
                    {
                        m->update_with_move(move_2.back());
                        g->execute_move(move_2.back());
                        move_2.pop_back();
                    }
                    else
                    {
                        std::cout << "ERROR Illegal pos! " << std::endl;
                        move_1.clear();
                        move_2.clear();
                        move_3.clear();
                        break;
                    }
                }
            }

            move_1.clear();
            move_2.clear();
            move_3.clear();

            std::vector<double> p = m->get_action_probs(g);
            int action = m->get_best_action_from_prob(p);
            // std::cout << "DEBUG action: " << action << std::endl;
            x = action / size;
            y = action % size;
            if (!g->is_illegal(x, y))
            {
                m->update_with_move(action);
                g->execute_move(action);
                std::cout << x << "," << y << std::endl;
            }
            else
            {
                std::cout << "ERROR Illegal move generated! " << std::endl;
            }
        }
        else if (command == "INFO")
        {
            unsigned int value = 0;
            std::string key;
            std::cin >> key;

            if (key == "timeout_turn")
            {
                char s_value[16] = "\0";
                std::cin >> s_value;
                if (isNumericString(s_value, strlen(s_value)))
                {
                    value = atoi(s_value);
                }
            }
            else if (key == "timeout_match")
            {
                char s_value[16] = "\0";
                std::cin >> s_value;
                if (isNumericString(s_value, strlen(s_value)))
                    value = atoi(s_value);
            }
            else if (key == "time_left")
            {
                char s_value[16] = "\0";
                std::cin >> s_value;
                if (isNumericString(s_value, strlen(s_value)))
                    value = atoi(s_value);

                if (value < 30000)
                {
                    if (nullptr != m)
                    {
                        delete m;
                        m = nullptr;
                    }
                    m = new MCTS(module.get(),
                                 (unsigned int)(u_num_mct_threads != 0 ? u_num_mct_threads : NUM_MCT_THREADS),
                                 C_PUCT,
                                 (unsigned int)log10((u_num_mct_sims != 0 ? u_num_mct_sims : NUM_MCT_SIMS) + 1) + 10,
                                 C_VIRTUAL_LOSS,
                                 BOARD_SIZE * BOARD_SIZE);
                }
            }
            else if (key == "max_memory")
            {
                char s_value[16] = "\0";
                std::cin >> s_value;
                if (isNumericString(s_value, strlen(s_value)))
                    value = atoi(s_value);
                // TODO
            }
            else if (key == "game_type")
            {
                char s_value[16] = "\0";
                std::cin >> s_value;
                if (isNumericString(s_value, strlen(s_value)))
                    value = atoi(s_value);
                // TODO
            }
            else if (key == "rule")
            {
                char s_value[16] = "\0";
                std::cin >> s_value;

                if (isPlaying)
                    continue;

                if (isNumericString(s_value, strlen(s_value)))
                    value = atoi(s_value);

                bool bChangeModule = false;
                // renju > caro > standard > free-style
                if (0 == value)
                {
                    if (s_model_path.find("free-style") == std::string::npos)
                    {
                        if (std::filesystem::exists(s_config_file_path))
                        {
                            std::cout << "MESSAGE change model path!" << std::endl;
                            toml_data = toml::parse(s_config_file_path);
                            if (toml_data["model"].is_table())
                            {
                                std::string s_tmp_path = exe_path.string() + toml::find<std::string>(toml_data["model"], "free_style_model");
                                if (std::filesystem::exists(s_tmp_path))
                                {
                                    std::cout << "MESSAGE model exists" << std::endl;
                                    s_model_path = s_tmp_path;
                                    std::cout << "MESSAGE model load path: " << s_model_path << std::endl;
                                    bChangeModule = true;
                                    g->set_rule(0);
                                }
                                else
                                {
                                    std::cout << "ERROR model not exists" << std::endl;
                                }
                            }
                        }
                        else
                            std::cout << "ERROR config file not exists" << std::endl;
                    }
                }
                else if (4 == (value & 4))
                {
                    if (s_model_path.find("renju") == std::string::npos)
                    {
                        if (std::filesystem::exists(s_config_file_path))
                        {
                            std::cout << "MESSAGE change model path!" << std::endl;
                            toml_data = toml::parse(s_config_file_path);
                            if (toml_data["model"].is_table())
                            {
                                std::string s_tmp_path = exe_path.string() + toml::find<std::string>(toml_data["model"], "renju_model");
                                if (std::filesystem::exists(s_tmp_path))
                                {
                                    std::cout << "MESSAGE model exists" << std::endl;
                                    s_model_path = s_tmp_path;
                                    std::cout << "MESSAGE model load path: " << s_model_path << std::endl;
                                    bChangeModule = true;
                                    g->set_rule(4);
                                }
                                else
                                {
                                    std::cout << "ERROR model not exists" << std::endl;
                                }
                            }
                        }
                        else
                            std::cout << "ERROR config file not exists" << std::endl;
                    }
                }
                else if (8 == (value & 8))
                {
                    // standard caro
                    if (1 == (value & 1))
                    {
                        if (s_model_path.find("standard_caro") == std::string::npos)
                        {
                            if (std::filesystem::exists(s_config_file_path))
                            {
                                std::cout << "MESSAGE change model path!" << std::endl;
                                toml_data = toml::parse(s_config_file_path);
                                if (toml_data["model"].is_table())
                                {
                                    std::string s_tmp_path = exe_path.string() + toml::find<std::string>(toml_data["model"], "standard_caro_model");
                                    if (std::filesystem::exists(s_tmp_path))
                                    {
                                        std::cout << "MESSAGE model exists" << std::endl;
                                        s_model_path = s_tmp_path;
                                        std::cout << "MESSAGE model load path: " << s_model_path << std::endl;
                                        bChangeModule = true;
                                        g->set_rule(9);
                                    }
                                    else
                                    {
                                        std::cout << "ERROR model not exists" << std::endl;
                                    }
                                }
                            }
                            else
                                std::cout << "ERROR config file not exists" << std::endl;
                        }
                    }
                    else
                    {
                        if (s_model_path.find("caro") == std::string::npos || (s_model_path.find("standard_caro") != std::string::npos))
                        {
                            if (std::filesystem::exists(s_config_file_path))
                            {
                                std::cout << "MESSAGE change model path!" << std::endl;
                                toml_data = toml::parse(s_config_file_path);
                                if (toml_data["model"].is_table())
                                {
                                    std::string s_tmp_path = exe_path.string() + toml::find<std::string>(toml_data["model"], "caro_model");
                                    if (std::filesystem::exists(s_tmp_path))
                                    {
                                        std::cout << "MESSAGE model exists" << std::endl;
                                        s_model_path = s_tmp_path;
                                        std::cout << "MESSAGE model load path: " << s_model_path << std::endl;
                                        bChangeModule = true;
                                        g->set_rule(8);
                                    }
                                    else
                                    {
                                        std::cout << "ERROR model not exists" << std::endl;
                                    }
                                }
                            }
                            else
                                std::cout << "ERROR config file not exists" << std::endl;
                        }
                    }
                }
                else if (1 == (value & 1))
                {
                    if (s_model_path.find("standard") == std::string::npos)
                    {
                        if (std::filesystem::exists(s_config_file_path))
                        {
                            std::cout << "MESSAGE change model path!" << std::endl;
                            toml_data = toml::parse(s_config_file_path);
                            if (toml_data["model"].is_table())
                            {
                                std::string s_tmp_path = exe_path.string() + toml::find<std::string>(toml_data["model"], "standard_model");
                                if (std::filesystem::exists(s_tmp_path))
                                {
                                    std::cout << "MESSAGE model exists" << std::endl;
                                    s_model_path = s_tmp_path;
                                    std::cout << "MESSAGE model load path: " << s_model_path << std::endl;
                                    bChangeModule = true;
                                    g->set_rule(1);
                                }
                                else
                                {
                                    std::cout << "ERROR model not exists" << std::endl;
                                }
                            }
                        }
                        else
                            std::cout << "ERROR config file not exists" << std::endl;
                    }
                }
                else
                    std::cout << "ERROR unsupport rule: " << value << std::endl;

                if (bChangeModule)
                {
                    if (std::filesystem::exists(s_model_path))
                    {
                        std::cout << "MESSAGE model exists" << std::endl;
                        std::cout << "MESSAGE game rule: " << g->get_rule() << std::endl;
                        module = std::make_shared<NeuralNetwork>(s_model_path, NUM_MCT_SIMS);
                        if (nullptr != m)
                        {
                            delete m;
                            m = nullptr;
                        }
                        m = new MCTS(module.get(),
                                     (unsigned int)(u_num_mct_threads != 0 ? u_num_mct_threads : NUM_MCT_THREADS),
                                     C_PUCT,
                                     (unsigned int)(u_num_mct_sims != 0 ? u_num_mct_sims : NUM_MCT_SIMS),
                                     C_VIRTUAL_LOSS,
                                     BOARD_SIZE * BOARD_SIZE);
                    }
                    else
                    {
                        std::cout << "ERROR model not exists" << std::endl;
                        continue;
                    }
                }
            }
            else if (key == "folder")
            {
                std::string t;
                std::cin >> t;
            }
            else
            {
                std::cout << "ERROR unsupport key: " << key << std::endl;
            }
        }
        else if (command == "ABOUT")
            std::cout << "name=\"Z2I\", version=\"0.2\", author=\"Joker2770\", country=\"CHN\"" << std::endl;
        else if (command == "END")
            break;
        else
            std::cout << "UNKNOWN unsupport command!" << std::endl;
    }

    if (nullptr != m)
    {
        delete m;
        m = nullptr;
    }

    if (nullptr != g)
    {
        delete g;
        g = nullptr;
    }

    return 0;
}