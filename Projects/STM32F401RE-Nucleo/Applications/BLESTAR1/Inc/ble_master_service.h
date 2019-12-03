/**
  ******************************************************************************
  * @file    ble_master_service.h 
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
#ifndef _BLE_MASTER_SERVICE_H_
#define _BLE_MASTER_SERVICE_H_

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "cube_hal.h"
#include "string.h"
#include "osal.h"
#include "hal_types.h"
#include "hal.h"
#include "hci_const.h"
#include "hci.h"
#include "sm.h"
#include "stm32_bluenrg_ble.h"
#include "bluenrg_gatt_server.h"
#include "bluenrg_gatt_aci.h"
#include "bluenrg_gap.h"
#include "bluenrg_gap_aci.h"
#include "bluenrg_hal_aci.h"
#include "bluenrg_aci_const.h"
#include "bluenrg_utils.h"
#include "debug.h"

#include "main.h"
   
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
 
/** @defgroup STM32F401RE-Nucleo_BLE_MASTER_SERVICE BLE_MASTER_SERVICE
 * @{
 */

/** @defgroup STM32F401RE-Nucleo_BLE_MASTER_SERVICE_Exported_Defines Exported Defines
 *  @{
 */
#define IDB04A1 0
#define IDB05A1 1
   
/* Supported peripheral nodes */
#define NODE_ME1                0x01 /* MOTENV1 */
#define NODE_FL1                0x02 /* FLIGHT1 */
#define NODE_AM1                0x03 /* ALLMEMS1 */

/* Matteo2 */
#define SENSING1                "TAI_100"
#define NODE_SE1                0x04 /* SENSING1 */ 

/* Matteo4 */
//#define SNSTLBOX                "MLC_100"     /* Matteo 4 - Default MLC name*/
#define SNSTLBOX                "MLCVIB1" 
#define NODE_STB                0x04 /* SENSORTILEBOX */
   
#define MAX_NUM_OF_NODES        1 /* max num of peripheral nodes can be connected to one central device */
#define NAME_BLESTAR1           'B','l','e','S','t','a','r','2'

#define BDADDR_SIZE             6
#define NODE_ID_B1              0 /* byte of the node BLE address used as 1st byte of the node ID */
#define NODE_ID_B2              1 /* byte of the node BLE address used as 2nd byte of the node ID */
#define NODE_ID_OFFSET          NODE_ID_B1   

#define ME1_WUP_EVENT_ENABLED   1             /* To enable/disable Wake Up events from MOTENV1 nodes                                    */
#define AM1_WUP_EVENT_ENABLED   0             /* To enable/disable Wake Up events from ALLMEMS1 nodes                                   */
                                              /* It is present, without the need of any additional board, on the X-NUCLEO-IKS01A2.      */
#define MIC_PRESENT             (0x20 | 0x04) /* MIC on the X-NUCLEO-CCA02M1 (in this case the fw running on the node is the ALLMEMS1). */
#define PLUX_PRESENT            (0x03)        /* Proximity and Lux sensor on X-NUCLEO-6180XA1 (in this case the fw running on the node  */
                                              /* is the FLIGHT1).                                                                       */
     
/** 
 * @brief  Master mode:
 *         (0x01) MODE 1 Slave and Master
 *                       Only 1 connection
 *                       6KB of RAM retention
 *         (0x02) MODE 2 Slave and Master
 *                       Only 1 connection
 *                       12KB of RAM retention
 *         (0x03) MODE 3 Master only (BlueNRG)
 *                       Master and Slave - up to 2 masters (BlueNRG-MS only)
 *                       Up to 8 connections
 *                       12KB of RAM retention
 *         (0x04) MODE 4 Master and Slave - up to 2 masters (BlueNRG-MS only)
 *                       Simultaneous advertising and scanning
 *                       Up to 4 connections
 *                       12KB of RAM retention
 */ 
#define MASTER_MODE (0x03)
     
#define BUF_LEN 27
   
#define ENABLE  1
#define DISABLE 0

