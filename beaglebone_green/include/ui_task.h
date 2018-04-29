#include "../main.h"

#define TOTAL_COMMANDS                            0x1F

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

#define COMMAND_SET_TEMPERATURE_MAX               0x12
#define COMMAND_SET_TEMPERATURE_MIN               0x13
#define COMMAND_SET_PRESSURE_MAX                  0x15
#define COMMAND_SET_PRESSURE_MIN                  0x16
#define COMMAND_SET_HUMIDITY_VAL                  0x17
#define COMMAND_SET_HUMIDITY_MAX                  0x18
#define COMMAND_SET_HUMIDITY_MIN                  0x19
#define COMMAND_SET_COUNT_VAL                     0x1A
#define COMMAND_SET_COUNT_MAX                     0x1B
#define COMMAND_SET_COUNT_MIN                     0x1C

#define COMMAND_START_SENSOR                      0x20
#define COMMAND_START_COUNTER                      0x20
#define COMMAND_STOP_SENSOR                      0x21
#define COMMAND_STOP_COUNTER                      0x21
#define COMMAND_STOP_COUNTER                      0x21

#define COMMAND_EXIT                          0x1E
#define COMMAND_INVALID                       0x1F

void command_list_init(void);
unsigned char parse_command(unsigned char* input);

char command_list[TOTAL_COMMANDS][32];

void *ui_task_thread(void *);
void *ui_heartbeat_notifier(void *);
