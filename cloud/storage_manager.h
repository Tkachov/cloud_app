#pragma once

#include <vector>
using std::vector;

namespace cloud {

class storage;

class storage_manager
{
	vector<storage*> storages;
	int current_storage;

	void save_settings();

public:
	storage_manager();
	~storage_manager();

	void load_settings();
	bool add_storage(storage* s, bool set_as_current = true);

	const vector<storage*>& get_storages() const;
	storage* get_current_storage() const;
};

} //namespace cloud