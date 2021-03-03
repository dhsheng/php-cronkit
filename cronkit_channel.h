#ifndef CRONKIT_CHANNEL_H
#define CRONKIT_CHANNEL_H

// fd[0] => Read
// fd[1] => Write
int cronkit_channel_init(int ch[2]);

#endif