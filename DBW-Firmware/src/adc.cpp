// #include "adc.h"
// #include <Arduino.h>

// static int pinToChannel(int pin) {
//     switch (pin) {
//         case A0: return 6;
//         case A1: return 7;
//         case A2: return 5;
//         case A3: return 4;
//         case A4: return 3;
//         default: return -1;
//     }
// }

// void adcInit() {
//     pmc_enable_periph_clk(ID_ADC);

//     ADC->ADC_CR = ADC_CR_SWRST;

//     ADC->ADC_MR =
//         ADC_MR_PRESCAL(1) |
//         ADC_MR_TRACKTIM(2) |
//         ADC_MR_TRANSFER(1);

//     ADC->ADC_CHER =
//         ADC_CHER_CH6 |  // A0
//         ADC_CHER_CH7 |  // A1
//         ADC_CHER_CH5 |   // A2
//         ADC_CHER_CH4 |   // A3
//         ADC_CHER_CH3;    // A4
// }

// uint16_t readADC(int pin) {
//     int ch = pinToChannel(pin);
//     if (ch < 0) return 0;

//     ADC->ADC_CR = ADC_CR_START;
//     while (!(ADC->ADC_ISR & (1 << ch)));

//     return ADC->ADC_CDR[ch];
// }
