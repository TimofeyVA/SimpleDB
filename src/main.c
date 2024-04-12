#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#include "server.h"


void handle_exit();
int check_address(const char*);
void programm_start();


void new_connect(int fd)
{

}

void new_msg(int fd, char* msg)
{

}


void close_connect(int fd)
{

}




int main(int ac, char** av)
{
    signal(SIGINT, handle_exit);

    char *addr;
    int port;

    if (ac != 3) {
        fprintf(stderr, "Args error: [ip] [port]\n");
        return -1;
    }
    else {
        if ((port = atoi(av[2])) == 0) {
            fprintf(stderr, "Args error: the port cannot be converted to int\n");
            return -1;
        }
        
        addr = av[1];

        if (check_address(addr) == -1) {
            fprintf(stderr, "Args error: invalid address\n");
            return -1;
        }
    }

    programm_start();

    int sock_serv = make_server(addr, port);

    if (sock_serv == -1) {
        perror("Server start error: ");
        return -1;
    }

    struct serv_event e;
    e.on_connect = new_connect;
    e.new_msg = new_msg;
    e.on_close = close_connect;

    server_loop(sock_serv, &e);

    return 0;
}


void handle_exit()
{
    printf("\nStopped\n");
    server_stop();
    _exit(0);
}

int check_address(const char* adr)
{
    size_t len = strlen(adr);
    if (len > 15 || len < 7) return -1;

    int points = 0;
    int nums = 0;

    int current_num = 0;

    char mask[8];
    mask[7] = '\0';
    for (size_t i = 0; i < 7; i++)
    {
        *(mask + i) = 'z';
    }
    
    int inx = 0;

    for (size_t i = 0; i < len; i++) {
        if (adr[i] != '.' && !('0' <= adr[i] && adr[i] <= '9')) {
            return -1;
        }

        if ('0' <= adr[i] && adr[i] <= '9') {
            current_num*=10;
            current_num += (int)(adr[i] - '0');
            mask[inx] = 'x';
        }
        if (adr[i] == '.') {
            if (current_num > 255) return -1;
            current_num = 0;
            nums++;
            points++;
            mask[++inx] = '.';
            inx++;
        }

    }

    if (points > 3 || nums > 4) return -1;
    if (strcmp(mask, "x.x.x.x") == 0) {
        if (current_num > 255) return -1;
        else return 0;
    }
    else {
        return -1;
    }
}


void programm_start()
{
    DIR* dir = opendir("./DATABASE");

    printf("Create dir database\n");

    if (dir) {
        printf("Dir is exist\n");
        closedir(dir);
    }
    else if (ENOENT == errno){
        if (mkdir("./DATABASE", 0777) == -1){
            perror("Cannot create dir: ");
            exit(-1);
        }
        printf("Dir is created\n");
    }
}