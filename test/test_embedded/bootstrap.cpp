#include <unity.h>

#include <stdio.h>
#include <unistd.h>
#include <errno.h>


#include "configure_test/configure_test.hpp"

#include "gpio_test/gpio_test.hpp"
#include "motor_test/motor_test.hpp"

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

void task_test(void *pvparameters)
{
    RUN_TEST(test_gpio_set_high);
}

int main(void)
{
    clock_setup();
    usart_setup();

    xCommandBus = xQueueCreate(MAX_COMMANDS_RECEIVE, MESSAGE_SIZE);
    xNotificationOperatorBus = xQueueCreate(MAX_NOTIFICATION_SEND, MESSAGE_SIZE);

    if (xCommandBus == NULL || xNotificationOperatorBus == NULL)
    {   
        printf("Failed\n");
        return -1;
    }

    vSemaphoreCreateBinary(xSemaphoreBinaryVariable);
    
    UNITY_BEGIN();
    xTaskCreate(task_test, "qualquer", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
    xTaskCreate(xOperator1, "Xop1", configMINIMAL_STACK_SIZE * 2, NULL, 1, &xHandleOperator1);
    xTaskCreate(xProcess1, "Xpr1", configMINIMAL_STACK_SIZE * 2, NULL, 1, &xHandleProcces1);
    xTaskCreate(xCommandManagerTask, "Xcm", configMINIMAL_STACK_SIZE * 2, NULL, 1, &xHandleCommandMan);

    vTaskStartScheduler();
    UNITY_END();
    return 0;
}