#include "server.h"

struct cnt_arr* connect_array;
pthread_mutex_t connect_lock = PTHREAD_MUTEX_INITIALIZER;
int size_array = 0;

struct args_srv
{
    int fd;
    struct serv_event* e;
};


int make_server(char* host, int port)
{
    struct sockaddr_in saddr;

    int socket_id = socket(PF_INET, SOCK_STREAM, 0);

    saddr.sin_addr.s_addr = inet_addr(host);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);

    if (bind(socket_id, (struct sockaddr*)&saddr, sizeof(saddr)) != 0) {
        return -1;
    }

    if (listen(socket_id, 1) != 0) {
        return -1;
    }

    return socket_id;
}

void server_loop(int socket, struct serv_event* e)
{
    int fd;
    while (1) {
        fd = accept(socket, NULL, NULL);

        pthread_mutex_lock(&connect_lock);
        connect_array = (struct cnt_arr*)realloc((void*)connect_array, ++size_array * sizeof(struct cnt_arr));
        connect_array[size_array - 1].socket = fd;

        struct args_srv args = {.fd = fd, .e = e};
        pthread_create(&connect_array[size_array - 1].thread, NULL, server_new_connect, (void*)&args);
        pthread_detach(connect_array[size_array - 1].thread);
        pthread_mutex_unlock(&connect_lock);
    }
}

void* server_new_connect(void * arg)
{
    struct args_srv* args = (struct args_srv*)arg;
    int socket = (*args).fd;

    struct Msg msg;

    while (read(socket, (void*)&msg, sizeof(struct Msg)) != -1) {
        switch (msg.type)
        {
        case MSGCONNECT:
            (*args).e->on_connect(socket);
            break;
        case MSGDEFAULT:
            (*args).e->new_msg(socket, msg.body);
            break;
        case MSGCLOSE:
            goto exit_cycle;
            break;
        }
    }

exit_cycle:
    (*args).e->on_close(socket);
    server_close_connect(socket);
    return NULL;
}

void server_close_connect(int fd)
{
    close(fd);
    pthread_mutex_lock(&connect_lock);

    int inx = -1;
    for (size_t i = 0; i < size_array; i++) {
        if (connect_array[i].socket == fd) {
            inx = i;
            break;
        }
    }

    struct cnt_arr *buf = (connect_array + inx);
    connect_array[inx] = connect_array[size_array - 1];
    connect_array[size_array - 1] = *buf;
    connect_array = (struct cnt_arr*)realloc((void*)connect_array, --size_array * sizeof(struct cnt_arr));

    pthread_mutex_unlock(&connect_lock);
}


void server_stop()
{
    pthread_mutex_lock(&connect_lock);
    for (size_t i = 0; i < size_array; i++) {
        close(connect_array[size_array - 1].socket);
        pthread_cancel(connect_array[size_array - 1].thread);
    }
    free(connect_array);
    size_array = 0;
    pthread_mutex_unlock(&connect_lock);
}
