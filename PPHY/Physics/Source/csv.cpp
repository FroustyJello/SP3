#include "csv.h"

CSV::CSV()
{
}

CSV::~CSV()
{
	delete theFile;
}

vector<string> CSV::Load(string filepath, vector<string>data)
{
	string line;
	theFile = new ifstream(filepath);

	while (std::getline(*theFile, line))
	{
		while (std::getline(*theFile, line))
		{
			data.push_back(line);
		}
	}

	std::cout << "Finished loading " << data.size() << "variables" << std::endl;

	return data;

}

void CSV::Print(vector<string> Data)
{
	for (int i = 0; i < Data.size(); i++)
	{
		std::cout << Data[i] << std::endl;
	}

	std::cout << "Successfully printed " << Data.size() << "Items" << std::endl;
}