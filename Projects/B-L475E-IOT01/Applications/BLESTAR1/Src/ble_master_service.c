/**
  ******************************************************************************
  * @file    ble_master_service.c
  * @author  Central LAB
  * @version V1.0.0
  * @date    11-Nov-2015
  * @brief   Add BLE master service using a vendor specific profile.
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
/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "ble_master_service.h"
#include "ble_slave_service.h"
#include "btle_config.h"
#include "gp_timer.h"

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
 
/** @defgroup B-L475E-IOT01_BLE_MASTER_SERVICE BLE_MASTER_SERVICE
 * @{
 */

/** @defgroup B-L475E-IOT01_BLE_MASTER_SERVICE_Private_Variables Private Variables
 * @{
 */
/* Private variables ---------------------------------------------------------*/
/**
 * @brief Properties mask
 */
static uint8_t props_mask[] = {
  0x01,
  0x02,
  0x04,
  0x08,
  0x10,
  0x20,
  0x40,
  0x80
};

extern char print_msg_buff[512];
extern UART_HandleTypeDef UartMsgHandle;

extern uint8_t wifi_present;
extern uint8_t wifi_data[256];
extern volatile uint8_t new_data;
extern volatile uint8_t *data;

volatile uint16_t connection_handle = 0;
uint8_t attribute_value[20];
uint8_t star_attr_value[256];

PeripheralDevices_t perDevs;
extern SlaveDevice_t slaveDev;
extern uint8_t notification_freq;

/* Primary Service UUID expected from Sensor demo peripherals */
uint8_t GENERIC_ACCESS_PROFILE_UUID[]    = {0x00, 0x18};
uint8_t GENERIC_ATTRIBUTE_PROFILE_UUID[] = {0x01, 0x18};

/* Services UUID */
uint8_t HARDWARE_SERVICE_UUID[]      = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0xb4,0x9a,0xe1,0x11,0x01,0x00,0x00,0x00,0x00,0x00};
uint8_t CONFIG_SERVICE_UUID[]        = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0xb4,0x9a,0xe1,0x11,0x0F,0x00,0x00,0x00,0x00,0x00};
uint8_t SOFTWARE_SERVICE_UUID[]      = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0xb4,0x9a,0xe1,0x11,0x02,0x00,0x00,0x00,0x00,0x00};

/* Characteristics UUID */
/* ENVIRONMENTAL */
uint8_t ENVIRONMENTAL_CHAR_UUID[]    = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x1d,0x00};
/* ENVIRONMENTAL for Sensor Tile */
uint8_t ENVIRONMENTAL_ST_CHAR_UUID[] = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x14,0x00};
/* ACC + GYRO + MAG */
uint8_t ACCGYROMAG_CHAR_UUID[]       = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0xE0,0x00};
/* MEMS SENSOR FUSION */
uint8_t SFUSION_CHAR_UUID[]          = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x01,0x00,0x00};
/* LED */
uint8_t LED_CHAR_UUID[]              = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x00,0x20};
/* WAKEUP */
uint8_t WAKEUP_EVENT_CHAR_UUID[]     = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x04,0x00,0x00};
/* MICROPHONE */
uint8_t MIC_EVENT_CHAR_UUID[]        = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x00,0x04};
/* PROXIMITY */
uint8_t PROXIMITY_CHAR_UUID[]        = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x00,0x02};
/* LUX */
uint8_t LUX_CHAR_UUID[]              = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x00,0x01};
/* CO */
uint8_t CO_CHAR_UUID[]               = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x80,0x00,0x00};                                        
/* CONFIGURATION */
uint8_t CONFIG_CHAR_UUID[]           = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x0F,0x00,0x02,0x00,0x00,0x00};

/**
 * @}
 */

/** @defgroup STM32F401RE-Nucleo_BLE_MASTER_SERVICE_Private_Defines Private Defines
 * @{
 */
/* Proximity range */
#define HIGH_RANGE_DATA_MAX 0x7FFE
#define LOW_RANGE_DATA_MAX  0x00FE
#define OUT_OF_RANGE_VALUE  0xFFFF
/**
 * @}
 */

/** @defgroup B-L475E-IOT01_BLE_MASTER_SERVICE_Exported_Functions Exported Functions
 * @{
 */

/**
 * @brief  Set a random BLE MAC address
 * @param  Pointer to array where to save the BLE MAC address 
 * @param  Hardware version
 * @param  Firmware version
 * @retval None
 */
void Set_Random_Address(uint8_t* bdaddr, uint8_t hwVersion, uint16_t fwVersion)
{  
  /* Create a Random BLE MAC */
  const char BoardName[8] = {NAME_BLESTAR1};
  int i;
  
  i = atoi((char *)BoardName+4); /* For creating MAC address related also to firmware version */
  /* initialize random seed: */
  srand (STM32_UUID[0] + STM32_UUID[1] + STM32_UUID[2] + (hwVersion+fwVersion)*i);
  
  for(i=0;i<5;i++) {
    bdaddr[i] = rand()&0xFF;
  }
  bdaddr[i] = 0xD0; 
}
   
/**
 * @brief  Get the BlueNRG HW and FW version
 * @param  Pointer to variable where to save hardware version number code 
 * @param  Pointer to variable where to save firmware version number code
 * @retval None
 */
void BlueNRG_GetVersion(uint8_t *hwVersion, uint16_t *fwVersion)
{
  /* get the BlueNRG HW and FW versions */
  getBlueNRGVersion(hwVersion, fwVersion);
  
  STAR_PRINTF("BlueNRG HW ver 0x%04x, FW ver 0x%04x\n", *hwVersion, *fwVersion);
  
  return;
}

/**
 * @brief  Initialize the struct where all peripheral devices are saved
 * @param  None 
 * @retval None
 */
void Init_Processes(void)
{
  int i;

  perDevs.discovery_enabled                         = TRUE;
  perDevs.device_found                              = FALSE;
  perDevs.connDevices                               = 0;
  perDevs.discDevices                               = 0;  
  perDevs.connDeviceIdx                             = 0;
  perDevs.readDeviceIdx                             = 0;
  for (i=0; i<MAX_NUM_OF_NODES; i++) {    
    Osal_MemSet(perDevs.devInfo[i].bdaddr, 0, 6);
    perDevs.devInfo[i].dev_v                        = 0;
    perDevs.connection_handle[i]                    = 0;    
    perDevs.is_connected[i]                         = FALSE;
    perDevs.is_disconnected[i]                      = TRUE;
    perDevs.is_unconnectable[i]                     = FALSE;
    perDevs.gen_access_profile_handle[i].start_h    = 0;
    perDevs.gen_access_profile_handle[i].end_h      = 0;
    perDevs.gen_attribute_profile_handle[i].start_h = 0;
    perDevs.gen_attribute_profile_handle[i].end_h   = 0;
    perDevs.hardware_service_handle[i].start_h      = 0;
    perDevs.hardware_service_handle[i].end_h        = 0;
    perDevs.software_service_handle[i].start_h      = 0;
    perDevs.software_service_handle[i].end_h        = 0;
    perDevs.environmental_char_handle[i]            = 0;
    perDevs.led_char_handle[i]                      = 0;
    perDevs.cfg_char_handle[i]                      = 0;
    perDevs.wup_char_handle[i]                      = 0;
    perDevs.prx_char_handle[i]                      = 0;
    perDevs.lux_char_handle[i]                      = 0;
    perDevs.co_char_handle[i]                       = 0;
    perDevs.agm_char_handle[i]                      = 0;
    perDevs.sfusion_char_handle[i]                  = 0;
    perDevs.led_char_read[i]                        = 0;
    perDevs.wup_char_read[i]                        = 0;
    perDevs.mic_char_read[i]                        = 0;
    perDevs.prx_char_read[i]                        = 0;
    perDevs.agm_char_read[i]                        = 0;
    perDevs.sfusion_char_read[i]                    = 0;
    perDevs.wup_event[i]                            = 0;
    perDevs.mic_event[i]                            = 0;    
    perDevs.prx_on[i]                               = 0;
    perDevs.agm_on[i]                               = 0;
    perDevs.sfusion_on[i]                           = 0;
  }
  perDevs.mic_event_enabled                         = 0;
  perDevs.prx_event_enabled                         = 0;
  perDevs.agm_event_enabled                         = 0;
  perDevs.sfusion_event_enabled                     = 0;
  perDevs.acc_event_enabled                         = 0;
  perDevs.gyr_event_enabled                         = 0;
  perDevs.mag_event_enabled                         = 0;
  perDevs.status = CONN_INIT;  
}

/**
 * @brief  Start searching for a peripheral device
 * @param  None 
 * @retval None
 */
void Start_Discovery(void)
{
  int ret; 
  
  /**
   * Turn off the discovery mode for the slave role since the simultaneous
   * scanning and advertising is not supported in Mode 3
   */
  if ((slaveDev.is_connected == FALSE) && (slaveDev.is_discoverable == TRUE))
  {
    ret = aci_gap_set_non_discoverable();
    if (ret != BLE_STATUS_SUCCESS){
      STAR_PRINTF("Error while setting non discoverable mode (0x%04x).\n", ret);
    }     
    HAL_Delay(100);
    slaveDev.is_discoverable = FALSE;
  }
  
  perDevs.status = START_DEVICE_DISCOVERY;
  
  /* scanInterval, scanWindow, own_address_type, filterDuplicates */
  ret = aci_gap_start_general_discovery_proc(SCAN_P, SCAN_L, PUBLIC_ADDR, 1);
  
  if (ret != BLE_STATUS_SUCCESS){
    STAR_PRINTF("Error while starting general discovery.\n");
    GAP_Start_General_Discovery_CB(DEVICE_DISCOVERY_COMPLETE, 0, NULL, NULL, NULL, NULL, NULL);
  }
  
}

/**
 * @brief  Save the found device in the struct containing all the found devices
 * @param  adv_type
 * @param  addr_type
 * @param  addr
 * @param  data_length
 * @param  data
 * @param  position in the struct
 * @param  device version
 * @param  acc event
 * @param  mic event
 * @retval None
 */
void Save_Device_Found(uint8_t adv_type, uint8_t* addr_type, tBDAddr addr,
                       uint8_t* data_length, uint8_t* data_RSSI, uint8_t pos, 
                       uint8_t dev_v, uint8_t wup_event, uint8_t mic_event, 
                       uint8_t prx_event, uint8_t agm_event, uint8_t sfusion_event)
{  
  perDevs.devInfo[pos].dev_v = dev_v;
  perDevs.wup_event[pos] = wup_event;
  perDevs.mic_event[pos] = mic_event;
  perDevs.prx_event[pos] = prx_event;
  perDevs.agm_event[pos] = agm_event;
  perDevs.sfusion_event[pos] = sfusion_event;
  Osal_MemCpy(perDevs.devInfo[pos].bdaddr, addr, 6);
  perDevs.devInfo[pos].addr_type = *addr_type;
  LOC_DEBUG("Saved device with addr[%02x %02x %02x %02x %02x %02x], addr_type 0x%02x, dev_v 0x%02x, wup_event %d, mic_event %d, prx_event %d, agm_event %d, sfusion_event %d, at pos %d\n\n",            
            perDevs.devInfo[pos].bdaddr[5],
            perDevs.devInfo[pos].bdaddr[4],
            perDevs.devInfo[pos].bdaddr[3],
            perDevs.devInfo[pos].bdaddr[2],
            perDevs.devInfo[pos].bdaddr[1],
            perDevs.devInfo[pos].bdaddr[0],
            perDevs.devInfo[pos].addr_type,
            perDevs.devInfo[pos].dev_v,
            perDevs.wup_event[pos],
            perDevs.mic_event[pos],
            perDevs.prx_event[pos],
            perDevs.agm_event[pos],
            perDevs.sfusion_event[pos],
            pos);  
}

/**
 * @brief  Connect the device as Master to a peripheral device
 * @param  None
 * @retval None
 */
void Connect_Peripheral(void)
{ 
  uint8_t index;   
  
  index = perDevs.connDeviceIdx;
  STAR_PRINTF("Client Create Connection with peripheral %d at pos %d\n",
         perDevs.connDevices+1, index+1);

  /*
   * Scan_Interval, Scan_Window, Peer_Address_Type, Peer_Address, Own_Address_Type, Conn_Interval_Min, 
   * Conn_Interval_Max, Conn_Latency, Supervision_Timeout, Conn_Len_Min, Conn_Len_Max    
   */  
  if (aci_gap_create_connection(SCAN_P, SCAN_L, perDevs.devInfo[index].addr_type,
                                perDevs.devInfo[index].bdaddr,
                                PUBLIC_ADDR, CONN_P1, CONN_P2, 0, SUPERV_TIMEOUT, CONN_L1, CONN_L2)) {
                                  
    STAR_PRINTF("Error while starting connection with peripheral %d (%02x%02x).\n", index+1, perDevs.devInfo[index].bdaddr[NODE_ID_B2], perDevs.devInfo[index].bdaddr[NODE_ID_B1]);
    Clock_Wait(100);
    perDevs.is_unconnectable[index] = TRUE; 
    GAP_ConnectionComplete_CB(perDevs.devInfo[index].bdaddr, 0, 0);
  }
}

/**
 * @brief  Services discovery
 * @param  None 
 * @retval None
 */
