#include "processcontroller.h"

ProcessController::ProcessController(int nprocs)
{
    int **incomingChannelsArray = new int*[nprocs];
    int **outgoingChannelsArray = new int*[nprocs];

    for(int i = 0; i < nprocs; i++)
    {
        // incoming channels for process i - i reads
        incomingChannelsArray[i] = new int[nprocs];
        // outgoing channels for process i
        outgoingChannelsArray[i] = new int[nprocs];
    }

    for(int i = 0; i < nprocs; i++)
    {
        for(int j = 0; j < nprocs; j++)
        {
            // pipe where process i writes and process j reads
            int process_i_to_j[2];
            // pipe where process j writes and process i reads
            int process_j_to_i[2];
            if(i == j)
            {
                // process i doesn't communicate with itself
                incomingChannelsArray[i][j] = -1;
                outgoingChannelsArray[i][j] = -1;
                continue;
            }
            if(pipe(process_i_to_j) == -1)
            {
                perror("pipe()");
                exit(EXIT_FAILURE);
            }
            if(pipe(process_j_to_i) == -1)
            {
                perror("pipe()");
                exit(EXIT_FAILURE);
            }
            // incoming channel from process j to process i (process j writes, process i reads)
            incomingChannelsArray[i][j] = process_j_to_i[STDIN_FILENO];
            // outgoing channel from process i to process j (process j reads, process i writes)
            outgoingChannelsArray[i][j] = process_i_to_j[STDOUT_FILENO];
            // incoming channel from process i to process j (process i writes, process j reads)
            incomingChannelsArray[j][i] = process_i_to_j[STDIN_FILENO];
            // outgoing channel from process j to process i (process i reads, process j writes)
            outgoingChannelsArray[j][i] = process_j_to_i[STDOUT_FILENO];
        }
    }
    
    for(int i = 0; i < nprocs; i++)
    {
        Process *process = new Process(nprocs);
        pid_t pid;

        int pParentChild[2];
        if(pipe(pParentChild) == -1)
        {
            perror("pipe()");
            exit(EXIT_FAILURE);
        }

        int pChildParent[2];
        if(pipe(pChildParent) == -1)
        {
            perror("pipe()");
            exit(EXIT_FAILURE);
        }
        Channel *parentToChildChannel = new Channel(pParentChild);
        Channel *childToParentChannel = new Channel(pChildParent);
        process->AddParentToChildChannel(parentToChildChannel);
        process->AddChildToParentChannel(childToParentChannel);
        process->SetIncomingChannels(incomingChannelsArray[i], nprocs);
        process->SetOutgoingChannels(outgoingChannelsArray[i], nprocs);

        if((pid = fork()) > 0)
        {
            // parent process
            pid_t childPid = pid;

            process->SetPid(childPid);
            
            processes.push_back(process);

            printf("Forked process %d\n", childPid);
            continue;
        }

        if(pid == 0)
        {
            // child process
            pid_t childPid = getpid();
            srand(childPid);
            process->SetPid(childPid);
            process->MainLoop();

            // the mainLoop ended, so we retrieve the localState and globalState for the current process
            printf("Process %d finished with localState %d and globalState %d\n", childPid, process->GetLocalState(), process->GetGlobalState());

            // we call the destructor for the child process after the mainLoop ends
            delete process;

            exit(EXIT_SUCCESS);
        }

        if(pid < 0)
        {
            // fork error
            perror("fork()");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < processes.size(); i++)
    {
        delete[] incomingChannelsArray[i];
        delete[] outgoingChannelsArray[i];
    }
    delete[] incomingChannelsArray;
    delete[] outgoingChannelsArray;
}

int ProcessController::TakeSnapshot(void)
{
    // parent process takes a snapshot adding up all children processes' local state
    printf("Snapshot initiated\n");
    int result = 0;
    for(int i = 0; i < processes.size(); i++)
    {
        result += Snapshot::TakeSnapshot(processes[i]);
    }
    DispatchSnapshot(result);
    return result;
}

void ProcessController::DispatchSnapshot(int result)
{
    Message message = Message(MessageTypes::MSG_RETURN_GLOBAL_STATE, result);
    for(int i = 0; i < processes.size(); i++)
    {
        message.SendMessage(processes[i]->GetParentToChildChannel());
    }
}

void ProcessController::SendInitSnapshotMessage(void)
{
    // parent process sends the init snapshot message to all its children processes
    for(int i = 0; i < processes.size(); i++)
    {
        Message message = Message(MessageTypes::MSG_INIT_SNAPSHOT);
        message.SendMessage(processes[i]->GetParentToChildChannel());
    }
}

void ProcessController::WaitChildren(void)
{
    // wait for its children to finish their execution and thus avoid zombie processes
    for(int i = 0; i < processes.size(); i++)
    {
        wait(NULL);
    }
}

void ProcessController::UnfreezeChildren(void)
{
    // the children continue after sending the snapshot if they received a continue message
    for(int i = 0; i < processes.size(); i++)
    {
        Message messageSent = Message(MessageTypes::MSG_CONTINUE_AFTER_SNAPSHOT);
        messageSent.SendMessage(processes[i]->GetParentToChildChannel());
    }
}

void ProcessController::SendQuitMessageToChildren(void)
{
    // the children will quit after receiving this message
    for(int i = 0; i < processes.size(); i++)
    {
        Message messageSent = Message(MessageTypes::MSG_QUIT);
        messageSent.SendMessage(processes[i]->GetParentToChildChannel());
    }
}

ProcessController::~ProcessController()
{
    for(int i = 0; i < processes.size(); i++)
    {
        delete processes[i];
        processes[i] = nullptr;
    }
}