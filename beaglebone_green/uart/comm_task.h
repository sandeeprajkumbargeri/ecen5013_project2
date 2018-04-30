#ifndef COMM_TASK_H_
#define COMM_TASK_H_

typedef struct receive_packet_buffer{
    uint8_t tiva_id;
    uint8_t command_id;
    uint8_t data;
}receive_packet_buffer;

typedef struct send_packet_buffer{
    uint8_t tiva_id;
    uint8_t route_id;
    uint8_t message[98];
}send_packet_buffer;

#define RECEIVE_PACKET_SIZE     sizeof(receive_packet_buffer)
#define SEND_PACKET_SIZE        sizeof(send_packet_buffer)

#endif
