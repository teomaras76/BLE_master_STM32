/**
  ******************************************************************************
  * @file    ble_slave_service.c
  * @author  Central LAB
  * @version V1.0.0
  * @date    05-Feb-2016
  * @brief   Add BLE slave service using a vendor specific profile.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
#include "ble_slave_service.h"
#include "ble_master_service.h"
#include "btle_config.h"

#include "wifi_support.h"
#include "wifi_const.h"

#include "gp_timer.h"

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
 
/** @defgroup STM32F401RE-Nucleo_BLE_SLAVE_SERVICE BLE_SLAVE_SERVICE
 * @{
 */

/** @defgroup STM32F401RE-Nucleo_BLE_SLAVE_SERVICE_Private_Variables Private Variables
 * @{
 */
/* Private variables ---------------------------------------------------------*/
extern char print_msg_buff[MAX_BUFFER_GLOBAL];
extern UART_HandleTypeDef UartMsgHandle;
   
SlaveDevice_t slaveDev;
extern PeripheralDevices_t perDevs;
extern uint8_t notification_freq;

/**
 * @}
 */

/** @defgroup STM32F401RE-Nucleo_BLE_SLAVE_SERVICE_Private_Macros Private Macros
 * @{
 */
#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
{\
  uuid_struct[0 ] = uuid_0 ; uuid_struct[1 ] = uuid_1 ; uuid_struct[2 ] = uuid_2 ; uuid_struct[3 ] = uuid_3 ; \
  uuid_struct[4 ] = uuid_4 ; uuid_struct[5 ] = uuid_5 ; uuid_struct[6 ] = uuid_6 ; uuid_struct[7 ] = uuid_7 ; \
  uuid_struct[8 ] = uuid_8 ; uuid_struct[9 ] = uuid_9 ; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
  uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}

/* Star Service and Characteristic */
#define COPY_STAR_SERVICE_UUID(uuid_struct)     COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x00,0x00,0x01,0x11,0xe1,0x9a,0xb4,0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_STAR_DATA_CHAR_UUID(uuid_struct)   COPY_UUID_128(uuid_struct,0x00,0x08,0x00,0x00,0x00,0x01,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_STAR_CONFIG_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x04,0x00,0x00,0x00,0x01,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/**
 * @}
 */

/** @defgroup STM32F401RE-Nucleo_BLE_SLAVE_SERVICE_Exported_Functions Exported Functions 
 * @{
 */

/**
 * @brief  Add all services using a vendor specific UUIDs
 * @param  None
 * @retval None
 */
void Add_All_Services(void)
{
  tBleStatus ret;
  uint8_t uuid[16];
  uint8_t max_attr_records;
  
  /**
   * Add HARDWARE service
   */  
  /**
   * The number of attribute records is given by the following formula: 
   * 1 + (2 x NUM of CHARS) + (1 x NUM of CHARS using an INDICATION or NOTIFY property)
   */
  max_attr_records = 1+(2*2)+(1*2);
  COPY_STAR_SERVICE_UUID(uuid);
  ret = aci_gatt_add_serv(UUID_TYPE_128, uuid, PRIMARY_SERVICE, max_attr_records,
                          &(slaveDev.star_hw_serv_handle));
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error while adding Star HW Service (0x%04x).\n", ret);
  }
  else {
    PRINTF("Star HW Gatt Service added (handle 0x%04x)\n", slaveDev.star_hw_serv_handle);
  }
  
  /**
   * Add char for configuring the Central node through the master 
   * (typically an Android/iOS device) 
   */
  COPY_STAR_CONFIG_CHAR_UUID(uuid);
  ret = aci_gatt_add_char(slaveDev.star_hw_serv_handle, UUID_TYPE_128, uuid, STAR_CHAR_MAX_VALUE_LEN,
                          CHAR_PROP_NOTIFY|CHAR_PROP_WRITE_WITHOUT_RESP,
                          ATTR_PERMISSION_NONE, 
                          GATT_NOTIFY_ATTRIBUTE_WRITE|GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,                          
                          16, 1, &(slaveDev.star_config_char_handle));
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error while adding Star Config Char (0x%04x).\n", ret);
  }
  else {
    PRINTF("Star HW Gatt Config Characteristic added (handle 0x%04x)\n", slaveDev.star_config_char_handle);
  }
    
  /**
   * Add char for all data sent from the Central node to the master 
   * (typically an Android/iOS device) 
   */
  COPY_STAR_DATA_CHAR_UUID(uuid);
  ret = aci_gatt_add_char(slaveDev.star_hw_serv_handle, UUID_TYPE_128, uuid, STAR_CHAR_MAX_VALUE_LEN,
                          CHAR_PROP_NOTIFY|CHAR_PROP_READ|CHAR_PROP_WRITE_WITHOUT_RESP,
                          ATTR_PERMISSION_NONE, 
                          GATT_NOTIFY_ATTRIBUTE_WRITE|GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                          16, 1, &(slaveDev.star_data_char_handle));
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error while adding Star Data Char (0x%04x).\n", ret);
  }
  else {
    PRINTF("Star HW Gatt Data Characteristic added (handle 0x%04x)\n", slaveDev.star_data_char_handle);
  }

  return;  
}

