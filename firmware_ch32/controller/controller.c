//    ch32-shared-memory - A minimal implementation of a shared memory on an SPI
//    bus
//    Copyright (C) 2026 Emilia Koziol

#include "ch32fun.h"
#include "ch32v003hw.h"

#define BUSREQ_PIN 1
#define BUS_GRANT_PIN 2

#define GPIO_CFG_CLR_MASK(pin) (0xF << ((pin) * 4))
#define GPIO_CFG_SET_MASK(pin, cfg) ((cfg) << ((pin) * 4))

void EXTI7_0_IRQHandler(void) __attribute__((interrupt));
void EXTI7_0_IRQHandler(void)
{
    if (EXTI->INTFR & (1U << BUSREQ_PIN)) {
        if (EXTI->FTENR & (1U << BUSREQ_PIN)) {
            // State A: Worker requested access (~{BUSREQ} fell down)
            // Migrate to rising edge detection to guarantee no events are lost
            EXTI->FTENR &= ~(1U << BUSREQ_PIN);
            EXTI->RTENR |= (1U << BUSREQ_PIN);

            // Grant the bus
            GPIOC->OUTDR &= ~(1U << BUS_GRANT_PIN);
        } else {
            // State B: Worker released access (~{BUSREQ} went up)
            // Migrate back to falling edge detection
            EXTI->FTENR |= (1U << BUSREQ_PIN);
            EXTI->RTENR &= ~(1U << BUSREQ_PIN);

            // Revoke grant
            GPIOC->OUTDR |= (1U << BUS_GRANT_PIN);
        }

        // Clear interrupt flag
        EXTI->INTFR = (1U << BUSREQ_PIN);
    }
}

int main()
{
    SystemInit();
    funGpioInitAll();

    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO;

    GPIOC->OUTDR |= (1U << BUS_GRANT_PIN);

    GPIOC->CFGLR &= ~(GPIO_CFG_CLR_MASK(BUSREQ_PIN) | GPIO_CFG_CLR_MASK(BUS_GRANT_PIN));

    GPIOC->CFGLR |= GPIO_CFG_SET_MASK(BUSREQ_PIN, GPIO_CFGLR_IN_FLOAT);
    GPIOC->CFGLR |= GPIO_CFG_SET_MASK(BUS_GRANT_PIN, GPIO_CFGLR_OUT_10Mhz_OD);

    AFIO->EXTICR = AFIO_EXTICR_EXTI1_PC | AFIO_EXTICR_EXTI2_PC;

    NVIC_EnableIRQ(EXTI7_0_IRQn);

    EXTI->FTENR |= (1U << BUSREQ_PIN);
    EXTI->INTENR |= (1U << BUSREQ_PIN);

    while (1) {
        Delay_Ms(600);
    }
}
