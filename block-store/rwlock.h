#include <shared_mutex>

typedef std::shared_mutex Lock;
typedef std::unique_lock< Lock >  WriteLock;
typedef std::shared_lock< Lock >  ReadLock;

namespace block_store {

Lock rwLock;

/*
 void ReadFunction()
{
     ReadLock r_lock(myLock);
     //Do reader stuff
}

void WriteFunction()
{
     WriteLock w_lock(myLock);
     //Do writer stuff
}
*/

}
