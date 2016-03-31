#include "request.h"
#include "single_threaded_curl.h"

namespace curl {

request::request(string URL):
	url(URL), post_fields(""), list(NULL)
{
}

request::~request() {
	curl_slist_free_all(list);
}

void request::add_post_field(string key_value_pair) {
	if (post_fields == "") post_fields = key_value_pair;
	else post_fields += "&" + key_value_pair;
}

void request::add_post_field(string key, string value) {
	add_post_field(key + "=" + value);
}

void request::add_header(string header_with_value) {
	list = curl_slist_append(list, header_with_value.c_str());
}

void request::add_header(string header, string value) {
	add_header(header + ": " + value);
}

string request::execute() {
	return single_threaded_curl::execute(*this);
}

} //namespace curl