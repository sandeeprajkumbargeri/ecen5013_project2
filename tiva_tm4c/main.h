/*
 * main.h
 *
 *  Created on: Apr 28, 2018
 *      Author: Prithvi Teja
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"

#include "UART/uart_comm.h"

#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "portable.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"

#define UART_COMM   1


#define SEMAPHORE_MAX_COUNT             100
#define SEMAPHORE_INITIAL_COUNT         0

#define CLOCK_FREQUENCY                 120000000 - 1

#define TIMER_FREQUENCY                 1
#define TIMER_PERIOD                    1000/(1.5 * 2 * TIMER_FREQUENCY)

#define ON                              pdTRUE
#define OFF                             pdFALSE

#define EVENT_US1_DONE                  ((EventBits_t) 1<<0)        //Event bit for notifying US1 is done getting acquisition
#define EVENT_PRINT_STRING              ((EventBits_t) 1<<1)        //Event bit for printing the string

#define TIVA_ID                         49

#define APP_ROUTE                       50
#define LOG_ROUTE                       51

#define BME_COMMANDS_ID_MIN             0                           //Start of Command Ids aimed at BME 280. All IDs between BME_COMMANDS_ID_MIN and BME_COMMANDS_ID_MAX are directed to BME
#define BME_COMMANDS_ID_MAX             60

#define HCSR04_COMMANDS_ID_MIN          61                           //Start of Command Ids aimed at HCSR04. All IDs between HCSR04_COMMANDS_ID_MIN and HCSR04_COMMANDS_ID_MAX are directed to HCSR04
#define HCSR04_COMMANDS_ID_MAX          120

EventGroupHandle_t event_group;
uint32_t sys_clock_get;

QueueHandle_t comm_send_queue, comm_receive_queue, bme280_queue, hcsr04_queue;                                    //Queue used to send the Log string from Task 2 to Task 3.
SemaphoreHandle_t comm_task_sem, sem_bme280_acq, sem_hcsr04_acq, sem_hcsr04_update;             //Semaphores for comm_task and various tasks.



#endif /* MAIN_H_ */