/**
 * @brief  This function is called to Enable/Disable MIC/PRX/AGM/SFusion notifications
 * @param  att_data : pointer to the modified attribute data
 * @param  connection handle
 * @param  peripheral device index
 * @param  feature mask
 * @param  notification frequency
 * @retval None
 */
void Change_Notification_Status(uint8_t *att_data, uint8_t  *attr_value, uint16_t conn_handle, uint8_t i, uint32_t feature_mask, uint8_t frequency)
{
  uint8_t  value_len;
  uint16_t attr_handle = 0x00;
  
  if (att_data[3] == 1) {
    /* Setting notification frequency */
    Set_Notification_Property(conn_handle, i, feature_mask, NOTIFICATION_FREQ_CMD, frequency);
    
    /* Stopping the scanning for new peripheral nodes */
    if (perDevs.discovery_enabled) {
      Set_New_Nodes_Scanning(0x02); 
    }
    
    /* Disabling all previously enabled notifications */
    Disable_All_Notifications();
  }
  
  /* Enabling/Disabling notifications */ 
  value_len = 2;
  attr_value[0] = att_data[3];
  if (feature_mask==FEATURE_MASK_MIC){
    attr_handle = perDevs.mic_char_handle[i] + 2;
    perDevs.mic_event_enabled = attr_value[0]; 
  }
  else if (feature_mask==FEATURE_MASK_PROX){
    attr_handle = perDevs.prx_char_handle[i] + 2;
    perDevs.prx_event_enabled = attr_value[0]; 
  }
  else if ((feature_mask==FEATURE_MASK_ACC) || 
           (feature_mask==FEATURE_MASK_GYR) || 
           (feature_mask==FEATURE_MASK_MAG)) {
    attr_handle = perDevs.agm_char_handle[i] + 2;
    perDevs.agm_event_enabled = attr_value[0];
  }
  else if (feature_mask==FEATURE_MASK_SENSORFUSION){
    attr_handle = perDevs.sfusion_char_handle[i] + 2;
    perDevs.sfusion_event_enabled = attr_value[0]; 
  }
  if (attr_handle != 0x00) {
    Write_Charac_Descriptor(conn_handle, attr_handle, value_len, attr_value);
  }
 
  if(attr_value[0]==0x00){
    for (uint8_t j=0; j<(perDevs.connDevices+perDevs.discDevices); j++) {
      if (!perDevs.wup_event[j] && perDevs.devInfo[j].dev_v == NODE_ME1 && perDevs.is_connected[j]) {
        attr_handle = perDevs.wup_char_handle[j] + 2;
        attr_value[0] = 0x01;
        Set_Notification_Property(perDevs.connection_handle[j], j, FEATURE_MASK_WAKEUP_EVENTS, WAKEUP_NOTIFICATION_CMD, 1);
        
        Write_Charac_Descriptor(perDevs.connection_handle[j], attr_handle, value_len, attr_value);          
        
        PRINTF("WUP notification on node [%d] ON\n", j);       
        perDevs.wup_event_enabled = 1;
        perDevs.wup_event[j] = 1;           
      }
    }
    if (perDevs.wup_event_enabled == 1)
      LOC_DEBUG("All WUP notifications turned ON\n");
    perDevs.status = ALL_DATA_READ;
  }
  else{
    for (uint8_t j=0; j<(perDevs.connDevices+perDevs.discDevices); j++) {
      if (perDevs.wup_event[j] && perDevs.devInfo[j].dev_v == NODE_ME1 && perDevs.is_connected[j]) {
        attr_handle = perDevs.wup_char_handle[j] + 2;
        attr_value[0] = 0x00;
        
        Write_Charac_Descriptor(perDevs.connection_handle[j], attr_handle, value_len, attr_value);          
        
        PRINTF("WUP notification on node [%d] OFF\n", j);
        perDevs.wup_event_enabled = 0;
        perDevs.wup_event[j] = 0;      
      }
    }
    if (perDevs.wup_event_enabled == 0)
      LOC_DEBUG("All WUP notifications turned OFF\n");
    perDevs.status = NOTIFICATIONS_DATA_READ;
  }
  attr_value[0] = att_data[3];
  perDevs.readDeviceIdx = i;
}
/**
 * @brief  This function is called when an attribute gets modified
 * @param  handle : handle of the attribute
 * @param  data_length : size of the modified attribute data
 * @param  att_data : pointer to the modified attribute data
 * @retval None
 */
