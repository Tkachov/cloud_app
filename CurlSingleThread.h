#pragma once
#include <string>
using std::string;

#include <curl/curl.h>

#include "base_exception.h"

class CurlSingleThread {
	static CurlSingleThread* _instance;
	static CurlSingleThread* instance();

	CURL* curl;

	CurlSingleThread();

public:
	class curl_exception: public base_exception {
	public:
		curl_exception(string msg): base_exception(msg) {};
	};

	~CurlSingleThread();

	//methods

	static string GET(string url, string post_fields = "", struct curl_slist* list = 0) throw (curl_exception); //TODO: very strange to have GET which is also POST
};