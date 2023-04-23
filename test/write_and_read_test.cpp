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

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include<iostream>
#include<fstream>

#include <tuple>
#include <vector>


using namespace std;

int main() {
	ofstream bestand;
	vector<vector<float>> v(7, vector<float>(5));
	const char* pointer;

	bestand.open("test", ios::out | ios::binary);
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 5; j++) {
			v[i][j] = i + 0.001 * j;
		}
	}
	cout << v.size() << endl;
	size_t bytes = 35 * sizeof(float);
	for (int i = 0; i < 7; i++) {
		pointer = reinterpret_cast<const char*>(&v[i][0]);
		bestand.write(reinterpret_cast<char*>(&v[i][0]), 5 * sizeof(float));
	}
	bestand.close();


	ifstream inlezen;
	vector<vector<float>> v2(7, vector<float>(5));
	inlezen.open("test", ios::in | ios::binary);
	//char byte[8];
	//bytes = v2.size() * sizeof(v2[0]);
	for (int i = 0; i < 7; i++) {
		inlezen.read(reinterpret_cast<char*>(&v2[i][0]), 5 * sizeof(float));
	}
	
		
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 5; j++) {
			cout << v[i][j] << endl;
		}
	}
}