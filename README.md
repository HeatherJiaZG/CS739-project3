# CS739 Project 3

Cloud lab machines:
```bash
ssh -p 22 tson1111@c220g5-110930.wisc.cloudlab.us # node0: 128.105.144.168
ssh -p 22 tson1111@c220g5-110904.wisc.cloudlab.us # node1: 128.105.144.142
ssh -p 22 tson1111@c220g5-110903.wisc.cloudlab.us # node2: 128.105.144.141
```

To build it on cloud lab:
```bash
cd ~/cs739-p3/build
cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ ..
cmake --build . --target server_main client_main -- -j 16
```

## Procotol Design
(P = Primary, B = Backup, C = Client)

### Workflow
#### Read
1. C issues a read request to P
2. handler function on P requires a reader lock
3. P sends block to C
4. P releases reader lock

#### Write
1. C issues a write request to P
2. handler function on P requires a writer lock
3. P sends sync RPC request to B
4. P update local contents by 4KB
5. P waits for ack from B
6. Upon getting ack from B, P sends ack to C

### Fault-Tolerance

#### Read
| When?       | P Crashed   |
| :----:      |    :----:   |
| Any time    | Client reissue request to B|

#### Write
| When?       | P Crashed   | B Crashed   |
| :----:      |    :----:   |    :----:   |
| TBA         | TBA         | TBA         |

### Concurrency Control (multiple clients)
read/write lock

## TODO
1. Support block storage on servers - a global unique space (i.e., a logical block device) (how?)
* Option1: Can we read/write to the same file and treat it as a central storage place?
2. Create template for clients (read+write, should be easy)
3. Central or replicated lock server? Note that the lock server can crash.
