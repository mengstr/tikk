#include "ch32v003fun/ch32v003fun.h"
#include "mcuflash.h"
#include "ports.h"

//
//       CH32V00J4M6 SOP8
// +----------+      +-----------+   		Pin1 PA1/T1CH2  --> 	Clock output
// |           \____/            |   	    Pin2            VSS
// |                             |          Pin3 PA2        --> 	Button common
// | PD6/PA1    PD4/PD5/SWIO/PD1 |   		Pin4            VDD
// | VSS                     PC4 |   		Pin5 PC1        --> 	LED pair1
// | PA2                     PC2 |          Pin6 PC2	    -->     LED pair2
// | VDD                     PC1 |          Pin7 PC4        --> 	Reset output
// |                             |          Pin8 PD4        <-- 	LED common(SWIO)
// +-----------------------------+
//

int data0;
int data1;

// --------------------------------------------------------
// data0 bit1-0 = reset mode
//		00 (0) = /reset, pulsed active at power up
//		01 (1) = reset, pulsed active at power up
//		10 (2) = /reset, held active until button pressed
//		11 (3) = reset, held active until button pressed
// --------------------------------------------------------
// data0 bit7-2 = frequency

#define FREQUENCIES 16

typedef struct frequency_T {
    int prescaler;
    int divider;
} frequency_T;

frequency_T frequencies[FREQUENCIES] = {
    {749, 63999}, //   1 Hz
    {149, 63999}, //   5 Hz
    {74, 63999},  //  10 Hz
    {14, 63999},  //  50 Hz
    {7, 59999},   // 100 Hz
    {1, 47999},   // 500 Hz
    {0, 47999},   //   1 KHz
    {0, 9599},    //   5 KHz
    {0, 4799},    //  10 KHz
    {0, 959},     //  50 KHz
    {0, 479},     // 100 KHz
    {0, 95},      // 500 KHz
    {0, 47},      //   1 MHz
    {0, 23},      //   2 MHz
    {0, 11},      //   4 MHz
    {0, 5},       //   8 MHz
};

#define LEFT_BUTTON 1
#define RIGHT_BUTTON 2

#define RESETBIT 4

void LEDbits(int common, int pair1, int pair2) {
    SetPortD(4, common);
    SetPortC(1, pair1);
    SetPortC(2, pair2);
}

void LEDoff() { LEDbits(FLOAT, FLOAT, FLOAT); }
void LED1() { LEDbits(LOW, HIGH, FLOAT); }
void LED2() { LEDbits(HIGH, LOW, FLOAT); }
void LED3() { LEDbits(LOW, FLOAT, HIGH); }
void LED4() { LEDbits(HIGH, FLOAT, LOW); }

int GetButtons() {
    LEDoff();
    SetPortA(2, PU); // Button Common
    int button = 0;

    SetPortC(1, LOW);
    SetPortC(2, FLOAT);
    Delay_Us(1);
    if (!(GPIOA->INDR & (1 << 2))) button |= 1;

    SetPortC(1, FLOAT);
    SetPortC(2, LOW);
    Delay_Us(1);
    if (!(GPIOA->INDR & (1 << 2))) button |= 2;

    LEDoff();
    SetPortA(2, FLOAT); // Button Common
    return button;
}

int BlinkLEDs(int pattern1, int pattern2) {
    int buttons;
    const int delay = 2;
    const int loops = 10;

    for (int i = 0; i < loops; i++) {

        if (pattern1 & 1) LED1();
        Delay_Ms(delay);
        buttons = GetButtons();
        if (buttons) return buttons;

        if (pattern1 & 2) LED2();
        Delay_Ms(delay);
        buttons = GetButtons();
        if (buttons) return buttons;

        if (pattern1 & 4) LED3();
        Delay_Ms(delay);
        buttons = GetButtons();
        if (buttons) return buttons;

        if (pattern1 & 8) LED4();
        Delay_Ms(delay);
        buttons = GetButtons();
        if (buttons) return buttons;
    }

    for (int i = 0; i < loops; i++) {
        if (pattern2 & 1) LED1();
        Delay_Ms(delay);
        buttons = GetButtons();
        if (buttons) return buttons;

        if (pattern2 & 2) LED2();
        Delay_Ms(delay);
        buttons = GetButtons();
        if (buttons) return buttons;

        if (pattern2 & 4) LED3();
        Delay_Ms(delay);
        buttons = GetButtons();
        if (buttons) return buttons;

        if (pattern2 & 8) LED4();
        Delay_Ms(delay);
        buttons = GetButtons();
        if (buttons) return buttons;
    }

    return 0;
}

void setFrequencyFromTable(int rate) {
    TIM1->PSC = frequencies[rate].prescaler;
    TIM1->ATRLR = frequencies[rate].divider;
    TIM1->CH2CVR = frequencies[rate].divider / 2;
    TIM1->SWEVGR |= TIM_UG;
}

