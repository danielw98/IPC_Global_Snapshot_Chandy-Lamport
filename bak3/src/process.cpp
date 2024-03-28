#include "../includes/process.h"

Process::Process(int nprocs, int procIdx)//, std::vector<Channel*> incomingChannels, std::vector<Channel*> outgoingChannels)
{
    pid = -1;
    localState = 0;
    globalState = 0;
    processState = ProcessState::PROC_WHITE;
    this->nprocs = nprocs;
    running = true;
    this->procIdx = procIdx;
}



void Process::MainLoop(void)
{
    bool localStateMessageSent = false;

    srand(getpid());
    while(running == true)
    {         
        // sleep for 60 msecseconds so the loop isn't being executed continuously
        usleep(60 * 1000);

        // we have something to read sent by the parent
        if(IsMessageReceived(parentToChild->p[STDIN_FILENO]) == true) 
        {
            // we receive and process the message sent by the parent
            Message message;
            message.RecvMessage(parentToChild);
            MessageTypes messageType = message.GetMessageType();

            if(messageType == MessageTypes::MSG_INIT_SNAPSHOT)
            {
                printf("Process %d initiated snapshot\n", getpid());
                processState = ProcessState::PROC_RED;
            }
            if(messageType == MessageTypes::MSG_QUIT)
            {
                printf("Process %d is quitting...\n", getpid());
                running = false;
                break;
            }
        }

        // check if process is in normal operating mode
        if(processState == ProcessState::PROC_WHITE)
        {
            // update local state and send and receive messages to and from other processes
            HandleComputation();
        }

        if(processState == ProcessState::PROC_RED && localStateMessageSent == false) // check whether the initSnapshot message was sent and the localState wasn't
        {
            // we have a RED process, which means we have to take our snapshot and send the correct localState
            // so we receive all incoming messages from all processes before doing that
            // every time a message is sent by other process, is queued up and ready to be received
            // check all children if they sent any messages and add them to the local state
            ReceiveMessagesFromProcesses();

            // the process sends its local state to the parent
            Message message = Message(MessageTypes::MSG_SEND_LOCAL_STATE, localState);
            message.SendMessage(childToParent);
            
            // the snaphot was initiated and the local state was sent to the parent
            localStateMessageSent = true;

            // collect the global state from the parent
            Message globalStateMessage;
            globalStateMessage.RecvMessage(parentToChild);
            
            if(globalStateMessage.GetMessageType() == MessageTypes::MSG_RETURN_GLOBAL_STATE)
            {
                globalState = globalStateMessage.GetData();
                printf("Process %d has localState %d and received globalState %d\n", getpid(), localState, globalState);
            }

            // receive the continue message from the parent
            Message continueMessage;
            continueMessage.RecvMessage(parentToChild);

            if(continueMessage.GetMessageType() == MessageTypes::MSG_CONTINUE_AFTER_SNAPSHOT)
            {
                // return to the initial state, the one before the snapshot was taken
                localStateMessageSent = false;
                processState = ProcessState::PROC_WHITE;
            }

            // check if message is of type messageQuit
            if(continueMessage.GetMessageType() == MessageTypes::MSG_QUIT)
            {
                printf("Process %d is quitting...\n", getpid());
                running = false;
                break;
            }
        }
        
    }
}

void Process::SetPid(pid_t pid)
{
    this->pid = pid;
}

pid_t Process::GetPid(void)
{
    return pid;
}

int Process::GetLocalState(void)
{
    return localState;
}

int Process::GetGlobalState(void)
{
    return globalState;
}

Channel* Process::GetChildToParentChannel(void)
{
    return childToParent;
}

void Process::AddParentToChildChannel(Channel *channel)
{
    parentToChild = channel;
}

Channel* Process::GetParentToChildChannel(void)
{
    return parentToChild;
}

void Process::AddChildToParentChannel(Channel *channel)
{
    childToParent = channel;
}

