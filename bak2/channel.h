#ifndef _CHANNEL_H_
#define _CHANNEL_H_
#include <sys/types.h>
#include <unistd.h>
// a channel is a one way communication between sender and receiver
// sender writes to the write end of the channel, and receiver reads
// from the read end of the channel
struct Channel {
public:
    int p[2];
public:
    Channel() {}
    Channel(int *p);
    ~Channel();
};
typedef Channel* ChannelPtr;
#endif /* _CHANNEL_H_ */