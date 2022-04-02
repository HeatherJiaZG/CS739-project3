namespace cpp block_store

service Client {
	binary read(1: i64 addr)
	i32 write(1: i64 addr, 2: binary content)
}

service PrimaryBackup {
	i32 sync(1: i64 addr, 2: binary content) // return -1 means backup is out-of-date
	map<binary, i64> get_timestamps(1: list<binary> primary_files) // timestamp is 0 if file not found on backup
	void sync_files(1: map<binary, binary> primary_files)
}