void Discover_Services(void)
{
  uint8_t  index = perDevs.connDeviceIdx;  
  uint16_t conn_handle = perDevs.connection_handle[index];
  uint8_t  ret;

  STAR_PRINTF("Discovering services for peripheral %d (0x%04x)...\n",
          index+1, conn_handle);
  if (perDevs.is_disconnected[index]) {
    STAR_PRINTF("Peripheral %d (0x%04x) disconnected\n", index+1, conn_handle);    
    GATT_Procedure_Complete_CB(conn_handle, BLE_STATUS_SUCCESS);
  }
  else {
    ret = aci_gatt_disc_all_prim_services(conn_handle);
    if (ret) {
      STAR_PRINTF("Error while discovering primary services (0x%02x)", ret);      
      GATT_Procedure_Complete_CB(conn_handle, BLE_STATUS_ERROR);    
    }    
  }
}

/**
 * @brief  Service Characteristics discovery
 * @param  None 
 * @retval None
 */
void Discover_Service_Chars(Handle_t* service_handle, char* serv_name)
{
  uint8_t  index = perDevs.connDeviceIdx;
  uint16_t conn_handle = perDevs.connection_handle[index];
  uint8_t  ret;

  STAR_PRINTF("Discovering %s Sevice Chars for peripheral %d (0x%04x)...\n",
         serv_name, index+1, conn_handle);
  if (perDevs.is_disconnected[index]) {
    STAR_PRINTF("Peripheral %d (0x%04x) disconnected\n", index+1, conn_handle);    
    GATT_Procedure_Complete_CB(conn_handle, BLE_STATUS_SUCCESS);
  }
  else {
    ret = aci_gatt_disc_all_charac_of_serv(conn_handle,
                                           service_handle[index].start_h,
                                           service_handle[index].end_h);
    if (ret != 0) {
      STAR_PRINTF("Error while discovering %s Service Chars\n", serv_name);      
      GATT_Procedure_Complete_CB(conn_handle, BLE_STATUS_ERROR);
    }
  }
}

/**
 * @brief  This function is called when a service is discovered
 * @param  connection handle
 * @param  event data length
 * @param  attribute data length
 * @param  attribute data list
 * @retval None
 */
void Primary_Services_CB(uint16_t handle, uint8_t event_data_length,
                         uint8_t attribute_data_length, uint8_t *attribute_data_list)
{
  uint16_t startHandle, endHandle;
  uint8_t  uuid[16];
  uint8_t  i, offset, numAttr;

  numAttr = (event_data_length - 1) / attribute_data_length;

  offset = 0;
  for (i=0; i<numAttr; i++) {
    startHandle = attribute_data_list[offset];
    endHandle = attribute_data_list[offset+2];

    //UUID Type
    if (attribute_data_length == 6) {      
      LOC_DEBUG("UUID_TYPE_16\n");
      uuid[0] = attribute_data_list[offset+4];
      uuid[1] = attribute_data_list[offset+5];
      LOC_DEBUG("S UUID-%02x%02x attrs[%u %u]\n", uuid[0], uuid[1], startHandle, endHandle);      
    }
    else {    
      LOC_DEBUG("UUID_TYPE_128\n");
      memcpy(uuid, attribute_data_list+offset+4, sizeof(uuid));
      LOC_DEBUG("S UUID-");
      for (uint8_t j = 0; j < 16; j++) {
        LOC_DEBUG("%02x", uuid[j]);
      }
      LOC_DEBUG(" attrs[%u %u]\n", startHandle, endHandle);      
    }
    
    if (memcmp(uuid, GENERIC_ACCESS_PROFILE_UUID, (attribute_data_length-4)) == 0) {
      STAR_PRINTF("  - Generic Access Profile (GAP)\n");
      perDevs.gen_access_profile_handle[perDevs.connDeviceIdx].start_h = startHandle;
      perDevs.gen_access_profile_handle[perDevs.connDeviceIdx].end_h   = endHandle;
    }
    else if (memcmp (uuid, GENERIC_ATTRIBUTE_PROFILE_UUID, (attribute_data_length-4)) == 0) {
      STAR_PRINTF("  - Generic Attribute Profile (GATT)\n");
      perDevs.gen_attribute_profile_handle[perDevs.connDeviceIdx].start_h = startHandle;
      perDevs.gen_attribute_profile_handle[perDevs.connDeviceIdx].end_h   = endHandle;
    }
    else if (memcmp (uuid, HARDWARE_SERVICE_UUID, (attribute_data_length-4)) == 0) {
      STAR_PRINTF("  - HARDWARE Service\n");
      perDevs.hardware_service_handle[perDevs.connDeviceIdx].start_h = startHandle;
      perDevs.hardware_service_handle[perDevs.connDeviceIdx].end_h   = endHandle;
    }
    else if (memcmp (uuid, CONFIG_SERVICE_UUID, (attribute_data_length-4)) == 0) {
      STAR_PRINTF("  - CFG Service\n");
      perDevs.configuration_service_handle[perDevs.connDeviceIdx].start_h = startHandle;
      perDevs.configuration_service_handle[perDevs.connDeviceIdx].end_h   = endHandle;
    }  
    else if (memcmp (uuid, SOFTWARE_SERVICE_UUID, (attribute_data_length-4)) == 0) {
      STAR_PRINTF("  - SOFTWARE Service\n");
      perDevs.software_service_handle[perDevs.connDeviceIdx].start_h = startHandle;
      perDevs.software_service_handle[perDevs.connDeviceIdx].end_h   = endHandle;
    }
    else {
      STAR_PRINTF("  - UNKNOWN Service\n");
    }
    
    offset += attribute_data_length;
  }

  STAR_PRINTF("  Service Discovery complete (numAttr=%u).\n", numAttr);
  
}

/**
 * @brief  This function is called when a characteristic is discovered
 * @param  connection handle
 * @param  event data length
 * @param  handle value pair length
 * @param  handle value pair
 * @retval None
 */
void Service_Chars_CB(uint16_t handle, uint8_t event_data_length, uint8_t handle_value_pair_length,
                      uint8_t *handle_value_pair)
{
  uint16_t declHandle, valueHandle;
  uint8_t  uuid[16];
  uint8_t  i, numChar, offset;
  uint8_t  print_properties = 0;
  uint8_t  idx = perDevs.connDeviceIdx;

  numChar = (event_data_length - 1) / handle_value_pair_length;

  offset = 0;
  for (i=0; i<numChar; i++) {
    // UUID Type
    if (handle_value_pair_length == 7) {
      LOC_DEBUG("Char UUID_TYPE_16\n");
      uuid[0] = handle_value_pair[offset+5];
      uuid[1] = handle_value_pair[offset+6];       
      LOC_DEBUG("C UUID-%02x%02x\n", uuid[0], uuid[1]);
    } else {
      LOC_DEBUG("Char UUID_TYPE_128\n\r");      
      memcpy(uuid, handle_value_pair+offset+5, sizeof(uuid));
      LOC_DEBUG("C UUID-");      
      for (uint8_t j = 0; j < 16; j++) {
        LOC_DEBUG("%02x", uuid[j]);
      }
      LOC_DEBUG("\n");
    }
    
    /**
     * Properties
     */
    LOC_DEBUG("Characteristic Properties:\n");        
    uint8_t broadcast       = (props_mask[0] & handle_value_pair[offset+2]);
    uint8_t read            = (props_mask[1] & handle_value_pair[offset+2])>>1;
    uint8_t writeWoResp     = (props_mask[2] & handle_value_pair[offset+2])>>2;
    uint8_t write           = (props_mask[3] & handle_value_pair[offset+2])>>3;
    uint8_t notify          = (props_mask[4] & handle_value_pair[offset+2])>>4;
    uint8_t indicate        = (props_mask[5] & handle_value_pair[offset+2])>>5;
    uint8_t authSignedWrite = (props_mask[6] & handle_value_pair[offset+2])>>6;    

    /*
     * Handles
     */
    declHandle  = handle_value_pair[offset];
    valueHandle = handle_value_pair[offset+3];

    if ((memcmp (uuid, ENVIRONMENTAL_CHAR_UUID, (handle_value_pair_length-5))    == 0) ||
        (memcmp (uuid, ENVIRONMENTAL_ST_CHAR_UUID, (handle_value_pair_length-5)) == 0))
    {
      perDevs.environmental_char_handle[idx] = declHandle;
      STAR_PRINTF("  - ENV Characteristic of periph %d (declH 0x%04x, valueH 0x%04x)\n", idx+1, declHandle, valueHandle);
      print_properties = 0; /* 1 to enable log print for this char */
    } 
    else if (memcmp (uuid, LED_CHAR_UUID, (handle_value_pair_length-5)) == 0) {
      perDevs.led_char_handle[idx] = declHandle;
      perDevs.led_char_read[idx]   = 0;
      STAR_PRINTF("  - LED Characteristic of periph %d (declH 0x%04x, valueH 0x%04x)\n", idx+1, declHandle, valueHandle);
      print_properties = 0; /* 1 to enable log print for this char */
    }
    else if (memcmp (uuid, CONFIG_CHAR_UUID, (handle_value_pair_length-5)) == 0) {
      perDevs.cfg_char_handle[idx] = declHandle;
      STAR_PRINTF("  - CFG Characteristic of periph %d (declH 0x%04x, valueH 0x%04x)\n", idx+1, declHandle, valueHandle);
      print_properties = 0; /* 1 to enable log print for this char */
    }
    else if (memcmp (uuid, WAKEUP_EVENT_CHAR_UUID, (handle_value_pair_length-5)) == 0) {
      perDevs.wup_char_handle[idx] = declHandle;
      perDevs.wup_char_read[idx]   = 0;
      STAR_PRINTF("  - WUP Characteristic of periph %d (declH 0x%04x, valueH 0x%04x)\n", idx+1, declHandle, valueHandle);
      print_properties = 0; /* 1 to enable log print for this char */
    }
    else if (memcmp (uuid, MIC_EVENT_CHAR_UUID, (handle_value_pair_length-5)) == 0) {
      perDevs.mic_char_handle[idx] = declHandle;
      perDevs.mic_char_read[idx]   = 0;      
      STAR_PRINTF("  - MIC Characteristic of periph %d (declH 0x%04x, valueH 0x%04x)\n", idx+1, declHandle, valueHandle);
      print_properties = 0; /* 1 to enable log print for this char */
    }
    else if (memcmp (uuid, PROXIMITY_CHAR_UUID, (handle_value_pair_length-5)) == 0) {
      perDevs.prx_char_handle[idx] = declHandle;
      perDevs.prx_char_read[idx]   = 0;
      perDevs.prx_on[i]            = 0;
      STAR_PRINTF("  - PRX Characteristic of periph %d (declH 0x%04x, valueH 0x%04x)\n", idx+1, declHandle, valueHandle);
      print_properties = 0; /* 1 to enable log print for this char */
    }
    else if (memcmp (uuid, LUX_CHAR_UUID, (handle_value_pair_length-5)) == 0) {
      perDevs.lux_char_handle[idx] = declHandle;
      STAR_PRINTF("  - LUX Characteristic of periph %d (declH 0x%04x, valueH 0x%04x)\n", idx+1, declHandle, valueHandle);
      print_properties = 0; /* 1 to enable log print for this char */
    }    
    else if (memcmp (uuid, CO_CHAR_UUID, (handle_value_pair_length-5)) == 0) {
      perDevs.co_char_handle[idx] = declHandle;
      STAR_PRINTF("  - CO Characteristic of periph %d (declH 0x%04x, valueH 0x%04x)\n", idx+1, declHandle, valueHandle);
      print_properties = 0; /* 1 to enable log print for this char */
    }  
    else if (memcmp (uuid, ACCGYROMAG_CHAR_UUID, (handle_value_pair_length-5)) == 0) {
      perDevs.agm_char_handle[idx] = declHandle;
      perDevs.agm_char_read[idx]   = 0;
      perDevs.agm_on[i]            = 0;
      STAR_PRINTF("  - AGM Characteristic of periph %d (declH 0x%04x, valueH 0x%04x)\n", idx+1, declHandle, valueHandle);
      print_properties = 0; /* 1 to enable log print for this char */
    }
    else if (memcmp (uuid, SFUSION_CHAR_UUID, (handle_value_pair_length-5)) == 0) {
      perDevs.sfusion_char_handle[idx] = declHandle;
      perDevs.sfusion_char_read[idx]   = 0;
      perDevs.sfusion_on[i]            = 0;
      STAR_PRINTF("  - SFUS Characteristic of periph %d (declH 0x%04x, valueH 0x%04x)\n", idx+1, declHandle, valueHandle);
      print_properties = 0; /* 1 to enable log print for this char */
    }
    else {
      STAR_PRINTF("  - UNKNOWN Characteristic of periph %d\n", idx+1);
    }
    /* log print */    
    if (print_properties) {
      STAR_PRINTF("    broadcast       = %d\n", broadcast);
      STAR_PRINTF("    read            = %d\n", read);
      STAR_PRINTF("    writeWoResp     = %d\n", writeWoResp);
      STAR_PRINTF("    write           = %d\n", write);
      STAR_PRINTF("    notify          = %d\n", notify);
      STAR_PRINTF("    indicate        = %d\n", indicate);
      STAR_PRINTF("    authSignedWrite = %d\n", authSignedWrite);
    }
    offset += handle_value_pair_length;
  }
    
  STAR_PRINTF("  Characteristic Discovery complete (numChar=%d).\n", numChar);
}

