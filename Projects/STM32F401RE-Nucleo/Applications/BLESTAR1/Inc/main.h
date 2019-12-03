  /**
  ******************************************************************************
  * @file    main.h
  * @author  Central LAB
  * @version V1.0.0
  * @date    17-May-2015
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#ifdef USE_STM32L0XX_NUCLEO
#include "stm32l0xx_hal.h"
#include "stm32l0xx_nucleo.h"
#endif
#ifdef USE_STM32F1xx_NUCLEO
#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"
#endif
#ifdef USE_STM32F4XX_NUCLEO
#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo.h"
#endif  

/** @addtogroup Projects
 *  @{
 */
 
/** @addtogroup STM32F401RE-Nucleo
 *  @{
 */

/** @defgroup STM32F401RE-Nucleo_Applications Applications
 *  @{
 */

/** @defgroup STM32F401RE-Nucleo_BLESTAR1 BLESTAR1
 *  @{
 */
 
/** @defgroup STM32F401RE-Nucleo_MAIN MAIN
 * @{
 */
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/** @defgroup STM32F401RE-Nucleo_MAIN_Exported_Defines Exported Defines 
 * @{
 */
/* Exported define -----------------------------------------------------------*/
/*
 * Enable/Disable the possibility to use WiFi (1 = WiFi Enabled, 0 = WiFi Disabled)
 */
#define ENABLE_WIFI 1

/*
 * Define the MAC_BLESTAR1 address, otherwise it will use a random MAC
 */
//#define MAC_BLESTAR1 0xbb, 0x00, 0x00, 0xE1, 0x80, 0x02

/*
 * Enable/Disable the possibility to receive MEMS (AGM and SFUSION) notifications 
 * (1 = MEMS Enabled, 0 = MEMS Disabled)
 * WARNING: To be used with ST SensNet V>2.0 application
 */   
#define ENABLE_MEMS 0

/*
 * Enable/Disable the possibility to receive CO sensor data notifications 
 * (1 = CO data Enabled, 0 = CO data Disabled)
 * WARNING: To be used with ST SensNet V>2.0 application
 */   
#define ENABLE_CO 0

/*
 * Define Local Debug (a lot of printf)
 */
//#define LOCAL_DEBUG
#ifdef LOCAL_DEBUG
#include <stdio.h>
#define LOC_DEBUG(...) PRINTF(__VA_ARGS__)
#else
#define LOC_DEBUG(...)
#endif

#define TIMx                           TIM3
#define TIMx_CLK_ENABLE()              __HAL_RCC_TIM3_CLK_ENABLE()

/* Definition for TIMx's NVIC */
#define TIMx_IRQn                      TIM3_IRQn
#define TIMx_IRQHandler                TIM3_IRQHandler



/**
 * @}
 */

/**
 * @}
 */
   
/**
 * @}
 */
   
/**
 * @}
 */
   
/**
 * @}
 */
 
/**
 * @}
 */
#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
