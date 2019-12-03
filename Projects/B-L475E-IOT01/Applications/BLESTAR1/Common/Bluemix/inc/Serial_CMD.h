/**
  *******************************************************************************
  * @file    Projects/Multi/Examples/IKS01A2/DataLog/Inc/Serial_CMD.h
  * @author  CL
  * @version V4.1.0
  * @date    01-November-2017
  * @brief   This file contains serial commands code
  *******************************************************************************
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
  ********************************************************************************
  */


    
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SERIAL_CMD_H
#define __SERIAL_CMD_H

/* Exported constants --------------------------------------------------------*/

#define DATALOG_MODE         100
#define DATALOG_FUSION_MODE    1
#define DATALOG_AR_MODE        2
#define DATALOG_CP_MODE        3
#define DATALOG_GR_MODE        4
#define DATALOG_EXT_MODE     101


/**********  GENERIC  CMD  (0x00 - 0x0F)  **********/

#define CMD_Read_PresString					0x02
#define CMD_CheckModeSupport					0x04
#define CMD_UploadData						0x05
#define CMD_UploadAR						0x05
#define CMD_UploadCP						0x06
#define CMD_ChangeSF						0x07
#define CMD_Start_Data_Streaming				0x08
#define CMD_Stop_Data_Streaming             			0x09
#define CMD_StartDemo						0x0A
#define CMD_UploadPM                            		0x0A /* Reuse of the CMD_StartDemo value because we do not have other free values */
#define CMD_Sleep_Sec						0x0B
#define CMD_Set_DateTime					0x0C
#define CMD_Get_DateTime					0x0D
#define CMD_Enter_DFU_Mode					0x0E
#define CMD_Reset						0x0F

												
/****************************************************/


/******** INERTIAL  CMD  (0x70 - 0x/7F)  ********/

#define CMD_LSM9DS1_Init					0x70
#define CMD_LSM9DS1_9AXES_Read					0x71
#define CMD_LSM9DS1_AHRS_Read					0x72
#define CMD_LSM9DS1_ACC_Read                                    0x73
#define CMD_LSM9DS1_GYR_Read 					0x74
#define CMD_LSM9DS1_MAG_Read 					0x75
#define CMD_LSM6DS0_Init					0x76
#define CMD_LSM6DS0_ACC_Read					0x77
#define CMD_LSM6DS0_GYR_Read					0x78
#define CMD_LSM6DS0_6AXES_Read					0x79
#define CMD_LIS3MDL_Init    					0x7A
#define CMD_LIS3MDL_Read					0x7B
#define CMD_SF_Init						0x7C
#define CMD_SF_Data						0x7D
#define CMD_DIL24_Init  					0x7E
#define CMD_DIL24_Read						0x7F
												
/****************************************************/


#define CMD_Reply_Add				                0x80


//TMsg.Data[2]
#define CMD_SUBCMD_SENSOR			                0x50

//TMsg.Data[3]
#define SUBCMD_GET_SENSOR_NAME					0x01
//receive: TMsg.Data[5]...:  Sensor Name
#define SUBCMD_READ_REGISTER					0x02
//send:    TMsg.Data[5]: Register
//receive: TMsg.Data[5]: Register; TMsg.Data[6]: Value
#define SUBCMD_WRITE_REGISTER					0x03
//send:    TMsg.Data[5]: Register; TMsg.Data[6]: Value
//receive: TMsg.Data[5]: Register; TMsg.Data[6]: Value
#define SUBCMD_GET_FS_LIST  					0x04
//receive: TMsg.Data[5]: int Count; TMsg.Data[9]: int value1 ...
#define SUBCMD_SET_FS       					0x05
//send:    TMsg.Data[5]: int full scale
//receive: TMsg.Data[5]: int selected full scale
#define SUBCMD_GET_ODR_LIST  					0x06
//receive: TMsg.Data[5]: int Count; TMsg.Data[9]: float value1 ...
#define SUBCMD_SET_ODR       					0x07
//send:    TMsg.Data[5]: float ODR
//receive: TMsg.Data[5]: float selected ODR

//all following commands will have TMsg.Data[4]: SUBCMD_SENSOR_ACC
#define SUBCMD_SET_MAGNITUDE                                    0x0D
//send:    TMsg.Data[5]: byte magnitude (1,..,7)
//receive: TMsg.Data[5]: byte magnitude
#define SUBCMD_SET_SENSING_AXIS                                 0x0E
//send:    TMsg.Data[5]: byte axis (1,2,3)
//receive: TMsg.Data[5]: byte axis
#define SUBCMD_SET_OP_MODE                                      0x0F
//send:    TMsg.Data[5]: byte mode (1,2,3)
//receive: TMsg.Data[5]: byte mode
#define SUBCMD_SET_FFT_SAMPLES                                  0x10
//send:    TMsg.Data[5]: byte samples (1,2,3)
//receive: TMsg.Data[5]: byte samples
#define SUBCMD_SET_HP_FILTER                                    0x11
//send:    TMsg.Data[5]: byte onoff (1,2)
//receive: TMsg.Data[5]: byte onoff (1,2)
#define SUBCMD_SET_ODR_INDEX                                    0x12
//send:    TMsg.Data[5]: byte index of requested ODR
//receive: TMsg.Data[5]: byte index of set ODR
#define SUBCMD_SET_FS_INDEX                                     0x13
//send:    TMsg.Data[5]: byte index of requested FS
//receive: TMsg.Data[5]: byte index of set FS
#define SUBCMD_GET_SENSOR_LIST                                  0x14
//receive: TMsg.Data[5]: comma sepparated list of available sensors
#define SUBCMD_SET_SENSOR_INDEX                                 0x15
//send:    TMsg.Data[5]: byte index of requested sensor
//receive: TMsg.Data[5]: byte index of set sensor
#define SUBCMD_GET_OP_MODE                                      0x16
//receive: TMsg.Data[5]: byte mode (1,2,3)
#define SUBCMD_RESTART                                          0x17
#define SUBCMD_GET_FFT_SAMPLES_LIST                             0x18
		
										
//TMsg.Data[4]
#define SUBCMD_SENSOR_ACC      					0x01
#define SUBCMD_SENSOR_GYR      					0x02
#define SUBCMD_SENSOR_MAG      					0x03
#define SUBCMD_SENSOR_TMP     					0x04
#define SUBCMD_SENSOR_HUM      					0x05
#define SUBCMD_SENSOR_PRS     					0x06
#endif /* __SERIAL_CMD_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
