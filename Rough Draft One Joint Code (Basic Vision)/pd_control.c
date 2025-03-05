#include "pd_control.h"
#include "motor_driver.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "kernel/dpl/DebugP.h"

extern QueueHandle_t pdControlQueue;  // Access queue from CAN task

// Define PD gains
#define KP  1.5   // Proportional gain
#define KD  0.05  // Derivative gain

// Sampling time (1ms for real-time control)
#define CONTROL_PERIOD_MS 1

// Motor control parameters
static float last_error = 0;

void Task_PDControl(void *pvParameters) {
    float current_position, desired_position, error, derivative, control_signal;

    while(1) {
        // Wait for new position data from CAN Task
        if (xQueueReceive(pdControlQueue, &desired_position, portMAX_DELAY)) {
            current_position = get_motor_position();

            // Compute PD control
            error = desired_position - current_position;
            derivative = (error - last_error) / (CONTROL_PERIOD_MS / 1000.0);
            control_signal = (KP * error) + (KD * derivative);

            last_error = error;

            // Send control signal back via CAN
            send_PD_output(control_signal);
        }

        vTaskDelay(pdMS_TO_TICKS(CONTROL_PERIOD_MS));  // Runs every 1ms
    }
}

// Function to send PD output back to CAN Task
void send_PD_output(float control_signal) {
    CAN_Message msg;
    msg.id = 2;  // Example node ID
    msg.data[0] = 6;  // PD Output Signal Name
    msg.data[1] = (uint8_t)control_signal;  // Convert to appropriate CAN data format
    CAN_transmit(&msg);
}