void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  uint8_t  i;
  uint16_t conn_handle;
  uint16_t attr_handle;
  uint8_t  value_len;
  uint8_t  attr_value[6];
  
  if (handle == slaveDev.star_config_char_handle + 1) {        
    perDevs.status = ALL_DATA_READ;
    if (att_data[0]==0x01) {
      /* Disabling all previously enabled notifications */
      Disable_All_Notifications();
    }
    
    Set_New_Nodes_Scanning(att_data[0]);  
  }
  else if (handle == slaveDev.star_config_char_handle + 2) {        
    if (att_data[0]==0x01) {
      PRINTF("Enable scanning notification\n");      
      STORE_LE_16(slaveDev.star_config_value,(HAL_GetTick()>>3)); /* Timestamp */
      slaveDev.star_config_value[2] = perDevs.discovery_enabled;
      slaveDev.star_config_value_len = 3;
            
      /* send the status of the scanning when notifications are enabled */
      Notify_Master(slaveDev.star_config_value_len, slaveDev.star_config_value, slaveDev.star_hw_serv_handle, slaveDev.star_config_char_handle);
      
      /* enable here the data notifications since sometimes the relative command gets lost (FIXME) */
      PRINTF("Enable data notification (0)\n");
      slaveDev.star_data_char_notify = 1; 
    }
    else if (att_data[0]==0x00) {
      PRINTF("Disable scanning notification\n");      
    }    
  }
  else if (handle == slaveDev.star_data_char_handle + 2) {
    if (att_data[0]==0x01) {
      PRINTF("Enable data notification (1)\n");
      slaveDev.star_data_char_notify = 1;
    }
    else if (att_data[0]==0x00) {
      PRINTF("Disable data notification\n");
      slaveDev.star_data_char_notify = 0;
    }
  } 
  else if (handle == slaveDev.star_data_char_handle + 1) {    
    /*
     * Forward the command to the proper peripheral node.
     * It can be:
     * - the ON/OFF of a LED
     * - the notification enabling/disabling for a MIC Level or a Proximity Sensor
     * The coming data are received according to the following format:
     * |Node ID | Type ID | value   | 
     * |2 bytes | 1 byte  | x bytes |
     */
    i = Get_Device_Index_From_Addr(att_data);
    if ((i < MAX_NUM_OF_NODES) && (!perDevs.is_disconnected[i]) && (perDevs.cfg_char_handle[i] != 0)) {
      conn_handle = perDevs.connection_handle[i];
      
      switch (att_data[2]) {
      case LED_TYPE_ID:
        attr_handle = perDevs.cfg_char_handle[i] + 1;
        value_len   = 5; /* 4 bytes for the feature mask + 1 byte for the command (0=ON, 1=OFF) */
        attr_value[0] = (uint8_t)(((uint32_t)FEATURE_MASK_LED_EVENTS) >> 24);
        attr_value[1] = (uint8_t)(((uint32_t)FEATURE_MASK_LED_EVENTS) >> 16);
        attr_value[2] = (uint8_t)(((uint32_t)FEATURE_MASK_LED_EVENTS) >> 8);
        attr_value[3] = (uint8_t)(((uint32_t)FEATURE_MASK_LED_EVENTS));
        attr_value[4] = att_data[3];      
      
        Write_Charac_Value_Without_Resp(conn_handle, attr_handle, value_len, attr_value);
        
        perDevs.readDeviceIdx = i;
        break;
      case MICLEVEL_TYPE_ID:      
        Change_Notification_Status(att_data, attr_value, conn_handle, i, FEATURE_MASK_MIC, notification_freq);
        PRINTF("MIC notifications [%d] %s\n", i, (attr_value[0]==1 ? "ON" : "OFF"));
        break;
      case PRX_TYPE_ID:
        Change_Notification_Status(att_data, attr_value, conn_handle, i, FEATURE_MASK_PROX, notification_freq*SENDING_INTERVAL_100MS_MULTIPLE);
        perDevs.prx_on[i] = attr_value[0];    
        PRINTF("PRX notifications [%d] %s\n", i, (attr_value[0]==1 ? "ON" : "OFF"));
        break;  
      case ACC_TYPE_ID:
        Change_Notification_Status(att_data, attr_value, conn_handle, i, FEATURE_MASK_ACC, SENDING_INTERVAL_100MS_MULTIPLE);
        perDevs.acc_event_enabled = attr_value[0];
        perDevs.agm_on[i] = attr_value[0];        
        PRINTF("ACC notifications [%d] %s\n", i, (attr_value[0]==1 ? "ON" : "OFF"));
        break;
      case GYR_TYPE_ID:
        Change_Notification_Status(att_data, attr_value, conn_handle, i, FEATURE_MASK_GYR, SENDING_INTERVAL_100MS_MULTIPLE);
        perDevs.gyr_event_enabled = attr_value[0];
        perDevs.agm_on[i] = attr_value[0];        
        PRINTF("GYR notifications [%d] %s\n", i, (attr_value[0]==1 ? "ON" : "OFF"));
        break;
      case MAG_TYPE_ID:
       Change_Notification_Status(att_data, attr_value, conn_handle, i, FEATURE_MASK_MAG, SENDING_INTERVAL_100MS_MULTIPLE);
       perDevs.mag_event_enabled = attr_value[0];
       perDevs.agm_on[i] = attr_value[0];
       PRINTF("MAG notifications [%d] %s\n", i, (attr_value[0]==1 ? "ON" : "OFF"));
       break;
      case SFUSION_TYPE_ID:   
        Change_Notification_Status(att_data, attr_value, conn_handle, i, FEATURE_MASK_SENSORFUSION, SENDING_INTERVAL_100MS_MULTIPLE /*NOTIFICATION_FREQ_100*/);  
        perDevs.sfusion_on[i] = attr_value[0];      
        PRINTF("SFUSION notifications [%d] %s\n", i, (attr_value[0]==1 ? "ON" : "OFF"));
        break;
      }
    }
  }  
}