#define MANUF_SPECIFIC_TYPE     0xFF
#define STM32_NUCLEO            0x80
#define SENSOR_TILE             0x02
#define BLUE_COIN               0x03
#define SENSOR_TILE_BOX         0x06 /* Matteo4 */
#define SENSOR_VIB1             0x76 /* Matteo5 */
#define SENSOR_VIB2             0x77 /* Matteo5 */
#define WUP_POS                 0x0A
#define MIC_POS                 0x1A
#define PRX_POS                 0x19
#define ACC_POS                 0x17
#define SFUS_POS                0x07
   
/**
 * @brief Feature mask for Temperature1 
 */#define SENSOR_TILE_BOX         0x06 /* Matteo5 */
   
#define FEATURE_MASK_TEMP1 0x00040000

/** 
 * @brief Feature mask for Temperature2 
 */
#define FEATURE_MASK_TEMP2 0x00010000

/** 
 * @brief Feature mask for Pressure 
 */
#define FEATURE_MASK_PRESS 0x00100000

/** 
 * @brief Feature mask for Humidity 
 */
#define FEATURE_MASK_HUM   0x00080000

/** 
 * @brief Feature mask for CO
 */
#define FEATURE_MASK_CO    0x00008000
   
/** 
 * @brief Feature mask for Accelerometer
 */
#define FEATURE_MASK_ACC   0x00800000

/** 
 * @brief Feature mask for Gyroscope
 */
#define FEATURE_MASK_GYR   0x00400000

/** 
 * @brief Feature mask for Magnetometer
 */
#define FEATURE_MASK_MAG   0x00200000
   
/** 
 * @brief Feature mask for Sensor fusion
 */
#define FEATURE_MASK_SENSORFUSION 0x00000100
   
/** 
 * @brief Feature mask for LED
 */
#define FEATURE_MASK_LED_EVENTS    0x20000000
   
/**
 * @brief Feature mask for WakeUp EVENT (here used as Motion Detector)
 */
#define FEATURE_MASK_WAKEUP_EVENTS 0x00000400                                   
   
/**
 * @brief Feature mask for PROXIMITY EVENT 
 */
#define FEATURE_MASK_PROX          0x02000000  

/**
 * @brief Feature mask for MICROPHONE
 */
#define FEATURE_MASK_MIC           0x04000000                                   

/**
 * @brief Command to set the WakeUp notification (here used as Motion Detector)
 */
#define WAKEUP_NOTIFICATION_CMD    'w'
   
/**
 * @brief Command to set the notification frequency
 */
#define NOTIFICATION_FREQ_CMD      255   

/**
 * @brief Notification frequency @50mS (default)
 */
#define NOTIFICATION_FREQ_50     0  
   
/**
 * @brief Notification frequency @100mS
 */
#define NOTIFICATION_FREQ_100    1
 
/**
 * @brief Notification frequency @1S
 */
#define NOTIFICATION_FREQ_1000   10

/**
 * @brief Notification frequency @5S
 */
#define NOTIFICATION_FREQ_5000   50

/**
 * @brief Set the notification frequency to 100ms multiple
 */
#define SENDING_INTERVAL_100MS_MULTIPLE 10
   
/**
 * @brief Notification frequency when wifi is enabled
 */
#define NOTIFICATION_FREQ_WIFI_ON    NOTIFICATION_FREQ_1000 /* reduced to 1000 from NOTIFICATION_FREQ_5000 */

/**
 * @brief Notification frequency when wifi is disabled
 */   
#define NOTIFICATION_FREQ_WIFI_OFF   NOTIFICATION_FREQ_1000

/**
 * @brief Data type length
 */
#define ONE_BYTE_LEN         1
#define TWO_BYTES_LEN        2
   
#define ENV_DATA_LEN_SHORT  8
#define ENV_DATA_LEN_LONG  12
#define PRESS_DATA_LEN      4
#define HUM_DATA_LEN        2
#define TEMP_DATA_LEN       2
#define LED_DATA_LEN        ONE_BYTE_LEN
#define MIC_DATA_LEN        ONE_BYTE_LEN
#define LUX_DATA_LEN        TWO_BYTES_LEN
#define PRX_DATA_LEN        TWO_BYTES_LEN
#define CO_DATA_LEN         4   
#define ACC_DATA_LEN        6
#define GYR_DATA_LEN        6
#define MAG_DATA_LEN        6
#define SFUSION_DATA_LEN    6
#define X_DATA_LEN          2
#define Y_DATA_LEN          2
#define Z_DATA_LEN          2
#define WUP_DATA_LEN        0
#define STATUS_DATA_LEN     0
   
