/*************************************************************************
    > File Name: pbrain-Z2I.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: Mon Apr 24 12:39:45 2023
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

#include "../mcts.h"
#include "../common.h"
#include "../onnx.h"

#include <string>
#include <cstring>
#include <vector>
#include <filesystem>
#include <iostream>
using namespace std;

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

vector<string> split(const string &str, const string &pattern)
{
    string::size_type pos;
    vector<string> result;
    string strs = str + pattern;
    size_t size = strs.size();
    for (size_t i = 0; i < size; ++i) {
        pos = strs.find(pattern, i);
        if (pos < size)
        {
            string s = strs.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

void toupper(string &str)
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
    cout << "MESSAGE ................................................................................." << endl;
    cout << "MESSAGE MIT License" << endl;
    cout << "MESSAGE " << endl;
    cout << "MESSAGE Copyright (c) 2023 Joker2770" << endl;
    cout << "MESSAGE " << endl;
    cout << "MESSAGE Permission is hereby granted, free of charge, to any person obtaining a copy" << endl;
    cout << "MESSAGE of this software and associated documentation files (the \"Software\"), to deal" << endl;
    cout << "MESSAGE in the Software without restriction, including without limitation the rights" << endl;
    cout << "MESSAGE to use, copy, modify, merge, publish, distribute, sublicense, and/or sell" << endl;
    cout << "MESSAGE copies of the Software, and to permit persons to whom the Software is" << endl;
    cout << "MESSAGE furnished to do so, subject to the following conditions:" << endl;
    cout << "MESSAGE MESSAGE " << endl;
    cout << "MESSAGE The above copyright notice and this permission notice shall be included in all" << endl;
    cout << "MESSAGE copies or substantial portions of the Software." << endl;
    cout << "MESSAGE " << endl;
    cout << "MESSAGE THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR" << endl;
    cout << "MESSAGE IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY," << endl;
    cout << "MESSAGE FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE" << endl;
    cout << "MESSAGE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER" << endl;
    cout << "MESSAGE LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM," << endl;
    cout << "MESSAGE OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE" << endl;
    cout << "MESSAGE SOFTWARE." << endl;
    cout << "MESSAGE " << endl;
    cout << "MESSAGE ................................................................................." << endl;

    std::shared_ptr<NeuralNetwork> module = nullptr;
    std::filesystem::path exe_path = std::filesystem::canonical(std::filesystem::path(argv[0])).remove_filename();
    string s_model_path = exe_path.string() + "free-style_15x15_502.onnx";
    cout << "MESSAGE model load path: " << s_model_path << endl;
    if (std::filesystem::exists(s_model_path))
    {
        cout << "MESSAGE model exists" << endl;
        module = std::make_shared<NeuralNetwork>(s_model_path, NUM_MCT_SIMS);
    }
    else
    {
        cout << "ERROR model not exists" << endl;
        return -1;
    }

    Gomoku *g = new Gomoku(BORAD_SIZE, N_IN_ROW, BLACK);
    g->set_rule(0);
    cout << "MESSAGE game rule: " << g->get_rule() << endl;

    unsigned int u_timeout_turn = 30000;
#ifdef USE_CUDA
    double per_sims = (double)(u_timeout_turn) / (double)(4 * 3000);
#else
    double per_sims = (double)(u_timeout_turn) / (double)(4 * 30000);
#endif
    cout << "DEBUG per_sims: " << per_sims << endl;

    MCTS *m;
    m = new MCTS(module.get(), NUM_MCT_THREADS, C_PUCT, (unsigned int)(NUM_MCT_SIMS * log(NUM_MCT_THREADS) * per_sims), C_VIRTUAL_LOSS, BORAD_SIZE * BORAD_SIZE);

    string command;
    unsigned int size;
	char dot;
    for (;;)
	{
		cin >> command;
		toupper(command);

		if (command == "START")
		{
            char s_size[4] = "\0";
            cin >> s_size;
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
            g = new Gomoku(BORAD_SIZE, N_IN_ROW, BLACK);

            if (size == 15)
            {
                cout << "OK" << endl;
            }
            else
			{
				cout << "ERROR" << endl;
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
            int res = m->get_best_action(g);
            m->update_with_move(res);
            g->execute_move(res);
            unsigned int x = res / size;
            unsigned int y = res % size;
            cout << x << "," << y << endl;
        }
        else if (command == "TURN")
        {
            unsigned int x, y;
            cin >> x >> dot >> y;
            if (!(g->is_illegal(x, y)))
            {
                int move = x * size + y;
                m->update_with_move(move);
                g->execute_move(move);
                std::vector<double> p = m->get_action_probs(g);
                int action = m->get_best_action_from_prob(p);
                x = action / size;
                y =  action % size;
                if (!g->is_illegal(x, y))
                {
                    m->update_with_move(action);
                    g->execute_move(action);
                    cout << x << "," << y << endl;
                }
                else
                {
                    cout << "ERROR Illegal move generated! " << endl;
                }
            }
            else
			{
				cout << "ERROR Illegal move from opponent! " << endl;
			}
        }
        else if (command == "BOARD")
		{
			unsigned int x, y, c;
            vector<int> move_1, move_2, move_3;

            if (nullptr != g)
            {
                delete g;
                g = nullptr;
            }
            g = new Gomoku(BORAD_SIZE, N_IN_ROW, BLACK);

            cin >> command;
			while (command != "DONE")
            {
                //cout << "DEBUG " << command << endl;
                vector<string> v_s = split(command, ",");
                if ((command.find_first_of(',', 0) != command.find_last_of(',', 0)) && v_s.size() == 3)
                {
                    // cout << "DEBUG " << v_s.at(0) << "," << v_s.at(1) << "," << v_s.at(2) << endl;
                    if (isNumericString(v_s.at(0).c_str(), v_s.at(0).length()) &&
                     isNumericString(v_s.at(1).c_str(), v_s.at(1).length()) &&
                      isNumericString(v_s.at(2).c_str(), v_s.at(2).length()))
                    {
                        x = atoi(v_s.at(0).c_str());
                        y = atoi(v_s.at(1).c_str());
                        c = atoi(v_s.at(2).c_str());
                        int move = x * size + y;
                        //cout << "DEBUG move: " << move << endl;
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
                        cout << "ERROR Illegal pos! " << endl;
                        move_1.clear();
                        move_2.clear();
                        move_3.clear();
                        break;
                    }
                }
                else
                {
                    cout << "ERROR Illegal board! " << endl;
                    move_1.clear();
                    move_2.clear();
                    move_3.clear();
                    break;
                }
                cin >> command;
            }
            // cout << "DEBUG move_1 size: " << move_1.size() << endl;
            // cout << "DEBUG move_2 size: " << move_2.size() << endl;
            // cout << "DEBUG move_3 size: " << move_3.size() << endl;

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
                        cout << "ERROR Illegal pos! " << endl;
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
                        cout << x << "," << y << endl;
                    }
                    else
                    {
                        cout << "ERROR Illegal move generated! " << endl;
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
                            cout << "ERROR Illegal pos! " << endl;
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
                            cout << "ERROR Illegal pos! " << endl;
                            move_1.clear();
                            move_2.clear();
                            move_3.clear();
                            break;
                        }
                    }
                    //last move of move_2
                    if (!(g->is_illegal(move_2.back() / size, move_2.back() % size)))
                    {
                        m->update_with_move(move_2.back());
                        g->execute_move(move_2.back());
                        move_2.pop_back();
                    }
                    else
                    {
                        cout << "ERROR Illegal pos! " << endl;
                        move_1.clear();
                        move_2.clear();
                        move_3.clear();
                        break;
                    }
                }
                else
                {
                    cout << "ERROR Illegal quantity gap! " << endl;
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
                        cout << "ERROR Illegal pos! " << endl;
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
                        cout << "ERROR Illegal pos! " << endl;
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
            //cout << "DEBUG action: " << action << endl;
            x = action / size;
            y = action % size;
            if (!g->is_illegal(x, y))
            {
                m->update_with_move(action);
                g->execute_move(action);
                cout << x << "," << y << endl;
            }
            else
            {
                cout << "ERROR Illegal move generated! " << endl;
            }
        }
        else if (command == "INFO")
		{
			unsigned int value = 0;
			string key;
			cin >> key;

            if (key == "timeout_turn")
            {
                char s_value[16] = "\0";
                cin >> s_value;
                if (isNumericString(s_value, strlen(s_value)))
                {
                    value = atoi(s_value);
                }

                u_timeout_turn = value;
            }
            else if (key == "timeout_match")
			{
                char s_value[16] = "\0";
                cin >> s_value;
                if (isNumericString(s_value, strlen(s_value)))
                    value = atoi(s_value);

                // if (value != 0)
                // {
                //     if (value < u_timeout_turn)
                //     {
                //         if (nullptr != m)
                //         {
                //             delete m;
                //             m = nullptr;
                //         }
                //         m = new MCTS(module, NUM_MCT_THREADS, C_PUCT, (unsigned int)(NUM_MCT_SIMS * NUM_MCT_THREADS * (value / u_timeout_turn) * per_sims + 1), C_VIRTUAL_LOSS, BORAD_SIZE * BORAD_SIZE);
                //     }
                // }
            }
            else if (key == "time_left")
            {
                char s_value[16] = "\0";
                cin >> s_value;
                if (isNumericString(s_value, strlen(s_value)))
                    value = atoi(s_value);

                if (value != 0)
                {
                    if (value < u_timeout_turn)
                    {
                        if (nullptr != m)
                        {
                            delete m;
                            m = nullptr;
                        }
                        m = new MCTS(module.get(), NUM_MCT_THREADS, C_PUCT, (unsigned int)(NUM_MCT_SIMS * log(NUM_MCT_THREADS) * per_sims * (value * 0.1 / u_timeout_turn) + 1), C_VIRTUAL_LOSS, BORAD_SIZE * BORAD_SIZE);
                    }
                }
            }
            else if (key == "max_memory")
			{
                char s_value[16] = "\0";
                cin >> s_value;
                if (isNumericString(s_value, strlen(s_value)))
                    value = atoi(s_value);
				// TODO
			}
			else if (key == "game_type")
			{
                char s_value[16] = "\0";
                cin >> s_value;
                if (isNumericString(s_value, strlen(s_value)))
                    value = atoi(s_value);
				// TODO
			}
			else if (key == "rule")
			{
                char s_value[16] = "\0";
                cin >> s_value;
                if (isNumericString(s_value, strlen(s_value)))
                    value = atoi(s_value);

                if (0 != value)
                {
                    bool bChangeModule = false;
                    // renju > caro > standard > free-style
                    if (0 == value)
                    {
                        bChangeModule = true;
                        s_model_path = exe_path.string() + "free-style_15x15_502.onnx";
                        cout << "MESSAGE model load path: " << s_model_path << endl;
                        g->set_rule(0);
                    }
                    else if (4 == (value & 4))
                    {
                        bChangeModule = true;
                        s_model_path = exe_path.string() + "renju_15x15_487.onnx";
                        cout << "MESSAGE model load path: " << s_model_path << endl;
                        g->set_rule(4);
                    }
                    else if (8 == (value & 8))
                    {
                        bChangeModule = true;
                        s_model_path = exe_path.string() + "caro_15x15_0.onnx";
                        cout << "MESSAGE model load path: " << s_model_path << endl;
                        g->set_rule(8);
                    }
                    else if (1 == (value & 1))
                    {
                        bChangeModule = true;
                        s_model_path = exe_path.string() + "standard_15x15_479.onnx";
                        cout << "MESSAGE model load path: " << s_model_path << endl;
                        g->set_rule(1);
                    }
                    else
                        cout << "ERROR unsupport rule: " << value << endl;

                    if (bChangeModule)
                    {
                        if (std::filesystem::exists(s_model_path))
                        {
                            cout << "MESSAGE model exists" << endl;
                            cout << "MESSAGE game rule: " << g->get_rule() << endl;
                            module = std::make_shared<NeuralNetwork>(s_model_path, NUM_MCT_SIMS);
                            if (nullptr != m)
                            {
                                delete m;
                                m = nullptr;
                            }
                            m = new MCTS(module.get(), NUM_MCT_THREADS, C_PUCT, (unsigned int)(NUM_MCT_SIMS * log(NUM_MCT_THREADS) * per_sims), C_VIRTUAL_LOSS, BORAD_SIZE * BORAD_SIZE);
                        }
                        else
                        {
                            cout << "ERROR model not exists" << endl;
                            continue;
                        }
                    }
                }
            }
			else if (key == "folder")
			{
				string t;
				cin >> t;
			}
            else
            {
                cout << "ERROR unsupport key: " << key << endl;
            }
        }
		else if (command == "ABOUT")
		{
			cout << "name=\"Z2I\", version=\"0.1\", author=\"Joker2770\", country=\"CHN\"" << endl;
		}
		else if (command == "END")
		{
			break;
		}
        else
            cout << "UNKNOWN unsupport command!" << endl;
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