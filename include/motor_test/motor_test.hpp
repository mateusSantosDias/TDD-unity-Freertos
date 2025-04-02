#ifndef MOTOR_TEST_HPP
#define MOTOR_TEST_HPP

#include <unity.h>

#include <motor_test/motor.hpp>

void xOperator1(void *pvparameters);
void xProcess1(void *pvparameters);

void test_xoperator_to_xcomand(void);
void test_xprocess1_to_xcommand(void);
#endif

void xOperator1(void *pvparameters)
{
    xSemaphoreTake(xSemaphoreBinaryVariable, portMAX_DELAY);
    Operator1.set_status(FREE);
    Operator1.set_task_id(0x01);
    xSemaphoreGive(xSemaphoreBinaryVariable);

    vTaskSuspend(NULL);
    
    RUN_TEST(test_xoperator_to_xcomand);
}

void xProcess1(void *pvparameters)
{
    xSemaphoreTake(xSemaphoreBinaryVariable, portMAX_DELAY);
    Procces1.set_status(FREE);
    Procces1.set_task_id(0x02);
    Procces1.set_command(COMMAND_1);
    xSemaphoreGive(xSemaphoreBinaryVariable);

    RUN_TEST(test_xprocess1_to_xcommand);
}

void test_xprocess1_to_xcommand(void)
{
    TEST_ASSERT_EQUAL(pdPASS, xQueueSend(xCommandBus, (void *)&Procces1, portMAX_DELAY));
    TEST_ASSERT_EQUAL(pdTRUE, ulTaskNotifyTake(pdTRUE, portMAX_DELAY));
}

void test_xoperator_to_xcomand(void)
{
    TEST_ASSERT_EQUAL(pdPASS, xQueueSend(xNotificationOperatorBus, (void *)NULL, portMAX_DELAY));
}