/**
 * @brief  Enable characteristic notifications 
 * @param  Connection Handle
 * @param  Position of the peripheral in the struct containing all peripherals
 * @param  Peripheral node type
 * @param  uint8_t indicating if the notification has to be enabled (1) or disabled (0)
 * @retval None
 */
void Enable_Notifications(uint16_t conn_handle, uint8_t index, uint8_t dev_v, uint8_t set_mode)
{
  uint16_t attr_handle = 0x00;
  uint8_t  attr_value[6]={0,0,0,0,0,0}; 
  uint8_t  attr_len;
    
  char* event_type;
  
  switch (perDevs.status) {    
    case ENABLE_ME1_LED_NOTIFICATIONS:
    {      
      event_type = "LED";
      attr_handle = perDevs.led_char_handle[index] + 2;
    }
    break;
    case ENABLE_ME1_WUP_NOTIFICATIONS:
    {             
      event_type = "WUPEVT";
      //Enable/Disable the WakeUp notifications on the Motenv1 (here used as Motion Detector) 
      Set_Notification_Property(conn_handle, index, FEATURE_MASK_WAKEUP_EVENTS, WAKEUP_NOTIFICATION_CMD, set_mode);
            
      attr_handle = perDevs.wup_char_handle[index] + 2;    
    }
    break;
    default:
    {
      event_type = "UNKNOWN";
    }
    break;
  }

  attr_value[0] = set_mode;
  attr_len = 2;
      
  STAR_PRINTF("%s notifications set to %d for periph %d (0x%04x)\n", event_type, set_mode, index+1, conn_handle);
  
  Write_Charac_Descriptor(conn_handle, attr_handle, attr_len, attr_value);
    
  LOC_DEBUG("Return from attempting to write %s char\n", node_type);
}

/**
 * @brief  Read sensor data
 * @param  None 
 * @retval None
 */
void Read_Sensor_Data(uint8_t index, uint16_t connection_handle, uint16_t attr_value_handle)
{     
  int ret = BLE_STATUS_SUCCESS;
  
  if (!perDevs.is_disconnected[index] && attr_value_handle) {
    LOC_DEBUG("Reading sensor data from periph %d (0x%04x - 0x%04x) - status=%d\n", index+1, connection_handle, attr_value_handle, perDevs.status);
    ret = aci_gatt_read_charac_val(connection_handle, attr_value_handle);
    if (ret) {
      STAR_PRINTF("Unable to read data from periph %d (err 0x%02x, 0x%04x - 0x%04x)\n", index+1, ret, connection_handle, attr_value_handle);      
      GATT_Procedure_Complete_CB(connection_handle, BLE_STATUS_ERROR);
    }    
  }
  else {
    GATT_Procedure_Complete_CB(connection_handle, BLE_STATUS_SUCCESS);
  }
}

/**
 * @brief  This function builds a Jason format string
 * @param  attribute value
 * @param  string indicating the data type (Acc, Gyr or Mag)
 * @param  address of the device
 * @param  data type (integer or float)
 * @retval None
 */
void Build_MEMS_Packet(uint8_t *attr_value, char *data_type, tBDAddr devAddr, uint8_t num_type)
{  
  int32_t tmp = 0;
  uint8_t tmp_data[256];
  char* sign = "";
  char axes[3] = {'X','Y','Z'};
  uint8_t i, j = 0;
  
  for (i=0; i<5; i++)
  {
    tmp = ((int8_t)attr_value[i+1]<<8) | attr_value[i];
    
    if (num_type==1) {    
      if (i==0) {
        sprintf((char *)wifi_data, "\"%s%c_0x%02x%02x\":%ld; ", data_type, axes[j], devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp);
      }
      else {
        sprintf((char *)tmp_data,  "\"%s%c_0x%02x%02x\":%ld; ", data_type, axes[j], devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp);
        strcat ((char *)wifi_data, (char *)tmp_data);    
      }
    }
    else {        
      sign = (tmp < 0) ? "-" : "";
      tmp = (tmp < 0) ? (-tmp) : (tmp);
      if (i==0) {
        sprintf((char *)wifi_data, "\"%s%c_0x%02x%02x\":%s%ld.%ld; ", data_type, axes[j], devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], sign, tmp/10, tmp%10);
      }
      else {
        sprintf((char *)tmp_data,  "\"%s%c_0x%02x%02x\":%s%ld.%ld; ", data_type, axes[j], devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], sign, tmp/10, tmp%10);
        strcat ((char *)wifi_data, (char *)tmp_data);
      }
    } 
    i++;
    j++;
  } 
}

/**
 * @brief  This function is called when a characteristic is discovered
 * @param  connection handle
 * @param  event data length
 * @param  attribute value
 * @retval None
 */
