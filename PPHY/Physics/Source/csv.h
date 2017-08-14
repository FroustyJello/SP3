#pragma once
#include <string>
#include <sstream>
#include <vector>
#include<fstream>
#include <iostream>

using std::ifstream;
using std::string;
using std::vector;
using std::stringstream;

class CSV
{
private:
	ifstream* theFile;
	vector<string>MonsterList;

public:
	CSV();
	~CSV();
	vector<string> Load(string filepath, vector<string>Data);
	void Print(vector<string> Data);
	//void Print(string filepath);
};