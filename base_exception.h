#pragma once
#include <exception>
#include <string>
using std::exception;
using std::string;

class base_exception: public exception {
	string message;
public:
	base_exception(string msg): message(msg) {};

	virtual const char* what() const {
		return message.c_str();
	}
};