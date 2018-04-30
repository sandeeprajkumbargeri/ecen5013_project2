#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdbool.h>
#include <semaphore.h>
#include <time.h>
#include <sys/syscall.h>
#include <signal.h>
#include <pthread.h>
#include <stdint.h>
#include <err.h>
#include <errno.h>

#define TOTAL_TASKS							3

#define MQ_LOGGER_PATH            "/mq_logger"
#define MQ_HEARTBEAT_PATH         "/mq_heartbeat"
#define SK_SOCK_COMM_PATH    			"/tmp/sk_sock_comm"
#define SK_UI_PATH    						"/tmp/sk_ui"

#define LOGGER_TASK_ID	        0
#define SOCK_COMM_TASK_ID       1
#define UI_TASK_ID       				2

#define LOGGER_TASK_NAME				"Logger Task"
#define SOCK_COMM_TASK_NAME			"Socket Communication Task"
#define UI_TASK_NAME						"User Interface Task"

#define CLOCK_ID                CLOCK_MONOTONIC
#define TIMER_EXPIRY_SEC					2
#define TIMER_EXPIRY_MSEC         0

extern pthread_t logger_task;
extern pthread_t sock_comm_task;
extern pthread_t ui_task;

extern int sock_ui, sock_comm, sock_comm_ip;

extern bool task_alive[TOTAL_TASKS], task_heartbeat[TOTAL_TASKS], send_heartbeat[TOTAL_TASKS];
extern char task_name[TOTAL_TASKS][30];

extern struct sigevent heartbeat_sevp;
extern char filename[64];

extern sem_t sem_logger, sem_sock_comm, sem_ui;
extern mqd_t mq_logger, mq_heartbeat;

void timer_expiry_handler(union sigval);
void heartbeat_msg_handler(union sigval);
void setup_mq(void);

typedef struct pthread_args
{
	int32_t data;
} pthread_args_t;

//Structure of the data which is communicated between the parent and the child using pipes.
typedef struct mq_payload_heartbeat
{
	char sender_id;
	bool heartbeat_status;
} mq_payload_heartbeat_t;

typedef struct sk_payload_ip_request
{
	uint8_t device_id;
	uint8_t route_id;
	uint8_t message[98];
} sk_payload_ip_message_t;

typedef struct sk_payload_ui_request
{
	uint32_t data;
	uint8_t device_id;
	uint8_t command;
} sk_payload_ui_request_t;

#define RECEIVE_PACKET_SIZE     sizeof(sk_payload_ip_message_t)
#define SEND_PACKET_SIZE        sizeof(sk_payload_ui_request_t)

FILE *log_file;

#endif
