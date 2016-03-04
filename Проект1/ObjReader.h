#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Model.h"
#include "Log.h"

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
		Log log("objReaderLog.txt");
		log.write("constructor(" + path + ")");
		if (_access(path.c_str(), 0) == -1) {
			log.write("file not found");
		}
		else {
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
	}

	void createModel() {


	}

	~ObjReader()
	{
		file.close();
	}
};
