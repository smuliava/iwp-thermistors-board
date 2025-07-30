//
// Created by smuli on 01.07.2025.
//
#include "Pwm.h"

#include <cmath>

#include "stm32g4xx_hal_tim.h"
#include "../Errors/Errors.h"

// Default PWM configuration: Frequency 500 Hz, Duty cycle 50%
#define DEFAULT_PWM_FREQUENCY_500_DC_50_PRESCALER 5u
#define DEFAULT_PWM_FREQUENCY_500_DC_50_ARR 55999u
#define DEFAULT_PWM_FREQUENCY_500_DC_50_CCR 28000u

using namespace std;

// Structure to hold calculated PWM parameters
struct PwmConfig {
  uint16_t prescaler; // PSC register value (actual divider is prescaler + 1)
  uint16_t arr;       // ARR register value (period is arr + 1)
  uint16_t ccr;       // CCR register value (pulse width)
  bool success;       // True if a valid configuration was found
  uint32_t actualFreq; // Calculated actual frequency
  float actualDuty;    // Calculated actual duty cycle
};

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim15;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;

static void MX_TIM1_Init(const uint32_t prescaler, const uint32_t arr, const uint32_t ccr)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = prescaler;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = arr;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim1);
}

static void MX_TIM2_Init(const uint32_t prescaler, const uint32_t arr, const uint32_t ccr)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = prescaler;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = arr;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim2);

}

static void MX_TIM3_Init(const uint32_t prescaler, const uint32_t arr, const uint32_t ccr)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = prescaler;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = arr;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim3);

}

static void MX_TIM4_Init(const uint32_t prescaler, const uint32_t arr, const uint32_t ccr)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = prescaler;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = arr;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim4);

}

static void MX_TIM8_Init(const uint32_t prescaler, const uint32_t arr, const uint32_t ccr)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  htim8.Instance = TIM8;
  htim8.Init.Prescaler = prescaler;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = arr;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim8);

}

static void MX_TIM15_Init(const uint32_t prescaler, const uint32_t arr, const uint32_t ccr)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  htim15.Instance = TIM15;
  htim15.Init.Prescaler = prescaler;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = arr;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim15, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim15);

}

static void MX_TIM16_Init(const uint32_t prescaler, const uint32_t arr, const uint32_t ccr)
{

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  htim16.Instance = TIM16;
  htim16.Init.Prescaler = prescaler;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = arr;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim16);

}

static void MX_TIM17_Init(const uint32_t prescaler, const uint32_t arr, const uint32_t ccr)
{

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  htim17.Instance = TIM17;
  htim17.Init.Prescaler = prescaler;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = arr;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim17, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim17, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim17);

}


static void StartPwmOut(TIM_HandleTypeDef *htim, const uint32_t Channel) {
  HAL_TIM_PWM_Start(htim, Channel);
  HAL_TIMEx_PWMN_Start(htim, Channel);
}

static void StopPwmOut(TIM_HandleTypeDef *htim, const uint32_t Channel) {
  HAL_TIM_PWM_Stop(htim, Channel);
  HAL_TIMEx_PWMN_Stop(htim, Channel);
}

static void PreInitPWMTimers() {
  MX_TIM1_Init(DEFAULT_PWM_FREQUENCY_500_DC_50_PRESCALER, DEFAULT_PWM_FREQUENCY_500_DC_50_ARR, DEFAULT_PWM_FREQUENCY_500_DC_50_CCR);
  MX_TIM2_Init(DEFAULT_PWM_FREQUENCY_500_DC_50_PRESCALER, DEFAULT_PWM_FREQUENCY_500_DC_50_ARR, DEFAULT_PWM_FREQUENCY_500_DC_50_CCR);
  MX_TIM3_Init(DEFAULT_PWM_FREQUENCY_500_DC_50_PRESCALER, DEFAULT_PWM_FREQUENCY_500_DC_50_ARR, DEFAULT_PWM_FREQUENCY_500_DC_50_CCR);
  MX_TIM4_Init(DEFAULT_PWM_FREQUENCY_500_DC_50_PRESCALER, DEFAULT_PWM_FREQUENCY_500_DC_50_ARR, DEFAULT_PWM_FREQUENCY_500_DC_50_CCR);
  MX_TIM8_Init(DEFAULT_PWM_FREQUENCY_500_DC_50_PRESCALER, DEFAULT_PWM_FREQUENCY_500_DC_50_ARR, DEFAULT_PWM_FREQUENCY_500_DC_50_CCR);
  MX_TIM15_Init(DEFAULT_PWM_FREQUENCY_500_DC_50_PRESCALER, DEFAULT_PWM_FREQUENCY_500_DC_50_ARR, DEFAULT_PWM_FREQUENCY_500_DC_50_CCR);
  MX_TIM16_Init(DEFAULT_PWM_FREQUENCY_500_DC_50_PRESCALER, DEFAULT_PWM_FREQUENCY_500_DC_50_ARR, DEFAULT_PWM_FREQUENCY_500_DC_50_CCR);
  MX_TIM17_Init(DEFAULT_PWM_FREQUENCY_500_DC_50_PRESCALER, DEFAULT_PWM_FREQUENCY_500_DC_50_ARR, DEFAULT_PWM_FREQUENCY_500_DC_50_CCR);
}

