//    ch32-shared-memory - A minimal implementation of a shared memory on an SPI
//    bus
//    Copyright (C) 2026 Emilia Koziol

#include "ch32fun.h"
#include "ch32v003hw.h"

#define BUSREQ_PIN 1
#define BUS_GRANT_PIN 2

#define GPIO_CFG_CLR_MASK(pin)      (0xF << ((pin) * 4))
#define GPIO_CFG_SET_MASK(pin, cfg) ((cfg)<< ((pin) * 4))

int main()
{
    SystemInit();
    funGpioInitAll();

    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;

    GPIOC->CFGLR &= ~(GPIO_CFG_CLR_MASK(BUSREQ_PIN) | GPIO_CFG_CLR_MASK(BUS_GRANT_PIN));

    GPIOC->CFGLR |= GPIO_CFG_SET_MASK(BUSREQ_PIN, GPIO_CFGLR_OUT_10Mhz_OD);
    GPIOC->CFGLR |= GPIO_CFG_SET_MASK(BUS_GRANT_PIN, GPIO_CFGLR_IN_FLOAT);

    while (1) {
    }
}
