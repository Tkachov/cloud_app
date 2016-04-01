#pragma once
#include <string>
#include <vector>
using std::string;
using std::vector;
using std::pair;

#include "cloud/storage_manager.h"
#include "cloud/storage.h"
using cloud::storage_manager;
using cloud::storage;

class machine {
	typedef vector< pair<string, void(machine::*)()> > answers_vector;

	bool working;
	storage_manager manager;

	//utils
	bool ask(answers_vector& answers);		
	string shorten_link(string& link);

	//answers handlers
	void info();
	void turn_on();
	void connect();
	void upload();
	void quit() { working = false; }

public:
	machine();
	~machine();

	bool is_working() { return working; }
	void work();	
};