/**
 * @brief  This function disables all notifications
 * @param  None
 * @retval None
 */
void Disable_All_Notifications (void)
{
  uint8_t j;
  uint16_t attr_handle;
  uint8_t  value_len;
  uint8_t  attr_value[2];
  char* feat = NULL;
  
  for (j=0; j<(perDevs.connDevices+perDevs.discDevices); j++) {
    if ((perDevs.mic_event[j]) && (perDevs.mic_event_enabled))  {
      feat = "MIC";      
      perDevs.mic_event_enabled = 0;
      perDevs.mic_event[j] = 0;
      attr_handle = perDevs.mic_char_handle[j] + 2;       
    }
    else if ((perDevs.prx_event[j]) && (perDevs.prx_event_enabled)) {
      feat = "PRX";
      perDevs.prx_event_enabled = 0;
      perDevs.prx_on[j] = 0;
      attr_handle = perDevs.prx_char_handle[j] + 2;        
    }
    else if ((perDevs.agm_event[j]) && (perDevs.agm_event_enabled)) {
      feat = "AGM";
      perDevs.acc_event_enabled = 0;
      perDevs.gyr_event_enabled = 0;
      perDevs.mag_event_enabled = 0;
      perDevs.agm_event_enabled = 0;
      perDevs.agm_on[j] = 0;
      attr_handle = perDevs.agm_char_handle[j] + 2;        
    }
    else if ((perDevs.sfusion_event[j]) && (perDevs.sfusion_event_enabled)) {
      feat = "SFUSION";
      perDevs.sfusion_event_enabled = 0;
      perDevs.sfusion_on[j] = 0;
      attr_handle = perDevs.sfusion_char_handle[j] + 2;        
    }
    else {
      continue;
    }
    
    value_len = 2;
    attr_value[0] = DISABLE;
    
    Write_Charac_Descriptor(perDevs.connection_handle[j], attr_handle, value_len, attr_value);
        
    PRINTF("Set OFF the %s notifications on node %d\n", feat, j);
    
    HAL_Delay(500);    
  }  
}

