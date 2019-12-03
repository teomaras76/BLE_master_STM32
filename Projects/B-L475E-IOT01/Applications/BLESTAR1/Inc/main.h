/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    26-October-2017
  * @brief   main application header file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
#define __main_h__
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"  
#include "stm32l475e_iot01.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "timedate.h"
#include "flash.h"
#include "net.h"
#include "iot_flash_config.h"
#include "msg.h"
#include "cloud.h"
#include "sensors_data.h"
#include "TargetFeatures.h"  
   
/*
 * Enable standard log
 */
#define PRINT_STAR_LOG 1
#if PRINT_STAR_LOG
  #define STAR_PRINTF(...) printf(__VA_ARGS__)
#else
  #define STAR_PRINTF(...)
#endif

/*
 * Enable debug log (a lot of printf)
 */
#define LOCAL_DEBUG 0
#if LOCAL_DEBUG
#include <stdio.h>
#define LOC_DEBUG(...) printf(__VA_ARGS__)
#else
#define LOC_DEBUG(...)
#endif

#define STM32_UUID ((uint32_t *)0x1FFF7A10)

/* Define the FP-NET-BLESTAR1 MAC address, otherwise it will use a random MAC */
//#define MAC_STAR 0xFF, 0xEE, 0xDD, 0xAA, 0xAA, 0xAA
  
#ifdef USE_MBED_TLS
extern int mbedtls_hardware_poll( void *data, unsigned char *output, size_t len, size_t *olen );
#endif /* USE_MBED_TLS */
 
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#if defined(USE_WIFI)
#define NET_IF  NET_IF_WLAN
#elif defined(USE_LWIP)
#define NET_IF  NET_IF_ETH
#elif defined(USE_C2C)
#define NET_IF  NET_IF_C2C
#endif

/* Exported functions --------------------------------------------------------*/
void    Error_Handler(void);
uint8_t Button_WaitForPush(uint32_t timeout);
void    Led_On(void);
void    Led_Off(void);
void    Led_Toggle(void);
void    PeriphClk_Config(void);

extern net_hnd_t hnet;

/* IAR */
#ifdef __ICCARM__  
extern const user_config_t lUserConfig;
/* Keil / armcc */
#elif defined ( __CC_ARM   )
extern user_config_t lUserConfig;
/* GNU Compiler */
#elif defined ( __GNUC__ )     
extern user_config_t lUserConfig;
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