#define TSTAMP_LEN          2
#define NODE_ID_LEN         2
#define TYPE_ID_LEN         1

#define ATTR_HEAD_LEN       TSTAMP_LEN+NODE_ID_LEN+TYPE_ID_LEN
#define ENV_DATA_LEN        PRESS_DATA_LEN+HUM_DATA_LEN+TEMP_DATA_LEN+TEMP_DATA_LEN
#define MEMS_DATA_LEN       ACC_DATA_LEN+GYR_DATA_LEN+MAG_DATA_LEN
   
/* Matteo */
#define AI_DATA_LEN         TWO_BYTES_LEN
   
#define INT_VALUE           1
#define FLOAT_VALUE         2
   
/**
 * @brief Notification frequency 
 */
      
/**
 * @}
 */
   
/** @defgroup STM32F401RE-Nucleo_BLE_MASTER_SERVICE_Exported_Types Exported Types
 *  @{
 */
typedef enum {
  CONN_INIT,                                   // 0
  START_DEVICE_DISCOVERY,                      // 1
  DEVICE_FOUND,                                // 2
  DEVICE_NOT_FOUND,                            // 3
  DEVICE_DISCOVERY_COMPLETE,                   // 4
  START_DEVICE_CONNECTION,                     // 5
  DEVICE_CONNECTED,                            // 6
  DEVICE_CONNECTION_COMPLETE,                  // 7
  START_DISCOVERABLE_MODE,                     // 8
  DISCOVERABLE_MODE_SET,                       // 9
  START_SERVICE_DISCOVERY,                     //10
  SERVICE_DISCOVERED,                          //11
  SERVICE_DISCOVERY_COMPLETE,                  //12
  START_HARDWARE_SERV_CHARS_DISCOVERY,         //13
  HARDWARE_SERV_CHARS_DISCOVERED,              //14
  HARDWARE_SERV_CHARS_DISCOVERY_COMPLETE,      //15
  START_SOFTWARE_SERV_CHARS_DISCOVERY,         //13
  SOFTWARE_SERV_CHARS_DISCOVERED,              //14
  SOFTWARE_SERV_CHARS_DISCOVERY_COMPLETE,      //15
  START_CONFIGURATION_SERV_CHARS_DISCOVERY,    //16
  CONFIGURATION_SERV_CHARS_DISCOVERED,         //17
  CONFIGURATION_SERV_CHARS_DISCOVERY_COMPLETE, //18    
  ENABLE_ME1_LED_NOTIFICATIONS,                //19
  ENABLE_ME1_WUP_NOTIFICATIONS,                //20
  NOTIFICATIONS_ENABLED,                       //21
  READ_INIT,                                   //22
  READING_PRX,                                 //23
  NOTIFY_PRX_TO_CLIENT,                        //24
  READING_ENVIRONMENTAL,                       //25
  NOTIFY_ENV_TO_CLIENT,                        //26
  READING_LED,                                 //27
  NOTIFY_LED_TO_CLIENT,                        //28
  READING_LUX,                                 //29
  NOTIFY_LUX_TO_CLIENT,                        //30
  READING_CO,                                  //31
  NOTIFY_CO_TO_CLIENT,                         //32
  READING_MIC,                                 //33
  NOTIFY_MIC_TO_CLIENT,                        //34
  READING_AGM,                                 //35
  NOTIFY_AGM_TO_CLIENT,                        //36
  READING_SFUSION,                             //37
  NOTIFY_SFUSION_TO_CLIENT,                    //38
  READING_PRESSURE,                            //39
  READING_HUMIDITY,                            //40
  READING_TEMPERATURE,                         //41
  ALL_DATA_READ,                               //42
  NOTIFICATIONS_DATA_READ,                     //43
  DISABLE_NOTIFICATIONS,                       //44
  READING_DISCONNECTION,                        //45
    
  /* Matteo */
  READING_AI_RECOGNITION,                       //46
  NOTIFY_AI_TO_CLIENT,                          //47
  ENABLE_SE1_HAR_NOTIFICATIONS

} Status_t;

typedef struct {
  uint16_t start_h;
  uint16_t end_h;
} Handle_t;

