#include "../include/ui_task.h"

void *ui_task_thread(void *args)
{
  int sock_ui = 0;
  char sockaddr_path_ui[32], sockaddr_path_comm[32];
  char command[64], response[64];
  struct sockaddr_un sockaddr_ui, sockaddr_comm;
  socklen_t sockaddr_length;
  sk_payload_ui_request_t request;

  command_list_init();
  sock_ui = socket(AF_UNIX, SOCK_DGRAM, 0);     //create a datagram socket

  if(sock_ui < 0)
    errExit("## ERROR ## Creating Socket: ");

  bzero(sockaddr_path_ui, sizeof(sockaddr_path_ui));
  strcpy(sockaddr_path_ui, SK_UI_PATH);
  remove(sockaddr_path_ui);

  bzero(sockaddr_path_comm, sizeof(sockaddr_path_comm));
  strcpy(sockaddr_path_comm, SK_SOCK_COMM_PATH);

  sockaddr_length = sizeof(sockaddr_un);
  bzero(&sockaddr_ui, sockaddr_length);
  bzero(&sockaddr_comm, sockaddr_length);

  sockaddr_ui.sun_family = AF_UNIX;
  strncpy(sockaddr_ui.sun_path, sockaddr_path_comm, sizeof(sockaddr_ui.sun_path) -1);

  sockaddr_comm.sun_family = AF_UNIX;
  strncpy(sockaddr_comm.sun_path, sockaddr_path_comm, sizeof(sockaddr_comm.sun_path) -1);

  if(bind(sock_ui, (struct sockaddr *) &sockaddr_ui, sizeof(struct sockaddr_un)) < 0)
    errExit("## ERROR ## Bind: ");

  printf("\n\t\t\t## AVAILABLE COMMANDS ##\n");
  for(int i = 0; i < TOTAL_COMMANDS; i++)
    printf("%s\n",command_list[i]);

  while(1)
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
      if(sendto(sock_ui, (const void *) &request, sizeof(request), 0, (const struct sockaddr *) &sockaddr_comm, sockaddr_length) < 0)
        errExit("## ERROR ## Sending request to remote: ");

      bzero(response, sizeof(response));

      if(recvfrom(sock_ui, (void *) response, sizeof(response), 0, (struct sockaddr *) &sockaddr_comm, &sockaddr_length) < 0)
        errExit("## ERROR ## Receiving response from remote: ");

      printf("-> %s\n", response);
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
  strcpy(command_list[COMMAND_TEMP_READ_TLOW], "temp read t-low");
  strcpy(command_list[COMMAND_TEMP_READ_THIGH], "temp read t-high");
  strcpy(command_list[COMMAND_TEMP_READ_DATA_REG], "temp read data");
  strcpy(command_list[COMMAND_TEMP_SET_SD_ON], "temp set shutdown on");
  strcpy(command_list[COMMAND_TEMP_SET_SD_OFF], "temp set shutdown off");
  strcpy(command_list[COMMAND_TEMP_READ_RESOLUTION], "temp read resolution");
  strcpy(command_list[COMMAND_TEMP_READ_FAULT_BITS], "temp read fault-bits");
  strcpy(command_list[COMMAND_TEMP_READ_EM], "temp read ext-mode");
  strcpy(command_list[COMMAND_TEMP_SET_EM_ON], "temp set ext-mode on");
  strcpy(command_list[COMMAND_TEMP_SET_EM_OFF], "temp set ext-mode off");

  strcpy(command_list[COMMAND_TEMP_SET_CONV_RATE_0], "temp set conv-rate 0.25hz");
  strcpy(command_list[COMMAND_TEMP_SET_CONV_RATE_1], "temp set conv-rate 1hz");
  strcpy(command_list[COMMAND_TEMP_SET_CONV_RATE_2], "temp set conv-rate 4hz");
  strcpy(command_list[COMMAND_TEMP_SET_CONV_RATE_3], "temp set conv-rate 8hz");
  strcpy(command_list[COMMAND_TEMP_READ_CONV_RATE], "temp read conv-rate");


  strcpy(command_list[COMMAND_LIGHT_SET_INTG_TIME_0], "light set intg-time 13.7ms");
  strcpy(command_list[COMMAND_LIGHT_SET_INTG_TIME_1], "light set intg-time 101ms");
  strcpy(command_list[COMMAND_LIGHT_SET_INTG_TIME_2], "light set intg-time 402ms");
  strcpy(command_list[COMMAND_LIGHT_READ_INTG_TIME], "light read intg-time");
  strcpy(command_list[COMMAND_LIGHT_READ_GAIN], "light read gain");
  strcpy(command_list[COMMAND_LIGHT_SET_GAIN_HIGH], "light set gain high");
  strcpy(command_list[COMMAND_LIGHT_SET_GAIN_LOW], "light set gain low");
  strcpy(command_list[COMMAND_LIGHT_SET_INT_ENABLE], "light set int enable");
  strcpy(command_list[COMMAND_LIGHT_SET_INT_DISABLE], "light set int disable");
  strcpy(command_list[COMMAND_LIGHT_READ_IDENTIFY_REG], "light read id");
  strcpy(command_list[COMMAND_LIGHT_READ_INT_TRSHLD_LOW], "light read trshld low");
  strcpy(command_list[COMMAND_LIGHT_READ_INT_TRSHLD_HIGH], "light read trshld low");
  strcpy(command_list[COMMAND_LIGHT_SET_INT_TRSHLD_LOW], "light set trshld low");
  strcpy(command_list[COMMAND_LIGHT_SET_INT_TRSHLD_HIGH], "light set trshld high");

  strcpy(command_list[COMMAND_EXIT], "exit");
}
