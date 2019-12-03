/**
 ******************************************************************************
 * @file    sensor_def.c
 * @author  Martin Polacek
 * @version V2.0.0
 * @date    13-October-2017
 * @brief   This file contains definitions of sensor settings for FFT demo project
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

/* Includes ------------------------------------------------------------------*/
#include "sensor_def.h"

/* Global variables ----------------------------------------------------------*/

 
/*
 * Format of lists (except sensing axis and opmode list):
 * "name{ n, a_1, a_2, ... , a_n}"
 */


// LSM6DSL
const float lsm6dsl_odr[] = {6, 208, 416, 833, 1660, 3330, 6660};
const float lsm6dsl_odr_om[] = {8, 0, 208, 416, 833, 1660, 3330, 6660, 0};
const uint32_t lsm6dsl_fs[] = {4, 2, 4, 8, 16};
/* lsm6dsl_axis is simulation only (not supported by sensor) */
const ACTIVE_AXIS_t lsm6dsl_axis[] = {X_AXIS, Y_AXIS, Z_AXIS, ALL_ACTIVE };
const uint32_t lsm6dsl_samples_list[] = {2, 256, 512};

// LSM303AGR
const float lsm303agr_odr[] = {5, 200, 400, 1344, 1620, 5376};
const float lsm303agr_odr_om[] = {12, 200, 400, 1620, 5376, 0, 200, 400, 1344, 0, 200, 400, 1344};
const uint32_t lsm303agr_fs[] = {4, 2, 4, 8, 16};
const OP_MODE_t lsm303agr_opmode[] = {LOW_PWR_MODE, NORMAL_MODE, HIGH_RES_MODE};
const ACTIVE_AXIS_t lsm303agr_axis[] = {X_AXIS, Y_AXIS, Z_AXIS, ALL_ACTIVE };
const uint32_t lsm303agr_samples_list[] = {3, 256, 512, 1024};

// LIS2DH12
const float lis2dh12_odr[] = {5, 200, 400, 1344, 1620, 5376};
const float lis2dh12_odr_om[] = {12, 200, 400, 1620, 5376, 0, 200, 400, 1344, 0, 200, 400, 1344};
const uint32_t lis2dh12_fs[] = {4, 2, 4, 8, 16};
const OP_MODE_t lis2dh12_opmode[] = {LOW_PWR_MODE, NORMAL_MODE, HIGH_RES_MODE};
const ACTIVE_AXIS_t lis2dh12_axis[] = {X_AXIS, Y_AXIS, Z_AXIS, ALL_ACTIVE };
const uint32_t lis2dh12_samples_list[] = {3, 256, 512, 1024};


const sensor_setting_t sensor_setting[] = 
{
  [LSM6DSL_X_0] = { "LSM6DSL", lsm6dsl_odr, lsm6dsl_odr_om, lsm6dsl_fs, NULL, lsm6dsl_axis, lsm6dsl_samples_list, 1},
  [LSM303AGR_X_0] = { "LSM303AGR", lsm303agr_odr, lsm303agr_odr_om, lsm303agr_fs, lsm303agr_opmode, lsm303agr_axis, lsm303agr_samples_list, 1},
  [LIS2DH12_0] = { "LIS2DH12", lis2dh12_odr, lis2dh12_odr_om, lis2dh12_fs, lis2dh12_opmode, lis2dh12_axis, lis2dh12_samples_list, 1},
  [LSM6DSL_X_1] = { "LSM6DSL_DIL", lsm6dsl_odr, lsm6dsl_odr_om, lsm6dsl_fs, NULL, lsm6dsl_axis, lsm6dsl_samples_list, 1},
};


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
