//
// Created by smuli on 01.07.2025.
//

#ifndef PWM_H
#define PWM_H

#define  F_CLK_TIM_INPUT 168000000UL

#include <string>

#include "stm32g4xx_hal_tim.h"

using namespace std;

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim);
static void PreInitPWMTimers();

// class Pwm {
// public:
//   Pwm(uint32_t fanNumber_F, string pinName, uint32_t frequency_Q);
//   void setDutyCycle(float dutyCycle);
//   void setDutyCycle(uint32_t dutyCycle);
//   void setFrequency(float frequency);
//   void setDutyCycleAndFrequency(float dutyCycle, float frequency);
// };

#endif //PWM_H
