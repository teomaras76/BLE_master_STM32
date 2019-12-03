/**
  ******************************************************************************
  * @file    sensors_data.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    26-October-2017
  * @brief   Manage sensors of STM32L475 IoT board.
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sensors_data.h"

#ifdef STM32L476xx
#include "stm32l4xx_hal_i2c.h"
#endif // STM32L476xx

#if defined (USE_WIFI_IOT01)
#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"
#include "vl53l0x_proximity.h"
#include "msg.h"
#elif (defined USE_STM32F4XX_NUCLEO) || (defined USE_STM32L4XX_NUCLEO)
#include "IBM_Watson_Config.h"
#include "msg.h"
#include "fft.h"
/* Sensors. Private variables ---------------------------------------------------------*/
void *ACCELERO_handle = NULL;
void *ACCELERO_handle_LSM6DSL = NULL;
void *GYRO_handle = NULL;
void *MAGNETO_handle = NULL;
void *HUMIDITY_handle = NULL;
void *TEMPERATURE_handle = NULL;
void *PRESSURE_handle = NULL;
uint8_t acquire_data( void );
float *get_fft_data( void );
#endif  // USE_WIFI_IOT01

#if  (defined USE_STM32F4XX_NUCLEO)
#include "stm32f4xx_hal.h"
#elif  (defined USE_STM32L4XX_NUCLEO)
#include "stm32l4xx_hal.h"
#endif // defined USE_STM32F4XX_NUCLEO

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/

static float    TEMPERATURE_Value;
static float    HUMIDITY_Value;
static float    PRESSURE_Value;
static int16_t  ACC_Value[3];
static float    GYR_Value[3];
static int16_t  MAG_Value[3];
static uint16_t PROXIMITY_Value;

/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  init_sensors
  * @param  none
  * @retval 0 in case of success
  *         -1 in case of failure
  */
#if defined (USE_WIFI_IOT01)
int init_sensors(void)
{
  int ret = 0;

  if (HSENSOR_OK != BSP_HSENSOR_Init())
  {
    msg_error("BSP_HSENSOR_Init() returns %d\n", ret);
    ret = -1;
  }

  if (TSENSOR_OK != BSP_TSENSOR_Init())
  {
    msg_error("BSP_TSENSOR_Init() returns %d\n", ret);
    ret = -1;
  }

  if (PSENSOR_OK != BSP_PSENSOR_Init())
  {
    msg_error("BSP_PSENSOR_Init() returns %d\n", ret);
    ret = -1;
  }

  if (MAGNETO_OK != BSP_MAGNETO_Init())
  {
    msg_error("BSP_MAGNETO_Init() returns %d\n", ret);
    ret = -1;
  }

  if (GYRO_OK != BSP_GYRO_Init())
  {
    msg_error("BSP_GYRO_Init() returns %d\n", ret);
    ret = -1;
  }

  if (ACCELERO_OK != BSP_ACCELERO_Init())
  {
    msg_error("BSP_ACCELERO_Init() returns %d\n", ret);
    ret = -1;
  }

  VL53L0X_PROXIMITY_Init();

  return ret;
}
#else
int init_sensors(void)
{
    int ret = 0;

#ifdef __IKS01A1
    /* Try to use LSM6DS3 DIL24 if present */
    if(BSP_ACCELERO_Init( LSM6DS3_X_0, &ACCELERO_handle)!=COMPONENT_OK){
      /* otherwise try to use LSM6DS on board */
      if(BSP_ACCELERO_Init(LSM6DS0_X_0, &ACCELERO_handle)!=COMPONENT_OK){
         ret = -1;
      }
    }
#else
  /* Try to use IIS2DH, LSM303AGR or LSM6DSL accelerometer */
    if(BSP_ACCELERO_Init( LSM303AGR_X_0, &ACCELERO_handle )!=COMPONENT_OK){
         ret = -1;
    }
#endif

    /* gyro sensor */
    if(BSP_GYRO_Init( GYRO_SENSORS_AUTO, &GYRO_handle )!=COMPONENT_OK){
         ret = -1;
    }

    /* magneto sensor */
    if(BSP_MAGNETO_Init( MAGNETOMETER_SENSOR_AUTO, &MAGNETO_handle )!=COMPONENT_OK){
         ret = -1;
    }

    /* Force to use HTS221 */
    if(BSP_HUMIDITY_Init( HTS221_H_0, &HUMIDITY_handle )!=COMPONENT_OK){
         ret = -1;
    }

    /* Force to use HTS221 */
    if(BSP_TEMPERATURE_Init( HTS221_T_0, &TEMPERATURE_handle )!=COMPONENT_OK){
         ret = -1;
    }

    /* Try to use LPS25HB DIL24 if present, otherwise use LPS25HB on board */
    if(BSP_PRESSURE_Init( PRESSURE_SENSORS_AUTO, &PRESSURE_handle )!=COMPONENT_OK){
         ret = -1;
    }

    /* Set ODR to 400Hz for FFT demo */
    if(BSP_ACCELERO_Set_ODR_Value( ACCELERO_handle, 400.0f ) != COMPONENT_OK)
    {
         ret = -1;
    }

    /*  Enable all the sensors */
    BSP_ACCELERO_Sensor_Enable( ACCELERO_handle );
    BSP_GYRO_Sensor_Enable( GYRO_handle );
    BSP_MAGNETO_Sensor_Enable( MAGNETO_handle );
    BSP_HUMIDITY_Sensor_Enable( HUMIDITY_handle );
    BSP_TEMPERATURE_Sensor_Enable( TEMPERATURE_handle );
    BSP_PRESSURE_Sensor_Enable( PRESSURE_handle );

#if (defined FFT_DEMO)
    if(0 == init_fft( TERMINAL ) )
    {
      printf("fft library initialized\n\r");
    }
    else
    {
      printf("FFT library init failed...\n\rPlease restart nucleo board.\n\r");
      ret = -2;
    }
#endif // FFT_DEMO
    return ret;
}
#endif

