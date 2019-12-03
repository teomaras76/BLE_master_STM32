/**
  ******************************************************************************
  * @file    stm32l475e_iot01_bluenrg_ext.h
  * @author  CL
  * @version V1.0.0
  * @date    04-July-2014
  * @brief   This file contains definitions for the SPI communication with the  
  *          BLE BlueNRG-MS device on the X-NUCLEO-IDB05A1 expansion board by 
  *          STMicroelectronics
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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
#ifndef __STM32L475E_IOT01_BLUENRG_EXT_H
#define __STM32L475E_IOT01_BLUENRG_EXT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"

/** @addtogroup BSP
  * @{
  */
  
/** @addtogroup STM32L475E_IOT01
  * @{
  */
  
/** @addtogroup STM32L475E_IOT01_BLUENRG_EXT
  * @{
  */

/** @defgroup STM32L475E_IOT01_BLUENRG_EXT_Exported_Defines BLUENRG Exported Defines
  * @{
  */
  
/**
* @brief SPI communication details between STM32L475E IOT01 and BlueNRG-MS
*        Expansion Board.
*/
// SPI Instance
#define BNRG_SPI_INSTANCE               SPI1
#define BNRG_SPI_CLK_ENABLE()           __HAL_RCC_SPI1_CLK_ENABLE()

// SPI Configuration
#define BNRG_SPI_MODE                   SPI_MODE_MASTER
#define BNRG_SPI_DIRECTION              SPI_DIRECTION_2LINES
#define BNRG_SPI_DATASIZE               SPI_DATASIZE_8BIT
#define BNRG_SPI_CLKPOLARITY            SPI_POLARITY_LOW
#define BNRG_SPI_CLKPHASE               SPI_PHASE_1EDGE
#define BNRG_SPI_NSS                    SPI_NSS_SOFT
#define BNRG_SPI_FIRSTBIT               SPI_FIRSTBIT_MSB
#define BNRG_SPI_TIMODE                 SPI_TIMODE_DISABLE
#define BNRG_SPI_CRCPOLYNOMIAL          7 
#define BNRG_SPI_BAUDRATEPRESCALER      SPI_BAUDRATEPRESCALER_16 /**< Keep clock running lower than 8Mhz */
#define BNRG_SPI_CRCCALCULATION         SPI_CRCCALCULATION_DISABLE

// SPI Reset Pin: PA.8
#define BNRG_SPI_RESET_PIN              GPIO_PIN_4
#define BNRG_SPI_RESET_MODE             GPIO_MODE_OUTPUT_PP
#define BNRG_SPI_RESET_PULL             GPIO_NOPULL
//#define BNRG_SPI_RESET_PULL             GPIO_PULLUP
#define BNRG_SPI_RESET_SPEED            GPIO_SPEED_FREQ_LOW
#define BNRG_SPI_RESET_ALTERNATE        0
#define BNRG_SPI_RESET_PORT             GPIOA
#define BNRG_SPI_RESET_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()

// SCLK: 
#define BNRG_SPI_SCLK_PIN               GPIO_PIN_5
#define BNRG_SPI_SCLK_MODE              GPIO_MODE_AF_PP
#define BNRG_SPI_SCLK_PULL              GPIO_NOPULL
//#define BNRG_SPI_SCLK_PULL              GPIO_PULLUP
#define BNRG_SPI_SCLK_SPEED             GPIO_SPEED_FREQ_VERY_HIGH
#define BNRG_SPI_SCLK_ALTERNATE         GPIO_AF5_SPI1
#define BNRG_SPI_SCLK_PORT              GPIOA
#define BNRG_SPI_SCLK_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

// MISO (Master Input Slave Output): 
#define BNRG_SPI_MISO_PIN               GPIO_PIN_6
#define BNRG_SPI_MISO_MODE              GPIO_MODE_AF_PP
#define BNRG_SPI_MISO_PULL              GPIO_NOPULL
#define BNRG_SPI_MISO_SPEED             GPIO_SPEED_FREQ_VERY_HIGH
#define BNRG_SPI_MISO_ALTERNATE         GPIO_AF5_SPI1
#define BNRG_SPI_MISO_PORT              GPIOA
#define BNRG_SPI_MISO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

// MOSI (Master Output Slave Input): 
#define BNRG_SPI_MOSI_PIN               GPIO_PIN_7
#define BNRG_SPI_MOSI_MODE              GPIO_MODE_AF_PP
#define BNRG_SPI_MOSI_PULL              GPIO_NOPULL
#define BNRG_SPI_MOSI_SPEED             GPIO_SPEED_FREQ_VERY_HIGH
#define BNRG_SPI_MOSI_ALTERNATE         GPIO_AF5_SPI1
#define BNRG_SPI_MOSI_PORT              GPIOA
#define BNRG_SPI_MOSI_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

// NSS/CSN/CS: PA.1
#define BNRG_SPI_CS_PIN                 GPIO_PIN_4
#define BNRG_SPI_CS_MODE                GPIO_MODE_OUTPUT_PP
#define BNRG_SPI_CS_PULL                GPIO_NOPULL
//#define BNRG_SPI_CS_PULL                GPIO_PULLUP
#define BNRG_SPI_CS_SPEED               GPIO_SPEED_FREQ_VERY_HIGH
#define BNRG_SPI_CS_ALTERNATE           0
#define BNRG_SPI_CS_PORT                GPIOC
#define BNRG_SPI_CS_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()

/* IRQ PIN and PORT defined in the hw.h */
#define BNRG_SPI_IRQ_MODE               GPIO_MODE_IT_RISING
#define BNRG_SPI_IRQ_PULL               GPIO_PULLDOWN
//#define BNRG_SPI_IRQ_PULL               GPIO_NOPULL
#define BNRG_SPI_IRQ_SPEED              GPIO_SPEED_FREQ_VERY_HIGH
#define BNRG_SPI_IRQ_ALTERNATE          0
#define BNRG_SPI_IRQ_PIN                GPIO_PIN_5
#define BNRG_SPI_IRQ_PORT               GPIOC
#define BNRG_SPI_IRQ_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()

#define BNRG_SPI_EXTI_PIN               BNRG_SPI_IRQ_PIN
#define BNRG_SPI_EXTI_PORT              BNRG_SPI_IRQ_PORT
#define BNRG_SPI_EXTI_IRQn              EXTI9_5_IRQn
#define BNRG_SPI_EXTI_IRQHandler        EXTI9_5_IRQHandler

//EXTI External Interrupt for user button
//#define PUSH_BUTTON_EXTI_IRQHandler EXTI15_10_IRQHandler
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_BLUENRG_EXT_Exported_Functions
  * @{
  */
  
void Enable_SPI_IRQ(void);
void Disable_SPI_IRQ(void);
void Clear_SPI_IRQ(void);
void Clear_SPI_EXTI_Flag(void);

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
#ifdef __cplusplus
}
#endif

#endif /* __STM32L475E_IOT01_BLUENRG_EXT_H */

    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

