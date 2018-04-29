#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define SOCK_COMM_LISTEN_PORT                 8888

struct sockaddr_un sockaddr_ui, sockaddr_comm;
struct sockaddr_in sockaddr_comm_ip, sockaddr_comm_client;
socklen_t sockaddr_length_ui, sockaddr_length_ip;

void *sock_comm_task_thread(void *);
void *heartbeat_sock_comm_notifier_thread(void *);
void *sock_comm_ui_handler_task_thread(void *);
