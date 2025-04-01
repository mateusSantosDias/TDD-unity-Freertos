#ifndef CONFIGURE_TEST_HPP
#define CONFIGURE_TEST_HPP

#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

void usart_setup(void);
void clock_setup();

#endif

void usart_setup(void)
{

    gpio_set_mode(
        GPIOA,
        GPIO_MODE_OUTPUT_50_MHZ,
        GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
        GPIO_USART1_TX);

    gpio_set_mode(

        GPIOA,
        GPIO_MODE_INPUT,
        GPIO_CNF_INPUT_FLOAT,
        GPIO_USART1_RX);

    usart_set_baudrate(USART1, 9600);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_mode(USART1, USART_MODE_TX_RX);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

    usart_enable(USART1);
}

void clock_setup()
{
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

    rcc_periph_clock_enable(RCC_USART1);
    rcc_periph_clock_enable(RCC_GPIOC);

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
}