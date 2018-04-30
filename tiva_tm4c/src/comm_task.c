/*
 * comm_task.c
 *
 *  Created on: Apr 28, 2018
 *      Author: Prithvi Teja
 */

#include "../include/comm_task.h"
#include "../include/bme280.h"
#include "string.h"

#include "FreeRTOS.h"
#include "portmacro.h"


//This task is used to communicate with the BeagleBone using one of the communication interface and based on the compile time switch a communication interface is chosen.
void vCommTask( void *pvParameters )
{
    send_packet_buffer send_buffer;
    receive_packet_buffer receive_buffer;
    BaseType_t returnVal;

    bzero((void *)&send_buffer, SEND_PACKET_SIZE);

    send_buffer.tiva_id = 49;
    send_buffer.route_id = 65;        //Sending it to the logger

    COMM_CONFIGURE(sys_clock_get);                                  //This macro calls the configure function for the communication interface underneath

    sprintf((char *)send_buffer.message, "Project For: Prithvi Veeravalli 04/06/2018");

    COMM_SEND((uint8_t *)&send_buffer, SEND_PACKET_SIZE);                //This function calls the send function for the communication interface underneath.


    while(1)
    {
        xSemaphoreTake(comm_task_sem, portMAX_DELAY);
        xSemaphoreGive(comm_task_sem);
        if(comm_task_events & COMM_TASK_SEND_EVENT)
        {
            xSemaphoreTake(comm_task_sem, portMAX_DELAY);
            while(uxQueueMessagesWaiting(comm_send_queue))
            {
                bzero((void *)&send_buffer, SEND_PACKET_SIZE);
                returnVal = xQueueReceive(comm_send_queue, (void *)&send_buffer, portMAX_DELAY);
                if(returnVal == pdPASS)
                {
                    COMM_SEND((uint8_t *)&send_buffer, SEND_PACKET_SIZE);         //This function calls the send function for the communication interface underneath.
                }
            }
            comm_task_events &= ~COMM_TASK_SEND_EVENT;
        }
        if(comm_task_events & COMM_TASK_RECEIVE_EVENT)
        {
            xSemaphoreTake(comm_task_sem, portMAX_DELAY);
            while(uxQueueMessagesWaiting(comm_receive_queue))
            {
                bzero((void *)&receive_buffer, RECEIVE_PACKET_SIZE);
                returnVal = xQueueReceive(comm_receive_queue, (void *)&receive_buffer, portMAX_DELAY);
                if(returnVal == pdPASS)
                {
                    if(receive_buffer.tiva_id == TIVA_ID)
                    {
                        if((receive_buffer.command_id >= BME_COMMANDS_ID_MIN)&&(receive_buffer.command_id <= BME_COMMANDS_ID_MAX))
                        {
                            xQueueSend(bme280_queue, (void *)&receive_buffer, portMAX_DELAY);
                            if(!(BME280_task_events & BME280_TASK_RECEIVE_EVENT))
                            {
                                BME280_task_events |= BME280_TASK_RECEIVE_EVENT;
                                xSemaphoreGive(sem_bme280_acq);
                            }
                        }

                    }
                }
            }
            comm_task_events &= ~COMM_TASK_RECEIVE_EVENT;

        }

    }

}

