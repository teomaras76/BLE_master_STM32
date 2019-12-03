/**
  ******************************************************************************
  * @file    btle_config.h 
  * @author  CL
  * @version V1.0.0
  * @date    16-Nov-2015
  * @brief   This file defines macros for the BTLE configuration
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
#ifndef _BTLE_CONFIG_
#define _BTLE_CONFIG_

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
 
/** @defgroup STM32F401RE-Nucleo_BTLE_CONFIG BTLE_CONFIG 
 * @{
 */

/** @defgroup STM32F401RE-Nucleo_BTLE_CONFIG_Exported_Defines Exported Defines
 *  @{
 */
/* Exported defines ----------------------------------------------------------*/
#define SCAN_P (0x0010) /*(0x7D0)*/ /*(0x0010)*/  /*(0x4000)*/
#define SCAN_L (0x0010) /*(0x7D0)*/ /*(0x0010)*/  /*(0x4000)*/

/** 
* @brief Supervision timeout, arg in msec.
*/
#define SUPERV_TIMEOUT (3200)  /* (60) */

/**
 * @}
 */

/** @addtogroup BTLE_CONFIG_Exported_Macros
 *  @{
 */
/* Exported macros -----------------------------------------------------------*/
/** 
* @brief Connection period, arg in msec.
*/
#define CONN_P(x)   ((int)((x)/1.25f))

/** 
* @brief Connection period MIN, arg in msec.
*/
#define CONN_P1     (CONN_P(135))  /* (CONN_P(50)) */ /* (CONN_P(10)) */

/** 
* @brief Connection period MAX, arg in msec.
*/
#define CONN_P2     (CONN_P(135))  /* (CONN_P(50)) */ /* (CONN_P(10)) */

/** 
* @brief Connection length, arg in msec.
*/
#define CONN_L(x)   ((int)((x)/0.625f))

/** 
* @brief Connection length MIN, arg in msec.
*/
#define CONN_L1     (CONN_L(8))    /* (CONN_L(1250)) */

/** 
* @brief Connection length MAX, arg in msec.
*/
#define CONN_L2     (CONN_L(8))    /* (CONN_L(1250)) */

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
#endif /* _BTLE_CONFIG_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
