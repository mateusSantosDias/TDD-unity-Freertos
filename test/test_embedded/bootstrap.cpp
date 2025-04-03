
#define UNITY_SUPPORT_TEST_CASE
#define UNITY_SUPPORT_VARIADIC_MACROS
#include <unity.h>

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "FreeRTOS.h"

#include "configure_test/configure_test.hpp"

#include "gpio_test/gpio_test.hpp"
#include "motor_test/motor_test.hpp"

SemaphoreHandle_t xSemaphoreBinaryUnity = NULL;

extern "C"
{
    ssize_t _write(int file, const char *ptr, ssize_t len);
}

int _write(int file, const char *ptr, ssize_t len)
{
    int i;

    if (file == STDOUT_FILENO || file == STDERR_FILENO)
    {
        for (i = 0; i < len; i++)
        {
            if (ptr[i] == '\n')
            {
                usart_send_blocking(USART1, '\r');
            }
            usart_send_blocking(USART1, ptr[i]);
        }
        return i;
    }
    errno = EIO;
    return -1;
}

void vApplicationStackOverflowHook(
    TaskHandle_t xTask __attribute__((unused)),
    char *pcTaskName __attribute__((unused)))
{

    while (1)
        ;
}

void tearDown(void)
{
}

void setUp(void)
{
}

static void test_if_tasks_and_semaphores_are_created(void)
{
    xCommandBus = xQueueCreate(MAX_COMMANDS_RECEIVE, MESSAGE_SIZE);
    xNotificationOperatorBus = xQueueCreate(MAX_NOTIFICATION_SEND, MESSAGE_SIZE);

    if (xCommandBus == NULL || xNotificationOperatorBus == NULL)
    {
        TEST_FAIL_MESSAGE("Bus Not Created");
    }

    vSemaphoreCreateBinary(xSemaphoreBinaryVariable);

    TEST_ASSERT_EQUAL_MESSAGE(pdPASS, xTaskCreate(xOperator1, "Xop1", configMINIMAL_STACK_SIZE * 2, NULL, configMAX_PRIORITIES - 3, &xHandleOperator1),
                              "Task creation failed");
    TEST_ASSERT_EQUAL_MESSAGE(pdPASS, xTaskCreate(xProcess1, "Xpr1", configMINIMAL_STACK_SIZE * 2, NULL, configMAX_PRIORITIES - 3, &xHandleProcces1),
                              "Task creation failed");
    TEST_ASSERT_EQUAL_MESSAGE(pdPASS, xTaskCreate(xCommandManagerTask, "Xcm", configMINIMAL_STACK_SIZE * 2, NULL, configMAX_PRIORITIES - 1, &xHandleCommandMan),
                              "Task creation failed");
    TEST_ASSERT_EQUAL_MESSAGE(pdPASS, xTaskCreate(xProccess2, "xop2", configMINIMAL_STACK_SIZE * 2, NULL, configMAX_PRIORITIES - 3, &xHandleProcces2),
                              "Task Not created");
}

void vRunTestCase(void)
{
    xSemaphoreTake(xSemaphoreBinaryUnity, portMAX_DELAY);

    UNITY_BEGIN();
    RUN_TEST(test_if_tasks_and_semaphores_are_created);
   

    xSemaphoreGive(xSemaphoreBinaryUnity);
}

void prvTestRunnerTask(void *pvParameters)
{
    vRunTestCase();
    vTaskSuspend(NULL);
}

int main(void)
{
    clock_setup();
    usart_setup();

    vSemaphoreCreateBinary(xSemaphoreBinaryUnity);

    xTaskCreate(prvTestRunnerTask, "TestRunner1", configMINIMAL_STACK_SIZE * 3, NULL, configMAX_PRIORITIES, NULL);
    vTaskStartScheduler();
    while (1)
        ;
    return 0;

}