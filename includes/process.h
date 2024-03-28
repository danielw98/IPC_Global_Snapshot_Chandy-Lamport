#ifndef _PROCESS_H_
#define _PROCESS_H_
#include "enums.h"
#include "channel.h"
#include "message.h"
#include <poll.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <set>

class Process {
private:
    Channel *parentToChild;
    Channel *childToParent;
    ProcessState processState;
    pid_t pid;
    int procIdx;
    int nprocs;
    int localState;
    int globalState;
    bool running;
    std::vector<int> incomingChannels;
    std::vector<int> outgoingChannels;

public:
    Process(int nrpocs, int procIdx);
    void MainLoop(void);
    void SetPid(pid_t pid);
    pid_t GetPid(void);
    int GetLocalState(void);
    int GetGlobalState(void);
    Channel* GetChildToParentChannel(void);
    void AddParentToChildChannel(Channel *channel);
    Channel* GetParentToChildChannel(void);
    void AddChildToParentChannel(Channel *channel);
    void HandleComputation(void);
    void SetIncomingChannels(int *incomingChannels, int nprocs);
    void SetOutgoingChannels(int *outgoingChannels, int nprocs);
    ~Process();

private:
    bool IsMessageReceived(int fd);
    std::set<int> GetProcessesList(void);
    void SendMessagesToProcesses(std::set<int>& procs);
    void ReceiveMessagesFromProcesses(void);
    void UpdateLocalState(void);
};

#endif /* _PROCESS_H */