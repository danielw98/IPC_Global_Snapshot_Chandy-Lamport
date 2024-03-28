#ifndef _PROCESS_H_
#define _PROCESS_H_
#include "enums.h"
#include "channel.h"
#include "message.h"
#include <poll.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

class Process {
private:
    Channel *parentToChild;
    Channel *childToParent;
    ProcessState processState;
    pid_t pid;
    int localState;
    int globalState;
    bool running;
    std::vector<int> incomingChannels;
    std::vector<int> outgoingChannels;
    struct pollfd fds[1];
public:
    Process(int nrpocs);
    void CheckForMessagesFromParent(void);
    void MainLoop(void);
    void SetPid(pid_t pid);
    pid_t GetPid(void);
    int GetLocalState(void);
    int GetGlobalState(void);
    Channel* GetChildToParentChannel(void);
    void AddParentToChildChannel(Channel *channel);
    Channel* GetParentToChildChannel(void);
    void AddChildToParentChannel(Channel *channel);
    void UpdateLocalState(void);
    void SetIncomingChannels(int *incomingChannels, int nprocs);
    void SetOutgoingChannels(int *outgoingChannels, int nprocs);
    ~Process();
private:
    bool is_message_received(void)
    {
        // check if a message was received from the parent 
        int ready = poll(fds, 1, 0);
        if (ready == -1) 
        {
            perror("poll()");
            exit(EXIT_FAILURE);
        }
        return fds[0].revents & POLLIN;
    }
};

#endif /* _PROCESS_H */