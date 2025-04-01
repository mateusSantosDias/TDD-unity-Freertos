#include <unity.h>

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <FreeRTOS.h>
#include <task.h>

#include "configure_test/configure_test.hpp"

#include "gpio_test/gpio_test.hpp"

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

void task_test(void* pvparameters)
{
    UNITY_BEGIN();
    RUN_TEST(test_gpio_set_high);
    UNITY_END();
}

int main(void)
{   
    clock_setup();
    usart_setup();

    xTaskCreate(task_test, "qualquer", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    vTaskStartScheduler();

    return 0;
}