void Char_Read_Value_CB(uint16_t handle, uint8_t data_length, uint8_t* attr_value)
{
  int32_t  tmp = 0;
  uint8_t  tmp_data[256];
  uint8_t  index;
  tBDAddr  devAddr;
  uint16_t attribute_handle;
  uint8_t  new_buffer = 0;
      
  Get_Device_From_Conn_Handle(handle, &index, devAddr);
  
  /*
   * Building the buffer in JSON format
   */  
  switch (perDevs.status)
  {
  case READING_ENVIRONMENTAL:    
    {      
      /* P in mBar, H in percentage, T2 and T1 value in Celtius degree */    
      if ((data_length != ENV_DATA_LEN_LONG) && (data_length != ENV_DATA_LEN_SHORT)) {
        STAR_PRINTF("Warning: received data length is %d (while expecting %d or %d) - status=%d\n", data_length, (ENV_DATA_LEN_LONG), (ENV_DATA_LEN_SHORT), perDevs.status);
      }
      else {
        tmp = (attr_value[5]<<24) | (attr_value[4]<<16) | (attr_value[3]<<8) | attr_value[2];
        sprintf((char *)wifi_data, "\"Pressure_0x%02x%02x\":%ld.%ld,", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp/100, tmp%100);
        if (data_length == ENV_DATA_LEN_LONG) {
          tmp = (attr_value[7]<<8) | attr_value[6];
          sprintf((char *)tmp_data, "\"Humidity_0x%02x%02x\":%ld.%ld,", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp/10, tmp%10);
          strcat((char *)wifi_data, (char *)tmp_data);
          //Showing only one Temperature from the same peripheral node
          //tmp = (attr_value[9]<<8) | attr_value[8];    
          //sprintf((char *)tmp_data, "\"Temperature2_0x%02x%02x\":%ld.%ld,", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp/10, tmp%10);
          //strcat((char *)wifi_data, (char *)tmp_data);
          tmp = (attr_value[11]<<8) | attr_value[10];
          sprintf((char *)tmp_data, "\"Temperature1_0x%02x%02x\":%ld.%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp/10, tmp%10);
          strcat((char *)wifi_data, (char *)tmp_data);
        }
        else { /* ENV_DATA_LEN_SHORT (that is when using the Sensor Tile) */
          tmp = (attr_value[7]<<8) | attr_value[6];
          sprintf((char *)tmp_data, "\"Temperature1_0x%02x%02x\":%ld.%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp/10, tmp%10);
          strcat((char *)wifi_data, (char *)tmp_data);
        }
        
        attribute_handle = slaveDev.star_data_char_handle;    
        if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {        
          /*
          * Modify the attribute value according to the following format:
          * Tstamp  | Node ID | P Type ID | value   | H Type ID | value   | T Type ID | value   | T Type ID | value   |
          * 2 bytes | 2 bytes | 1 byte    | 4 bytes | 1 byte    | 2 bytes | 1 byte    | 2 bytes | 1 byte    | 2 bytes |
          */
          Create_New_Attr_Value(attr_value, devAddr, PRESS_TYPE_ID, attr_value+TSTAMP_LEN, data_length-TSTAMP_LEN);
          if (data_length == ENV_DATA_LEN_LONG) {  
            slaveDev.notification_data.data_length = ATTR_HEAD_LEN+PRESS_DATA_LEN+(3*TYPE_ID_LEN)+HUM_DATA_LEN+(2*TEMP_DATA_LEN);              
          }
          else { /* ENV_DATA_LEN_SHORT (that is when using the Sensor Tile) */
            slaveDev.notification_data.data_length = ATTR_HEAD_LEN+PRESS_DATA_LEN+TYPE_ID_LEN+TEMP_DATA_LEN;
          }
                    
          slaveDev.notification_data.attribute_value = star_attr_value;
          Osal_MemCpy(slaveDev.notification_data.devAddr, devAddr, 6);
          slaveDev.notification_data.attribute_handle = attribute_handle;
        }
        new_buffer = 1;
      }
    }
    break;  
  case READING_LED:
    {      
      /* the status (0=OFF, 1=ON) */
      if (data_length != TSTAMP_LEN+LED_DATA_LEN) {
        STAR_PRINTF("Warning: received data length is %d (while expecting %d) - status=%d\n", data_length, (TSTAMP_LEN+LED_DATA_LEN), perDevs.status);
      }
      else {
        tmp = attr_value[2];
        sprintf((char *)wifi_data, "\"LED_0x%02x%02x\":%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp);
        attribute_handle = slaveDev.star_data_char_handle;
        if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {     
          /*
          * Modify the attribute value according to the following format:
          * Timestamp | Node ID | LED Type ID | value  |
          * 2 bytes   | 2 bytes | 1 byte      | 1 byte |
          */
          Create_New_Attr_Value(attr_value, devAddr, LED_TYPE_ID, attr_value+TSTAMP_LEN, LED_DATA_LEN);
          
          slaveDev.notification_data.data_length = ATTR_HEAD_LEN+LED_DATA_LEN;
          slaveDev.notification_data.attribute_value = star_attr_value;      
          Osal_MemCpy(slaveDev.notification_data.devAddr, devAddr, 6);
          slaveDev.notification_data.attribute_handle = attribute_handle;
        }
        new_buffer = 1;
      }
    }
    break;
  case READING_MIC:               
    {
      tmp = 0; /* fake value used to just notify the mic presence */
      sprintf((char *)wifi_data, "\"Mic1_0x%02x%02x\":%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp);
      attribute_handle = slaveDev.star_data_char_handle;
      if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {
        uint8_t val = tmp;
        /*
         * Modify the attribute value according to the following format:
         * Timestamp | Node ID | MIC Type ID | value  |
         * 2 bytes   | 2 bytes | 1 byte      | 1 byte |
         */
        // STORE_LE_16(star_attr_value, (HAL_GetTick()>>3)); /* Timestamp */
        Create_New_Attr_Value(attr_value, devAddr, MICLEVEL_TYPE_ID, &val, MIC_DATA_LEN);
        
        slaveDev.notification_data.data_length = ATTR_HEAD_LEN+MIC_DATA_LEN;
        slaveDev.notification_data.attribute_value = star_attr_value;      
        Osal_MemCpy(slaveDev.notification_data.devAddr, devAddr, 6);
        slaveDev.notification_data.attribute_handle = attribute_handle;
      }
      new_buffer = 1;
    }    
    break;
  case READING_LUX:
    {      
      /* Lux value */
      if (data_length != TSTAMP_LEN+LUX_DATA_LEN) {
        STAR_PRINTF("Warning: received data length is %d (while expecting %d) - status=%d\n", data_length, (TSTAMP_LEN+LUX_DATA_LEN), perDevs.status);
      }
      else {
        tmp = (attr_value[3]<<8) | attr_value[2];
        sprintf((char *)wifi_data, "\"LUX_0x%02x%02x\":%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp);
        attribute_handle = slaveDev.star_data_char_handle;
        if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {     
          /*
          * Modify the attribute value according to the following format:
          * Timestamp | Node ID | LUX Type ID | value   |
          * 2 bytes   | 2 bytes | 1 byte      | 2 bytes |
          */
          Create_New_Attr_Value(attr_value, devAddr, LUX_TYPE_ID, attr_value+TSTAMP_LEN, LUX_DATA_LEN);
          
          slaveDev.notification_data.data_length = ATTR_HEAD_LEN+LUX_DATA_LEN;
          slaveDev.notification_data.attribute_value = star_attr_value;      
          Osal_MemCpy(slaveDev.notification_data.devAddr, devAddr, 6);
          slaveDev.notification_data.attribute_handle = attribute_handle;
        }
        new_buffer = 1;
      }
    }
    break;
  case READING_CO:
    {      
      /* CO value */
      if (data_length != TSTAMP_LEN+CO_DATA_LEN) {
        STAR_PRINTF("Warning: received data length is %d (while expecting %d) - status=%d\n", data_length, (TSTAMP_LEN+CO_DATA_LEN), perDevs.status);
      }
      else {
        tmp = (attr_value[5]<<24) | (attr_value[4]<<16) | (attr_value[3]<<8) | attr_value[2];
        sprintf((char *)wifi_data, "\"CO_0x%02x%02x\":%ld.%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp/100, tmp%100);
        attribute_handle = slaveDev.star_data_char_handle;
        if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {     
          /*
          * Modify the attribute value according to the following format:
          * Timestamp | Node ID | CO Type ID | value   |
          * 2 bytes   | 2 bytes | 1 byte     | 4 bytes |
          */
          Create_New_Attr_Value(attr_value, devAddr, CO_TYPE_ID, attr_value+TSTAMP_LEN, CO_DATA_LEN);
          
          slaveDev.notification_data.data_length = ATTR_HEAD_LEN+CO_DATA_LEN;
          slaveDev.notification_data.attribute_value = star_attr_value;      
          Osal_MemCpy(slaveDev.notification_data.devAddr, devAddr, 6);
          slaveDev.notification_data.attribute_handle = attribute_handle;
        }
        new_buffer = 1;
      }
    }
    break;
  case READING_PRX:
    {
      /* the distance value in mm */      
      tmp = 0; /* fake value used to just notify the prx presence */
      sprintf((char *)wifi_data, "\"PRX_0x%02x%02x\":%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp);
      attribute_handle = slaveDev.star_data_char_handle;
      if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {   
        uint8_t val[PRX_DATA_LEN];
        Osal_MemSet(val, tmp, sizeof(val));
        /*
         * Modify the attribute value according to the following format:
         * Timestamp | Node ID | Proximity Type ID | value   |
         * 2 bytes   | 2 bytes | 1 byte            | 2 bytes |
         */
        Create_New_Attr_Value(attr_value, devAddr, PRX_TYPE_ID, val, PRX_DATA_LEN);
        
        slaveDev.notification_data.data_length = ATTR_HEAD_LEN+PRX_DATA_LEN;
        slaveDev.notification_data.attribute_value = star_attr_value;      
        Osal_MemCpy(slaveDev.notification_data.devAddr, devAddr, 6);
        slaveDev.notification_data.attribute_handle = attribute_handle;
      }
      new_buffer = 1;
    }
    break;
    case READING_DISCONNECTION:
      {      
        /* Device disconnected */
        attribute_handle = slaveDev.star_data_char_handle;
        /*
        * Modify the attribute value according to the following format:
        * Timestamp | Node ID | Status Type ID  | 
        * 2 bytes   | 2 bytes |     1 byte      |
        */
        Create_New_Attr_Value(attr_value, devAddr, STATUS_TYPE_ID, NULL, STATUS_DATA_LEN);
        
        slaveDev.notification_data.data_length = ATTR_HEAD_LEN+STATUS_DATA_LEN;
        slaveDev.notification_data.attribute_value = star_attr_value;      
        Osal_MemCpy(slaveDev.notification_data.devAddr, devAddr, 6);
        slaveDev.notification_data.attribute_handle = attribute_handle;
        
        new_buffer = 1;
      }
    break;
    case READING_AGM:
    {
      /* the acceleration value in mg */      
      tmp = 0; /* fake value used to just notify the agm presence */          
      sprintf((char *)wifi_data, "\"AGM_0x%02x%02x\":%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp);
      attribute_handle = slaveDev.star_data_char_handle;
      if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {
        uint8_t val[MEMS_DATA_LEN];
        Osal_MemSet(val, tmp, sizeof(val));
        /*
         * Modify the attribute value according to the following format:
         * Timestamp | Node ID | ACC Type ID | value   | GYR Type ID | value   | MAG Type ID | value   |
         * 2 bytes   | 2 bytes | 1 byte      | 6 bytes | 1 byte      | 6 bytes | 1 byte      | 6 bytes |
         */
        Create_New_Attr_Value(attr_value, devAddr, ACC_TYPE_ID, val, MEMS_DATA_LEN);
        
        slaveDev.notification_data.data_length = ATTR_HEAD_LEN+ACC_DATA_LEN+(2*TYPE_ID_LEN)+GYR_DATA_LEN+MAG_DATA_LEN;
        slaveDev.notification_data.attribute_value = star_attr_value;      
        Osal_MemCpy(slaveDev.notification_data.devAddr, devAddr, 6);
        slaveDev.notification_data.attribute_handle = attribute_handle;
      }
      new_buffer = 1;
    }
    break;
    case READING_SFUSION:
    {
      /* the mems sensor fusion value */      
      tmp = 0; /* fake value used to just notify the sensor fusion presence */          
      sprintf((char *)wifi_data, "\"SFUSION_0x%02x%02x\":%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp);
      attribute_handle = slaveDev.star_data_char_handle;
      if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {    
        uint8_t val[3];
        Osal_MemSet(val, tmp, sizeof(val));
        /*
         * Modify the attribute value according to the following format:
         * Timestamp | Node ID | SFUSION Type ID |  value  |
         * 2 bytes   | 2 bytes |     1 byte      | 6 bytes |
         */
        Create_New_Attr_Value(attr_value, devAddr, SFUSION_TYPE_ID, val, SFUSION_DATA_LEN);
        
        slaveDev.notification_data.data_length = ATTR_HEAD_LEN+SFUSION_DATA_LEN;
        slaveDev.notification_data.attribute_value = star_attr_value;      
        Osal_MemCpy(slaveDev.notification_data.devAddr, devAddr, 6);
        slaveDev.notification_data.attribute_handle = attribute_handle;
      }
      new_buffer = 1;
    }
    break;
  default:
    break;
  }
  
  if (new_buffer == 1) {
    strcat((char *)wifi_data, "\n");
    STAR_PRINTF("%s", (char *)wifi_data); /* print locally the buffer in JSON format */
    
    data = wifi_data;
    new_data = 1;
  }

}

/**
 * @brief  This function retrieves the peripheral device index and address 
 *         from the connection handle
 * @param  connection handle
 * @param  address of the device
 * @retval None
 */
void Get_Device_From_Conn_Handle(uint16_t handle, uint8_t *index, tBDAddr devAddr)
{
  uint8_t i;
  
  for (i=0; i<MAX_NUM_OF_NODES; i++)
  {
    if (perDevs.connection_handle[i] == handle) {
      Osal_MemCpy(devAddr, perDevs.devInfo[i].bdaddr, 6);
      *index = i;
      break;
    }    
  }
}

/**
 * @brief  Set the new status.
 * @retval None
 */
void Set_New_Status(void) 
{  
  uint8_t i = perDevs.readDeviceIdx;
  
  switch (perDevs.status) {
  case START_SERVICE_DISCOVERY:
    perDevs.status = SERVICE_DISCOVERED;
    break;
  case START_HARDWARE_SERV_CHARS_DISCOVERY:
    perDevs.status = HARDWARE_SERV_CHARS_DISCOVERED;
    break;
  case START_SOFTWARE_SERV_CHARS_DISCOVERY:
    perDevs.status = SOFTWARE_SERV_CHARS_DISCOVERED;
    break;    
  case START_CONFIGURATION_SERV_CHARS_DISCOVERY:
    perDevs.status = CONFIGURATION_SERV_CHARS_DISCOVERED;
    break;
//  case ENABLE_FL1_PRX_NOTIFICATIONS:
//    perDevs.status = NOTIFICATIONS_ENABLED;
//    break;
  case ENABLE_ME1_LED_NOTIFICATIONS:
    if (perDevs.wup_event[perDevs.connDeviceIdx]) {
      perDevs.status = ENABLE_ME1_WUP_NOTIFICATIONS;
    }
    else { 
      perDevs.status = NOTIFICATIONS_ENABLED;
    }
    break; 
  case ENABLE_ME1_WUP_NOTIFICATIONS:
    perDevs.status = NOTIFICATIONS_ENABLED;
    break;  
  case READING_ENVIRONMENTAL:
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {    
      perDevs.status = NOTIFY_ENV_TO_CLIENT;
    }
#if ENABLE_CO 
    else if (perDevs.co_char_handle[i]!=0) {
        perDevs.status = READING_CO;
    }
#endif
     else if ((perDevs.devInfo[i].dev_v == NODE_FL1) &&
              (perDevs.prx_on[i]==0) &&
              (perDevs.lux_char_handle[i]!=0)) {
      perDevs.status = READING_LUX;
    }
    else {
      perDevs.status = ALL_DATA_READ;
    }        
    break; 
  case NOTIFY_ENV_TO_CLIENT:    
    if (perDevs.devInfo[i].dev_v == NODE_FL1) {
      if ((perDevs.prx_on[i]==0) && (perDevs.lux_char_handle[i]!=0)) {
        perDevs.status = READING_LUX;
      }
      else if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify) && 
               (perDevs.prx_event[i]) && (!perDevs.prx_char_read[i])) {
        perDevs.status = READING_PRX;
      }
#if ENABLE_MEMS
      else if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify) && 
               (perDevs.agm_event[i]) && (!perDevs.agm_char_read[i])) {
        perDevs.status = READING_AGM;
      }
      else if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify) && 
               (perDevs.sfusion_event[i]) && (!perDevs.sfusion_char_read[i])) {
        perDevs.status = READING_SFUSION;
      }
#endif
      else {
        perDevs.status = ALL_DATA_READ;
      }
    }
    else if (perDevs.devInfo[i].dev_v == NODE_ME1) {
      if (!perDevs.led_char_read[i]) {
        perDevs.status = READING_LED;
      }
#if ENABLE_MEMS
      else if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify) && 
               (perDevs.agm_event[i]) && (!perDevs.agm_char_read[i])) {
        perDevs.status = READING_AGM;
      }
      else if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify) && 
               (perDevs.sfusion_event[i]) && (!perDevs.sfusion_char_read[i])) {
        perDevs.status = READING_SFUSION;
      }
#endif
#if ENABLE_CO 
      else if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify) && 
               (perDevs.co_char_handle[i] != 0)) {
        perDevs.status = READING_CO;
      }
#endif
      else {
        perDevs.status = ALL_DATA_READ;
      }
    }
    else if (perDevs.devInfo[i].dev_v == NODE_AM1) {
      if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify) &&
          (perDevs.mic_event[i]) && (!perDevs.mic_char_read[i])) {
        perDevs.status = READING_MIC;
      }
#if ENABLE_MEMS
      else if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify) && 
               (perDevs.agm_event[i]) && (!perDevs.agm_char_read[i])) {
        perDevs.status = READING_AGM;
      }      
      else if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify) && 
               (perDevs.sfusion_event[i]) && (!perDevs.sfusion_char_read[i])) {
        perDevs.status = READING_SFUSION;
      }      
#endif
      else {
        perDevs.status = ALL_DATA_READ;
      }
    }
    break;
  case READING_CO:
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {
      perDevs.status = NOTIFY_CO_TO_CLIENT;
    }    
    else {
      perDevs.status = ALL_DATA_READ;
    }
    break;
  case READING_LED:
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {
      perDevs.status = NOTIFY_LED_TO_CLIENT;
    }    
    else {
      perDevs.status = ALL_DATA_READ;
    }
    break;
  case NOTIFY_LED_TO_CLIENT:
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify) && 
        (!perDevs.led_char_read[i])) {
      perDevs.led_char_read[i] = 1;
    }
#if ENABLE_MEMS
    else if (perDevs.agm_event[i]) {
      perDevs.status = READING_AGM;
    }
#endif    
    else {
      perDevs.status = ALL_DATA_READ;
    }
    break;
  case READING_LUX:    
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {
      perDevs.status = NOTIFY_LUX_TO_CLIENT;
    }    
    else {
      perDevs.status = ALL_DATA_READ;
    }
    break;
  case NOTIFY_LUX_TO_CLIENT:
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify) && 
        (perDevs.prx_event[i]) && (!perDevs.prx_char_read[i])) {
      perDevs.status = READING_PRX;
    }
    else {
      perDevs.status = ALL_DATA_READ;
    }
    break;
  case NOTIFY_MIC_TO_CLIENT:
    perDevs.mic_char_read[i] = 1;
#if ENABLE_MEMS
    if (perDevs.agm_event[i]) {
      perDevs.status = READING_AGM;
    }
    else if (perDevs.sfusion_event[i]) {
      perDevs.status = READING_SFUSION;
    }
    else {
      perDevs.status = ALL_DATA_READ;
    }
#else
    perDevs.status = ALL_DATA_READ;
#endif
    break;
  case NOTIFY_PRX_TO_CLIENT:
    perDevs.prx_char_read[i] = 1;
#if ENABLE_MEMS
    if (perDevs.agm_event[i]) {
      perDevs.status = READING_AGM;
    }
    else if (perDevs.sfusion_event[i]) {
      perDevs.status = READING_SFUSION;
    }
#if ENABLE_CO 
    else if (perDevs.prx_char_handle[i] != 0) {
      perDevs.status = READING_CO;
    }
#endif
    else {
      perDevs.status = ALL_DATA_READ;
    }
#else 
    perDevs.status = ALL_DATA_READ;
#endif
    break;
  case NOTIFY_AGM_TO_CLIENT:
    perDevs.agm_char_read[i] = 1;
    if (perDevs.sfusion_event[i]) {
      perDevs.status = READING_SFUSION;
    }
#if ENABLE_CO 
    else if (perDevs.co_char_handle[i] != 0) {
      perDevs.status = READING_CO;
    }
#endif
    else {
      perDevs.status = ALL_DATA_READ;
    }
    break;
  case NOTIFY_SFUSION_TO_CLIENT:
    perDevs.sfusion_char_read[i] = 1;
#if ENABLE_CO 
    if (perDevs.co_char_handle[i] != 0) {
      perDevs.status = READING_CO;
    }
    else 
