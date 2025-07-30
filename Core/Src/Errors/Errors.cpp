//
// Created by smuli on 01.07.2025.
//
#include "Errors.h"

#include "cmsis_gcc.h"

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
[[noreturn]] void Error_Handler() {
  __disable_irq();
  while (true) {
  }
}
