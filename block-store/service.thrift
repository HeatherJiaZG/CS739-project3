namespace cpp block_store

service Client {
	binary read(1: i64 addr)
	i32 write(1: i64 addr, 2: binary content)
}

service PrimaryBackup {
	i32 heartbeat(1: i32 msg)
	i32 sync(1: list<i64> addr, 2: list<binary> content)
}