#endif
    {
      perDevs.status = ALL_DATA_READ;
    }
    break;
  case NOTIFY_CO_TO_CLIENT:
    perDevs.status = ALL_DATA_READ;
    break;
  case ALL_DATA_READ:
    if (!perDevs.is_disconnected[i]) {
      if (perDevs.environmental_char_handle[i] != 0) {
        perDevs.status = READING_ENVIRONMENTAL;
      }
#if ENABLE_CO 
      else if (perDevs.co_char_handle[i] != 0) {
        perDevs.status = READING_CO;
      } 
#endif
    }
    break;
  default:
    break;
  }
  
}

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  addr : Address of peer device
 * @param  handle : Connection handle
 * @retval None
 */
void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle, uint8_t role)
{  
  uint8_t i;
  uint8_t index = perDevs.connDeviceIdx;
  
  /* MASTER role */
  if (role == 0)
  { 
    if (perDevs.is_unconnectable[index] == FALSE) {
      perDevs.connection_handle[index]=handle;    
      perDevs.is_disconnected[index]=FALSE;
      
      STAR_PRINTF("Connected to peripheral: ");
      for (i=5; i>0; i--) {
        STAR_PRINTF("%02X-", addr[i]);
      }
      STAR_PRINTF("%02X (%d/%d - 0x%04x)\n", addr[0], index+1, perDevs.connDevices+1, handle);      
      
      perDevs.status = DEVICE_CONNECTED;    
    }
    else {
      perDevs.is_unconnectable[index] = FALSE;
      perDevs.device_found = FALSE;
      perDevs.status = DEVICE_NOT_FOUND;
    }
  }
  /* SLAVE role */
  else {
    slaveDev.conn_handle = handle;
    slaveDev.is_connected = TRUE;
    slaveDev.is_discoverable = FALSE;
    
    STAR_PRINTF("Connected to master: ");
    for(i=5; i>0; i--){
      STAR_PRINTF("%02X-", addr[i]);
    }
    STAR_PRINTF("%02X\n", addr[0]);    
  }
}

/**
 * @brief  This function is called when the peer device gets disconnected.
 * @param  The connection handle
 * @retval None
 */
void GAP_DisconnectionComplete_CB(uint16_t conn_handle)
{
  uint8_t i;
   
  for (i=0; i<MAX_NUM_OF_NODES; i++) {
    //STAR_PRINTF("conn_handle=0x%04x conn_handle[%d]=0x%04x\n", conn_handle, i, perDevs.connection_handle[i]);
    if (conn_handle == perDevs.connection_handle[i]) {
      if (perDevs.is_disconnected[i] == TRUE) {
        perDevs.is_unconnectable[i] = TRUE; 
        STAR_PRINTF("Peripheral 0x%02x%02x disconnected!\n", 
             perDevs.devInfo[i].bdaddr[NODE_ID_B2], perDevs.devInfo[i].bdaddr[NODE_ID_B1]);        
      }
      else {
        perDevs.is_disconnected[i]  = TRUE;
        perDevs.is_unconnectable[i] = FALSE;         
        if (perDevs.is_connected[i] == TRUE) {
          perDevs.connDevices--;
          perDevs.discDevices++;
          perDevs.is_connected[i] = FALSE;
        }
        /* Notify device disconnection to client */
        perDevs.status = READING_DISCONNECTION;
        Char_Read_Value_CB(perDevs.connection_handle[i], 0, NULL);
        Notify_Master(slaveDev.notification_data.data_length, slaveDev.notification_data.attribute_value,
                  slaveDev.star_hw_serv_handle, slaveDev.notification_data.attribute_handle);
        perDevs.status = CONN_INIT;        
        
        STAR_PRINTF("Peripheral 0x%02x%02x disconnected! (%d still connected)\n", 
               perDevs.devInfo[i].bdaddr[NODE_ID_B2], perDevs.devInfo[i].bdaddr[NODE_ID_B1], 
               perDevs.connDevices);
      }
      break;
    }
  }
  
  if (conn_handle == slaveDev.conn_handle) {
    slaveDev.is_discoverable         = FALSE;
    slaveDev.is_connected            = FALSE;
    slaveDev.conn_handle             = 0;    
    slaveDev.star_data_char_notify   = 0;
    for (i=0; i<MAX_NUM_OF_NODES; i++) {
      perDevs.led_char_read[i]       = 0;
      perDevs.wup_char_read[i]       = 0;
      perDevs.mic_char_read[i]       = 0;
      perDevs.prx_char_read[i]       = 0;      
      perDevs.agm_char_read[i]       = 0;
      perDevs.sfusion_char_read[i]   = 0;
      perDevs.prx_on[i]              = 0;
      perDevs.agm_on[i]              = 0;
      perDevs.sfusion_on[i]          = 0;      
    }    
    perDevs.discovery_enabled     = TRUE;
    perDevs.status = DISABLE_NOTIFICATIONS;
    LOC_DEBUG("GAP_DisconnectionComplete_CB: status DISABLE_NOTIFICATIONS (%d)\n", perDevs.status);
  }
}

/**
 * @brief  This function is called when the discovery phase starts.
 * @param  reason DEVICE_FOUND or DISCOVERY_COMPLETE
 * @param  evt_type 
 * @param  addr_type 
 * @param  addr 
 * @param  data_length 
 * @param  data 
 * @param  RSSI 
 * @retval None
 */
void GAP_Start_General_Discovery_CB(Status_t status, uint8_t adv_type,
                                    uint8_t* addr_type, tBDAddr addr,
                                    uint8_t* data_length,
                                    uint8_t* data, uint8_t* RSSI)
{
  uint8_t alreadyIn = 1;
  uint8_t index;
  uint8_t i;
  uint8_t found_zero_pos = 0;
  
  tBDAddr zeroAddr;
  Osal_MemSet(zeroAddr, 0 , 6);
  
  switch (status) {
  case DEVICE_FOUND:
    {
      switch(adv_type) {
      case ADV_IND:
        {       
          if (perDevs.status != DEVICE_FOUND) {
            //STAR_PRINTF("adv peerAddr[%02x %02x %02x %02x %02x %02x]\n",
            //       addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
            //      STAR_PRINTF("data ");
            //      for (uint8_t j=0; j<*data_length; j++) {
            //        STAR_PRINTF("%02x ", data[j]);
            //      }
            //      STAR_PRINTF("\n");                        
            uint8_t peripheral_v;
            uint8_t wup_event = 0;
            uint8_t mic_event = 0;
            uint8_t prx_event = 0;
            uint8_t agm_event = 0;
            uint8_t sfusion_event = 0;
            
            uint8_t peripheral_name_len = data[3];           
            uint8_t* manuf_data = data+4+peripheral_name_len;
            uint32_t features = 0;
              
            if ((manuf_data[1] == MANUF_SPECIFIC_TYPE) && 
                ((manuf_data[3] == STM32_NUCLEO) || (manuf_data[3] == SENSOR_TILE)|| (manuf_data[3] == BLUE_COIN)) &&
                (peripheral_name_len == 8)) {
                  features = (manuf_data[4]<<24) | (manuf_data[5]<<16) | (manuf_data[6]<<8) | manuf_data[7];
                  wup_event = (features & FEATURE_MASK_WAKEUP_EVENTS) >> WUP_POS;
                  mic_event = (features & FEATURE_MASK_MIC) >> MIC_POS;
                  prx_event = (features & FEATURE_MASK_PROX) >> PRX_POS;
                  agm_event = (features & FEATURE_MASK_ACC) >> ACC_POS;
                  sfusion_event = (features & FEATURE_MASK_SENSORFUSION) >> SFUS_POS;
                  if (mic_event) {
                    peripheral_v = NODE_AM1;
                  }
                  else if (prx_event) {
                    peripheral_v = NODE_FL1;
                  }
                  else {
                    peripheral_v = NODE_ME1;
                  }
                  for (i=0; i<peripheral_name_len-1; i++) {
                    STAR_PRINTF("%c", data[5+i]);
                  }
                  STAR_PRINTF(" peripheral found (%02x%02x)\n", addr[NODE_ID_B2], addr[NODE_ID_B1]);              
                  for (i=0; i<MAX_NUM_OF_NODES; i++) {
                    if (perDevs.is_disconnected[i]) {
                      if (memcmp(zeroAddr, perDevs.devInfo[i].bdaddr, 6)==0) {
                        if (!found_zero_pos) {
                          index = i;
                          perDevs.connDeviceIdx = i;
                          found_zero_pos = 1;
                          alreadyIn = 0;
                        }
                      }
                      else if (memcmp(addr, perDevs.devInfo[i].bdaddr, 6)==0) {
                        index = i;
                        perDevs.connDeviceIdx = i;
                        alreadyIn = 0;
                        perDevs.discDevices--;
                        break;
                      }
                      else {
                        if (!found_zero_pos) {
                          index = i;
                          perDevs.connDeviceIdx = i;                      
                          alreadyIn = 0;
                        }
                      }
                    }                
                  }             
                  if ((!alreadyIn) && (perDevs.connDevices < MAX_NUM_OF_NODES)) {                
                    /*
                    * Save the found peripheral device in the struct containing all the
                    * found peripheral devices.
                    */                
                    Save_Device_Found(adv_type, addr_type, addr, data_length, data, index, peripheral_v, wup_event, mic_event, prx_event, agm_event, sfusion_event);
                    perDevs.status = DEVICE_FOUND;
                    STAR_PRINTF("Peripheral %d inserted (%02x%02x%02x%02x%02x%02x) \n", perDevs.connDevices+1, addr[5], addr[4], addr[3],addr[2], addr[1], addr[0]);                
                  }                
                }            
          }
        }
        break;
      }    
    }
    break;
    
  case DEVICE_DISCOVERY_COMPLETE:
    {
      STAR_PRINTF("DEVICE_DISCOVERY_COMPLETE\n");
      if (perDevs.status == DEVICE_FOUND) {
        perDevs.device_found = TRUE;
      }
      else {
        perDevs.device_found = FALSE;
      }
      perDevs.status = DEVICE_DISCOVERY_COMPLETE;
    }
    break;
  default:
	break;
  }
}

/**
 * @brief  This function is called when there is a notification from the sever.
 * @param  conn_handle Connection handle for which the command is given.
 * @param  error_code  Indicates whether the procedure completed with error
 *                     (BLE_STATUS_FAILED) or was successful (BLE_STATUS_SUCCESS)
 * @retval None
 */
void GATT_Procedure_Complete_CB(uint16_t conn_handle, uint8_t error_code)
{
  if (error_code != BLE_STATUS_SUCCESS) {
    STAR_PRINTF("GATT Procedure completed with error 0x%02x (0x%04x)\n",
            error_code, conn_handle);
  }
  else {
    LOC_DEBUG("GATT Procedure completed with success (0x%04x)\n", conn_handle);
  }
    
  Set_New_Status();
}

/**
 * @brief  This function creates, from an attribute value received from a 
 *         peripheral node, a new attribute value to be sent to the client
 * @param  tstamp        Timestamp
 * @param  data_type_id  Data Type ID 
 * @param  devAddr       Device address
 * @param  data_length   Data Length
 * @retval None
 */
