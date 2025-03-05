#include <stdlib.h>
#include <kernel/dpl/DebugP.h>
#include "ti_drivers_config.h"
#include "ti_board_config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "can_comm.h"
#include "pd_control.h"
#include "error_handler.h"

// Task Priorities
#define MAIN_TASK_PRI  (configMAX_PRIORITIES - 1)
#define CAN_TASK_PRI   (configMAX_PRIORITIES - 2)
#define PD_TASK_PRI    (configMAX_PRIORITIES - 3)
#define ERROR_TASK_PRI (configMAX_PRIORITIES - 1)  // Highest Priority

// Stack Sizes
#define MAIN_TASK_SIZE (16384U / sizeof(configSTACK_DEPTH_TYPE))
#define TASK_STACK_SIZE (1024U / sizeof(configSTACK_DEPTH_TYPE))

// Stack Memory
StackType_t gMainTaskStack[MAIN_TASK_SIZE] __attribute__((aligned(32)));
StaticTask_t gMainTaskObj;
TaskHandle_t gMainTask;

StackType_t gCanTaskStack[TASK_STACK_SIZE] __attribute__((aligned(32)));
StackType_t gPdTaskStack[TASK_STACK_SIZE] __attribute__((aligned(32)));
StackType_t gErrorTaskStack[TASK_STACK_SIZE] __attribute__((aligned(32)));

// Task Control Blocks
StaticTask_t gCanTaskObj;
StaticTask_t gPdTaskObj;
StaticTask_t gErrorTaskObj;

// Task Handles
TaskHandle_t gCanTask;
TaskHandle_t gPdTask;
TaskHandle_t gErrorTask;

// Task Function Prototypes
void Task_CANComm(void *args);
void Task_PDControl(void *args);
void Task_ErrorHandler(void *args);

void freertos_main(void *args) {
    // Create CAN Communication Task
    gCanTask = xTaskCreateStatic(Task_CANComm, "CANComm",
                                 TASK_STACK_SIZE, NULL,
                                 CAN_TASK_PRI, gCanTaskStack, &gCanTaskObj);
    
    // Create PD Control Task
    gPdTask = xTaskCreateStatic(Task_PDControl, "PDControl",
                                TASK_STACK_SIZE, NULL,
                                PD_TASK_PRI, gPdTaskStack, &gPdTaskObj);

    // Create Error Handling Task
    gErrorTask = xTaskCreateStatic(Task_ErrorHandler, "ErrorHandler",
                                   TASK_STACK_SIZE, NULL,
                                   ERROR_TASK_PRI, gErrorTaskStack, &gErrorTaskObj);

    // Delete the main setup task (it's no longer needed)
    vTaskDelete(NULL);
}

int main(void) {
    // Initialize system and board
    System_init();
    Board_init();

    // Create the main task which initializes other tasks
    gMainTask = xTaskCreateStatic(freertos_main, "freertos_main",
                                  MAIN_TASK_SIZE, NULL,
                                  MAIN_TASK_PRI, gMainTaskStack, &gMainTaskObj);
    configASSERT(gMainTask != NULL);

    // Start FreeRTOS Scheduler (Runs all created tasks)
    vTaskStartScheduler();

    // Should never reach here (unless an error occurs)
    DebugP_assertNoLog(0);
    return 0;
}
