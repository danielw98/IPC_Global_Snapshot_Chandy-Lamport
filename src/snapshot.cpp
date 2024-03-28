#include "../includes/snapshot.h"

int Snapshot::TakeSnapshot(Process *process)
{
    int localValue;
    Message message;

    message.RecvMessage(process->GetChildToParentChannel());
    localValue = message.GetData();

    // return the state given by the child process
    return localValue;
}