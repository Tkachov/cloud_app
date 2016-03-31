#include "dropbox_storage.h"

#include <fstream>
using std::ifstream;

#include <iostream>
using std::cout;

#include <curl/curl.h>

#include "../curl/request.h"
#include "../json/utils.h"
using namespace json;

namespace cloud { namespace dropbox {

//static:

const string dropbox_storage::KEY = dropbox_storage::load_contents("private/app_key.txt");
const string dropbox_storage::SECRET = dropbox_storage::load_contents("private/app_secret.txt");

string dropbox_storage::load_contents(string filename) {
	ifstream fin(filename.c_str());
	string key;
	fin >> key;
	fin.close();
	return key;
}

//static public:

string dropbox_storage::get_auth_link() {
	const string DROPBOX_URL = "https://www.dropbox.com/1/oauth2/authorize";
	string url = DROPBOX_URL;
	url += "?response_type=code";
	url += "&redirect_uri=http%3A%2F%2Flocalhost%3A12345%2F";// http://localhost:12345/"; //it's "http%3A%2F%2Flocalhost%3A12345%2F" if you want to open that url
	url += "&client_id=" + KEY;
	return url;
}

dropbox_storage* dropbox_storage::connect_with_code(string code) {
	const string TOKEN_URL = "https://api.dropboxapi.com/1/oauth2/token";	
	if (code == "") throw storage::storage_auth_exception();

	curl::request rq(TOKEN_URL);
	rq.add_post_field("code=" + code);
	rq.add_post_field("grant_type=authorization_code");
	rq.add_post_field("client_id=" + KEY);
	rq.add_post_field("client_secret=" + SECRET);
	rq.add_post_field("&redirect_uri=http%3A%2F%2Flocalhost%3A12345%2F");
	
	string data = rq.execute();
	string token = json::get_value(data, "access_token");
	string uid = json::get_value(data, "uid");

	if (token == "" || uid == "")
		throw base_exception(data);
		//throw storage::storage_auth_exception();
	return new dropbox_storage(token, uid);
}

//non-static public:

dropbox_storage::~dropbox_storage() {}

string dropbox_storage::info() {
	curl::request rq("https://api.dropboxapi.com/1/account/info");
	rq.add_header("Authorization: Bearer " + token);
	return rq.execute();	
}

//private

dropbox_storage::dropbox_storage(string& gathered_token, string& gathered_uid) {
	token = gathered_token;
	uid = gathered_uid;
}

} } //namespace cloud::dropbox