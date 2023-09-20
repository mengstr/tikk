#include "ch32v003fun/ch32v003fun.h"
#include "ports.h"

void SetPortA(int bitno, int mode) {
    GPIOA->CFGLR &= ~(0xf << (4 * bitno));
    switch (mode) {
    case LOW:
        GPIOA->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * bitno);
        GPIOA->BSHR = (1 << (16 + bitno));
        break;
    case HIGH:
        GPIOA->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * bitno);
        GPIOA->BSHR = (1 << bitno);
        break;
    case FLOAT:
        GPIOA->CFGLR |= (GPIO_CNF_IN_FLOATING) << (4 * bitno);
        GPIOA->BSHR = (1 << bitno);
        break;
    case PU:
        GPIOA->CFGLR |= (GPIO_CNF_IN_PUPD) << (4 * bitno);
        GPIOA->OUTDR |= (1 << bitno);
        break;
    }
}

void SetPortC(int bitno, int mode) {
    GPIOC->CFGLR &= ~(0xf << (4 * bitno));
    switch (mode) {
    case LOW:
        GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * bitno);
        GPIOC->BSHR = (1 << (16 + bitno));
        break;
    case HIGH:
        GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * bitno);
        GPIOC->BSHR = (1 << bitno);
        break;
    case FLOAT:
        GPIOC->CFGLR |= (GPIO_CNF_IN_FLOATING) << (4 * bitno);
        GPIOC->BSHR = (1 << bitno);
        break;
    case PU:
        GPIOC->CFGLR |= (GPIO_CNF_IN_PUPD) << (4 * bitno);
        GPIOC->OUTDR |= (1 << bitno);
        break;
    }
}

void SetPortD(int bitno, int mode) {
    GPIOD->CFGLR &= ~(0xf << (4 * bitno));
    switch (mode) {
    case LOW:
        GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * bitno);
        GPIOD->BSHR = (1 << (16 + bitno));
        break;
    case HIGH:
        GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * bitno);
        GPIOD->BSHR = (1 << bitno);
        break;
    case FLOAT:
        GPIOD->CFGLR |= (GPIO_CNF_IN_FLOATING) << (4 * bitno);
        GPIOD->BSHR = (1 << bitno);
        break;
    case PU:
        GPIOD->CFGLR |= (GPIO_CNF_IN_PUPD) << (4 * bitno);
        GPIOD->OUTDR |= (1 << bitno);
        break;
    }
}

