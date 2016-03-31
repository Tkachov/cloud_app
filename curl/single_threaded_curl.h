#pragma once
#include <string>
using std::string;

#include <curl/curl.h>

#include "../base_exception.h"

namespace curl {

class request; //ok, so now we've got a cycle dependency

class single_threaded_curl {
	static single_threaded_curl* _instance;
	static single_threaded_curl* instance();

	CURL* curl;

	single_threaded_curl();

public:
	class curl_exception: public base_exception {
	public:
		curl_exception(string msg): base_exception(msg) {};
	};

	~single_threaded_curl();

	//methods

	//static string GET(string url, string post_fields = "", struct curl_slist* list = 0) throw (curl_exception); //TODO: very strange to have GET which is also POST
	static string execute(request& rq);
};

} //namespace curl