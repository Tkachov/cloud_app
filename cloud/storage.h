#pragma once
#include <string>
using std::string;

#include "../base_exception.h"

namespace cloud {

class storage {
public:	
	virtual string info() = 0;
	virtual bool upload(string file) = 0;

	class storage_auth_exception: public base_exception {
	public:
		storage_auth_exception(): base_exception("failed to auth") {};
	};
};

} //namespace cloud