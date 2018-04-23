#include "../main.h"

#define TOTAL_COMMANDS                        0x1F

#define COMMAND_HELP                          0x00

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
#define COMMAND_LIGHT_SET_GAIN_HIGH           0x15
#define COMMAND_LIGHT_SET_GAIN_LOW            0x16
#define COMMAND_LIGHT_SET_INT_ENABLE          0x17
#define COMMAND_LIGHT_SET_INT_DISABLE         0x18
#define COMMAND_LIGHT_READ_IDENTIFY_REG       0x19
#define COMMAND_LIGHT_READ_INT_TRSHLD_LOW     0x1A
#define COMMAND_LIGHT_READ_INT_TRSHLD_HIGH    0x1B
#define COMMAND_LIGHT_SET_INT_TRSHLD_LOW      0x1C
#define COMMAND_LIGHT_SET_INT_TRSHLD_HIGH     0x1D

#define COMMAND_EXIT                          0x1E
#define COMMAND_INVALID                       0x1F

void command_list_init(void);
unsigned char parse_command(unsigned char* input);

char command_list[TOTAL_COMMANDS][32];

void *ui_task_thread(void *);
void *ui_heartbeat_notifier(void *);
