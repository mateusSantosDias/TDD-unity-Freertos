#ifndef MOTOR_HPP
#define MOTOR_HPP

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue>


#include "task_class.hpp"

#define MAX_COMMANDS_RECEIVE 8
#define MAX_NOTIFICATION_SEND 8

#define MESSAGE_SIZE 10 * sizeof(uint8_t)

QueueHandle_t xCommandBus;
QueueHandle_t xNotificationOperatorBus;

TaskHandle_t xHandleProcces1;
TaskHandle_t xHandleOperator1;
TaskHandle_t xHandleCommandMan;

SemaphoreHandle_t xSemaphoreBinaryVariable = NULL;

TaskClass Procces1;

TaskClass Operator1;

void xCommandManagerTask(void *pvParameters);

void verify_notification_end(TaskClass buffer_task,
                             QueueHandle_t bus,
                             TickType_t delay,
                             uint8_t procces_count);

void consumition_off_buffer_tmp(TaskClass *buffer_tasks_tmp,
                                TaskClass *buffer_procces_task,
                                uint8_t buffer_proccess_count,
                                uint8_t buffer_tmp_count);

void verify_notification_end(TaskClass *buffer_task,
                             QueueHandle_t bus,
                             TickType_t delay,
                             uint8_t buffer_proccess_count);

void consumition_off_buffer_tmp(TaskClass *buffer_tasks_tmp,
                                TaskClass *buffer_procces_task,
                                uint8_t processCount,
                                uint8_t buffer_tmp_count);
#endif

void verify_notification_end(TaskClass *buffer_task,
                             QueueHandle_t bus,
                             TickType_t delay,
                             uint8_t buffer_proccess_count)
{

    task_status flag_status_aux = FREE;

    if (xQueueReceive(bus, NULL, delay) == pdPASS)
    {
        xSemaphoreTake(xSemaphoreBinaryVariable, portMAX_DELAY);

        /***********COMMAND1 VERIFY***********************************************/
        if (buffer_task[buffer_proccess_count].get_command() == COMMAND_1 && Operator1.get_status() == FREE)
        {
            flag_status_aux = FREE;
        }
        else if (buffer_task[buffer_proccess_count].get_command() == COMMAND_1)
        {
            flag_status_aux = BUSY;
        }
        /************************************************************************/

        /***********TASK VERIFY ON PROCESSOR1************************************/
        if (Procces1.get_task_id() == buffer_task[buffer_proccess_count].get_task_id() && flag_status_aux == FREE)
        {
            xTaskNotifyGive(xHandleProcces1);

            buffer_proccess_count++;
        }
        /**************************************************************************/

        if (buffer_proccess_count >= sizeof(buffer_task))
            buffer_proccess_count = 0x00;

        xSemaphoreGive(xSemaphoreBinaryVariable);
    }
}

void consumition_off_buffer_tmp(TaskClass *buffer_tasks_tmp,
                                TaskClass *buffer_procces_task,
                                uint8_t processCount,
                                uint8_t buffer_tmp_count)
{

    xSemaphoreTake(xSemaphoreBinaryVariable, portMAX_DELAY);

    /***********COMMAND1 REGISTER**********************************************/
    if (buffer_tasks_tmp[buffer_tmp_count].get_command() == COMMAND_1 && Operator1.get_status() == FREE)
    {
        buffer_procces_task[processCount] = buffer_tasks_tmp[buffer_tmp_count];

        vTaskResume(xHandleOperator1);
    }
    /*************************************************************************/

    if (buffer_tmp_count >= sizeof(buffer_tasks_tmp))
        buffer_tmp_count = 0x00;

    xSemaphoreGive(xSemaphoreBinaryVariable);
}

void xCommandManagerTask(void *pvParameters)
{

    task_status flag_status_aux;

    uint8_t count = 0x00;
    uint8_t tmp_count = 0x00;

    TaskClass tasks[8];
    TaskClass buffer_task[8];
    TaskClass buffer_task_tmp[8];

    uint8_t i = 0x00;

    while (1)
    {

        verify_notification_end(buffer_task, xNotificationOperatorBus, pdMS_TO_TICKS(500), count);

        consumition_off_buffer_tmp(buffer_task_tmp, buffer_task, count, tmp_count);

        if (xQueueReceive(xCommandBus, &tasks, pdMS_TO_TICKS(100)) == pdPASS)
        {

            xSemaphoreTake(xSemaphoreBinaryVariable, portMAX_DELAY);

            /***********COMMAND1 REGISTER**********************************************/
            if (tasks[i].get_command() == COMMAND_1 && Operator1.get_status() == FREE)
            {

                buffer_task[i] = tasks[i];

                vTaskResume(xHandleOperator1);
            }
            else if (tasks[i].get_command() == COMMAND_1)
            {
                buffer_task_tmp[i] = tasks[i];
            }
            /**************************************************************************/

            xSemaphoreGive(xSemaphoreBinaryVariable);

            i++;

            if (i >= sizeof(tasks))
                i = 0x00;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
/***********************************************************/