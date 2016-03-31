#include "machine.h"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

#include <curl/curl.h>

#include "cloud\dropbox_storage.h"
#include "base_exception.h"
using cloud::dropbox::dropbox_storage;

machine::machine() : working(true) {
	current_storage = 0;

	cout << "+=================================================+\n";
	cout << "|                                                 |\n";
	cout << "|         CCCC  L       OOO   U   U  DDDD         |\n";
	cout << "|        C      L      O   O  U   U  D   D        |\n";
	cout << "|        C      L      O   O  U   U  D   D        |\n";
	cout << "|        C      L      O   O  U   U  D   D        |\n";
	cout << "|         CCCC  LLLLL   OOO    UUU   DDDD         |\n";
	cout << "|                                                 |\n";
	cout << "| M   M   AAA    CCCC  H   H  IIIII  N   N  EEEEE |\n";
	cout << "| MM MM  A   A  C      H   H    I    NN  N  E     |\n";
	cout << "| M M M  AAAAA  C      HHHHH    I    N N N  EEEEE |\n";
	cout << "| M   M  A   A  C      H   H    I    N  NN  E     |\n";
	cout << "| M   M  A   A   CCCC  H   H  IIIII  N   N  EEEEE |\n";
	cout << "|                                                 |\n";
	cout << "+=================================================+\n";
	cout << "\n";
};

machine::~machine() { delete current_storage;  }

void machine::work() {
	answers_vector answers;
	if (!current_storage) {
		answers.push_back(std::make_pair("Get Dropbox OAuth URL", &machine::turn_on));
		answers.push_back(std::make_pair("Provide Dropbox OAuth code", &machine::connect));
	} else {
		answers.push_back(std::make_pair("See Dropbox account info", &machine::info));
	}
	answers.push_back(std::make_pair("Quit", &machine::quit));
	ask(answers);
}

//private here:

//utils

bool machine::ask(answers_vector& answers) {
	cout << "----\nSelect an action:\n";
	for (int i = 0; i<answers.size(); ++i)
		cout << (i + 1) << ": " << answers[i].first << "\n";
	cout << "----\n\n";
	cout.flush();

	int answer_index;
	cin >> answer_index;
	if (answer_index > 0 && answer_index <= answers.size()) {
		(this->*answers[answer_index - 1].second)();
		return true;
	}

	cout << "\n";
	return false;
}

//answers handlers

void machine::turn_on() {
	//TODO: url shortener	
	cout << "Navigate to this URL and press \"Allow\":\n";
	cout << dropbox_storage::get_auth_link() << "\n\n";
	cout.flush();
}

void machine::connect() {
	cout << "Enter your code:\n";
	string code;
	cin >> code;
	if (code == "") return; //cancelled

	try {
		current_storage = dropbox_storage::connect_with_code(code);
	}
	catch (base_exception& e) {
		cout << e.what() << "\n";
	}
	catch (...) {
		cout << "failed\n";
	}
}

void machine::info() {	
	cout << "\nThis is your account info:\n\n";
	cout.flush();
	cout << current_storage->info();
	cout << "\n\n";
	cout.flush();	
}