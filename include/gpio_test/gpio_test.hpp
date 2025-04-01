#ifndef GPIO_TEST_HPP
#define GPIO_TEST_HPP

#include "libopencm3/stm32/gpio.h"
#include <unity.h>

void gpio_setHigh(uint32_t GPIO_portx, uint16_t GPIO_pinx);

void test_gpio_set_high(void)
{
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    gpio_setHigh(GPIOC, GPIO13);

    TEST_ASSERT_EQUAL(0x01, gpio_get(GPIOC, GPIO13) >> 13);
}

#endif

void gpio_setHigh(uint32_t GPIO_portx, uint16_t GPIO_pinx)
{
    gpio_set(GPIO_portx, GPIO_pinx);
}