typedef struct {
  uint8_t  dev_v;     //0x01 MOTENV-V2 device ("ME1V2XY"), 0x02 FLIGHT1 device
  tBDAddr  bdaddr;
  uint8_t  addr_type; //0x00 Public Device Address, 0x01 Random Device Address
} devInfo_t;

typedef struct {
  devInfo_t devInfo[MAX_NUM_OF_NODES];
  uint8_t   discovery_enabled;
  uint8_t   device_found;
  uint8_t   connDevices;
  uint8_t   discDevices;  
  uint8_t   connDeviceIdx;
  uint8_t   readDeviceIdx;
  uint16_t  connection_handle[MAX_NUM_OF_NODES];
  uint8_t   is_connected[MAX_NUM_OF_NODES];
  uint8_t   is_disconnected[MAX_NUM_OF_NODES];
  uint8_t   is_unconnectable[MAX_NUM_OF_NODES];
  Handle_t  gen_access_profile_handle[MAX_NUM_OF_NODES];
  Handle_t  gen_attribute_profile_handle[MAX_NUM_OF_NODES];
  Handle_t  hardware_service_handle[MAX_NUM_OF_NODES];
  Handle_t  configuration_service_handle[MAX_NUM_OF_NODES];
  Handle_t  software_service_handle[MAX_NUM_OF_NODES];
  uint16_t  environmental_char_handle[MAX_NUM_OF_NODES];
  uint16_t  led_char_handle[MAX_NUM_OF_NODES];
  uint16_t  cfg_char_handle[MAX_NUM_OF_NODES];
  uint16_t  wup_char_handle[MAX_NUM_OF_NODES];
  uint16_t  mic_char_handle[MAX_NUM_OF_NODES];
  uint16_t  prx_char_handle[MAX_NUM_OF_NODES];
  uint16_t  lux_char_handle[MAX_NUM_OF_NODES];
  uint16_t  co_char_handle[MAX_NUM_OF_NODES];
  uint16_t  agm_char_handle[MAX_NUM_OF_NODES];
  uint16_t  sfusion_char_handle[MAX_NUM_OF_NODES];
  uint8_t   led_char_read[MAX_NUM_OF_NODES];
  uint8_t   wup_char_read[MAX_NUM_OF_NODES];
  uint8_t   mic_char_read[MAX_NUM_OF_NODES];
  uint8_t   prx_char_read[MAX_NUM_OF_NODES];
  uint8_t   agm_char_read[MAX_NUM_OF_NODES];
  uint8_t   sfusion_char_read[MAX_NUM_OF_NODES];
  uint8_t   wup_event[MAX_NUM_OF_NODES];
  uint8_t   mic_event[MAX_NUM_OF_NODES];
  uint8_t   prx_event[MAX_NUM_OF_NODES];
  uint8_t   agm_event[MAX_NUM_OF_NODES];
  uint8_t   sfusion_event[MAX_NUM_OF_NODES];
  uint8_t   prx_on[MAX_NUM_OF_NODES];
  uint8_t   agm_on[MAX_NUM_OF_NODES];
  uint8_t   sfusion_on[MAX_NUM_OF_NODES];
  uint8_t   mic_event_enabled;
  uint8_t   prx_event_enabled;
  uint8_t   agm_event_enabled;
  uint8_t   sfusion_event_enabled;
  uint8_t   wup_event_enabled;
  uint8_t   acc_event_enabled;
  uint8_t   gyr_event_enabled;
  uint8_t   mag_event_enabled;
  Status_t  status;  
  
  /* Matteo */
  uint16_t  AI_char_handle[MAX_NUM_OF_NODES];
  
} PeripheralDevices_t;

