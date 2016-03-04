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
	string mtllib;
	string name;
	Model res;

public:
	ObjReader(string path, string n)
	{
		Model res(name);
		name = n;
		Log log("objReaderLog.txt");
		log.write("constructor(" + path + name + ".obj)");
		if (_access((path + name + ".obj").c_str(), 0) == -1) {
			log.write("obj not found");
		}
		else {
			int i = 0;
			log.write("obj found");
			file.open(path + name + ".obj");
			while (getline(file, line)) {
				if (line.substr(0, 7) == "mtllib ") {
					mtllib = line.substr(7);
				}
				else if (line.substr(0, 9) == "# object ") {
					res.objects.push_back(Object(line.substr(9)));
				}
			}
		}
	}

	Model getModel() {
		return res;
	}

	~ObjReader()
	{
		file.close();
	}
};
