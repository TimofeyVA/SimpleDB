#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>



#define BUFSZ 256

#define MSGCONNECT 0
#define MSGCLOSE -1
#define MSGDEFAULT 1

struct Msg
{
    int type;
    char body[BUFSZ];
};

struct serv_event
{
    void (*on_connect)(int);
    void (*on_close)(int);
    void (*new_msg)(int, char*);
};

struct cnt_arr
{
    int socket;
    pthread_t thread;
};

extern pthread_mutex_t connect_lock;
extern struct cnt_arr* connect_array;
extern int size_array;
extern int make_server(char*, int);
extern void server_loop(int, struct serv_event*);
extern void* server_new_connect(void*);
extern void server_close_connect(int);

extern void server_stop();


#endif