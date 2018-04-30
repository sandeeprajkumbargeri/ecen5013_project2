/*
 * uart_comm.h
 *
 *  Created on: Apr 28, 2018
 *      Author: Prithvi Teja
 */

#ifndef UART_UART_COMM_H_
#define UART_UART_COMM_H_

#include <stdint.h>
#include <termios.h>

int UART4_Init(void);
int UART4Send(int uart_fd, const uint8_t *write_buffer, uint32_t ui32Count);
int UART4Recv(int uart_fd, const uint8_t *read_buffer, uint32_t ui32Count);


#endif /* UART_UART_COMM_H_ */
