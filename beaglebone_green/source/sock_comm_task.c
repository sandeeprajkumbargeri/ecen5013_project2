#include "../main.h"
#include "../include/sock_comm_task.h"
#include "../include/logger_task.h"
#include <errno.h>

void *sock_comm_task_thread(void *args)
{
  char sockaddr_path[32];
  char response[64];
  struct sockaddr_un sockaddr_ui, sockaddr_comm;
  socklen_t sockaddr_length;
  sk_payload_ui_request_t request_ui;
  sk_payload_ip_request_t request_ip;
  mq_payload_heartbeat_t heartbeat_sock_comm;
  char log_message[128];
  pthread_t heartbeat_sock_comm_notifier_task;

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

  sockaddr_length = sizeof(sockaddr_un);
  bzero(&sockaddr_comm, sockaddr_length);
  //bzero(&sock_uiaddr, sockaddr_length);

  sockaddr_comm.sun_family = AF_UNIX;
  strncpy(sockaddr_comm.sun_path, sockaddr_path, sizeof(sockaddr_comm.sun_path) -1);

  if(bind(sock_comm, (struct sockaddr *) &sockaddr_comm, sockaddr_length) < 0)
  {
    bzero(log_message, sizeof(log_message));
    sprintf(log_message, "## SOCK COMM ## Bind failed. %s", strerror(errno));
    LOG(mq_logger, log_message);
  }

  // if(mq_send(mq_heartbeat, (char *) &heartbeat_sock_comm, sizeof(heartbeat_sock_comm), 1) < 0)
  // {
  //   bzero(log_message, sizeof(log_message));
  //   sprintf(log_message, "## SOCK COMM ## Unable to send heartbeat. %s", strerror(errno));
  //   LOG(mq_logger, log_message);
  // }

  pthread_create(&heartbeat_sock_comm_notifier_task, NULL, heartbeat_sock_comm_notifier_thread, (void *) NULL);

  while(!close_app)
  {
    bzero(&request, sizeof(request));

    if(recvfrom(sock_comm, (void *) &request, sizeof(request), 0, (struct sockaddr *) &sockaddr_ui, &sockaddr_length) < 0)
    {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## SOCK COMM ## Error receiving request from external application. %s", strerror(errno));
      LOG(mq_logger, log_message);
    }

    bzero(log_message, sizeof(log_message));
    sprintf(log_message, "## SOCK COMM ## Received request from user. %s", strerror(errno));
    LOG(mq_logger, log_message);

    if(request.command > 0x00 && request.command < 0x10)
    {
      if(mq_send(mq_temp, (const char *) &request, sizeof(mq_temp_light_payload_t), 0) < 0)
      {
        bzero(log_message, sizeof(log_message));
        sprintf(log_message, "## SOCK COMM ## Error sending request to temp task. %s", strerror(errno));
        LOG(mq_logger, log_message);
      }

      temp_asynch = true;
      sem_post(&sem_temp);
    }

    if(request.command > 0x0F && request.command < 0x1E)
    {
      if(mq_send(mq_light, (const char *) &request, sizeof(mq_temp_light_payload_t), 0) < 0)
      {
        bzero(log_message, sizeof(log_message));
        sprintf(log_message, "## SOCK COMM ## Error sending request to light task. %s", strerror(errno));
        LOG(mq_logger, log_message);
      }

      light_asynch = true;
      sem_post(&sem_light);
    }

    bzero(response, sizeof(response));

    if(mq_receive(mq_sock_comm, (char *) response, sizeof(response), 0) < 0)
    {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## SOCK COMM ## Error receiving response from temp/light task. %s", strerror(errno));
      LOG(mq_logger, log_message);
    }

    if(sendto(sock_comm, (const void *) response, sizeof(response), 0, (const struct sockaddr *) &app_sockaddr, sockaddr_length) < 0)
    {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## SOCK COMM ## Error sending response to external application. %s", strerror(errno));
      LOG(mq_logger, log_message);
    }
  }
  bzero(log_message, sizeof(log_message));
  sprintf(log_message, "## SOCK COMM ## Exiting the sock comm thread(task). I received an exit request");
  fprintf(log_file, "%s\n", log_message);
  pthread_join(heartbeat_sock_comm_notifier_desc, NULL);
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
