IDIR=include/
SDIR=source/
CC=gcc
CFLAGS=-g -lm -lpthread -lrt
RFLAGS=-rf

execute: main.o sock_comm_task.o logger_task.o ui_task.o
	$(CC) -o execute main.o sock_comm_task.o logger_task.o ui_task.o $(CFLAGS)

sock_comm_task.o: $(SDIR)sock_comm_task.c $(IDIR)sock_comm_task.h main.h $(IDIR)logger_task.h
	$(CC) -c $(SDIR)sock_comm_task.c -g

logger_task.o: $(SDIR)logger_task.c $(IDIR)logger_task.h main.h
	$(CC) -c $(SDIR)logger_task.c -g

ui_task.o: $(SDIR)ui_task.c $(IDIR)sock_comm_task.h $(IDIR)ui_task.h
	$(CC) -c $(SDIR)ui_task.c -g

main.o: main.c main.h $(IDIR)logger_task.h
	$(CC) -c main.c -g
.PHONY: clean

clean:
	rm $(RFLAGS) *.o $(SDIR)*.o execute
