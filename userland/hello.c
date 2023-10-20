#include <stdio.h>
#include <ipc.h>
#include <string.h>

int main(int argc, char **argv) {
    printf("Hello,world!\n");

    message_t msg;
    int id = 0;
    
    while(1) {
        msg.type = IPC_SEND;
        msg.src = 0;
        sprintf(msg.m1, "Hello,world! %d\n", id);
        ipc(3, &msg, IPC_SEND);
        id++;
        for(int i = 0; i < 100000000; i++);
    }
    return 0;
}
