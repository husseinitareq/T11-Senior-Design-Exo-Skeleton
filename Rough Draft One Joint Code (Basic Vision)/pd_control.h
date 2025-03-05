#ifndef PD_CONTROL_H
#define PD_CONTROL_H

#include "FreeRTOS.h"
#include "queue.h"
#include "drivers/can.h"

extern QueueHandle_t pdControlQueue;

void Task_PDControl(void *pvParameters);
void set_PD_setpoint(float new_setpoint);
void send_PD_output(float control_signal);

#endif // PD_CONTROL_H
