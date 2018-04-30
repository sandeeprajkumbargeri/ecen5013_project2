#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <strings.h>
#include "../include/uart_comm.h"

int UART4_Init(void)
{
  int fd;
  struct termios SerialPortSettings;

  fd = open("/dev/ttyO4",O_RDWR | O_NOCTTY);
  if(fd == -1)
  {
     return fd;
  }
  else
     printf("\n  ttyO4 Opened Successfully\n");

  tcgetattr(fd, &SerialPortSettings);

  cfsetispeed(&SerialPortSettings,B9600);
  cfsetospeed(&SerialPortSettings,B9600);

  SerialPortSettings.c_cflag &= ~PARENB;
  SerialPortSettings.c_cflag &= ~CSTOPB;
  SerialPortSettings.c_cflag &= ~CSIZE;
  SerialPortSettings.c_cflag |=  CS8;
  SerialPortSettings.c_cflag &= ~CRTSCTS;
  SerialPortSettings.c_cflag |= CREAD | CLOCAL;
  SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);
  SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  SerialPortSettings.c_cc[VMIN]  = 1; // Read 10 characters
  SerialPortSettings.c_cc[VTIME] = 0;  // Wait indefinitely

  tcsetattr(fd,TCSANOW,&SerialPortSettings);

  return fd;
}

int UART4Send(int uart_fd, const uint8_t *write_buffer, uint32_t ui32Count)
{
  int bytes_written;

  bytes_written = write(uart_fd,(void *)write_buffer, ui32Count);
  if(bytes_written < 0)
  {
    return bytes_written;
  }
  return bytes_written;

}
int UART4Recv(int uart_fd, const uint8_t *read_buffer, uint32_t ui32Count)
{
  int bytes_read;

  bytes_read = read(uart_fd, (void *)read_buffer, ui32Count);
  if(bytes_read < 0)
  {
    return bytes_read;
  }
  return bytes_read;
}

// void main()
// {
//   int fd;
//   char write_buffer[] = "ABCDEFGHIJKLMNO";
//   int  bytes_written  =  0;
//   fd = UART4_Init();
//   if(fd < 0)
//   {
//      printf("\nError in opening ttyO4(UART4)\n");
//      return;
//   }
//
//   bytes_written = UART4Send(fd, (uint8_t *)write_buffer, sizeof(write_buffer));
//   if(bytes_written < 0)
//   {
//     perror("Failed to write to Serial Port ttyO4(UART4)");
//   }
//   char buffer[100];
//   bzero(buffer, sizeof(buffer));
//
//   int bytes_read;
//
//   bytes_read = UART4Recv(fd, (uint8_t *)buffer, sizeof(buffer));
//
//   if(bytes_read < 0)
//   {
//     perror("Error reading from serial port");
//   }
//
//   printf("\nReceived Message: %s\n", buffer);
// }