static bool ShouldSendComputation(void)
{
    // this can be any condition
    return rand() % 20 == 5;
}

static int ComputeMessage(void)
{
    // this can be any computation
    return 1 + rand() % 9;
}

std::set<int> Process::GetProcessesList(void)
{
    // send a different computation to different processes available
    // send a computation result to between 1 and nprocs-1 other processes
    int nprocs_to_send_computation = 1 + rand() % (nprocs-1);
    std::set<int> procIdxArr;
    while(procIdxArr.size() != nprocs_to_send_computation)
    {
        int proc = rand() % nprocs;
        // can't send messages to itself
        if(proc == procIdx || procIdxArr.count(proc) > 0)
        {
            continue;
        }
        procIdxArr.insert(proc);
    }
    return procIdxArr;
}

void Process::SendMessagesToProcesses(std::set<int>& procs)
{
    // send messages to processes on outgoing channels
    for(auto it = procs.begin(); it != procs.end(); it++)
    {
        // get the process idx that we're sending to
        int proc = *it;
        // compute the message
        int msg = ComputeMessage();
        // subtract the message send from localState
        localState -= msg;
        // send the message
        printf("Process %d is sending message %d to process %d\n", procIdx, msg, proc);
        Message::SendMessageToProcess(outgoingChannels[proc], msg);
    }
}

void Process::ReceiveMessagesFromProcesses(void)
{
    // check for received messages on incoming channels from other processes
    // and if there are any, add them to the localState
    for(int i = 0; i < incomingChannels.size(); i++)
    {
        if(i != procIdx)
        {
            // there can be multiple messages received between checks, so we empty the channel
            while(IsMessageReceived(incomingChannels[i]) == true)
            {
                int messageReceived = Message::ReadMessageFromProcess(incomingChannels[i]);
                printf("Process %d received message %d from process %d\n", procIdx, messageReceived, i);
                localState += messageReceived;
            }
        }
    }
}

void Process::UpdateLocalState(void)
{
    // this can be any computation
    this->localState += rand() % 10;
}

void Process::HandleComputation(void)
{
    // update local state for the current process based on the computation it is doing
    UpdateLocalState();
    // every now and then, send messages to other processes
    if(ShouldSendComputation() == true)
    {
        // create a set of processes that will receive a message from the current process
        std::set<int> procIdxArr = GetProcessesList();
        SendMessagesToProcesses(procIdxArr);
    }

    // every time a message is sent by other process, is queued up and ready to be received
    // check all children if they sent any messages and add them to the local state
    ReceiveMessagesFromProcesses();
}

Process::~Process()
{
    delete this->childToParent;
    delete this->parentToChild;
    for(int i = 0; i < incomingChannels.size(); i++)
        if(outgoingChannels[i] != -1)
            close(incomingChannels[i]);
    for(int i = 0; i < outgoingChannels.size(); i++)
        if(outgoingChannels[i] != -1)
            close(outgoingChannels[i]);
}

void Process::SetIncomingChannels(int *incomingChannels, int nprocs)
{
    for(int i = 0; i < nprocs; i++)
    {
        this->incomingChannels.push_back(incomingChannels[i]);
    }
}

void Process::SetOutgoingChannels(int *outgoingChannels, int nprocs)
{
    for(int i = 0; i < nprocs; i++)
    {
        this->outgoingChannels.push_back(outgoingChannels[i]);
    }
}

bool Process::IsMessageReceived(int fd)
{
    struct pollfd pollDescriptor;
    // setting the file descriptor for polling to the
    // read descriptor to check for data
    pollDescriptor.fd = fd;
    // setting the type of events to poll for equal to POLLIN = 'there is data to read'
    pollDescriptor.events = POLLIN;
    int ready = poll(&pollDescriptor, 1, 0);
    if (ready == -1) 
    {
        perror("poll()");
        exit(EXIT_FAILURE);
    }
    // check if a message was received in the given descriptor
    return pollDescriptor.revents & POLLIN;
}