/**
 * @brief  This function sets some notification properties (e.g. the 
 *         notification period) by sending a command according to the
 *         following format:
 *         FEATURE_MASK (4bytes) + Command (1byte) + Data (1byte)
 *         In case of the notification period the command is 255 while the allowed data are:
 *         50 @5S, 10 @1S, 1 @100mS, 0 @50mS (default).
 * @param  connection handle
 * @param  peripheral device index
 * @param  feature mask                          
 * @param  command
 * @param  data                          
 * @retval None
 */
void Set_Notification_Property(uint16_t conn_handle, uint8_t i, uint32_t feature_mask, uint8_t command, uint8_t data)
{
  uint16_t attr_handle;
  uint8_t  value_len;
  uint8_t  attr_value[6];
          
  attr_handle = perDevs.cfg_char_handle[i] + 1;      
  attr_value[0] = (uint8_t)((feature_mask) >> 24);
  attr_value[1] = (uint8_t)((feature_mask) >> 16);
  attr_value[2] = (uint8_t)((feature_mask) >> 8);
  attr_value[3] = (uint8_t)((feature_mask));        
  attr_value[4] = command; 
  attr_value[5] = data;
  value_len = 6;  /* FEATURE_MASK len (4bytes) + Command len (1byte) + Data len (1byte) */
  
  Write_Charac_Value_Without_Resp(conn_handle, attr_handle, value_len, attr_value);
  
  /* The gatt write function above is w/out response, so we just wait for a short time before going on */
  HAL_Delay(300);
}

/**
 * @brief  This function enables or disables the new peripheral scanning 
 *         sending a notification about the new status to the master
 * @param  The enable/disable command
 * @retval None
 */
void Set_New_Nodes_Scanning(uint8_t enabled) 
{              
  STORE_LE_16(slaveDev.star_config_value,(HAL_GetTick()>>3)); /* Timestamp */
  switch (enabled) {
  case 0x01:
    perDevs.discovery_enabled = TRUE;
    PRINTF("Scanning started (%d)\n", enabled);
    break;
  case 0x02:
    perDevs.discovery_enabled = FALSE;
    PRINTF("Scanning stopped (%d)\n", enabled);
    break;
  default:
    PRINTF("Scanning set to unrecognized value (%d)\n", enabled);
    break;
  }
    
  slaveDev.star_config_value[2] = enabled;
  slaveDev.star_config_value_len = 3;                   
  Notify_Master(slaveDev.star_config_value_len, slaveDev.star_config_value, slaveDev.star_hw_serv_handle, slaveDev.star_config_char_handle);
    
}

/**
 * @brief  This function forwards to the peripheral device the command coming 
 *         from the Cloud
 * @param  The command value
 * @param  The command length
 * @retval None
 */
void Forward_Command_To_BlueNRG(uint8_t* data_ptr, uint8_t data_length)
{
  uint16_t handle;
  uint8_t  buffer[4] = {0, 0, 0, 0};
  uint8_t  even = 0;
  uint8_t  num, odd;
  uint8_t  i;  
  uint8_t cmd_type;
  
  cmd_type = (data_ptr[0] - '0') << 4 | (data_ptr[1] - '0');
  //PRINTF("command %d\n", cmd_type);
  
  switch (cmd_type)
  {
  case 0:
    PRINTF("Scan cmd type (%d)\n", cmd_type);  
    handle = slaveDev.star_config_char_handle + 1;
    break;  
  case 1:
    PRINTF("LED/MIC/PRX cmd type (%d)\n", cmd_type);
    handle = slaveDev.star_data_char_handle + 1;
    break;   
  default:
    PRINTF("Unknown cmd type (%d)\n", cmd_type);
    return;
    break;
  }
    
  /**
  * For Scan cmd type:
  * |Value  |
  * |1 byte |
  * For LED/MIC/PRX cmd type:
  * |Node ID | Type ID | Value   | 
  * |2 bytes | 1 byte  | x bytes |
  */
  for (i=2; i<data_length; i++) {
    switch (data_ptr[i])
    {
    case 'A':
    case 'a':
      num = 10;
      break;
    case 'B':
    case 'b':
      num = 11;
      break;
    case 'C':
    case 'c':
      num = 12;
      break;
    case 'D':
    case 'd':
      num = 13;
      break;
    case 'E':
    case 'e':
      num = 14;
      break;
    case 'F':
    case 'f':
      num = 15;
      break;
    default:
      num = data_ptr[i] - '0';
      break;
    }
    if (!(i%2)) {
      even = num << 4;
    }
    else {
      odd = num;
      buffer[(i-2)/2] = even | odd;
      //PRINTF("%02x ", buffer[i/2]);
    }
  }
  //PRINTF("\n");    
  
  Attribute_Modified_CB(handle, (data_length-2)/2, buffer);

}

