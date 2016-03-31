#pragma once

#include <string>
using std::string;

#include <curl/curl.h>

namespace curl {

class request {
	string url, post_fields;
	struct curl_slist* list;

public:
	request(string url);
	~request();

	void add_post_field(string key_value_pair);
	void add_post_field(string key, string value);
	void add_header(string header_with_value);
	void add_header(string header, string value);

	string execute();

	string get_url() { return url;  }
	string get_post_fields() { return post_fields; }
	struct curl_slist* get_headers_list() { return list; }
};

} //namespace curl