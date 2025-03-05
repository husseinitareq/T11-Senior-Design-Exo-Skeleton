#ifndef CAN_COMM_H
#define CAN_COMM_H

#include "drivers/can.h"
#include "FreeRTOS.h"
#include "queue.h"

// Define Queue Handle for PD Control Task Communication
extern QueueHandle_t pdControlQueue;

void Task_CANComm(void *pvParameters);
void process_CAN_message(CAN_Message *msg);
void send_motor_status();
void handle_emergency_message(CAN_Message *msg);

#endif // CAN_COMM_H