// resetMode 0 :  /reset, pulsed active at power up
// resetMode 1 :  reset, pulsed active at power up
// resetMode 2 :  /reset, held active until button pressed
// resetMode 3 :  reset, held active until button pressed
void ActiveReset(int resetMode) {
    SetPortC(4, resetMode == 0 || resetMode == 2 ? LOW : HIGH);
}

void IdleReset(int resetMode) {
    SetPortC(4, resetMode == 0 || resetMode == 2 ? HIGH : LOW);
}

void SetupReset() {
    int timeout = 0;
    int resetMode = data0 & 0x03;

    // while (GetButtons() & RIGHT_BUTTON) BlinkLEDs(0x0F, 0);

    for (;;) {
        while (GetButtons() & RIGHT_BUTTON) {}
        int buttons = BlinkLEDs(1 << resetMode, 0);
        if (buttons & RIGHT_BUTTON) {
            timeout = 0;
            resetMode++;
            if (resetMode > 3) resetMode = 0;
        }
        if (timeout++ > 30) break;
    }

    data0 = (data0 & 0xfc) | resetMode;
    FlashOptionData(data0, data1);
}

int main() {
    SystemInit();

    // Enable GPIOA, GPIOC, GPIOD and TIM1
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC |
                      RCC_APB2Periph_GPIOD | RCC_APB2Periph_TIM1;

    
    // Get and, if neccessary, initialise the stored settings
    data0 = OB->Data0;
    data1 = OB->Data1;
    if (data0 == 0xff || data1 == 0xff) {
        data0 = 0;
        data1 = 0;
        FlashOptionData(data0, data1);
    }


    int resetMode = data0 & 0x03;
    ActiveReset(resetMode);

    SetPortA(2, PU); // Button Common
    LEDoff();

    // PA1 is T1CH2, 50MHz Output alt func, push-pull
    GPIOA->CFGLR &= ~(0xf << (4 * 1));
    GPIOA->CFGLR |= (GPIO_Speed_50MHz | GPIO_CNF_OUT_PP_AF) << (4 * 1);

    RCC->APB2PRSTR |= RCC_APB2Periph_TIM1; // Reset TIM1 to init all regs
    RCC->APB2PRSTR &= ~RCC_APB2Periph_TIM1;
    TIM1->PSC = 65535;                 // Prescaler
    TIM1->ATRLR = 65535;               // Auto Reload - sets period
    TIM1->SWEVGR |= TIM_UG;            // Reload immediately
    TIM1->CCER |= TIM_CC2E | TIM_CC2P; // Enable CH2 output, positive pol
    TIM1->CHCTLR1 |=
        TIM_OC2M_2 |
        TIM_OC2M_1;         // CH2 Mode is output, PWM1 (CC1S = 00, OC1M = 110)
    TIM1->CH2CVR = 32768;   // Set the Capture Compare Register value to
                            // 50% initially
    TIM1->BDTR |= TIM_MOE;  // Enable TIM1 outputs
    TIM1->CTLR1 |= TIM_CEN; // Enable TIM1

    int rate = (data0 >> 2)&0x0f;
    setFrequencyFromTable(rate);
    // for (;;) {Delay_Ms(250);LED1();Delay_Ms(250);LED2();Delay_Ms(250);LED3();Delay_Ms(250);LED4();}

    if (GetButtons() & RIGHT_BUTTON) SetupReset();

    int inReset = 1;
    uint32_t sysTickEnd = SysTick->CNT + 6E6;
    for (;;) {

        if (inReset == 1) {
            int buttons = BlinkLEDs(5, 10);
            // Manual reset release mode
            if (resetMode >= 2) {
                if (buttons & RIGHT_BUTTON) {
                    IdleReset(resetMode);
                    Delay_Ms(100);
                    if (BlinkLEDs(0, 0) != 0) Delay_Ms(100);
                    inReset = 0;
                }
            } else {
                // Auto reset mode
                if (SysTick->CNT - sysTickEnd < 0x80000000UL) {
                    IdleReset(resetMode);
                    inReset = 0;
                }
            }
        }
        if (inReset == 2) {
            // reset button pressed mode
            BlinkLEDs(5, 10);
            if (SysTick->CNT - sysTickEnd < 0x80000000UL) {
                IdleReset(resetMode);
                inReset = 0;
            }
        }

        int buttons = GetButtons();

        if (buttons & RIGHT_BUTTON) {
            ActiveReset(resetMode);
            inReset = 2;
            sysTickEnd = SysTick->CNT + 6E6;
        }

        if (buttons & LEFT_BUTTON) {
            rate++;
            if (rate >= FREQUENCIES) rate = 0;
            setFrequencyFromTable(rate);
            data0=(data0&0x03)|(rate<<2);
            FlashOptionData(data0, data1);
            Delay_Ms(100);
            while (GetButtons()) {}
            Delay_Ms(100);
        }

        if (inReset == 0) BlinkLEDs(rate, rate);
    }
}
