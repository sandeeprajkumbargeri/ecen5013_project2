#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define COMMAND_TEMP_READ_TLOW                0x01
#define COMMAND_TEMP_READ_THIGH               0x02
#define COMMAND_TEMP_READ_DATA_REG            0x03
#define COMMAND_TEMP_SET_SD_ON                0x04
#define COMMAND_TEMP_SET_SD_OFF               0x05
#define COMMAND_TEMP_READ_RESOLUTION          0x06
#define COMMAND_TEMP_READ_FAULT_BITS          0x07
#define COMMAND_TEMP_READ_EM                  0x08
#define COMMAND_TEMP_SET_EM_ON                0x09
#define COMMAND_TEMP_SET_EM_OFF               0x0A

#define COMMAND_TEMP_SET_CONV_RATE_0          0x0B
#define COMMAND_TEMP_SET_CONV_RATE_1          0x0C
#define COMMAND_TEMP_SET_CONV_RATE_2          0x0D
#define COMMAND_TEMP_SET_CONV_RATE_3          0x0E
#define COMMAND_TEMP_READ_CONV_RATE           0x0F

#define COMMAND_LIGHT_SET_INTG_TIME_0         0x10
#define COMMAND_LIGHT_SET_INTG_TIME_1         0x11
#define COMMAND_LIGHT_SET_INTG_TIME_2         0x12
#define COMMAND_LIGHT_READ_INTG_TIME          0x13
#define COMMAND_LIGHT_READ_GAIN               0x14
#define COMMAND_LIGHT_SET_GAIN_HIGH           0x16
#define COMMAND_LIGHT_SET_GAIN_LOW            0x15
#define COMMAND_LIGHT_SET_INT_ENABLE          0x18
#define COMMAND_LIGHT_SET_INT_DISABLE         0x17
#define COMMAND_LIGHT_READ_IDENTIFY_REG       0x19
#define COMMAND_LIGHT_READ_INT_TRSHLD_LOW     0x1A
#define COMMAND_LIGHT_READ_INT_TRSHLD_HIGH    0x1B
#define COMMAND_LIGHT_SET_INT_TRSHLD_LOW      0x1C
#define COMMAND_LIGHT_SET_INT_TRSHLD_HIGH     0x1D

void *sock_comm_task_thread(void *);
void *sock_comm_heartbeat_notifier(void *);
