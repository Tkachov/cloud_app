#include "single_threaded_curl.h"
#include "request.h"

namespace curl {

//totally not multithreading solution

single_threaded_curl* single_threaded_curl::_instance = 0;

single_threaded_curl* single_threaded_curl::instance() {
	if(_instance == 0) _instance = new single_threaded_curl();
	return _instance;
}

single_threaded_curl::single_threaded_curl() {
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if(!curl) throw curl_exception("failed to init curl"); //TODO: add some curl's error info
}

single_threaded_curl::~single_threaded_curl() {
	if(curl) curl_easy_cleanup(curl);
	curl_global_cleanup();
}

//public statics

string data;

size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up) {
	for (int i = 0; i<size*nmemb; ++i) data.push_back(buf[i]);
	return size*nmemb;
}

/*
string single_threaded_curl::GET(string url, string post_fields, struct curl_slist* list) {
	CURL* curl = instance()->curl;
	CURLcode res;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);
	if(list != 0) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
	//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE); //might be useful if you lose your CA certs bundle

	data = "";
	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
		throw curl_exception("Failed to GET:\n\t" + string(curl_easy_strerror(res)));

	return data;
}
*/

string single_threaded_curl::execute(request& rq) {	
	CURL* curl = instance()->curl;
	CURLcode res;
	curl_easy_setopt(curl, CURLOPT_URL, rq.get_url().c_str());	
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, rq.get_headers_list());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, rq.get_post_fields().length());
	curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, rq.get_post_fields().c_str());
	//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, rq.get_post_fields().c_str());
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	data = "";
	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
		throw curl_exception("Failed to GET:\n\t" + string(curl_easy_strerror(res)));

	return data;
}

} //namespace curl