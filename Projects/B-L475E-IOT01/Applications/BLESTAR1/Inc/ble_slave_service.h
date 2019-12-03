/**
  ******************************************************************************
  * @file    ble_slave_service.h 
  * @author  Central LAB
  * @version V1.0.0
  * @date    11-Nov-2015
  * @brief   
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
#ifndef _BLE_SLAVE_SERVICE_H_
#define _BLE_SLAVE_SERVICE_H_

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
//#include "cube_hal.h"
#include "string.h"
#include "osal.h"
#include "hal_types.h"
#include "hal.h"
#include "hci_const.h"
#include "hci.h"
#include "hci_le.h"
#include "sm.h"
#include "bluenrg_gatt_server.h"
#include "bluenrg_gatt_aci.h"
#include "bluenrg_gap.h"
#include "bluenrg_gap_aci.h"
#include "bluenrg_hal_aci.h"
#include "bluenrg_aci_const.h"
#include "bluenrg_utils.h"
#include "debug.h"

/** @addtogroup Projects
 *  @{
 */
 
/** @addtogroup B-L475E-IOT01
 *  @{
 */

/** @defgroup B-L475E-IOT01_Applications Applications
 *  @{
 */

/** @defgroup B-L475E-IOT01_BLESTAR1 BLESTAR1
 *  @{
 */
 
/** @defgroup B-L475E-IOT01_BLE_SLAVE_SERVICE BLE_SLAVE_SERVICE
 * @{
 */

/** @defgroup B-L475E-IOT01_BLE_SLAVE_SERVICE_Exported_Defines Exported Defines
 *  @{
 */
/* Attribute value format of the STAR_CHAR */   
/* Timestamp | Node ID | Type ID | value   | Type ID | value   | Type ID | value   | … */
/* 2 bytes   | 2 bytes | 1 byte  | x bytes | 1 byte  | x bytes | 1 byte  | x bytes | … */
#define STAR_CHAR_MAX_VALUE_LEN   20
      
/**
 * @}
 */  

/** @defgroup B-L475E-IOT01_BLE_SLAVE_SERVICE_Exported_Macros Exported Macros
 * @{
 */
/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)     ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8)  ) )

#define STORE_LE_32(buf, val)    ( ((buf)[0] =  (uint8_t) (val)     ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8)  ) , \
                                   ((buf)[2] =  (uint8_t) (val>>16) ) , \
                                   ((buf)[3] =  (uint8_t) (val>>24) ) )

#define STORE_BE_32(buf, val)    ( ((buf)[3] =  (uint8_t) (val)     ) , \
                                   ((buf)[2] =  (uint8_t) (val>>8)  ) , \
                                   ((buf)[1] =  (uint8_t) (val>>16) ) , \
                                   ((buf)[0] =  (uint8_t) (val>>24) ) )   
/**
 * @}
 */  
   
/** @addtogroup B-L475E-IOT01_BLE_SLAVE_SERVICE_Exported_Types Exported Types
 *  @{
 */
typedef struct {
  uint8_t data_length;
  uint8_t *attribute_value;
  tBDAddr devAddr;
  uint16_t attribute_handle;  
} NotificationData_t;
   
typedef struct {
  uint8_t  is_discoverable;
  uint8_t  is_connected;  
  uint16_t conn_handle;
  uint16_t star_hw_serv_handle;
  uint16_t star_data_char_handle;
  uint16_t star_config_char_handle;
  uint8_t  star_data_char_notify;
  uint8_t  star_config_value[3];
  uint8_t  star_config_value_len;
  NotificationData_t notification_data;
} SlaveDevice_t;

/**
 * @}
 */

/** @addtogroup B-L475E-IOT01_BLE_SLAVE_SERVICE_Exported_Functions Exported Functions
 *  @{
 */
void Add_All_Services              (void);
void Change_Notification_Status    (uint8_t *att_data, uint8_t  *attr_value, uint16_t conn_handle, uint8_t i, 
                                    uint32_t feature_mask, uint8_t frequency);
void Attribute_Modified_CB         (uint16_t handle, uint8_t data_length, uint8_t *att_data);
uint8_t Get_Device_Index_From_Addr (uint8_t *addr);
void Notify_Master                 (uint8_t data_length, uint8_t* attribute_value,
                                    uint16_t serv_handle, uint16_t char_handle);
void Disable_All_Notifications     (void);
void Set_Notification_Property     (uint16_t conn_handle, uint8_t i, uint32_t feature_mask, 
                                    uint8_t command, uint8_t data);
void Set_New_Nodes_Scanning        (uint8_t enabled);
void Set_Slave_Discoverable        (void);
void Forward_Command_To_BlueNRG    (uint8_t* data_ptr, uint8_t data_length);
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
#ifdef __cplusplus
}
#endif

#endif /* _BLE_SLAVE_SERVICE_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

