#include <string>
#include <vector>

#pragma once
using namespace std;

class Object
{
private:
	string name;

public:

	Object() {
		name = "null";
	}

	Object(string n) {
		name = n;
	}

	~Object()
	{

	}
};

class Model
{
private:
	string matLibrary;
	string name;

public:
	
	vector <Object> objects;

	Model() {
		name = "uninitialised";
	}
	
	Model(string n)
	{
		name = n;
	}

	string getName() {
		return name;
	}

	~Model()
	{

	}
};



