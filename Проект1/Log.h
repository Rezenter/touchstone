#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

#pragma once

using namespace std;

class Log
{
private:
	ofstream file;
public:
	Log(string path)
	{
		file.open(path);
		this->write("");
	}

	void write(string out) {
		struct tm newtime;
		__time64_t long_time;
		char timebuf[26];
		errno_t err;
		_time64(&long_time);
		err = _localtime64_s(&newtime, &long_time);
		err = asctime_s(timebuf, 26, &newtime);
		string res = timebuf;
		res = res.substr(0, res.length()-1);
		file << res << ": " << out << endl;
	}

	~Log()
	{
		file.close();
	}
};