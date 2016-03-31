#pragma once
#include <string>
using std::string;

#include "../base_exception.h"

namespace json {
	class json_exception: public base_exception {
	public:
		json_exception(string msg): base_exception(msg) {};
	};

	//methods

	string get_value(string json, string name) throw (json_exception);
}