void Create_New_Attr_Value(uint8_t *tstamp, tBDAddr  devAddr, uint8_t data_type_id, uint8_t *data, uint8_t data_length)
{     
  uint8_t* new_attr_value = star_attr_value;
  
  Osal_MemCpy(new_attr_value, tstamp, TSTAMP_LEN); /* Timestamp */  
  new_attr_value += TSTAMP_LEN;
  Osal_MemCpy(new_attr_value, devAddr+NODE_ID_OFFSET, NODE_ID_LEN); /* Node ID */  
  new_attr_value += NODE_ID_LEN;
  Osal_MemCpy(new_attr_value, &data_type_id, TYPE_ID_LEN); /* Data Type ID */
  new_attr_value += TYPE_ID_LEN;
  
  switch(data_type_id)
  {
  case CO_TYPE_ID:
    {
      Osal_MemCpy(new_attr_value, data, CO_DATA_LEN); /* CO sensor value */
    }
    break;
  case PRESS_TYPE_ID:
    {
      Osal_MemCpy(new_attr_value, data, PRESS_DATA_LEN); /* Pressure value */
      new_attr_value  += PRESS_DATA_LEN;
      data += PRESS_DATA_LEN;
      
      if (data_length == ENV_DATA_LEN_LONG-TSTAMP_LEN) {  
        data_type_id = HUM_TYPE_ID;
        Osal_MemCpy(new_attr_value, &data_type_id, TYPE_ID_LEN);   /* Humidity Type ID */
        new_attr_value  += TYPE_ID_LEN;
        Osal_MemCpy(new_attr_value, data,          HUM_DATA_LEN);  /* Humidity value */
        new_attr_value  += HUM_DATA_LEN;
        data += HUM_DATA_LEN;
        data_type_id = TEMP_TYPE_ID;        
        Osal_MemCpy(new_attr_value, &data_type_id, TYPE_ID_LEN);   /* Temperature Type ID */
        new_attr_value  += TYPE_ID_LEN;
        Osal_MemCpy(new_attr_value, data,          TEMP_DATA_LEN); /* Temperature value */
        new_attr_value  += TEMP_DATA_LEN;
        data += TEMP_DATA_LEN;
        Osal_MemCpy(new_attr_value, &data_type_id, TYPE_ID_LEN);   /* Temperature Type ID */
        new_attr_value  += TYPE_ID_LEN;
        Osal_MemCpy(new_attr_value, data,          TEMP_DATA_LEN); /* Temperature value */
      }
      else { /* Sensor Tile */        
        data_type_id = TEMP_TYPE_ID;
        Osal_MemCpy(new_attr_value, &data_type_id, TYPE_ID_LEN);   /* Temperature Type ID */
        new_attr_value  += TYPE_ID_LEN;
        Osal_MemCpy(new_attr_value, data,          TEMP_DATA_LEN); /* Temperature value */
      }        
    }
    break;
  case LED_TYPE_ID:
  case MICLEVEL_TYPE_ID:
    {
      Osal_MemCpy(new_attr_value, data, ONE_BYTE_LEN); /* LED or MIC value */
    }
    break;  
  case PRX_TYPE_ID:
  case LUX_TYPE_ID:
    {
      Osal_MemCpy(new_attr_value, data, TWO_BYTES_LEN); /* LUX or PRX value */
    }
    break;
  case ACC_TYPE_ID:
    {
      Osal_MemCpy(new_attr_value, data, X_DATA_LEN); /* ACC value */
      new_attr_value += X_DATA_LEN;
      data += X_DATA_LEN;
      Osal_MemCpy(new_attr_value, data, Y_DATA_LEN);
      new_attr_value += Y_DATA_LEN;
      data += Y_DATA_LEN;
      Osal_MemCpy(new_attr_value, data, Z_DATA_LEN);
      new_attr_value += Z_DATA_LEN;
      data += Z_DATA_LEN;
      if (data_length == MEMS_DATA_LEN) {         
        data_type_id = GYR_TYPE_ID;
        Osal_MemCpy(new_attr_value, &data_type_id, TYPE_ID_LEN); /* GYR Type ID */
        new_attr_value += TYPE_ID_LEN;
        Osal_MemCpy(new_attr_value, data, X_DATA_LEN);           /* GYR value */
        new_attr_value += X_DATA_LEN;
        data += X_DATA_LEN;
        Osal_MemCpy(new_attr_value, data, Y_DATA_LEN);
        new_attr_value += Y_DATA_LEN;
        data += Y_DATA_LEN;
        Osal_MemCpy(new_attr_value, data, Z_DATA_LEN);
        new_attr_value += Z_DATA_LEN;
        data += Z_DATA_LEN;
        
        data_type_id = MAG_TYPE_ID;
        Osal_MemCpy(new_attr_value, &data_type_id, TYPE_ID_LEN); /* MAG Type ID */
        new_attr_value += TYPE_ID_LEN;
        Osal_MemCpy(new_attr_value, data, X_DATA_LEN);           /* MAG value */
        new_attr_value += X_DATA_LEN;
        data += X_DATA_LEN;
        Osal_MemCpy(new_attr_value, data, Y_DATA_LEN);
        new_attr_value += Y_DATA_LEN;
        data += Y_DATA_LEN;
        Osal_MemCpy(new_attr_value, data, Z_DATA_LEN);
      }
    }
    break;
  case GYR_TYPE_ID:
  case MAG_TYPE_ID:
  case SFUSION_TYPE_ID:  
    {
      Osal_MemCpy(new_attr_value, data, X_DATA_LEN); /* X or Q1 value */
      new_attr_value += X_DATA_LEN;
      data += X_DATA_LEN;
      Osal_MemCpy(new_attr_value, data, Y_DATA_LEN); /* Y or Q2 value */
      new_attr_value += Y_DATA_LEN;
      data += Y_DATA_LEN;
      Osal_MemCpy(new_attr_value, data, Z_DATA_LEN); /* Z or Q3 value */
    }
    break;
  default:
    break;
  }
}

/**
 * @brief  This function is called when there is a notification from the sever.
 * @param  conn_handle Handle of the connection
 * @param  attr_handle Handle of the attribute
 * @param  attr_len    Length of attribute value in the notification
 * @param  attr_value  Attribute value in the notification
 * @retval None
 */
