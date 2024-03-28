#include "../includes/channel.h"

Channel::Channel(int *p)
{
    this->p[STDIN_FILENO] = p[STDIN_FILENO];
    this->p[STDOUT_FILENO] = p[STDOUT_FILENO];
}

Channel::~Channel()
{
    close(p[0]);
    close(p[1]);
}