#ifndef __LOGGER_TASK_H
#define __LOGGER_TASK_H

#include <time.h>
#include "../main.h"

#define LOG(mq_logger, log_message)({\
        	if(mq_send(mq_logger, (const char *) log_message, sizeof(log_message), 0) < 0)\
                	perror("Error Sending Request to Temp Task");\
		})

void *logger_task_thread(void *);
void *logger_heartbeat_notifier(void *);
void update_time(char *, size_t);


#endif
