namespace cpp block_store

service Client {
	binary read(1: i64 addr)
	i32 write(1: i64 addr, 2: binary content)
}

service PrimaryBackup {
	i32 heartbeat(1: i32 msg)
	i32 sync(1: i64 addr, 2: binary content) // return -1 means backup file is out-of-date
	i32 sync_entire(1: binary content)
}

