#include "../includes/message.h"

Message::Message()
{
    messageType = MessageTypes::MSG_NONE;
    data = 0;
}

Message::Message(MessageTypes messageType, int data)
{
    this->messageType = messageType;
    this->data = data;
}

void Message::SendMessage(Channel *channel)
{
    write(channel->p[STDOUT_FILENO], &messageType, sizeof(this->messageType));
    if(this->messageType == MSG_SEND_LOCAL_STATE || this->messageType == MSG_RETURN_GLOBAL_STATE)
    {
        write(channel->p[STDOUT_FILENO], &data, sizeof(data));
    }
}

void Message::RecvMessage(Channel *channel)
{
    read(channel->p[STDIN_FILENO], &messageType, sizeof(MessageTypes));
    if(this->messageType == MSG_SEND_LOCAL_STATE || this->messageType == MSG_RETURN_GLOBAL_STATE)
    {
        read(channel->p[STDIN_FILENO], &data, sizeof(data));
    }
}

MessageTypes Message::GetMessageType(void)
{
    return messageType;
}

int Message::GetData(void)
{
    return data;
}

void Message::SendMessageToProcess(int fd, int data)
{
    write(fd, &data, sizeof(int));
}

int Message::ReadMessageFromProcess(int fd)
{
    int data;
    read(fd, &data, sizeof(int));
    return data;
}