#include "../include/ui_task.h"
#include "../include/logger_task.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

void *ui_task_thread(void *args)
{
  int sock_ui = 0;
  char sockaddr_path_ui[32], sockaddr_path_comm[32];
  char command[64], response[64];
  struct sockaddr_un sockaddr_ui, sockaddr_comm;
  socklen_t sockaddr_length;
  sk_payload_ui_request_t request;
  char log_message[128];
  pthread_t heartbeat_ui_notifier_task;

  command_list_init();
  sock_ui = socket(AF_UNIX, SOCK_DGRAM, 0);     //create a datagram socket

  if(sock_ui < 0)
  {
    bzero(log_message, sizeof(log_message));
    sprintf(log_message, "## UI ## Error Creating Socket. %s", strerror(errno));
    LOG(mq_logger, log_message);
  }

  bzero(sockaddr_path_ui, sizeof(sockaddr_path_ui));
  strcpy(sockaddr_path_ui, SK_UI_PATH);
  remove(sockaddr_path_ui);

  bzero(sockaddr_path_comm, sizeof(sockaddr_path_comm));
  strcpy(sockaddr_path_comm, SK_SOCK_COMM_PATH);

  sockaddr_length = sizeof(struct sockaddr_un);
  bzero(&sockaddr_ui, sockaddr_length);
  bzero(&sockaddr_comm, sockaddr_length);

  sockaddr_ui.sun_family = AF_UNIX;
  strncpy(sockaddr_ui.sun_path, sockaddr_path_ui, sizeof(sockaddr_ui.sun_path) -1);

  sockaddr_comm.sun_family = AF_UNIX;
  strncpy(sockaddr_comm.sun_path, sockaddr_path_comm, sizeof(sockaddr_comm.sun_path) -1);

  if(bind(sock_ui, (struct sockaddr *) &sockaddr_ui, sizeof(struct sockaddr_un)) < 0)
  {
    bzero(log_message, sizeof(log_message));
    sprintf(log_message, "## UI ## Error Creating Socket. %s", strerror(errno));
    LOG(mq_logger, log_message);
  }

  printf("\n\t\t\t## AVAILABLE COMMANDS ##\n");
  for(int i = 0; i < TOTAL_COMMANDS; i++)
    printf("%s\n",command_list[i]);

  pthread_create(&heartbeat_ui_notifier_task, NULL, ui_heartbeat_notifier_thread, (void *) NULL);

  while(!close_app)
  {
    bzero(command, sizeof(command));
    printf("\n$ ");
    fgets(command, sizeof(command), stdin);

    command[strlen(command) - 1] = '\0';
    bzero(&request, sizeof(request));
    request.command = parse_command(command);

    if(request.command == COMMAND_INVALID)
    {
      printf("\n## ERROR ## Invalid command.\n");
      continue;
    }

    else if(request.command == COMMAND_EXIT)
    {
      close_app = true;
      break;
    }

    else if(request.command == COMMAND_HELP)
    {
      printf("\n\t\t\t## AVAILABLE COMMANDS ##\n");
      for(int i = 0; i < TOTAL_COMMANDS; i++)
        printf("%s\n",command_list[i]);
    }

    else
    {
      if(request.command >= COMMAND_SET_TEMPERATURE_MAX && request.command <= COMMAND_SET_RANGE)
        {
          printf("Enter value: ");
          scanf("%u", &request.data);
        }

      if((request.command == COMMAND_SET_TEMPERATURE_MAX) && (request.data > MAX_SETTABLE_TEMP) ||
      (request.command == COMMAND_SET_TEMPERATURE_MIN) && (request.data < MIN_SETTABLE_TEMP) ||
      (request.command == COMMAND_SET_PRESSURE_MAX) && (request.data > MAX_SETTABLE_PRESS) ||
      (request.command == COMMAND_SET_PRESSURE_MIN) && (request.data < MIN_SETTABLE_PRESS) ||
      (request.command == COMMAND_SET_HUMIDITY_MAX) && (request.data > MAX_SETTABLE_HUMID) ||
      (request.command == COMMAND_SET_HUMIDITY_MIN) && (request.data < MIN_SETTABLE_HUMID))
      {
        printf("## ERROR ## Value out of bounds.\n");
        continue;
      }

      if(sendto(sock_ui, (const void *) &request, sizeof(request), 0, (const struct sockaddr *) &sockaddr_comm, sockaddr_length) < 0)
        {
          bzero(log_message, sizeof(log_message));
          sprintf(log_message, "## UI ## Sending request to remote: %s", strerror(errno));
          LOG(mq_logger, log_message);
        }

      bzero(response, sizeof(response));

      if(recvfrom(sock_ui, (void *) response, sizeof(response), 0, (struct sockaddr *) &sockaddr_comm, &sockaddr_length) < 0)
      {
        bzero(log_message, sizeof(log_message));
        sprintf(log_message, "## UI ## Receiving response from remote: %s", strerror(errno));
        LOG(mq_logger, log_message);
      }

      printf("-> %s\n", response);
    }
  }

  pthread_exit(0);
}

