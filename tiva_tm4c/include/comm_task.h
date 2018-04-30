/*
 * comm_task.h
 *
 *  Created on: Apr 28, 2018
 *      Author: Prithvi Teja
 */

#ifndef COMM_TASK_H_
#define COMM_TASK_H_

#include "main.h"

#ifdef  UART_COMM
#include "../UART/uart_comm.h"

#define COMM_CONFIGURE(ui32SrcClock)            UART_Configure(ui32SrcClock)
#define COMM_SEND(pui8Buffer, BufferLength)     UARTSend(pui8Buffer, BufferLength)

#elif   SPI_COMM
//#include "spi_comm.h"



#endif  //COMM_METHOD

typedef struct receive_packet_buffer{
    uint8_t tiva_id;
    uint8_t command_id;
    uint8_t data;
}receive_packet_buffer;

typedef struct send_packet_buffer{
    uint8_t tiva_id;
    uint8_t route_id;
    uint8_t message[98];
}send_packet_buffer;

#define RECEIVE_PACKET_SIZE     sizeof(receive_packet_buffer)
#define SEND_PACKET_SIZE        sizeof(send_packet_buffer)
#define COMM_TASK_SEND_EVENT    1<<0
#define COMM_TASK_RECEIVE_EVENT    1<<1

volatile BaseType_t comm_task_events;

receive_packet_buffer receive_packet;
uint8_t packet_received;                                    //This variable is used as a flag to check if a packet has been received

void vCommTask( void *pvParameters );

#endif
/* COMM_TASK_H_ */

