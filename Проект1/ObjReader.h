#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Model.h"

#pragma once

using namespace std;

class ObjReader
{
private:
	ifstream file;
	string line;
	string mtlib;

public:
	ObjReader(string path)
	{
		file.open(path);
		while (getline(file, line)) {
			istringstream iss(line);
			line = iss.str();
			if (line.substr(0, 7) == "mtlib ") {
				mtlib = line.substr(8);
			}
			else if (line.substr(0, 1) == "#") {
				int i = 0;
			}
		}
	}

	~ObjReader()
	{
		file.close();
	}
};
