#include "inc/i2c0_rw.h"
#include "inc/bme280.h"
#include "inc/hcsr04.h"

//#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#//include <strings.h>

#include "FreeRTOS.h"
#include "portmacro.h"
#include "task.h"
#include "portable.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"

//#include <stdio.h>
//#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
//#include "inc/hw_i2c.h"
//#include "driverlib/debug.h"
#include "driverlib/gpio.h"
//#include "driverlib/i2c.h"
//#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
//#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/uartstdio.h"

//#include "inc/hw_sysctl.h"
//#include "inc/hw_types.h"

#define CLOCK_FREQUENCY     120000000 - 1
//#define CLOCK_FREQUENCY     32000000

#define UART_PORT_0         0
#define UART_BAUD_RATE      115200

#define TIMER_FREQUENCY     1
#define TIMER_PERIOD        1000/(1.5 * 2 * TIMER_FREQUENCY)

#define ON                  pdTRUE
#define OFF                 pdFALSE

#define EVENT_US1_DONE            ((EventBits_t) 1<<0)        //Event bit for notifying US1 is done getting acquisition
#define EVENT_PRINT_STRING            ((EventBits_t) 1<<1)        //Event bit for printing the string

SemaphoreHandle_t sem_bme280_acq, sem_hcsr04_acq, sem_hcsr04_update;

EventGroupHandle_t event_group;
uint32_t g_ui32SysClock;
