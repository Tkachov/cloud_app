#pragma once

#include "file.h"

#include <string>
#include <map>
using std::string;
using std::map;

namespace files {

class directory: public file {
	map<string, file*> contents;
public:
	directory(): file("", "", 0, 0) {}; //for map
	directory(string path);
	directory(string path, string name);
	~directory();

	void add_file(file* f);
	void recalculate();
	void print(int level = 1);
};

} //namespace files