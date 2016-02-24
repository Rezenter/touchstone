#include <string>

#pragma once
using namespace std;

class Model
{
private:
	string matLibrary;

public:

	Model(string matL)
	{
		matLibrary = matL;
	}

	~Model()
	{
	}
};

