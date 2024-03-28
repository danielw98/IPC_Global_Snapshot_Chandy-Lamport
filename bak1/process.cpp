#include "process.h"

Process::Process(int nrpocs)//, std::vector<Channel*> incomingChannels, std::vector<Channel*> outgoingChannels)
{
    pid = -1;
    localState = 0;
    globalState = 0;
    processState = ProcessState::PROC_WHITE;
    running = true;
    // incomingChannels = incomingChannels;
    // outgoingChannels = outgoingChannels;
}

void Process::CheckForMessagesFromParent(void)
{
    fds[0].fd = parentToChild->p[STDIN_FILENO];
    fds[0].events = POLLIN;
}


void Process::MainLoop(void)
{
    bool localStateMessageSent = false;

    // setting the file descriptor for polling to the
    // read descriptor of the parent to child channel
    fds[0].fd = parentToChild->p[STDIN_FILENO];
    // setting the type of events to poll for equal to POLLIN = 'there is data to read'
    fds[0].events = POLLIN;

    while(running == true)
    {         
        // sleep for 20 msecseconds so the loop isn't being executed continuously
        usleep(20 * 1000);

        if(is_message_received() == true) // we have something to read
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
            UpdateLocalState();
        }

        if(processState == ProcessState::PROC_RED && localStateMessageSent == false) // check whether the initSnapshot message was sent and the localState wasn't
        {
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

void Process::UpdateLocalState(void)
{
    this->localState += rand() % 10;
}

Process::~Process()
{
    delete this->childToParent;
    delete this->parentToChild;
}

void Process::SetIncomingChannels(int *incomingChannels, int nprocs)
{
    for(int i = 0; i < nprocs; i++)
        this->incomingChannels.push_back(incomingChannels[i]);
}

void Process::SetOutgoingChannels(int *outgoingChannels, int nprocs)
{
    for(int i = 0; i < nprocs; i++)
        this->outgoingChannels.push_back(outgoingChannels[i]);
}