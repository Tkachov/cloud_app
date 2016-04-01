#pragma once
#include <fstream>
#include <string>
using std::ofstream;
using std::string;

#include "../base_exception.h"

namespace cloud {

class storage {
public:	
	//api
	virtual string info() = 0;
	virtual bool upload(string file) = 0;

	//utils
	virtual void save(ofstream& fout) = 0;
	virtual string name() const = 0;

	class storage_auth_exception: public base_exception {
	public:
		storage_auth_exception(): base_exception("failed to auth") {};
	};
};

} //namespace cloud