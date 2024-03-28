#ifndef _SNAPSHOT_H_
#define _SNAPSHOT_H_
#include "process.h"
#include "message.h"
class Snapshot {
public:
    static int TakeSnapshot(Process *process);
};
#endif /* _SNAPHSOT_H_ */