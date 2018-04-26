#include "../main.h"
#include "../include/sock_comm_task.h"
#include "../include/logger_task.h"
#include <errno.h>

void *sock_comm_task_thread(void *args)
{
  char sockaddr_path[32];
  char response[64];
  sk_payload_ip_request_t request_ip;
  mq_payload_heartbeat_t heartbeat_sock_comm;
  char log_message[128];
  pthread_t heartbeat_sock_comm_notifier_task, sock_comm_ui_handler_task;

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

  sockaddr_length = sizeof(struct sockaddr_un);
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

  sock_comm_ip = socket(AF_INET, SOCK_DGRAM, 0);     //create a datagram socket for internal comm

  if(sock_comm < 0)
  {
    bzero(log_message, sizeof(log_message));
    sprintf(log_message, "## SOCK COMM ## Error Creating Socket. %s", strerror(errno));
    LOG(mq_logger, log_message);
  }

  sockaddr_length_ip = sizeof(struct sockaddr_in);

  //This code populates the sockaddr_in struct with the information about our socket
	bzero(&sockaddr_comm_ip,sockaddr_length_ip);                    //zero the struct
	sockaddr_comm_ip.sin_family = AF_INET;                   //address family
	sockaddr_comm_ip.sin_port = htons(SOCK_COMM_LISTEN_PORT);        //htons() sets the port # to network byte order
  sockaddr_comm_ip.sin_addr.s_addr = htonl(INADDR_ANY); //supplies the IP address of the local machine

  // if(mq_send(mq_heartbeat, (char *) &heartbeat_sock_comm, sizeof(heartbeat_sock_comm), 1) < 0)
  // {
  //   bzero(log_message, sizeof(log_message));
  //   sprintf(log_message, "## SOCK COMM ## Unable to send heartbeat. %s", strerror(errno));
  //   LOG(mq_logger, log_message);
  // }

  pthread_create(&heartbeat_sock_comm_notifier_task, NULL, heartbeat_sock_comm_notifier_thread, (void *) NULL);
  pthread_create(&sock_comm_ui_handler_task, NULL, sock_comm_ui_handler_task_thread, (void *) NULL);

  while(!close_app)
  {

    if(recvfrom(sock_comm_ip, (void *) &request_ip, sizeof(request_ip), 0, (struct sockaddr *) &sockaddr_s, &sockaddr_length) < 0)
    {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## SOCK COMM ## Error receiving request from user: %s", strerror(errno));
      LOG(mq_logger, log_message);
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

void *sock_comm_ui_handler_task_thread(void *args)
{
  while(!close_app)
  {
    sk_payload_ui_request_t request_ui;

    if(recvfrom(sock_comm, (void *) &request_ui, sizeof(request_ui), 0, (struct sockaddr *) &sockaddr_comm_ui, &sockaddr_length_ui) < 0)
    {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## SOCK COMM ## Error receiving request from user: %s", strerror(errno));
      LOG(mq_logger, log_message);
    }

    bzero(log_message, sizeof(log_message));
    sprintf(log_message, "## SOCK COMM ## Received request from user. %s");
    LOG(mq_logger, log_message);
  }
}


int find_available_slot(void)
{
	int slot = 0;

	sem_wait(&sem_slots);
	for(slot = 0; slot < MAX_THREADS; slot++)
	{
		if(thread_slot_tracker[slot] == THREAD_FREE)
			break;
	}
	sem_post(&sem_slots);

	if(slot < MAX_THREADS)
		return slot;
	else
		return -1;
}


void set_thread_slot_state(int thread_slot, unsigned char state)
{
	sem_wait(&sem_slots);
	thread_slot_tracker[thread_slot] = state;
	sem_post(&sem_slots);
}
