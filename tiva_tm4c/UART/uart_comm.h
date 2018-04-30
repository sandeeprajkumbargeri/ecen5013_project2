/*
 * uart_comm.h
 *
 *  Created on: Apr 28, 2018
 *      Author: Prithvi Teja
 */

#ifndef UART_UART_COMM_H_
#define UART_UART_COMM_H_

#include <stdint.h>

#define UART_PORT               0
#define BAUD_RATE               115200

void UARTStdioIntHandler(void);
void UART_Configure(uint32_t ui32SrcClock);
void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count);


#endif /* UART_UART_COMM_H_ */