void *ui_heartbeat_notifier_thread(void *args)
{
  mq_payload_heartbeat_t heartbeat_ui;
  char log_message[128];

  bzero(&heartbeat_ui, sizeof(heartbeat_ui));
  heartbeat_ui.sender_id = UI_TASK_ID;
  heartbeat_ui.heartbeat_status = true;

  while(!close_app)
  {
    sem_wait(&sem_ui);

    if(send_heartbeat[UI_TASK_ID])
    {
        if(mq_send(mq_heartbeat, (char *) &heartbeat_ui, sizeof(heartbeat_ui), 1) < 0)
        {
          bzero(log_message, sizeof(log_message));
          sprintf(log_message, "## SOCK COMM ## Unable to send heartbeat. %s", strerror(errno));
          LOG(mq_logger, log_message);
        }

        send_heartbeat[UI_TASK_ID] = false;
    }
  }
  pthread_exit(0);
}

unsigned char parse_command(unsigned char* input)
{
		unsigned int i = 0;

		for(i = 0; i < TOTAL_COMMANDS; i++)
		{
			if(strcmp(input, command_list[i]) == 0)
				return (unsigned char) i;
		}

		return (COMMAND_INVALID);
}

void command_list_init(void)
{
  bzero(command_list, sizeof(command_list));
  strcpy(command_list[COMMAND_HELP], "help");

  strcpy(command_list[COMMAND_GET_TEMPERATURE_VAL], "get temp val");
  strcpy(command_list[COMMAND_GET_TEMPERATURE_MAX], "get temp alert max");
  strcpy(command_list[COMMAND_GET_TEMPERATURE_MIN], "get temp alert min");
  strcpy(command_list[COMMAND_GET_PRESSURE_VAL], "get press val");
  strcpy(command_list[COMMAND_GET_PRESSURE_MAX], "get press alert max");
  strcpy(command_list[COMMAND_GET_PRESSURE_MIN], "get press alert min");
  strcpy(command_list[COMMAND_GET_HUMIDITY_VAL], "get humid val");
  strcpy(command_list[COMMAND_GET_HUMIDITY_MAX], "get humid alert max");
  strcpy(command_list[COMMAND_GET_HUMIDITY_MIN], "get humid alert min");
  strcpy(command_list[COMMAND_GET_COUNT_VAL], "get count val");
  strcpy(command_list[COMMAND_GET_COUNT_MAX], "get count alert max");
  strcpy(command_list[COMMAND_GET_COUNT_MIN], "get count alert min");
  strcpy(command_list[COMMAND_GET_RANGE_VAL], "get range");

  strcpy(command_list[COMMAND_SET_TEMPERATURE_MAX], "set temp alert max");
  strcpy(command_list[COMMAND_SET_TEMPERATURE_MIN], "set temp alert min");
  strcpy(command_list[COMMAND_SET_PRESSURE_MAX], "set press alert max");
  strcpy(command_list[COMMAND_SET_PRESSURE_MIN], "set press alert min");
  strcpy(command_list[COMMAND_SET_HUMIDITY_MAX], "set humid alert max");
  strcpy(command_list[COMMAND_SET_HUMIDITY_MIN], "set humid alert min");
  strcpy(command_list[COMMAND_SET_COUNT_MAX], "set count alert max");
  strcpy(command_list[COMMAND_SET_COUNT_MIN], "set count alert min");
  strcpy(command_list[COMMAND_SET_RANGE], "set range");

  strcpy(command_list[COMMAND_START_SENSOR], "start sensor");
  strcpy(command_list[COMMAND_START_COUNTER], "start count");
  strcpy(command_list[COMMAND_STOP_SENSOR], "stop sensor");
  strcpy(command_list[COMMAND_STOP_COUNTER], "stop count");
  strcpy(command_list[COMMAND_RESET_COUNTER], "reset count");

  strcpy(command_list[COMMAND_REQUEST_ID], "which client");

  strcpy(command_list[COMMAND_EXIT], "exit");
}
