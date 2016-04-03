#include "dropbox_storage.h"

#include <fstream>
using std::ifstream;

#include <iostream>
using std::cout;

#include <curl/curl.h>
#include "../json/json11.hpp"
using namespace json11;

#include "../curl/request.h"

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
	string error_message;
	Json json = Json::parse(data, error_message);
	if(error_message.size())
		throw base_exception(error_message);

	string token = json["access_token"].string_value();
	string uid = json["uid"].string_value();

	if (token == "" || uid == "")
		throw base_exception(data);
		//throw storage::storage_auth_exception();
	return new dropbox_storage(token, uid);
}

dropbox_storage* dropbox_storage::load(ifstream& fin) {
	string token, uid;
	fin >> token;
	fin >> uid;
	return new dropbox_storage(token, uid);
}

//non-static public:

dropbox_storage::~dropbox_storage() {}

string dropbox_storage::info() {
	curl::request rq("https://api.dropboxapi.com/1/account/info");
	rq.add_header("Authorization: Bearer " + token);
	return rq.execute();	
}

bool dropbox_storage::upload(string file) {
	string file_binary_contents;
	ifstream fin(file.c_str(), std::ifstream::binary);
	char buf[4096];
	while (!fin.eof()) {
		fin.read(buf, 4096);
		file_binary_contents.append(string(buf, fin.gcount()));
	}
	fin.close();

	string api_args;

	string path = file;
	if (file.find('/') != -1)
		path = file.substr(file.rfind('/')+1);
	if (file.find('\\') != -1)
		path = file.substr(file.rfind('\\')+1);
	path = "/" + path;
	string mode = "overwrite";
	bool autorename = false;
	bool mute = false;

	//TODO: json11
	api_args = "{" +
		string("\"path\": \"") + path + string("\", ") +
		"\"mode\": \"" + mode + "\", " +
		"\"autorename\": " + (autorename?"true":"false") + ", "
		"\"mute\": " + (mute?"true":"false") +
	"}";

	curl::request rq("https://content.dropboxapi.com/2/files/upload");
	rq.add_header("Authorization: Bearer " + token);
	rq.add_header("Dropbox-API-Arg", api_args);
	rq.add_header("Content-Type: application/octet-stream");
	rq.add_post_field(file_binary_contents);
	cout << rq.execute();

	return false;
}

vector<string> dropbox_storage::list_directory(string directory, bool recursive) {
	curl::request rq("https://api.dropboxapi.com/2/files/list_folder");
	rq.add_header("Authorization: Bearer " + token);
	rq.add_header("Content-Type: application/json");

	Json json = Json::object({
		{ "path", directory }, //"path": directory,
		{ "recursive", recursive },
		{ "include_media_info", false },
		{ "include_deleted", false },
	});

	rq.add_post_field(json.dump());

	string data = rq.execute();
	string error_message;
	Json answer = Json::parse(data, error_message);
	if (error_message.size()) throw base_exception("Json Error 1: " + error_message + string("\n\n") + data);
	if (answer["error_summary"].string_value().size())
		throw base_exception("Dropbox Error: " + answer["error_summary"].string_value());

	vector<string> result;
	bool has_more = true;
	while(has_more) {
		for(auto item: answer["entries"].array_items()) {
			result.push_back(item["path_display"].string_value());
		}
		has_more = answer["has_more"].bool_value();

		if (has_more) {
			curl::request rq("https://api.dropboxapi.com/2/files/list_folder/continue");
			rq.add_header("Authorization: Bearer " + token);
			rq.add_header("Content-Type: application/json");

			Json json = Json::object({
				{ "cursor", answer["cursor"].string_value() },
			});

			rq.add_post_field(json.dump());
			
			data = rq.execute();
			answer = Json::parse(data, error_message);
			if (error_message.size()) throw base_exception("Json Error 2: " + error_message + string("\n\n") + data);
			if (answer["error_summary"].string_value().size())
				throw base_exception("Dropbox Error: " + answer["error_summary"].string_value());
		}
	}

	return result;
}

string get_substr(string s, size_t beginning, size_t ending) {
	//beginning inclusive, ending exclusive
	if (beginning == -1) throw base_exception("get_substr: bad beginning index");
	if (ending == -1) throw base_exception("get_substr: bad ending index"); //or may be just get the string until its end then?
	return s.substr(beginning, ending - beginning);
}

int parse_int(string s) {
	return atoi(s.c_str());
}

