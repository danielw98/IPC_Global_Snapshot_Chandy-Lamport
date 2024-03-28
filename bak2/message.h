#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#include "enums.h"
#include "channel.h"

class Message {
private:
    MessageTypes messageType;
    int data;
public:
    Message();
    Message(MessageTypes messageType, int data = 0);
    void SendMessage(Channel *channel);
    void RecvMessage(Channel *channel);
    MessageTypes GetMessageType(void);
    int GetData(void);
    static void SendMessageToProcess(int fd, int data);
    static int ReadMessageFromProcess(int fd);
};

#endif /* _MESSAGE_H_ */