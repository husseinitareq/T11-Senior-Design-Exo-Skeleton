#include "can_comm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "kernel/dpl/DebugP.h"

// Define Queue Handle for PD Control Task Communication
QueueHandle_t pdControlQueue;

void Task_CANComm(void *pvParameters) {
    CAN_Message msg;
    float received_position;

    // Create queue for PD control commands
    pdControlQueue = xQueueCreate(5, sizeof(float));

    while(1) {
        // Check for incoming messages
        if (CAN_receive(&msg)) {
            received_position = msg.data[1]; // Assume position data is in msg.data[1]

            // Send position data to PD Control Task
            xQueueSend(pdControlQueue, &received_position, portMAX_DELAY);
        }

        // (Optional) If PD Task processed control, transmit back
        send_motor_status();

        vTaskDelay(pdMS_TO_TICKS(10));  // Runs every 10ms
    }
}

// Process received CAN messages
void process_CAN_message(CAN_Message *msg) {
    uint16_t msgID = (msg->data[0] << 8) | msg->data[1];  // Extract Message ID
    uint8_t action = msg->data[2];
    uint8_t targetID = msg->data[3];
    uint8_t sourceID = msg->data[4];

    DebugP_log("Received CAN | ID: %d, Action: %d, Target: %d, Source: %d", msgID, action, targetID, sourceID);

    // Handle Emergency Messages
    if (msgID >= EMERGENCY_ID_MIN && msgID <= EMERGENCY_ID_MAX) {
        handle_emergency_message(msg);
    }
    // Process Normal Commands
    else {
        switch (action) {
            case 4:  // Position Control
                xQueueSend(pdControlQueue, &msg->data[5], portMAX_DELAY);
                break;
            case 5:  // Velocity Control
                set_motor_velocity(msg->data[5]);
                break;
            default:
                DebugP_log("Unknown Action: %d", action);
                break;
        }
    }
}

// Send a CAN message (e.g., Motor Status)
void send_motor_status() {
    CAN_Message msg;
    msg.id = 2;  // Node ID (Example: Right Hip)
    msg.data[0] = 3;  // Joint Angle Signal Name
    msg.data[1] = get_motor_position();
    msg.data[2] = 0;  // Placeholder for future data

    CAN_transmit(&msg);
    DebugP_log("Sent CAN | Node: %d, Angle: %d", msg.id, msg.data[1]);
}

// Handle Emergency Messages (e.g., Stop Motors)
void handle_emergency_message(CAN_Message *msg) {
    DebugP_log("Emergency Stop Triggered!");
    stop_motors();
}