/**
 * @brief  This function retrieves the device index from the peripheral device address
 * @param  last two bytes of the device address
 * @retval the device index
 */
uint8_t Get_Device_Index_From_Addr (uint8_t *addr)
{
  uint8_t i;
  
  for (i=0; i<MAX_NUM_OF_NODES; i++)
  {
    if ((perDevs.devInfo[i].bdaddr[NODE_ID_B2] == addr[0]) && (perDevs.devInfo[i].bdaddr[NODE_ID_B1] == addr[1])) {     
      return i;
    }
  }
  return MAX_NUM_OF_NODES;
}

/**
 * @brief  This function forwards to master all notifications from peripherals
 * @param  event data length
 * @param  attribute value
 * @param  handle of the service
 * @param  handle of the characteristic to be updated
 * @retval None
 */
void Notify_Master(uint8_t data_length, uint8_t* data_value, uint16_t serv_handle, uint16_t char_handle)
{
  uint8_t charValOffset = 0x00;
    
  struct timer t;
  Timer_Set(&t, CLOCK_SECOND*10);
  while(aci_gatt_update_char_value(serv_handle, char_handle, charValOffset, data_length,   
                                   data_value)==BLE_STATUS_INSUFFICIENT_RESOURCES){
    LOC_DEBUG("Error while updating characteristic:BLE_STATUS_INSUFFICIENT_RESOURCES\n.\n");
    // Radio is busy (buffer full).
    if(Timer_Expired(&t))
      break;
  }
}

/**
  * @brief  Set the device as a slave in discoverable mode
  * @param  None
  * @retval None
  */
void Set_Slave_Discoverable()
{
  const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME, NAME_BLESTAR1};
  uint8_t ret;
    
  uint8_t manuf_data[21] = {
    2,0x0A,0x00 /* 0 dBm */, // Trasmission Power
    9,0x09,NAME_BLESTAR1,    // Complete Name
    7,0xFF,0x01 /*SKD version */,
           0x81 /* NUCLEO-Board remote */,
           0x00 /* Led Prox+Lux */,
           0x0C /* Star Config Char + Star Data Char */,
           0x00 /* SensorFusionShort */,
           0x00 /* SensorFusionFloat */
  };
  
  /* Turn off the discovery mode */
  if ((perDevs.connDevices == MAX_NUM_OF_NODES) && (slaveDev.is_discoverable == TRUE))
  {
     PRINTF("Setting non discoverable mode\n");
     ret = aci_gap_set_non_discoverable();
     if (ret != BLE_STATUS_SUCCESS){
       PRINTF("Error while setting non discoverable mode (0x%04x).\n", ret);
     }     
     HAL_Delay(100);
  }
  
  if (perDevs.discovery_enabled == TRUE) {    
    /* disable scan response */
    LOC_DEBUG("Disabling scan response\n");
    ret = hci_le_set_scan_resp_data(0,NULL);
    if (ret) {
      PRINTF("Error while setting scan response data! (0x%02x)\n", ret);
    }    
    
    
    PRINTF("General Discoverable Mode\n");
    /*
    Advertising_Event_Type, Adv_Interval_Min, Adv_Interval_Max, Address_Type, Adv_Filter_Policy,
    Local_Name_Length, Local_Name, Service_Uuid_Length, Service_Uuid_List, Slave_Conn_Interval_Min,
    Slave_Conn_Interval_Max
    */  
    ret = aci_gap_set_discoverable(ADV_IND, 0x20, 0x100, PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                   sizeof(local_name)+1, local_name, 0, NULL, 0x0006, 0x0008);  
    if (ret) {
      PRINTF("Error while setting discoverable mode! (0x%02x)\n", ret);
    }  
    
    
    /* Send Advertising data */
    LOC_DEBUG("Updating advertising data\n");
    ret = aci_gap_update_adv_data(21, manuf_data);  
    if (ret) {
      PRINTF("Error while updating adv data! (0x%02x)\n", ret);
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