static PwmConfig calculatePwmConfig(const uint32_t pwmFreq, const float dutyCyclePercent) {
  PwmConfig config = {0, 0, 0, false, 0, 0.0f};
  uint32_t prescalerVal;

  if (pwmFreq >= 3000) {
    prescalerVal = 0;
  } else if (pwmFreq >= 1300) {
    prescalerVal = 1;
  } else if (pwmFreq >= 860) {
    prescalerVal = 2;
  } else {
    float minPrescalerPlus1Float = (1.0f * F_CLK_TIM_INPUT) / (static_cast<float>(pwmFreq) * 65536.0f);
    prescalerVal = static_cast<uint32_t>(ceil(minPrescalerPlus1Float)) - 1;
  }

  const float periodTotalTicksFloat = (1.0f * F_CLK_TIM_INPUT) / (static_cast<float>(prescalerVal + 1u) * static_cast<float>(pwmFreq));
  const uint32_t calculatedArrTemp = static_cast<uint32_t>(round(periodTotalTicksFloat)) - 1;

  config.prescaler = static_cast<uint16_t>(prescalerVal);
  config.arr = static_cast<uint16_t>(calculatedArrTemp);

  config.ccr = static_cast<uint16_t>(round((dutyCyclePercent / 100.0f) * static_cast<float>(config.arr + 1)));

  if (config.ccr > (config.arr + 1)) {
    config.ccr = config.arr + 1;
  }

  config.actualFreq = F_CLK_TIM_INPUT / ((config.prescaler + 1) * (config.arr + 1));
  config.actualDuty = (static_cast<float>(config.ccr) / static_cast<float>(config.arr + 1)) * 100.0f;

  config.success = true;
  return config;
}

static void InitPWMOut(TIM_HandleTypeDef *htim, const uint32_t Channel, const uint32_t frequency, const float dutyCycle) {
  if (htim->Instance != nullptr) {
    const PwmConfig pwmCfg = calculatePwmConfig(frequency, dutyCycle);

    __HAL_TIM_SET_PRESCALER(htim, pwmCfg.prescaler - 1);
    __HAL_TIM_SET_AUTORELOAD(htim, pwmCfg.arr - 1);
    __HAL_TIM_SET_COMPARE(htim, Channel, pwmCfg.ccr);

    htim->Instance->EGR |= TIM_EGR_UG;
  }
}

static void InitPWMOut0(const uint32_t frequency, const float dutyCycle) {
  InitPWMOut(&htim1, TIM_CHANNEL_1, frequency, dutyCycle);
}

static void InitPWMOut1(const uint32_t frequency, const float dutyCycle) {
  InitPWMOut(&htim2, TIM_CHANNEL_1, frequency, dutyCycle);
}

static void InitPWMOut2(const uint32_t frequency, const float dutyCycle) {
  InitPWMOut(&htim3, TIM_CHANNEL_1, frequency, dutyCycle);
}

static void InitPWMOut3(const uint32_t frequency, const float dutyCycle) {
  InitPWMOut(&htim4, TIM_CHANNEL_1, frequency, dutyCycle);
}

static void InitPWMOut4(const uint32_t frequency, const float dutyCycle) {
  InitPWMOut(&htim8, TIM_CHANNEL_1, frequency, dutyCycle);
}

static void InitPWMOut5(const uint32_t frequency, const float dutyCycle) {
  InitPWMOut(&htim16, TIM_CHANNEL_1, frequency, dutyCycle);
}

static void InitPWMOut6(const uint32_t frequency, const float dutyCycle) {
  InitPWMOut(&htim16, TIM_CHANNEL_1, frequency, dutyCycle);
}

static void InitPWMOut7(const uint32_t frequency, const float dutyCycle) {
  InitPWMOut(&htim17, TIM_CHANNEL_1, frequency, dutyCycle);
}

