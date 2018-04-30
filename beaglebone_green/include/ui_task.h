#include "../main.h"

#define TOTAL_COMMANDS                            31

#define COMMAND_HELP                              0x00

#define COMMAND_GET_TEMPERATURE_VAL               0x01
#define COMMAND_GET_TEMPERATURE_MAX               0x02
#define COMMAND_GET_TEMPERATURE_MIN               0x03
#define COMMAND_GET_PRESSURE_VAL                  0x04
#define COMMAND_GET_PRESSURE_MAX                  0x05
#define COMMAND_GET_PRESSURE_MIN                  0x06
#define COMMAND_GET_HUMIDITY_VAL                  0x07
#define COMMAND_GET_HUMIDITY_MAX                  0x08
#define COMMAND_GET_HUMIDITY_MIN                  0x09
#define COMMAND_GET_COUNT_VAL                     0x0A
#define COMMAND_GET_COUNT_MAX                     0x0B
#define COMMAND_GET_COUNT_MIN                     0x0C
#define COMMAND_GET_RANGE_VAL                     0x0D

#define COMMAND_SET_TEMPERATURE_MAX               0x0E
#define COMMAND_SET_TEMPERATURE_MIN               0x0F
#define COMMAND_SET_PRESSURE_MAX                  0x10
#define COMMAND_SET_PRESSURE_MIN                  0x11
#define COMMAND_SET_HUMIDITY_MAX                  0x12
#define COMMAND_SET_HUMIDITY_MIN                  0x13
#define COMMAND_SET_COUNT_MAX                     0x14
#define COMMAND_SET_COUNT_MIN                     0x15
#define COMMAND_SET_RANGE                         0x16

#define COMMAND_START_SENSOR                      0x17
#define COMMAND_START_COUNTER                     0x18
#define COMMAND_STOP_SENSOR                       0x19
#define COMMAND_STOP_COUNTER                      0x1A
#define COMMAND_RESET_COUNTER                     0x1B

#define COMMAND_REQUEST_ID                        0x1C

#define COMMAND_EXIT                              0x1D
#define COMMAND_INVALID                           0x1E


#define MAX_SETTABLE_TEMP                         125
#define MIN_SETTABLE_TEMP                         0
#define MAX_SETTABLE_PRESS                         950
#define MIN_SETTABLE_PRESS                         750
#define MAX_SETTABLE_HUMID                         0
#define MIN_SETTABLE_HUMID                         75

void command_list_init(void);
unsigned char parse_command(unsigned char* input);

char command_list[TOTAL_COMMANDS][32];

void *ui_task_thread(void *);
void *ui_heartbeat_notifier_thread(void *);
