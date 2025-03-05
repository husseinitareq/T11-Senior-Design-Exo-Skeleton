#include "error_handler.h"
#include "motor_driver.h"
#include "FreeRTOS.h"
#include "task.h"
#include "kernel/dpl/DebugP.h"

#define CURRENT_THRESHOLD 10.0  // Example threshold, change as needed THIS ERROR HANDLE NEEDS TO COMMUNICATE WITH THE CAN TASK WHICH IT DOESNT

void Task_ErrorHandler(void *pvParameters) {
    while(1) {
        float motor_current = get_motor_current();  // Function to read motor current

        if (motor_current > CURRENT_THRESHOLD) {
            DebugP_log("ERROR: Current Spike Detected! Stopping Motors.");
            stop_motors();
            while(1);  // Halt system
        }

        vTaskDelay(pdMS_TO_TICKS(5));  // Check every 5ms
    }
}