void GATT_Notification_CB(uint16_t conn_handle, uint16_t attr_handle, uint8_t attr_len, uint8_t *attr_value)
{  
  int32_t  tmp = 0;
  uint8_t  tmp_data[256];
  uint8_t  notify = 0;
  tBDAddr  devAddr;
  uint8_t  index = 0;
  uint16_t slave_attr_handle;
  
  static uint8_t wupevt_value = 0;
   
  /* 
   * Retrieving the peripheral device index and address from the connection handle 
   */
  Get_Device_From_Conn_Handle(conn_handle, &index, devAddr);  
  slave_attr_handle = slaveDev.star_data_char_handle;
  
  /*
   * Building the buffer in JSON format
   */
  if (attr_handle == perDevs.wup_char_handle[index]+1) {
    /**
     * The motion is detected when a WakeUp event notification is received.
     * The received data is composed by 2 bytes for the tstamp + 2 (or 3) bytes 
     * for the value. Since we transmit only information about the detected 
     * motion, we don't care of the last 2 (or 3) bytes.
     */
    if ((attr_len != TSTAMP_LEN+2) && (attr_len != TSTAMP_LEN+3))
      STAR_PRINTF("Warning: received data length is %d (while expecting %d or %d)\n", attr_len, (TSTAMP_LEN+2), (TSTAMP_LEN+3));
    wupevt_value = !wupevt_value;
    tmp = wupevt_value;
    sprintf((char *)wifi_data, "\"WUpEvt_0x%02x%02x\":%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp);
    notify = 1;
        
    /*
     * Modify the attribute value according to the following format:
     * Timestamp | Node ID | WakeUp Type ID | 
     * 2 bytes   | 2 bytes | 1 byte         | 
     */
    Create_New_Attr_Value(attr_value, devAddr, WUP_TYPE_ID, NULL, WUP_DATA_LEN);
    
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {      
      perDevs.status = ALL_DATA_READ;                  
      Notify_Master(ATTR_HEAD_LEN+WUP_DATA_LEN, star_attr_value, slaveDev.star_hw_serv_handle, slave_attr_handle);      
    }  
  }
  else if (attr_handle == perDevs.mic_char_handle[index]+1) {
    /* The level in dB (the mic level is detected when a mic event notification is received) */
    /* The Sensor Tile board has only Mic1 so it sends 3 bytes */
    if ((attr_len != TSTAMP_LEN+(2*MIC_DATA_LEN)) && (attr_len != TSTAMP_LEN+MIC_DATA_LEN))
      STAR_PRINTF("Warning: received data length is %d (while expecting %d or %d)\n", attr_len, (TSTAMP_LEN+MIC_DATA_LEN), (TSTAMP_LEN+(2*MIC_DATA_LEN)));
    tmp = attr_value[2];
    sprintf((char *)wifi_data, "\"Mic1_0x%02x%02x\":%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp);
    /* We do not send the audio level from Mic2 anyway */    
    //tmp = attr_value[3];
    //sprintf((char *)tmp_data,  ",\"Mic2_0x%02x%02x\":%d",  devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp);    
    //strcat((char *)wifi_data, (char *)tmp_data);
    notify = 1;
    
    /*
     * Modify the attribute value according to the following format:
     * Timestamp | Node ID | MIC Event Type ID | value  |
     * 2 bytes   | 2 bytes | 1 byte            | 1 byte |
     */
    Create_New_Attr_Value(attr_value, devAddr, MICLEVEL_TYPE_ID, attr_value+TSTAMP_LEN, MIC_DATA_LEN);
          
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {     
      perDevs.status = ALL_DATA_READ;      
      Notify_Master(ATTR_HEAD_LEN+MIC_DATA_LEN, star_attr_value, slaveDev.star_hw_serv_handle, slave_attr_handle);
    }  
  }
  else if (attr_handle == perDevs.prx_char_handle[index]+1) {
    /* the distance value (in mm) */
    if (attr_len != TSTAMP_LEN+PRX_DATA_LEN)
      STAR_PRINTF("Warning: received data length is %d (while expecting %d)\n", attr_len, (TSTAMP_LEN+PRX_DATA_LEN));
    
    uint16_t value = (attr_value[3]<<8) | attr_value[2];
    if ((value & 0x8000) >> 15 == 1) { /* if first bit == 1 => high range value (i.e. PRX data from 53L0A1 or Bluecoin)*/
      value = (value & ~0x8000);
      if (value > HIGH_RANGE_DATA_MAX) {
        tmp = OUT_OF_RANGE_VALUE;
      }
      else {
        tmp = value;
      }
    }
    else {
      tmp = value;
    }
    
    sprintf((char *)wifi_data, "\"PRX_0x%02x%02x\":%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp);
    notify = 1;
    
    /*
     * Modify the attribute value according to the following format:
     * Timestamp | Node ID | Proximity Type ID | value   |
     * 2 bytes   | 2 bytes | 1 byte            | 2 bytes |
     */
    Create_New_Attr_Value(attr_value, devAddr, PRX_TYPE_ID, attr_value+TSTAMP_LEN, PRX_DATA_LEN); /* Timestamp, Node ID, Proximity Type ID, data, len */
        
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {
      perDevs.status = ALL_DATA_READ;           
      Notify_Master(ATTR_HEAD_LEN+PRX_DATA_LEN, star_attr_value, slaveDev.star_hw_serv_handle, slave_attr_handle);      
    }
  }
  else if (attr_handle == perDevs.agm_char_handle[index]+1) {
    uint8_t type_id = 0;
    uint8_t data_len;
    uint8_t value_offset = 0;
    
    /* Acc values in mg, Gyro values in dps, Mag values in mGa */
    if (attr_len != TSTAMP_LEN+MEMS_DATA_LEN)
      STAR_PRINTF("Warning: received data length is %d (while expecting %d)\n", attr_len, (TSTAMP_LEN+MEMS_DATA_LEN));
    
    if (perDevs.acc_event_enabled) {      
      Build_MEMS_Packet(attr_value+2, "Acc", devAddr, INT_VALUE);      
      type_id = ACC_TYPE_ID;
      data_len = ACC_DATA_LEN;
      value_offset = TSTAMP_LEN;
    }
    else if (perDevs.gyr_event_enabled) {     
      Build_MEMS_Packet(attr_value+8, "Gyr", devAddr, FLOAT_VALUE);            
      type_id = GYR_TYPE_ID;
      data_len = GYR_DATA_LEN;
      value_offset = TSTAMP_LEN+ACC_DATA_LEN;
    }
    else if (perDevs.mag_event_enabled) {      
      Build_MEMS_Packet(attr_value+14, "Mag", devAddr, INT_VALUE);      
      type_id = MAG_TYPE_ID;
      data_len = MAG_DATA_LEN;
      value_offset = TSTAMP_LEN+ACC_DATA_LEN+GYR_DATA_LEN;
    }
    notify = 1;
    /*
     * Modify the attribute value according to the following format:
     * Timestamp | Node ID | Acc/Gyro/Mag Type ID | value   |
     * 2 bytes   | 2 bytes | 1 byte               | 6 bytes |
     */
    Create_New_Attr_Value(attr_value, devAddr, type_id, attr_value+value_offset, data_len); 
      
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify) && (perDevs.agm_on[index])) {
      perDevs.status = NOTIFICATIONS_DATA_READ;      
      Notify_Master(ATTR_HEAD_LEN+data_len, star_attr_value, slaveDev.star_hw_serv_handle, slave_attr_handle);      
    }
    else {
      perDevs.status = ALL_DATA_READ;
    }
  }
  else if (attr_handle == perDevs.sfusion_char_handle[index]+1) {
    /* Quaternion values */
    tmp = (int8_t)attr_value[3]<<8 | attr_value[2];
    sprintf((char *)wifi_data, "\"Q1_0x%02x%02x\":%s0.%.4ld; ", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], (tmp<0 ? "-" : ""), (tmp<0 ? -tmp : tmp));
    tmp = (int8_t)attr_value[5]<<8 | attr_value[4];
    sprintf((char *)tmp_data, "\"Q2_0x%02x%02x\":%s0.%.4ld; ", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], (tmp<0 ? "-" : ""), (tmp<0 ? -tmp : tmp));
    strcat((char *)wifi_data, (char *)tmp_data);
    tmp = (int8_t)attr_value[7]<<8 | attr_value[6];
    sprintf((char *)tmp_data, "\"Q3_0x%02x%02x\":%s0.%.4ld; ", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], (tmp<0 ? "-" : ""), (tmp<0 ? -tmp : tmp));
    strcat((char *)wifi_data, (char *)tmp_data);
   
    notify = 1;
    /*
     * Modify the attribute value according to the following format:
     * Timestamp | Node ID | SFusion Type ID |  value  |
     * 2 bytes   | 2 bytes |     1 byte      | 6 bytes |
     */
    Create_New_Attr_Value(attr_value, devAddr, SFUSION_TYPE_ID, attr_value+TSTAMP_LEN, SFUSION_DATA_LEN);
      
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify) && (perDevs.sfusion_on[index])) {
      perDevs.status = NOTIFICATIONS_DATA_READ;
      Notify_Master(ATTR_HEAD_LEN+SFUSION_DATA_LEN, star_attr_value, slaveDev.star_hw_serv_handle, slave_attr_handle);     
      perDevs.sfusion_char_read[index] = 1;
    }
    else {
      perDevs.status = ALL_DATA_READ;
    }
  }
  else if (attr_handle == perDevs.environmental_char_handle[index]+1) {
    /* P value in mBar, H value in percentage, T2 and T1 values in Celtius degree */
    if (attr_len != TSTAMP_LEN+ENV_DATA_LEN)
      STAR_PRINTF("Warning: received data length is %d (while expecting %d)\n", attr_len, (TSTAMP_LEN+ENV_DATA_LEN));
    tmp = (attr_value[5]<<24) | (attr_value[4]<<16) | (attr_value[3]<<8) | attr_value[2];
    sprintf((char *)wifi_data, "\"Pressure_0x%02x%02x\":%ld.%ld,", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp/100, tmp%100);
    tmp = (attr_value[7]<<8) | attr_value[6];
    sprintf((char *)tmp_data,  "\"Humidity_0x%02x%02x\":%ld.%ld,", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp/10, tmp%10);
    strcat((char *)wifi_data, (char *)tmp_data);
    /* Showing only one Temperature from the same peripheral node */
    //tmp = (attr_value[9]<<8) | attr_value[8];    
    //sprintf((char *)tmp_data, "\"Temperature2_0x%02x%02x\":%d.%d,", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp/10, tmp%10);
    //strcat((char *)wifi_data, (char *)tmp_data);
    tmp = (attr_value[11]<<8) | attr_value[10];
    sprintf((char *)tmp_data,  "\"Temperature1_0x%02x%02x\":%ld.%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp/10, tmp%10);
    strcat((char *)wifi_data, (char *)tmp_data);
    notify = 1;
    
    /*
     * Modify the attribute value according to the following format:
     * Tstamp  | Node ID | P Type ID | value   | H Type ID | value   | T Type ID | value   | T Type ID | value   |
     * 2 bytes | 2 bytes | 1 byte    | 4 bytes | 1 byte    | 2 bytes | 1 byte    | 2 bytes | 1 byte    | 2 bytes |
     */
    Create_New_Attr_Value(attr_value, devAddr, PRESS_TYPE_ID, attr_value+TSTAMP_LEN, attr_len-TSTAMP_LEN);
          
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {
      Notify_Master(ATTR_HEAD_LEN+PRESS_DATA_LEN+(3*TYPE_ID_LEN)+HUM_DATA_LEN+(2*TEMP_DATA_LEN), 
                    star_attr_value, slaveDev.star_hw_serv_handle, slave_attr_handle);      
    }
  } 
  else if (attr_handle == perDevs.led_char_handle[index]+1) {
    /* the status (0x01 = ON, 0x00 = OFF) */
    if (attr_len != TSTAMP_LEN+LED_DATA_LEN)
      STAR_PRINTF("Warning: received data length is %d (while expecting %d)\n", attr_len, (TSTAMP_LEN+LED_DATA_LEN));
    tmp = attr_value[2];
    sprintf((char *)wifi_data, "\"LED_0x%02x%02x\":%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp);
    notify = 1;
      
    /*
     * Modify the attribute value according to the following format:
     * Timestamp | Node ID | LED Type ID | value  |
     * 2 bytes   | 2 bytes | 1 byte      | 1 byte |
     */
    Create_New_Attr_Value(attr_value, devAddr, LED_TYPE_ID, attr_value+TSTAMP_LEN, LED_DATA_LEN);
            
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {            
      if (perDevs.led_char_read[index]==1) {
        perDevs.status = ALL_DATA_READ;
      }
      Notify_Master(ATTR_HEAD_LEN+LED_DATA_LEN, star_attr_value, slaveDev.star_hw_serv_handle, slave_attr_handle);      
    }
  }
  else if (attr_handle == perDevs.lux_char_handle[index]+1) {
    /* Lux value */
    if (attr_len != TSTAMP_LEN+LUX_DATA_LEN)
      STAR_PRINTF("Warning: received data length is %d (while expecting %d)\n", attr_len, (TSTAMP_LEN+LUX_DATA_LEN));    
    tmp = (attr_value[3]<<8) | attr_value[2];
    sprintf((char *)wifi_data, "\"LUX_0x%02x%02x\":%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp);
    notify = 1;
    
    /*
     * Modify the attribute value according to the following format:
     * Timestamp | Node ID | LUX Type ID | value   |
     * 2 bytes   | 2 bytes | 1 byte      | 2 bytes |
     */
    Create_New_Attr_Value(attr_value, devAddr, LUX_TYPE_ID, attr_value+TSTAMP_LEN, LUX_DATA_LEN);
            
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {            
      Notify_Master(ATTR_HEAD_LEN+LUX_DATA_LEN, star_attr_value, slaveDev.star_hw_serv_handle, slave_attr_handle);      
    }
  }
  else if (attr_handle == perDevs.co_char_handle[index]+1) {
    /* CO value */
    if (attr_len != TSTAMP_LEN+CO_DATA_LEN)
      STAR_PRINTF("Warning: received data length is %d (while expecting %d)\n", attr_len, (TSTAMP_LEN+CO_DATA_LEN));
    tmp = (attr_value[5]<<24) | (attr_value[4]<<16) | (attr_value[3]<<8) | attr_value[2];
    sprintf((char *)wifi_data, "\"CO_0x%02x%02x\":%ld.%ld", devAddr[NODE_ID_B2], devAddr[NODE_ID_B1], tmp/100, tmp%100);
#if ENABLE_CO        
    notify = 1;
#else
    notify = 0;
#endif    
    /*
     * Modify the attribute value according to the following format:
     * Timestamp | Node ID | CO Type ID | value   |
     * 2 bytes   | 2 bytes | 1 byte     | 4 bytes |
     */
    Create_New_Attr_Value(attr_value, devAddr, CO_TYPE_ID, attr_value+TSTAMP_LEN, CO_DATA_LEN);
            
    if ((slaveDev.is_connected) && (slaveDev.star_data_char_notify)) {            
      Notify_Master(ATTR_HEAD_LEN+CO_DATA_LEN, star_attr_value, slaveDev.star_hw_serv_handle, slave_attr_handle);      
    }
  }
     
  if (notify == 1) {  
    strcat((char *)wifi_data, "\n");
    STAR_PRINTF("NOTIFICATION => %s", (char *)wifi_data); /* print locally the buffer in JSON format */
    
    data = wifi_data;
    if ((attr_handle!=perDevs.sfusion_char_handle[index]+1) && (attr_handle!=perDevs.agm_char_handle[index]+1))
      new_data = 1;            
  }  
}

/**
 * @brief  This function is called whenever there is an ACI event to be processed.
 * @note   Inside this function each event must be identified and correctly
 *         parsed.
 * @param  pckt  Pointer to the ACI packet
 * @retval None
 */
void HCI_Event_CB(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
  
  if(hci_pckt->type != HCI_EVENT_PKT)
    return;
  
  switch(event_pckt->evt){
    
  case EVT_DISCONN_COMPLETE:
    {      
      evt_disconn_complete *evt = (void *)event_pckt->data;
      STAR_PRINTF("EVT_DISCONN_COMPLETE (reason 0x%x)\n", evt->reason);
      GAP_DisconnectionComplete_CB(evt->handle);
    }
    break;
    
  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      switch(evt->subevent){
      case EVT_LE_CONN_COMPLETE:
        {
          LOC_DEBUG("EVT_LE_CONN_COMPLETE\n");
          evt_le_connection_complete *cc = (void *)evt->data;
          GAP_ConnectionComplete_CB(cc->peer_bdaddr, cc->handle, cc->role);
        }
        break;
      case EVT_LE_ADVERTISING_REPORT:
        {
          // This event is generated only by X-NUCLEO-IDB05A1 version but not by X-NUCLEO-IDB04A1 (which generates DEVICE_FOUND EVT)
          // Formally the structure related to both events are identical except that for the ADV REPORT
          // there is one more field (number of reports) which is not forwarded to upper layer.
          // Thus we need to move one byte over (((uint8_t*)evt->data)+1) before parsing the ADV REPORT. 
          le_advertising_info *pr = (le_advertising_info*) (((uint8_t*)evt->data)+1);
          
          LOC_DEBUG("EVT_LE_ADVERTISING_REPORT evt_type=%d bdaddr_type=%d bdaddr=[%02x %02x %02x %02x %02x %02x] data_length=%d RSSI=%d\n",
                    pr->evt_type, pr->bdaddr_type,
                    pr->bdaddr[5], pr->bdaddr[4], pr->bdaddr[3], pr->bdaddr[2], pr->bdaddr[1], pr->bdaddr[0],
                    pr->data_length, pr->data_RSSI[pr->data_length]);
          if (perDevs.connDevices < MAX_NUM_OF_NODES) {
            GAP_Start_General_Discovery_CB(DEVICE_FOUND,
                                           pr->evt_type,
                                           &pr->bdaddr_type,
                                           pr->bdaddr,
                                           &pr->data_length,
                                           &pr->data_RSSI[0],
                                           &pr->data_RSSI[pr->data_length]);
          }
        }
        break;
      }
    }
    break;
    
  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      switch(blue_evt->ecode)
      {
        case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
        {
          LOC_DEBUG("EVT_BLUE_GATT_ATTRIBUTE_MODIFIED\n");
            evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
            Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
        }
        break;
        
        case EVT_BLUE_GATT_WRITE_PERMIT_REQ:
        {
          LOC_DEBUG("EVT_BLUE_GATT_WRITE_PERMIT_REQ\n");
        }
        break;
        
        case EVT_BLUE_GATT_READ_PERMIT_REQ:
        {
          LOC_DEBUG("EVT_BLUE_GATT_READ_PERMIT_REQ\n");
        }
        break;
        
        case EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP:
        {
          LOC_DEBUG("EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP\n");          
          evt_att_read_by_group_resp *evt = (evt_att_read_by_group_resp*)blue_evt->data;
          Primary_Services_CB(evt->conn_handle, evt->event_data_length,
                              evt->attribute_data_length, evt->attribute_data_list);          
        }
        break;
        
        case EVT_BLUE_GATT_NOTIFICATION:
        {
          LOC_DEBUG("EVT_BLUE_GATT_NOTIFICATION\n");
          evt_gatt_attr_notification *evt = (evt_gatt_attr_notification*)blue_evt->data;
          GATT_Notification_CB(evt->conn_handle, evt->attr_handle, evt->event_data_length - 2, evt->attr_value);
        }
        break;
        
        case EVT_BLUE_ATT_READ_BY_TYPE_RESP:
        {
          LOC_DEBUG("EVT_BLUE_ATT_READ_BY_TYPE_RESP\n");
          evt_att_read_by_type_resp *evt = (evt_att_read_by_type_resp*)blue_evt->data;
          Service_Chars_CB(evt->conn_handle, evt->event_data_length, 
                           evt->handle_value_pair_length, evt->handle_value_pair);  
        }
        break;
        
        case EVT_BLUE_GATT_DISC_READ_CHAR_BY_UUID_RESP:
        {
          LOC_DEBUG("EVT_BLUE_GATT_DISC_READ_CHAR_BY_UUID_RESP\n");          
          //evt_gatt_disc_read_char_by_uuid_resp *resp = (void*)blue_evt->data;          
        }
        break;
        
        case EVT_BLUE_ATT_READ_RESP:
        {
          LOC_DEBUG("EVT_BLUE_ATT_READ_RESP\n");
          evt_att_read_resp *evt = (evt_att_read_resp*)blue_evt->data;
          Char_Read_Value_CB(evt->conn_handle, evt->event_data_length, evt->attribute_value);
        }
        break;
        
        case EVT_BLUE_GATT_PROCEDURE_COMPLETE:
        {
          LOC_DEBUG("EVT_BLUE_GATT_PROCEDURE_COMPLETE\n");
          /* Wait for gatt procedure complete event trigger related to Discovery Charac by UUID */
          evt_gatt_procedure_complete *pr = (void*)blue_evt->data;   
          //STAR_PRINTF("EVT_BLUE_GATT_PROCEDURE_COMPLETE Conn Handle = 0x%04x, Data Len = %d, Error Code = %d\n",pr->conn_handle, pr->data_length, pr->error_code);
          GATT_Procedure_Complete_CB(pr->conn_handle, pr->error_code);
        }
        break;
        
        case EVT_BLUE_GAP_DEVICE_FOUND:
        {
          STAR_PRINTF("EVT_BLUE_GAP_DEVICE_FOUND\n");
          evt_gap_device_found *pr = (evt_gap_device_found*)blue_evt->data;
          LOC_DEBUG("EVT_BLUE_GAP_DEVICE_FOUND evt_type=%d\n", pr->evt_type);
          if (perDevs.connDevices < MAX_NUM_OF_NODES) {
            GAP_Start_General_Discovery_CB(DEVICE_FOUND, pr->evt_type, &pr->bdaddr_type,
                                           pr->bdaddr, &pr->data_length, &pr->data_RSSI[0],
                                           &pr->data_RSSI[pr->data_length]);
          }
        }
        break;
          
        case EVT_BLUE_GAP_PROCEDURE_COMPLETE:
        {
          evt_gap_procedure_complete *pr = (evt_gap_procedure_complete*)blue_evt->data;
          STAR_PRINTF("EVT_BLUE_GAP_PROCEDURE_COMPLETE (code=0x%02X)\n", pr->procedure_code);            
          switch(pr->procedure_code) {
            case GAP_LIMITED_DISCOVERY_PROC:
            case GAP_GENERAL_DISCOVERY_PROC:                
              GAP_Start_General_Discovery_CB(DEVICE_DISCOVERY_COMPLETE, 0, NULL, NULL, NULL, NULL, NULL);
            break;
          }
        }
        break;
        
      }
    }
    break;
  }    
}

