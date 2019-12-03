/**
 ******************************************************************************
 * @file    sensor_def.h
 * @author  Martin Polacek
 * @version V2.0.0
 * @date    13-October-2017
 * @brief   header for sensor_def.c.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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
#ifndef __SENSOR_DEF_H
#define __SENSOR_DEF_H


/* Includes ------------------------------------------------------------------*/
#include "accelerometer.h"
#include "x_nucleo_iks01a2_accelero.h"

/* Typedefs ------------------------------------------------------------------*/

typedef struct
{
  const char* name;
  const float *odr_list;
  const float *odr_om_list;
  const uint32_t *fs_list;
  const OP_MODE_t *opmode_list;
  const ACTIVE_AXIS_t *axis_list;
  const uint32_t *samples_list;
  const uint8_t hp_filter_available;
} sensor_setting_t;

extern const sensor_setting_t sensor_setting[];

#endif /* __SENSOR_DEF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
