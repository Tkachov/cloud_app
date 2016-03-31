#include "CurlSingleThread.h"

//totally not multithreading solution

CurlSingleThread* CurlSingleThread::_instance = 0;

CurlSingleThread* CurlSingleThread::instance() {
	if(_instance == 0) _instance = new CurlSingleThread();
	return _instance;
}

CurlSingleThread::CurlSingleThread() {
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if(!curl) throw curl_exception("failed to init curl"); //TODO: add some curl's error info
}

CurlSingleThread::~CurlSingleThread() {
	if(curl) curl_easy_cleanup(curl);
	curl_global_cleanup();
}

//public statics

string data;

size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up) {
	for (int i = 0; i<size*nmemb; ++i) data.push_back(buf[i]);
	return size*nmemb;
}

string CurlSingleThread::GET(string url, string post_fields, struct curl_slist* list) {
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