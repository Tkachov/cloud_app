#pragma once

#include <string>
using std::string;

class machine {
	bool working;

	string token, uid;

	void info();
	void turn_on();
	void quit() { working = false; }
	string load(string fn);

public:
	machine();
	~machine();

	bool is_working() { return working; }
	void work();

	bool parse_json(string json);
};
