#ifndef MOTOR_TEST_HPP
#define MOTOR_TEST_HPP

#include <unity.h>

#include <motor_test/motor.hpp>

extern SemaphoreHandle_t xSemaphoreBinaryUnity;

void xOperator1(void *pvparameters);
void xProcess1(void *pvparameters);
void xProccess2(void *pvParameters);

void test_xoperator_to_xcomand(void);
void test_xprocess_to_xcommand(void);
#endif

void test_xprocess1_to_xcommand(void)
{
    TEST_ASSERT_EQUAL_MESSAGE(pdPASS, xQueueSend(xCommandBus, (void *)&Procces1, pdMS_TO_TICKS(100)),
                              "Message from Process task to Command Manager not indeed");
}

void test_xprocess2_to_xcommand(void)
{
    TEST_ASSERT_EQUAL_MESSAGE(pdPASS, xQueueSend(xCommandBus, (void *)&Procces2, pdMS_TO_TICKS(100)),
                              "Message from Process task to Command Manager not indeed");
}

void test_xcommand_to_xproccess1(void)
{
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, Procces1.get_status(),
                              "Message from Command Manager not received from Proccess Task");
}

void test_xcommand_to_xproccess2(void)
{
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, Procces2.get_status(),
                              "Message from Command Manager not received from Proccess Task");
}

void test_xoperator_to_xcomand(void)
{
    TEST_ASSERT_EQUAL_MESSAGE(PROCCESS_TERMINATED, Operator1.get_status(),
                              "The state of operator task not changed");

    TEST_ASSERT_EQUAL_MESSAGE(pdPASS, xQueueSend(xNotificationOperatorBus, (void *)NULL, pdMS_TO_TICKS(100)),
                              "Message from operator task to Command Manager not indeed");
}

void xOperator1(void *pvparameters)
{
    xSemaphoreTake(xSemaphoreBinaryVariable, portMAX_DELAY);
    Operator1.set_status(FREE);
    Operator1.set_task_id(0x01);
    xSemaphoreGive(xSemaphoreBinaryVariable);

    while (1)
    {

        xSemaphoreTake(xSemaphoreBinaryUnity, portMAX_DELAY);

        xSemaphoreTake(xSemaphoreBinaryVariable, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(150));

        Operator1.set_status(PROCCESS_TERMINATED);

        xSemaphoreGive(xSemaphoreBinaryVariable);

        UNITY_BEGIN();
        RUN_TEST(test_xoperator_to_xcomand);

        xSemaphoreGive(xSemaphoreBinaryUnity);

        vTaskSuspend(NULL);
    }
}

void xProcess1(void *pvparameters)
{
    xSemaphoreTake(xSemaphoreBinaryVariable, portMAX_DELAY);
    Procces1.set_status(FREE);
    Procces1.set_task_id(0x02);
    Procces1.set_command(COMMAND_1);
    xSemaphoreGive(xSemaphoreBinaryVariable);

    xSemaphoreTake(xSemaphoreBinaryUnity, portMAX_DELAY);

    UNITY_BEGIN();
    RUN_TEST(test_xprocess1_to_xcommand);

    xSemaphoreGive(xSemaphoreBinaryUnity);

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    xSemaphoreTake(xSemaphoreBinaryUnity, portMAX_DELAY);

    UNITY_BEGIN();
    RUN_TEST(test_xcommand_to_xproccess1);

    xSemaphoreGive(xSemaphoreBinaryUnity);

    vTaskSuspend(NULL);
}

void xProccess2(void *pvParameters)
{
    xSemaphoreTake(xSemaphoreBinaryVariable, portMAX_DELAY);
    Procces2.set_status(FREE);
    Procces2.set_task_id(0x03);
    Procces2.set_command(COMMAND_1);
    xSemaphoreGive(xSemaphoreBinaryVariable);

    xSemaphoreTake(xSemaphoreBinaryUnity, portMAX_DELAY);

    UNITY_BEGIN();
    RUN_TEST(test_xprocess2_to_xcommand);

    xSemaphoreGive(xSemaphoreBinaryUnity);

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    xSemaphoreTake(xSemaphoreBinaryUnity, portMAX_DELAY);

    UNITY_BEGIN();
    RUN_TEST(test_xcommand_to_xproccess2);
    UNITY_END();
    xSemaphoreGive(xSemaphoreBinaryUnity);

    vTaskSuspend(NULL);
}