//    ch32-shared-memory - A minimal implementation of a shared memory on an SPI
//    bus
//    Copyright (C) 2026 Emilia Koziol

#include "ch32fun.h"
#include "ch32v003hw.h"

#define BUSREQ_PIN 1
#define BUS_GRANT_PIN 2

#define GPIO_CFG_CLR_MASK(pin) (0xF << ((pin) * 4))
#define GPIO_CFG_SET_MASK(pin, cfg) ((cfg) << ((pin) * 4))

void request_bus(void)
{
    // Enable negative edge
    EXTI->FTENR |= (1U << BUS_GRANT_PIN);

    // Clear interrupt flag (by setting 1)
    EXTI->INTFR = (1U << BUS_GRANT_PIN);

    // Enable the interrupt line
    EXTI->INTENR |= (1U << BUS_GRANT_PIN);

    // Request by pulling BUSREQ low
    GPIOC->OUTDR &= ~(1U << BUSREQ_PIN);
}

void EXTI7_0_IRQHandler(void) __attribute__((interrupt));
void EXTI7_0_IRQHandler(void)
{
    if (EXTI->INTFR & (1U << BUS_GRANT_PIN)) {

        // Clear interrupt flag
        EXTI->INTFR = (1U << BUS_GRANT_PIN);

        // Disable interrupt on BUS_GRANT
        EXTI->INTENR &= ~(1U << BUS_GRANT_PIN);

        // Execute transfer here
        Delay_Us(800);

        // Signal bus return
        GPIOC->OUTDR |= (1U << BUSREQ_PIN);
    }
}

int main()
{
    SystemInit();
    funGpioInitAll();

    RCC->APB2PCENR = RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO;

    GPIOC->OUTDR |= (1U << BUSREQ_PIN);

    GPIOC->CFGLR &= ~(GPIO_CFG_CLR_MASK(BUSREQ_PIN) | GPIO_CFG_CLR_MASK(BUS_GRANT_PIN));
    GPIOC->CFGLR |= GPIO_CFG_SET_MASK(BUSREQ_PIN, GPIO_CFGLR_OUT_10Mhz_OD);
    GPIOC->CFGLR |= GPIO_CFG_SET_MASK(BUS_GRANT_PIN, GPIO_CFGLR_IN_FLOAT);

    AFIO->EXTICR = AFIO_EXTICR_EXTI1_PC | AFIO_EXTICR_EXTI2_PC;

    NVIC_EnableIRQ(EXTI7_0_IRQn);

    while (1) {
        request_bus();
        Delay_Ms(200);
    }
}
