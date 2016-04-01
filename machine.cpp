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
#include "curl\request.h"
#include "json\utils.h"
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
		answers.push_back(std::make_pair("Upload a file", &machine::upload));
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

string load(string name) {
	string n;
	ifstream fin(name.c_str());
	fin >> n;
	fin.close();
	return n;
}

string machine::shorten_link(string& link) {
	static const string GOOGLE_URL_SHORTENER_KEY = load("private/google_url_shortener/api_key.txt");
	curl::request rq("https://www.googleapis.com/urlshortener/v1/url?key="+GOOGLE_URL_SHORTENER_KEY);
	rq.add_header("Content-Type: application/json");
	rq.add_post_field(string("{\"longUrl\": \"")+link+string("\"}"));

	string data = rq.execute();
	string new_link = json::get_value(data, "id");
	if (new_link == "") return link;
	return new_link;
}

//answers handlers

void machine::turn_on() {	
	cout << "Navigate to this URL and press \"Allow\":\n";
	cout << shorten_link(dropbox_storage::get_auth_link()) << "\n\n";
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

void machine::upload() {
	cout << "Enter file name:\n";
	string code;
	cin >> code;
	if (code == "") return; //cancelled

	try {
		current_storage->upload(code);
	}
	catch (base_exception& e) {
		cout << e.what() << "\n";
	}
	catch (...) {
		cout << "failed\n";
	}
}