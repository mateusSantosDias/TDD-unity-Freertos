#ifndef MOTOR_TEST_HPP
#define MOTOR_TEST_HPP

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue>

#define MAX_COMMANDS_RECEIVE 8
#define MAX_NOTIFICATION_SEND 8

#define MESSAGE_SIZE 10 * sizeof(uint8_t)

typedef enum
{
    FREE,
    BUSY,
    PROCCESS_TERMINATED
} task_status;

typedef enum
{
    COMMAND_1,
    COMMAND_2
} commands;

typedef enum
{
    task_procces_1,
    task_procces_2,
    task_operator_1,
    task_operator_2
} task_ids;

QueueHandle_t xCommandBus;
QueueHandle_t xNotificationOperatorBus;

TaskHandle_t  xHandleProcces1;
TaskHandle_t  xHandleOperator1;


task_status global_control_task_op_1;

void xCommandManagerTask(void *pvParameters);
void verify_notification_end(uint8_t *bufferNotification, uint8_t *bufferProcess, QueueHandle_t bus, TickType_t delay);
void consumition_off_buffer_tmp(uint8_t *bufferTmp, uint8_t *bufferProcess, uint8_t processCount);

#endif



void verify_notification_end(uint8_t *bufferNotification, uint8_t *bufferProcess, QueueHandle_t bus, TickType_t delay)
{
    uint8_t buffer_proccess_count = 0x00;

    task_status flag_status_aux;

    if (xQueueReceive(bus, &bufferNotification, delay) == pdPASS)
    {

        switch (bufferProcess[buffer_proccess_count])
        {

        /***********TASK REGISTER ON PROCESSOR************************************/
        case task_operator_1:
            if (global_control_task_op_1 == FREE)
            {
                flag_status_aux = FREE;
            }
            break;

        default:
            break;
        }
        /************************************************************************/

        switch (bufferProcess[buffer_proccess_count + 1])
        {

        case task_procces_1:
            if (flag_status_aux == FREE)
            {
                xTaskNotifyGive(xHandleProcces1);
                buffer_proccess_count += 2;
            }
            break;

        default:
            break;
        }

        if (buffer_proccess_count >= sizeof(bufferProcess))
        {
            buffer_proccess_count = 0x00;
        }
    }
}



void consumition_off_buffer_tmp(uint8_t *bufferTmp, uint8_t *bufferProcess, uint8_t processCount)
{
    uint8_t buffer_tmp_count = 0x00;
    task_status flag_status_aux;

    if (bufferTmp[buffer_tmp_count] != 0x00)
    {

        switch (bufferTmp[buffer_tmp_count])
        {

        /***********COMMAND REGISTER ON PROCESSOR***********************************/
        case task_operator_1:
            if (global_control_task_op_1 == FREE)
            {
                flag_status_aux = FREE;

                bufferTmp[buffer_tmp_count] = 0x00;
                vTaskResume(xHandleOperator1);
                bufferProcess[processCount] = task_operator_1;
            }
            break;

        default:
            break;
        }

        /**************************************************************************/

        switch (bufferTmp[buffer_tmp_count + 1])
        {

        /***********TASK REGISTER ON PROCESSOR************************************/
        case task_procces_1:

            if (flag_status_aux == FREE)
            {
                bufferProcess[processCount + 1] = task_procces_1;
                bufferTmp[buffer_tmp_count + 1] = 0x00;
                buffer_tmp_count += 2;
            }

            break;

        default:
            break;
        }

        if (buffer_tmp_count >= sizeof(bufferProcess))
        {
            buffer_tmp_count = 0x00;
        }
        /**************************************************************************/
    }
}





void xCommandManagerTask(void *pvParameters)
{
    uint8_t command_buffer[MAX_COMMANDS_RECEIVE] = {0};
    uint8_t command_buffer_tmp[MAX_COMMANDS_RECEIVE] = {0};

    uint8_t buffer_proccess[MAX_COMMANDS_RECEIVE] = {0};
    uint8_t buffer_proccess_tmp[MAX_COMMANDS_RECEIVE] = {0};

    uint8_t buffer_notification[MAX_NOTIFICATION_SEND] = {0};

    uint8_t buffer_proccess_count = 0x00;
    uint8_t buffer_tmp_count = 0x00;

    task_status flag_status_aux;

    while (1)
    {

        verify_notification_end(buffer_notification, buffer_proccess, xNotificationOperatorBus, pdMS_TO_TICKS(500));
        consumition_off_buffer_tmp(buffer_proccess_tmp, buffer_proccess, buffer_proccess_count);

        if (xQueueReceive(xCommandBus, &command_buffer, pdMS_TO_TICKS(100)) == pdPASS)
        {
            for (uint8_t i = 0, j; i < sizeof(command_buffer); i++)
            {
                switch (command_buffer[i])
                {

                /***********COMMAND REGISTER ON PROCESSOR***********************************/
                case COMMAND_1:
                    if (global_control_task_op_1 == FREE)
                    {
                        flag_status_aux = FREE;

                        buffer_proccess[j] = task_operator_1;
                        vTaskResume(xHandleOperator1);
                    }
                    else
                    {
                        flag_status_aux = BUSY;

                        command_buffer_tmp[buffer_tmp_count] = command_buffer[i];
                        command_buffer_tmp[buffer_tmp_count + 1] = command_buffer[i + 1];
                    }
                    break;

                default:
                    break;
                }
                /*****************************************************************/

                i++;

                switch (command_buffer[i])
                {

                /***********TASK REGISTER ON PROCESSOR************************************/
                case task_procces_1:
                    if (flag_status_aux == FREE)
                    {

                        buffer_proccess[j + 1] = task_procces_1;
                        j += 2;
                    }
                    break;

                default:
                    break;
                }
                if (j >= 8)
                    j = 0x00;
                /**************************************************************************/
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
/***********************************************************/