/**
  * @brief  fill the payload with the sensor values
  * @param  none
  * @param PayloadBuffer is the char pointer for the Payload buffer to be filled
  * @param PayloadSize size of the above buffer
  * @retval 0 in case of success
  *         -1 in case of failure
  */
int PrepareMqttPayload(char * PayloadBuffer, int PayloadSize, char * deviceID)
{
  char * Buff = PayloadBuffer;
  int BuffSize = PayloadSize;
  int snprintfreturn = 0;
  uint32_t FFTmaxAmpl = 0;
  uint32_t FFTMaxFreq = 0;  
  char motorStatus[] = "OK";    
    
#if defined (USE_WIFI_IOT01)
  TEMPERATURE_Value = BSP_TSENSOR_ReadTemp();
  HUMIDITY_Value = BSP_HSENSOR_ReadHumidity();
  PRESSURE_Value = BSP_PSENSOR_ReadPressure();
  PROXIMITY_Value = VL53L0X_PROXIMITY_GetDistance();
  BSP_ACCELERO_AccGetXYZ(ACC_Value);
  BSP_GYRO_GetXYZ(GYR_Value);
  BSP_MAGNETO_GetXYZ(MAG_Value);
#else
  BSP_TEMPERATURE_Get_Temp(TEMPERATURE_handle,(float *)&TEMPERATURE_Value);
  BSP_HUMIDITY_Get_Hum(HUMIDITY_handle,(float *)&HUMIDITY_Value);
  BSP_PRESSURE_Get_Press(PRESSURE_handle,(float *)&PRESSURE_Value);

  /* ACC_Value[0] = X axis, ACC_Value[1] = Y axis, ACC_Value[2] = Z axis */
  SensorAxes_t sensValue;
  BSP_ACCELERO_Get_Axes(ACCELERO_handle, &sensValue);
  ACC_Value[0] = sensValue.AXIS_X;
  ACC_Value[1] = sensValue.AXIS_Y;
  ACC_Value[2] = sensValue.AXIS_Z;

  BSP_GYRO_Get_Axes(GYRO_handle,&sensValue);
  GYR_Value[0] = sensValue.AXIS_X;
  GYR_Value[1] = sensValue.AXIS_Y;
  GYR_Value[2] = sensValue.AXIS_Z;

  BSP_MAGNETO_Get_Axes(MAGNETO_handle,&sensValue);
  MAG_Value[0] = sensValue.AXIS_X;
  MAG_Value[1] = sensValue.AXIS_Y;
  MAG_Value[2] = sensValue.AXIS_Z;

#if (defined FFT_DEMO) && (!defined __IKS01A1)
    if(0!=prepare_fft_data())
    {
        printf("problem while creating FFT data\n\r");
        return 1;
    }

    FFTmaxAmpl = get_fft_max_freq_amp();
    FFTMaxFreq = get_fft_max_freq();
    printf("FFTmaxAmpl: %u, FFTMaxFreq: %u\r\n", FFTmaxAmpl, FFTMaxFreq);
#endif // (defined FFT_DEMO) &&(!defined __IKS01A1)


#endif

 #if (((defined BLUEMIX) && (defined FFT_DEMO)) | (defined USE_WIFI_IOT01))
    snprintfreturn = snprintf( Buff, BuffSize, "{\"d\":{"
             "\"temperature\": %.2f, \"humidity\": %.2f, \"pressure\": %.2f, "
             "\"acc_x\": %d, \"acc_y\": %d, \"acc_z\": %d, "
             "\"Motor_status\": \"%s\", "
             "\"FFTMaxFreqAmp\": %u, \"FFTMaxFreq\": %u "
               "}}",
             TEMPERATURE_Value, HUMIDITY_Value, PRESSURE_Value, 
             ACC_Value[0], ACC_Value[1], ACC_Value[2],     
             motorStatus,
             FFTmaxAmpl, FFTMaxFreq  );    

 #else
  if (deviceID != NULL)
  {
    snprintfreturn = snprintf( Buff, BuffSize, "{\"deviceId\":\"%s\","
             "\"temperature\": %.2f, \"humidity\": %.2f, \"pressure\": %.2f, \"proximity\": %d, "
             "\"acc_x\": %d, \"acc_y\": %d, \"acc_z\": %d, "
             "\"gyr_x\": %.0f, \"gyr_y\": %.0f, \"gyr_z\": %.0f, "
             "\"mag_x\": %d, \"mag_y\": %d, \"mag_z\": %d"
             "}",
             deviceID,
             TEMPERATURE_Value, HUMIDITY_Value, PRESSURE_Value, PROXIMITY_Value,
             ACC_Value[0], ACC_Value[1], ACC_Value[2],
             GYR_Value[0], GYR_Value[1], GYR_Value[2],
             MAG_Value[0], MAG_Value[1], MAG_Value[2] );
  }
  else
  {
  snprintfreturn = snprintf( Buff, BuffSize, "{\n \"state\": {\n  \"reported\": {\n"
           "   \"temperature\": %.2f,\n   \"humidity\": %.2f,\n   \"pressure\": %.2f,\n   \"proximity\": %d,\n"
           "   \"acc_x\": %d, \"acc_y\": %d, \"acc_z\": %d,\n"
           "   \"gyr_x\": %.0f, \"gyr_y\": %.0f, \"gyr_z\": %.0f,\n"
           "   \"mag_x\": %d, \"mag_y\": %d, \"mag_z\": %d\n"
           "  }\n }\n}",
           TEMPERATURE_Value, HUMIDITY_Value, PRESSURE_Value, PROXIMITY_Value,
           ACC_Value[0], ACC_Value[1], ACC_Value[2],
           GYR_Value[0], GYR_Value[1], GYR_Value[2],
           MAG_Value[0], MAG_Value[1], MAG_Value[2] );
  }
 #endif  // (defined BLUEMIX) && (defined FFT_DEMO)
  /* Check total size to be less than buffer size
            * if the return is >=0 and <n, then
            * the entire string was successfully formatted; if the return is
            * >=n, the string was truncated (but there is still a null char
            * at the end of what was written); if the return is <0, there was
            * an error.
            */
  if (snprintfreturn >= 0 && snprintfreturn < PayloadSize)
  {
      return 0;
  }
  else if(snprintfreturn >= PayloadSize)
  {
      msg_error("Data Pack truncated\n");
      return 0;
  }
  else
  {
      msg_error("Data Pack Error\n");
      return -1;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
