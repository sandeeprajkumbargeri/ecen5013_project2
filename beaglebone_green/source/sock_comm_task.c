#include "../main.h"
#include "../include/sock_comm_task.h"
#include "../include/logger_task.h"
#include "../include/uart_comm.h"
#include <errno.h>

#define UART
//#define ETHERNET

extern bool close_app;

void *sock_comm_task_thread(void *args)
{
  char sockaddr_path[32], sockaddr_path_ui[32];
  char response[64];
  sk_payload_ip_message_t message_ip;
  mq_payload_heartbeat_t heartbeat_sock_comm;
  char log_message[128];
  pthread_t heartbeat_sock_comm_notifier_task, sock_comm_ui_handler_task;
  int bytes_read = 0;

  bzero(&heartbeat_sock_comm, sizeof(heartbeat_sock_comm));
  heartbeat_sock_comm.sender_id = SOCK_COMM_TASK_ID;
  heartbeat_sock_comm.heartbeat_status = true;

  sock_comm = socket(AF_UNIX, SOCK_DGRAM, 0);     //create a datagram socket for internal comm

  if(sock_comm < 0)
  {
    bzero(log_message, sizeof(log_message));
    sprintf(log_message, "## SOCK COMM ## Error Creating Socket. %s", strerror(errno));
    LOG(mq_logger, log_message);
  }

  bzero(sockaddr_path, sizeof(sockaddr_path));
  sprintf(sockaddr_path, SK_SOCK_COMM_PATH);
  remove(sockaddr_path);

  bzero(sockaddr_path_ui, sizeof(sockaddr_path_ui));
  strcpy(sockaddr_path_ui, SK_UI_PATH);

  sockaddr_length_ui = sizeof(struct sockaddr_un);
  bzero(&sockaddr_comm, sockaddr_length_ui);
  //bzero(&sock_uiaddr, sockaddr_length);

  sockaddr_ui.sun_family = AF_UNIX;
  strncpy(sockaddr_ui.sun_path, sockaddr_path_ui, sizeof(sockaddr_ui.sun_path) -1);

  sockaddr_comm.sun_family = AF_UNIX;
  strncpy(sockaddr_comm.sun_path, sockaddr_path, sizeof(sockaddr_comm.sun_path) -1);

  if(bind(sock_comm, (struct sockaddr *) &sockaddr_comm, sockaddr_length_ui) < 0)
  {
    bzero(log_message, sizeof(log_message));
    sprintf(log_message, "## SOCK COMM ## Bind failed. %s", strerror(errno));
    LOG(mq_logger, log_message);
  }

  #ifdef UART
    uart_handler = UART4_Init();

    if(uart_handler == -1)
    {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## SOCK COMM ## Error Creating Socket. %s", strerror(errno));
      LOG(mq_logger, log_message);
      pthread_exit(0);
    }

    else
    {
       bzero(log_message, sizeof(log_message));
       sprintf(log_message, "## SOCK COMM ## Error Creating Socket. %s", strerror(errno));
       LOG(mq_logger, log_message);
    }
  #endif

  #ifdef ETHERNET
    sock_comm_ip = socket(AF_INET, SOCK_DGRAM, 0);     //create a datagram socket for internal comm

    if(sock_comm_ip < 0)
    {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## SOCK COMM ## Error Creating Socket. %s", strerror(errno));
      LOG(mq_logger, log_message);
      pthread_exit(0);
    }

    sockaddr_length_ip = sizeof(struct sockaddr_in);

    //This code populates the sockaddr_in struct with the information about our socket
    bzero(&sockaddr_comm_ip,sockaddr_length_ip);                    //zero the struct
    sockaddr_comm_ip.sin_family = AF_INET;                   //address family
    sockaddr_comm_ip.sin_port = htons(SOCK_COMM_LISTEN_PORT);        //htons() sets the port # to network byte order
    sockaddr_comm_ip.sin_addr.s_addr = htonl(INADDR_ANY); //supplies the IP address of the local machine
  #endif

  // if(mq_send(mq_heartbeat, (char *) &heartbeat_sock_comm, sizeof(heartbeat_sock_comm), 1) < 0)
  // {
  //   bzero(log_message, sizeof(log_message));
  //   sprintf(log_message, "## SOCK COMM ## Unable to send heartbeat. %s", strerror(errno));
  //   LOG(mq_logger, log_message);
  // }

  pthread_create(&heartbeat_sock_comm_notifier_task, NULL, heartbeat_sock_comm_notifier_thread, (void *) NULL);
  pthread_create(&sock_comm_ui_handler_task, NULL, sock_comm_ui_handler_task_thread, (void *) NULL);

  bzero(&sockaddr_comm_client, sizeof(sockaddr_comm_client));

  while(!close_app)
  {
    #ifdef ETHERNET
    if(recvfrom(sock_comm_ip, (void *) &message_ip, sizeof(message_ip), 0, (struct sockaddr *) &sockaddr_comm_client, &sockaddr_length_ip) < 0)
    {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## SOCK COMM ## Error receiving request from client (SOCKET): %s", strerror(errno));
      LOG(mq_logger, log_message);
    }
    #endif

    #ifdef UART
    bytes_read = UART4Recv(uart_handler, (uint8_t *)&message_ip, sizeof(message_ip));

    if(bytes_read < 0)
    {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## SOCK COMM ## Error receiving request from client (UART): %s", strerror(errno));
      LOG(mq_logger, log_message);
      continue;
    }
    #endif

    if(message_ip.route_id == LOGGER_TASK_ID)
    {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## CLIENT %u ## %s.", message_ip.device_id, message_ip.message);
      LOG(mq_logger, log_message);
    }

    else if(message_ip.route_id == UI_TASK_ID)
    {
      if(sendto(sock_comm, (const void *) message_ip.message, sizeof(message_ip.message), 0, (const struct sockaddr *) &sockaddr_ui, sockaddr_length_ui) < 0)
        {
          bzero(log_message, sizeof(log_message));
          sprintf(log_message, "## SOCK COMM ## Error sending response from client to ui_task: %s", strerror(errno));
          LOG(mq_logger, log_message);
        }
    }

  }

    bzero(log_message, sizeof(log_message));
    sprintf(log_message, "## SOCK COMM ## Exiting the sock comm thread(task). I received an exit request");
    fprintf(log_file, "%s\n", log_message);
    pthread_join(heartbeat_sock_comm_notifier_task, NULL);
    pthread_join(sock_comm_ui_handler_task, NULL);
    pthread_exit(0);
}

