# CS739 Project 3

Cloud lab machines:
```bash
ssh -p 22 tson1111@c220g5-111017.wisc.cloudlab.us # node0
ssh -p 22 tson1111@c220g5-111007.wisc.cloudlab.us # node1
ssh -p 22 tson1111@c220g5-111005.wisc.cloudlab.us # node2
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
4. P update local contents
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
TBA

## TODO
1. Support block storage on servers (how?)
2. Create template for clients (read+write, should be easy)
3. TBA 