/**
  * @brief  Connection Process: scan for new peripheral nodes and establish 
  *         new connections
  * @param  None
  * @retval None
  */
void Connection_Process(void)
{   
  uint8_t  dev_v;
  
  if (perDevs.status == CONN_INIT) {    
    if ((perDevs.connDevices < MAX_NUM_OF_NODES) && (perDevs.discovery_enabled)) {
      /* Start the discovery of new peripheral nodes */    
      Start_Discovery();
    }
    else {
      perDevs.status = DEVICE_CONNECTED;
    }
  }
    
  if (perDevs.status == DEVICE_DISCOVERY_COMPLETE) {
    if (perDevs.device_found == TRUE) {
      /* Establish connection with a peripheral device */
      perDevs.status = START_DEVICE_CONNECTION;    
      Connect_Peripheral();
    }
    else {
      perDevs.status = DEVICE_NOT_FOUND;
    }
  }
        
  if ((perDevs.status == DEVICE_CONNECTED) || (perDevs.status == DEVICE_NOT_FOUND)) {
    /* Make the central node discoverable as slave */
#if 0
    if (slaveDev.is_connected == FALSE) {
      perDevs.status = START_DISCOVERABLE_MODE;
      Set_Slave_Discoverable();                
      slaveDev.is_discoverable = TRUE;        
    }
#endif
    if (perDevs.device_found == TRUE) {
      perDevs.status = DISCOVERABLE_MODE_SET;
      perDevs.device_found = FALSE;
    }
    else {  
      perDevs.readDeviceIdx = perDevs.connDevices+perDevs.discDevices-1;
      perDevs.status = READ_INIT;
    }
  }   
  
  if (perDevs.status == DISCOVERABLE_MODE_SET) {
    /* Search for all services */
    perDevs.status = START_SERVICE_DISCOVERY;    
    Discover_Services();
  }

  if (perDevs.status == SERVICE_DISCOVERED) {
    /* Search for the characteristics of the HARDWARE service */    
    if (perDevs.hardware_service_handle[perDevs.connDeviceIdx].start_h) {
      perDevs.status = START_HARDWARE_SERV_CHARS_DISCOVERY;  
      Discover_Service_Chars(perDevs.hardware_service_handle, "HW");
    }
    else {        
      perDevs.status = HARDWARE_SERV_CHARS_DISCOVERED;            
    }    
  }
  
  if (perDevs.status == HARDWARE_SERV_CHARS_DISCOVERED) {
    /* Search for the characteristics of the SOFTWARE service */    
    if (perDevs.software_service_handle[perDevs.connDeviceIdx].start_h) {
      perDevs.status = START_SOFTWARE_SERV_CHARS_DISCOVERY;  
      Discover_Service_Chars(perDevs.software_service_handle, "SW");
    }
    else {        
      perDevs.status = SOFTWARE_SERV_CHARS_DISCOVERED;            
    }    
  }
  
  if (perDevs.status == SOFTWARE_SERV_CHARS_DISCOVERED) {    
    /* Search for the characteristics of the CONFIGURATION service */
    if (perDevs.configuration_service_handle[perDevs.connDeviceIdx].start_h) {
      perDevs.status = START_CONFIGURATION_SERV_CHARS_DISCOVERY;
      Discover_Service_Chars(perDevs.configuration_service_handle, "CFG");
    }
    else { 
      perDevs.status = CONFIGURATION_SERV_CHARS_DISCOVERED;
    }
  }
  
  if (perDevs.status == CONFIGURATION_SERV_CHARS_DISCOVERED) {    
    if (!perDevs.is_disconnected[perDevs.connDeviceIdx]) {
      dev_v = perDevs.devInfo[perDevs.connDeviceIdx].dev_v;
      if (perDevs.devInfo[perDevs.connDeviceIdx].dev_v == NODE_ME1) {
        perDevs.status = ENABLE_ME1_LED_NOTIFICATIONS;
      }
      else if (perDevs.devInfo[perDevs.connDeviceIdx].dev_v == NODE_AM1) {
        perDevs.status = NOTIFICATIONS_ENABLED;
      }
      else if (perDevs.devInfo[perDevs.connDeviceIdx].dev_v == NODE_FL1) {
        perDevs.status = NOTIFICATIONS_ENABLED;
      }
    }
    else {
      perDevs.status = NOTIFICATIONS_ENABLED;
    }
  }
  
  if ((perDevs.status == ENABLE_ME1_LED_NOTIFICATIONS) ||
      (perDevs.status == ENABLE_ME1_WUP_NOTIFICATIONS)) {
    /* Enabling notifications on peripheral node */
    uint8_t i = perDevs.connDeviceIdx;
    uint16_t connection_handle = perDevs.connection_handle[i];
    Enable_Notifications(connection_handle, i, dev_v, 1);
  }
  
  if (perDevs.status == NOTIFICATIONS_ENABLED) {
    if (!perDevs.is_disconnected[perDevs.connDeviceIdx]) {
      perDevs.is_connected[perDevs.connDeviceIdx] = TRUE;
      perDevs.connDeviceIdx++;
      perDevs.connDevices++;
      STAR_PRINTF("Connected Devices %d\n", perDevs.connDevices);
    }
    perDevs.readDeviceIdx = perDevs.connDevices-1;
    perDevs.status = READ_INIT;
  }

}

/**
  * @brief  Reading Process: read data from peripheral nodes 
  * @param  None
  * @retval None
  */
void Reading_Process(void)
{
  uint8_t i = perDevs.readDeviceIdx;
  
  if ((perDevs.status == NOTIFICATIONS_DATA_READ) && (perDevs.sfusion_event_enabled) &&
      (!perDevs.sfusion_char_read[i])) {
    perDevs.status = ALL_DATA_READ;    
  }
  
  if (perDevs.status == READ_INIT) {    
    if (perDevs.connDevices > 0) {
      perDevs.status = ALL_DATA_READ; 
      Set_New_Status();      
    }
    else {
      perDevs.status = CONN_INIT;
    }
  }
  
  if (perDevs.status == READING_ENVIRONMENTAL) {
      Read_Sensor_Data(i, perDevs.connection_handle[i], perDevs.environmental_char_handle[i]+1);
  }
  
  if (perDevs.status == NOTIFY_ENV_TO_CLIENT) {
    Notify_Master(slaveDev.notification_data.data_length, slaveDev.notification_data.attribute_value,
                  slaveDev.star_hw_serv_handle, slaveDev.notification_data.attribute_handle);
    Set_New_Status();
  }
  
  if (perDevs.status == READING_LED) {
    Read_Sensor_Data(i, perDevs.connection_handle[i], perDevs.led_char_handle[i]+1);
  }
  
  if (perDevs.status == NOTIFY_LED_TO_CLIENT) {
    Notify_Master(slaveDev.notification_data.data_length, slaveDev.notification_data.attribute_value,
                  slaveDev.star_hw_serv_handle, slaveDev.notification_data.attribute_handle);
    Set_New_Status();
  }
  
  if (perDevs.status == READING_LUX) {    
    Read_Sensor_Data(i, perDevs.connection_handle[i], perDevs.lux_char_handle[i]+1);
  }
  
  if (perDevs.status == NOTIFY_LUX_TO_CLIENT) {
    Notify_Master(slaveDev.notification_data.data_length, slaveDev.notification_data.attribute_value,
                  slaveDev.star_hw_serv_handle, slaveDev.notification_data.attribute_handle);
    Set_New_Status();
  }
  
  if (perDevs.status == READING_CO) {    
    Read_Sensor_Data(i, perDevs.connection_handle[i], perDevs.co_char_handle[i]+1);
  }
  
  if (perDevs.status == NOTIFY_CO_TO_CLIENT) {
    Notify_Master(slaveDev.notification_data.data_length, slaveDev.notification_data.attribute_value,
                  slaveDev.star_hw_serv_handle, slaveDev.notification_data.attribute_handle);
    Set_New_Status();
  }
  
  if (perDevs.status == READING_MIC) { 
    HAL_Delay(300);
    /* Sending a 0 value to master just to notify the MIC sensor presence */
    Char_Read_Value_CB(perDevs.connection_handle[i], 0, NULL);
    perDevs.status = NOTIFY_MIC_TO_CLIENT;
    Notify_Master(slaveDev.notification_data.data_length, slaveDev.notification_data.attribute_value,
                  slaveDev.star_hw_serv_handle, slaveDev.notification_data.attribute_handle);
    Set_New_Status();
  }
  
  if (perDevs.status == READING_PRX) {
    HAL_Delay(300);    
    /* Sending a 0 value to master just to notify the PRX sensor presence */
    Char_Read_Value_CB(perDevs.connection_handle[i], 0, NULL);
    perDevs.status = NOTIFY_PRX_TO_CLIENT;
    Notify_Master(slaveDev.notification_data.data_length, slaveDev.notification_data.attribute_value,
                  slaveDev.star_hw_serv_handle, slaveDev.notification_data.attribute_handle);
    Set_New_Status();
  }
  
  if (perDevs.status == READING_AGM) {
    HAL_Delay(300);    
    /* Sending a 0 value to master just to notify the AGM sensors presence */
    Char_Read_Value_CB(perDevs.connection_handle[i], 0, NULL);
    perDevs.status = NOTIFY_AGM_TO_CLIENT;
    Notify_Master(slaveDev.notification_data.data_length, slaveDev.notification_data.attribute_value,
                  slaveDev.star_hw_serv_handle, slaveDev.notification_data.attribute_handle);
    Set_New_Status();
  }
  
  if (perDevs.status == READING_SFUSION) {
    HAL_Delay(300);    
    /* Sending a 0 value to master just to notify the SFusion feature presence */
    Char_Read_Value_CB(perDevs.connection_handle[i], 0, NULL);
    perDevs.status = NOTIFY_SFUSION_TO_CLIENT;
    Notify_Master(slaveDev.notification_data.data_length, slaveDev.notification_data.attribute_value,
                  slaveDev.star_hw_serv_handle, slaveDev.notification_data.attribute_handle);
    Set_New_Status();
  }
  
  if (perDevs.status == ALL_DATA_READ) {
    LOC_DEBUG("readDevIdx=%d, connDevices=%d\n", i, perDevs.connDevices);
    if (i > 0) {
      perDevs.readDeviceIdx--; 
    } 
    perDevs.status = READ_INIT; 
    LOC_DEBUG("Reading_Process: status READ_INIT (%d)\n", perDevs.status);
    if ((slaveDev.is_connected == FALSE) && (slaveDev.is_discoverable == FALSE)) {
#if ENABLE_MEMS
      Disable_All_Notifications(); /* Called here to disable the SFUSION notifications */
#endif
      Set_Slave_Discoverable();
      slaveDev.is_discoverable = TRUE;
    }    
    if (i == 0) {
      perDevs.status = CONN_INIT;
    }
  }
  
  /**
   * This status is set when the Master gets disconnected from the Central node
   * and before restarting the advertising.
   */
  if (perDevs.status == DISABLE_NOTIFICATIONS) {    
   
    /* Disabling all previously enabled notifications */
    Disable_All_Notifications();
          
    perDevs.status = ALL_DATA_READ;
    perDevs.readDeviceIdx = i;
        
  }
}

/**
  * @brief  Write a characteristic descriptor 
  * @param  the connection handle
  * @param  the attribute handle
  * @param  the attribute length
  * @param  the attribute value
  * @retval None
  */
void Write_Charac_Descriptor(uint16_t conn_handle, uint16_t attr_handle, uint8_t attr_len, uint8_t* attr_value) 
{
  struct timer t;
  
  Timer_Set(&t, CLOCK_SECOND*10);    
  while (aci_gatt_write_charac_descriptor(conn_handle, attr_handle, attr_len, attr_value) == BLE_STATUS_NOT_ALLOWED) {      
    /* Radio is busy */
    if (Timer_Expired(&t)) {
      STAR_PRINTF("aci_gatt_write_charac_descriptor failed.\n");
      break;
    }
  }  
}

/**
  * @brief  Write a characteristic value without waiting for any response from the server
  * @param  the connection handle
  * @param  the attribute handle
  * @param  the attribute length
  * @param  the attribute value
  * @retval None
  */
void Write_Charac_Value_Without_Resp(uint16_t conn_handle, uint16_t attr_handle, uint8_t attr_len, uint8_t* attr_value)
{
  struct timer t;
  
  Timer_Set(&t, CLOCK_SECOND*10);
  while(aci_gatt_write_without_response(conn_handle, attr_handle, attr_len, attr_value)==BLE_STATUS_INSUFFICIENT_RESOURCES){
    // Radio is busy (buffer full).
    if(Timer_Expired(&t)) {
      STAR_PRINTF("aci_gatt_write_without_response failed.\n");
      break;
    }
  }  
}

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
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