void *heartbeat_sock_comm_notifier_thread(void *args)
{
  mq_payload_heartbeat_t heartbeat_sock_comm;
  char log_message[128];

  bzero(&heartbeat_sock_comm, sizeof(heartbeat_sock_comm));
  heartbeat_sock_comm.sender_id = SOCK_COMM_TASK_ID;
  heartbeat_sock_comm.heartbeat_status = true;

  while(!close_app)
  {
    sem_wait(&sem_sock_comm);

    if(send_heartbeat[SOCK_COMM_TASK_ID])
    {
        if(mq_send(mq_heartbeat, (char *) &heartbeat_sock_comm, sizeof(heartbeat_sock_comm), 1) < 0)
        {
          bzero(log_message, sizeof(log_message));
          sprintf(log_message, "## SOCK COMM ## Unable to send heartbeat. %s", strerror(errno));
          LOG(mq_logger, log_message);
        }

        send_heartbeat[SOCK_COMM_TASK_ID] = false;
    }
  }
  pthread_exit(0);
}

void *sock_comm_ui_handler_task_thread(void *args)
{
  char log_message[128];
  while(!close_app)
  {
    sk_payload_ui_request_t request_ui;
    int bytes_written = 0;
    char test_string[12] = "Hello World";

    if(recvfrom(sock_comm, (void *) &request_ui, sizeof(request_ui), 0, (struct sockaddr *) &sockaddr_ui, &sockaddr_length_ui) < 0)
    {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## SOCK COMM ## Error receiving request from user: %s", strerror(errno));
      LOG(mq_logger, log_message);
    }

    bzero(log_message, sizeof(log_message));
    sprintf(log_message, "## SOCK COMM ## Received request from user.");
    LOG(mq_logger, log_message);

    #ifndef UART
      //bytes_written = UART4Send(uart_handler, (uint8_t *)&request_ui, sizeof(request_ui));
      bytes_written = UART4Send(uart_handler, (uint8_t *)test_string, sizeof(test_string));

      if(bytes_written < 0)
      {
        bzero(log_message, sizeof(log_message));
        sprintf(log_message, "## SOCK COMM ## Error sending request to client. Failed to write to UART Port4");
        LOG(mq_logger, log_message);
        continue;
      }
    #endif

    #ifdef ETHERNET
      if(sockaddr_comm_client.sin_addr.s_addr == 0)
      {
        printf("## ERROR ## No client connection available.\n");
        continue;
      }

      else
      {
        if(sendto(sock_comm_ip, (const void *) &request_ui, sizeof(request_ui), 0, (const struct sockaddr *) &sockaddr_comm_client, sockaddr_length_ip) < 0)
          {
            bzero(log_message, sizeof(log_message));
            sprintf(log_message, "## SOCK COMM ## Error sending request to client.");
            LOG(mq_logger, log_message);
          }
      }
    #endif
  }
  pthread_exit(0);
}
