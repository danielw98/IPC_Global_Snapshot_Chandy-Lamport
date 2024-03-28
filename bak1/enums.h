#ifndef _ENUMS_H_
#define _ENUMS_H_

typedef enum MessageTypes {
    MSG_INIT_SNAPSHOT,
    MSG_SEND_LOCAL_STATE,
    MSG_RETURN_GLOBAL_STATE,
    MSG_CONTINUE_AFTER_SNAPSHOT,
    MSG_QUIT,
    MSG_NONE
} MessageTypes;


typedef enum ProcessState {
    // normal operating mode of the process
    PROC_WHITE,
    // process is involved in a snapshot
    PROC_RED
} ProcessState;

#endif /* _ENUMS_H_*/