size_t convert_to_timestamp(string date_in_ISO_8601) {
	if (date_in_ISO_8601 == "") return 0;

	//2015-05-12T15:50:38Z	
	size_t first_hyphen = date_in_ISO_8601.find('-');
	size_t second_hyphen = date_in_ISO_8601.find('-', first_hyphen + 1);
	size_t T_separator = date_in_ISO_8601.find('T', second_hyphen + 1);
	size_t first_colon = date_in_ISO_8601.find(':', T_separator + 1);
	size_t second_colon = date_in_ISO_8601.find(':', first_colon + 1);
	size_t Z_separator = date_in_ISO_8601.find('Z', second_colon + 1);	
	//now note '+1' which means if there ever was '-1' result of find(), we still did a valid find() from 0th char

	string year = get_substr(date_in_ISO_8601, 0, first_hyphen);
	string month = get_substr(date_in_ISO_8601, first_hyphen+1, second_hyphen);
	string day = get_substr(date_in_ISO_8601, second_hyphen+1, T_separator);
	string hour = get_substr(date_in_ISO_8601, T_separator+1, first_colon);
	string minute = get_substr(date_in_ISO_8601, first_colon+1, second_colon);
	string second = get_substr(date_in_ISO_8601, second_colon+1, Z_separator);
	//now note only 'ending' argument was not '+1' (which means I could've make that function such that -1 means 'until the end')

	int Y = parse_int(year);
	int M = parse_int(month);
	int D = parse_int(day);
	int h = parse_int(hour);
	int m = parse_int(minute);
	int s = parse_int(second);

	//ok, now I compose a timestamp based on my basic perception of time/date
	//yeah, I know about leap years and leap seconds and all, but still we don't care there

	size_t days = D-1;
	for (int i = 1970; i < Y; ++i)
		if ((i % 4 == 0 && i % 100 != 0) || (i % 400 == 0))
			days += 366;
		else
			days += 365;

	int mdays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	for (int i = 1; i < M; ++i) {
		days += mdays[i-1];
		if (i == 2)
			if ((Y % 4 == 0 && Y % 100 != 0) || (Y % 400 == 0))
				days += 1;
	}

	size_t hours = days * 24 + h;
	size_t minutes = hours * 60 + m;
	return minutes * 60 + s;
}

vector<file_record> dropbox_storage::list_directory_files(string directory, bool recursive) {
	curl::request rq("https://api.dropboxapi.com/2/files/list_folder");
	rq.add_header("Authorization: Bearer " + token);
	rq.add_header("Content-Type: application/json");

	Json json = Json::object({
		{ "path", directory }, //"path": directory,
		{ "recursive", recursive },
		{ "include_media_info", false },
		{ "include_deleted", false },
	});

	rq.add_post_field(json.dump());

	string data = rq.execute();
	string error_message;
	Json answer = Json::parse(data, error_message);
	if (error_message.size()) throw base_exception("Json Error 1: " + error_message + string("\n\n") + data);
	if (answer["error_summary"].string_value().size())
		throw base_exception("Dropbox Error: " + answer["error_summary"].string_value());

	vector<file_record> result;
	bool has_more = true;
	while (has_more) {
		for (auto item : answer["entries"].array_items()) {
			result.push_back(
				file_record(
					item["path_lower"].string_value(),
					item["name"].string_value(),
					convert_to_timestamp(item["server_modified"].string_value())
				)
			);
		}
		has_more = answer["has_more"].bool_value();

		if (has_more) {
			curl::request rq("https://api.dropboxapi.com/2/files/list_folder/continue");
			rq.add_header("Authorization: Bearer " + token);
			rq.add_header("Content-Type: application/json");

			Json json = Json::object({
				{ "cursor", answer["cursor"].string_value() },
			});

			rq.add_post_field(json.dump());

			data = rq.execute();
			answer = Json::parse(data, error_message);
			if (error_message.size()) throw base_exception("Json Error 2: " + error_message + string("\n\n") + data);
			if (answer["error_summary"].string_value().size())
				throw base_exception("Dropbox Error: " + answer["error_summary"].string_value());
		}
	}

	return result;
}

bool dropbox_storage::download(string file) {
	Json json = Json::object({
		{ "path", file },
	});

	curl::request rq("https://content.dropboxapi.com/2/files/download");
	rq.add_header("Authorization: Bearer " + token);
	rq.add_header("Dropbox-API-Arg", json.dump());
	rq.add_header("Content-Type: "); //required to be empty (as we do POST, it's usually app/form-url-encoded)
	
	string data = rq.execute();

	string local_filename = "downloads" + file;
	ofstream fout(local_filename.c_str(), std::ifstream::binary);
	fout.write(data.c_str(), data.length());
	fout.close();

	return false;
}

void dropbox_storage::save(ofstream& fout) {
	fout << "dropbox\n";
	fout << token << "\n";
	fout << uid << "\n";
}

string dropbox_storage::name() const {
	//TODO: when storage is created, request user info and keep his username somewhere
	//saving this username into storage_manager's file would make it easier to find necessary service
	return "dropbox<" + uid + ">";
}

//private

dropbox_storage::dropbox_storage(string& gathered_token, string& gathered_uid) {
	token = gathered_token;
	uid = gathered_uid;
}

} } //namespace cloud::dropbox