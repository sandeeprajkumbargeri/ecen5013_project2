#include "../include/logger_task.h"
#include <errno.h>

void *logger_task_thread(void *args)
{
  char log_info[128];
  pthread_t logger_heartbeat_notifier_desc;
  char current_time[32] = {0};
  char log_message[128];

  mq_payload_heartbeat_t logger_heartbeat;

  bzero(&logger_heartbeat, sizeof(logger_heartbeat));
  logger_heartbeat.sender_id = LOGGER_TASK_ID;
  logger_heartbeat.heartbeat_status = true;

  bzero(log_info, sizeof(log_info));

  if(mq_send(mq_heartbeat, (char *) &logger_heartbeat, sizeof(logger_heartbeat), 1) < 0)
  {
    bzero(log_message, sizeof(log_message));
    sprintf(log_message, "## LOGGER ## Unable to send startup heartbeat. %s", strerror(errno));
    LOG(mq_logger, log_message);
  }

  pthread_create(&logger_heartbeat_notifier_desc, NULL, logger_heartbeat_notifier, (void *) NULL);

  while(!close_app)
  {
    if(mq_receive(mq_logger, (char *) log_info, sizeof(log_info), 0) < 0)
      perror("## LOGGER ## Message Queue Receive");

    log_file = fopen(filename, "a");

    if(log_file == NULL)
	  {
	    printf("\n## ERROR ## Cannot create the file \"%s\". Exiting.\n", filename);
	    exit(1);
	  }

    update_time(current_time, sizeof(current_time));
    fprintf(log_file, "%s :: %s\n", current_time, log_info);
    //length = fwrite(log_info, sizeof(char), strlen(log_info), log_file);
    fclose(log_file);
  }
  fclose(log_file);
  bzero(log_info, sizeof(log_info));
  sprintf(log_info, "## LOGGER ## Exiting the logger thread. I received an exit request");
  fprintf(log_file, "%s\n", log_info);
  pthread_join(logger_heartbeat_notifier_desc, NULL);
  pthread_exit(0);
}

void update_time(char *current_time, size_t length)
{
  time_t present_time;
  struct tm *local_time;

  bzero(current_time, length);
  time(&present_time);
  local_time = localtime(&present_time);
  strcpy(current_time, asctime(local_time));
  current_time[strlen(current_time) - 1] = '\0';
}

void *logger_heartbeat_notifier(void *args)
{
  mq_payload_heartbeat_t logger_heartbeat;
  char log_message[128];

  bzero(&logger_heartbeat, sizeof(logger_heartbeat));
  logger_heartbeat.sender_id = LOGGER_TASK_ID;
  logger_heartbeat.heartbeat_status = true;

  while(!close_app)
  {
    sem_wait(&sem_logger);

    if(send_heartbeat[LOGGER_TASK_ID])
    {
        if(mq_send(mq_heartbeat, (char *) &logger_heartbeat, sizeof(logger_heartbeat), 1) < 0)
        {
          bzero(log_message, sizeof(log_message));
          sprintf(log_message, "## LOGGER ## Unable to send heartbeat. %s", strerror(errno));
          LOG(mq_logger, log_message);
        }

        send_heartbeat[LOGGER_TASK_ID] = false;
    }
  }
  bzero(log_message, sizeof(log_message));
  sprintf(log_message, "## LOGGER ## Exiting the logger thread. %s", strerror(errno));
  LOG(mq_logger, log_message);
  pthread_exit(0);
}
