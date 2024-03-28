#ifndef _PROCESS_CONTROLLER_H_
#define _PROCESS_CONTROLLER_H_
#include <vector>
#include <sys/wait.h>
#include "channel.h"
#include "process.h"
#include "snapshot.h"

class ProcessController {
private:
    std::vector<Process*> processes;
public:
    ProcessController(int nprocs);
    int TakeSnapshot(void);
    void DispatchSnapshot(int result);
    void SendInitSnapshotMessage(void);
    void WaitChildren(void);
    void UnfreezeChildren(void);
    void SendQuitMessageToChildren(void);
    ~ProcessController();
};

#endif /* _PROCESS_CONTROLLER_H_ */