typedef enum {
  UNKNOWN_TYPE_ID,  //0
  PRESS_TYPE_ID,    //1   (its data value is 4 bytes long)
  HUM_TYPE_ID,      //2   (its data value is 2 bytes long)
  TEMP_TYPE_ID,     //3   (its data value is 2 bytes long)
  LED_TYPE_ID,      //4   (its data value is 1 byte long)
  PRX_TYPE_ID,      //5   (its data value is 2 bytes long)
  WUP_TYPE_ID,      //6   (no data value)
  MICLEVEL_TYPE_ID, //7   (its data value is 1 byte long)
  LUX_TYPE_ID,      //8   (its data value is 2 bytes long)
  ACC_TYPE_ID,      //9   (its data value is 6 bytes long)  
  GYR_TYPE_ID,      //10  (its data value is 6 bytes long)
  MAG_TYPE_ID,      //11  (its data value is 6 bytes long)
  STATUS_TYPE_ID,   //12  (no data value)
  SFUSION_TYPE_ID,  //13  (its data value is 8 bytes long)
  CO_TYPE_ID,        //14  (its data value is 4 bytes long)
                    //    (increase the enum to add new data type IDs)
  /* Matteo */
  AI_REC_ID         //15  
  
} Data_Type_ID_t;

/**
 * @}
 */

/** @defgroup STM32F401RE-Nucleo_BLE_MASTER_SERVICE_Exported_Functions Exported Functions
 *  @{
 */
void    BlueNRG_Init                   (void);
void    BlueNRG_GetVersion             (uint8_t *hwVersion, uint16_t *fwVersion);
void    Set_Random_Address             (uint8_t* bdaddr, uint8_t hwVersion, uint16_t fwVersion);
uint8_t Get_BlueNRG_Exp_Board_Type     (void);
void    Set_BlueNRG_Exp_Board_Type     (uint8_t hwVersion);
void    Set_New_Status                 (void);
void    Init_Processes                 (void);
void    Start_Discovery                (void);
void    Save_Device_Found              (uint8_t adv_type, uint8_t* addr_type, tBDAddr addr,
                                        uint8_t* data_length, uint8_t* data_RSSI, uint8_t pos, uint8_t dev_v, 
                                        uint8_t wup_event, uint8_t mic_event, uint8_t prx_event, 
                                        uint8_t agm_event, uint8_t sfusion_event);
void    Connect_Peripheral             (void);
void    Discover_Services              (void);
void    Discover_Service_Chars         (Handle_t* service_handle, char* serv_name);
void    Enable_Notifications           (uint16_t conn_handle, uint8_t index, uint8_t dev_v, uint8_t set_mode);
void    Read_Sensor_Data               (uint8_t index, uint16_t connection_handle, uint16_t attr_value_handle);
void    Primary_Services_CB            (uint16_t handle, uint8_t data_length,
                                        uint8_t attribute_data_length, uint8_t *attribute_data_list);
void    Service_Chars_CB               (uint16_t handle, uint8_t data_length, uint8_t handle_value_pair_length,
                                        uint8_t *handle_value_pair);
void    Build_MEMS_Packet              (uint8_t *attr_value, char *data_type, tBDAddr devAddr, uint8_t num_type);

void    Char_Read_Value_CB             (uint16_t handle, uint8_t data_length, uint8_t* attribute_value);
void    GAP_ConnectionComplete_CB      (uint8_t addr[6], uint16_t handle, uint8_t role);
void    GAP_DisconnectionComplete_CB   (uint16_t conn_handle);
void    GAP_Start_General_Discovery_CB (Status_t status, uint8_t adv_type,
                                        uint8_t* addr_type, tBDAddr addr,
                                        uint8_t* data_length,
                                        uint8_t* data, uint8_t* RSSI);
void    GATT_Procedure_Complete_CB     (uint16_t conn_handle, uint8_t error_code);
void    Create_New_Attr_Value          (uint8_t *tstamp, tBDAddr  devAddr, uint8_t data_type_id, uint8_t *data, uint8_t data_length);
void    GATT_Notification_CB           (uint16_t conn_handle, uint16_t attr_handle, uint8_t attr_len,
                                        uint8_t *attr_value);
void    HCI_Event_CB                   (void *pckt);
void    Connection_Process             (void);
void    Reading_Process                (void);
void    Get_Device_From_Conn_Handle    (uint16_t handle, uint8_t *index, tBDAddr devAddr);
void    Write_Charac_Descriptor        (uint16_t conn_handle, uint16_t attr_handle, uint8_t attr_len,
                                        uint8_t* attr_value);
void    Write_Charac_Value_Without_Resp (uint16_t conn_handle, uint16_t attr_handle, uint8_t attr_len, 
                                         uint8_t* attr_value);
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

#endif /* _BLE_MASTER_SERVICE_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

