/**
******************************************************************************
* @file    wifi_support.c 
* @author  AAS / CL
* @version V1.0.0
* @date    23-Nov-2015
* @brief   This file implements the generic functions for WiFi communication.
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

/*******************************************************************************
 * Include Files
*******************************************************************************/
#include <stdio.h>
#include "string.h"
#include "wifi_support.h"
#include "cube_hal.h"
#include "MQTTClient.h"  
#include "stdbool.h"

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
 
/** @defgroup STM32F401RE-Nucleo_WIFI_SUPPORT WIFI_SUPPORT 
 * @{
 */
 
/*******************************************************************************
 * Macros
*******************************************************************************/

/******************************************************************************
 * Local Variable Declarations
******************************************************************************/

/******************************************************************************
 * Global Variable Declarations
******************************************************************************/
/** @defgroup STM32F401RE-Nucleo_WIFI_SUPPORT_Exported_Variables Exported Variables
 *  @{
 */
uint8_t        console_input[1];
uint8_t        console_count=0;
wifi_bool      set_AP_config = WIFI_FALSE;
wifi_bool      SSID_found = WIFI_FALSE;
char           console_ssid[40];
char           console_psk[20];
char           console_host[20];
uWifiTokenInfo UnionWifiToken;

extern UART_HandleTypeDef UartMsgHandle;
extern char print_msg_buff[MAX_BUFFER_GLOBAL];

/* Matteo - UART1*/
extern UART_HandleTypeDef huart1;
extern char print_msg_buff_UART1[512];


wifi_state_t wifi_state;
wifi_config  config;
Network n;

TIM_HandleTypeDef  MQTTtimHandle;

/**
 * @}
 */
 
/******************************************************************************
 * Function Declarations
******************************************************************************/
/** @defgroup STM32F401RE-Nucleo_WIFI_SUPPORT_Exported_Functions Exported Functions
 *  @{
 */

/******************************************************************************
 * Function Definitions 
******************************************************************************/
/**
  * @brief  Query the User for SSID, password, encryption mode and hostname
  * @param  None
  * @retval WiFi_Status_t
  */
System_Status_t wifi_get_AP_settings(void)
{
  System_Status_t status = MODULE_ERROR;
  uint8_t console_input[1], console_count=0;
  char console_ssid[NDEF_WIFI];
  char console_psk[NDEF_WIFI];
  WiFi_Priv_Mode mode;
  
  PRINTF("\r\nEnter the SSID:\n");
  fflush(stdout);  
  console_count = 0;
  console_count = scanf("%s",console_ssid);
  PRINTF("\r\n");
  // FIXME : Why 39. NDEF is 32
  if (console_count == NDEF_WIFI) {
    PRINTF("Exceeded number of ssid characters permitted");
    return status;
  }    
  
  PRINTF("Enter the password:");
  fflush(stdout);
  console_count = 0;  
  console_count = scanf("%s",console_psk);
  PRINTF("\r\n");
  // FIXME : Why 19. NDEF is 32
  if (console_count == NDEF_WIFI) {
    PRINTF("Exceeded number of psk characters permitted");
    return status;
  }  

  PRINTF("Enter the authentication mode(0:Open, 1:WEP, 2:WPA2/WPA2-Personal):"); 
  fflush(stdout);
  scanf("%s",console_input);
  PRINTF("\r\n");
  //PRINTF("entered =%s\r\n",console_input);
  switch (console_input[0])
  {
  case '0':
    mode = None;
    break;
  case '1':
    mode = WEP;
    break;
  case '2':
    mode = WPA_Personal;
    break;
  default:
    PRINTF("\r\nWrong Entry. Priv Mode is not compatible\n");
    return status;              
  }
  
  memcpy(WIFITOKEN.NetworkSSID, console_ssid, strlen(console_ssid));
  memcpy(WIFITOKEN.NetworkKey, console_psk, strlen(console_psk));
  if (mode == None)
    memcpy(WIFITOKEN.AuthenticationType, "NONE", strlen("NONE"));
  else if (mode == WEP)
    memcpy(WIFITOKEN.AuthenticationType, "WEP", strlen("WEP"));
  else
    memcpy(WIFITOKEN.AuthenticationType, "WPA2", strlen("WPA2"));
  
  status = MODULE_SUCCESS;
  
  return status;
}

/**
  * @brief  Initialize MQTT interface
  * @param  None
  * @retval System_Status_t (MODULE_SUCCESS/MODULE_ERROR)
  */
System_Status_t InitMQTT(void)
{
  System_Status_t status = MODULE_ERROR;

  /* Initialize network interface for MQTT  */
  NewNetwork(&n);
  /* Initialize MQTT timers */
  status = MQTTtimer_init();
  if(status!=MODULE_SUCCESS)
  {
    printf("\r\n[E].Error in MQTTtimer_init \r\n");
    return status;
  }  

  status = MODULE_SUCCESS;
  return status;
} 

/**
  * @brief  Init timer for MQTT
  * @param  None
  * @retval None
  */
System_Status_t MQTTtimer_init(void) {
  System_Status_t status = MODULE_ERROR;
  
  __TIM4_CLK_ENABLE();
  
  MQTTtimHandle.Instance = TIM4;
  MQTTtimHandle.Init.Period = 10-1;    //1 ms
  MQTTtimHandle.Init.Prescaler =  (uint32_t)(SystemCoreClock / 10000) - 1;
  MQTTtimHandle.Init.ClockDivision = 0;
  MQTTtimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;  
  if(HAL_TIM_Base_Init(&MQTTtimHandle) != HAL_OK)
  {
    return status;
  }
  
  if (HAL_TIM_Base_Start_IT(&MQTTtimHandle) != HAL_OK)
  {
    return status;
  }
  
  HAL_NVIC_SetPriority(TIM4_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(TIM4_IRQn);
  
  status = MODULE_SUCCESS;
  return status;
}

/**
  * @brief  Save Access Point parameters to FLASH
  * @param  None
  * @retval System_Status_t (MODULE_SUCCESS/MODULE_ERROR)
  */
System_Status_t SaveSSIDPasswordToMemory(void)
{
  System_Status_t status = MODULE_ERROR;
  
  /* Reset Before The data in Memory */
  status = ResetSSIDPasswordInMemory();
  
  if (status) {
    /* Store in Flash Memory */
    uint32_t Address = BLUEMSYS_FLASH_ADD;
    int32_t WriteIndex;
    
    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();
    
    /* Write the Magic Number */
    {
      uint32_t MagicNumber = WIFI_CHECK_SSID_KEY;
      if (HAL_FLASH_Program(TYPEPROGRAM_WORD, Address,MagicNumber) == HAL_OK) {
        Address = Address + 4;
      } else {
        PRINTF("\r\nError while writing in FLASH");
        status = MODULE_ERROR;
      }
    }
    
    /* Write the Wifi */
    for (WriteIndex=0;WriteIndex<UNION_DATA_SIZE;WriteIndex++) {
      if (HAL_FLASH_Program(TYPEPROGRAM_WORD, Address,UnionWifiToken.Data[WriteIndex]) == HAL_OK){
        Address = Address + 4;
      } else {
        PRINTF("\r\nError while writing in FLASH");
        status = MODULE_ERROR;
      }
    }
    
    /* Lock the Flash to disable the flash control register access (recommended
    to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
    
    PRINTF("\n\rSaveSSIDPasswordToMemory OK");
  }
  else
    PRINTF("\n\rError while resetting FLASH memory");
  
  return status;
}

/**
  * @brief  Erase Access Point parameters from FLASH
  * @param  None
  * @retval System_Status_t (MODULE_SUCCESS/MODULE_ERROR)
  */
System_Status_t ResetSSIDPasswordInMemory(void)
{
  /* Reset Calibration Values in FLASH */
  System_Status_t status = MODULE_ERROR;

  /* Erase First Flash sector */
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SectorError = 0;

  EraseInitStruct.TypeErase = TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = VOLTAGE_RANGE_3;
  EraseInitStruct.Sector = BLUEMSYS_FLASH_SECTOR;
  EraseInitStruct.NbSectors = 1;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK){
    /* Error occurred while sector erase. 
      User can add here some code to deal with this error. 
      SectorError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
      FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
      PRINTF("\n\rError while erasing FLASH memory");
  } else
      status = MODULE_SUCCESS;
  
  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  return status;
}

/**
  * @brief  Read Access Point parameters from FLASH
  * @param  None
  * @retval System_Status_t (MODULE_SUCCESS/MODULE_ERROR)
  */
System_Status_t ReCallSSIDPasswordFromMemory(void)
{
  System_Status_t status = MODULE_ERROR;
  
  uint32_t Address = BLUEMSYS_FLASH_ADD;
  __IO uint32_t data32 = *(__IO uint32_t*) Address;
  if(data32== WIFI_CHECK_SSID_KEY){
    int32_t ReadIndex;

    for(ReadIndex=0;ReadIndex<UNION_DATA_SIZE;ReadIndex++){
      Address +=4;
      data32 = *(__IO uint32_t*) Address;
      UnionWifiToken.Data[ReadIndex]=data32;
    }
    status = MODULE_SUCCESS;
  }
  else
    PRINTF("\r\nFLASH Keyword not found.");
  
  return status;
}

/******** Wi-Fi Indication User Callback *********/
void ind_wifi_on()
{
  //printf("\r\n[D]. Wi-Fi on \r\n");
  wifi_state = wifi_state_ready;
}

void ind_wifi_connected()
{
  wifi_state = mqtt_socket_create;
}

void ind_wifi_resuming()
{
  printf("\r\nwifi resuming from sleep user callback... \r\n");
}

/**
  * @brief  Wifi callback activated when remote server is closed  
  * @param  socket_closed_id : socket identifier  
  * @retval None
  */
void ind_wifi_socket_client_remote_server_closed(uint8_t * socket_closed_id)
{
  PRINTF("\r\n[E]. Remote disconnection from server. Trying to reconnect to MQTT broker... \r\n");
  // HAL_Delay (2000); 
  
  //spwf_socket_close()
  if (wifi_state != wifi_state_reconnect)
    wifi_state = mqtt_socket_create;
}

/**
  * @brief  Wi-Fi callback activated in case of connection error
  * @param  wifi_status : Error Code  
  * @retval None
  */
void ind_wifi_connection_error(WiFi_Status_t wifi_status)
{
  PRINTF("\r\n [E]. WiFi connection error. Trying to reconnect to AP after some seconds ... \r\n"); 
 
  switch (wifi_status)
  {
     case WiFi_DE_AUTH:
         printf("[E] Error code WiFi_DE_AUTH  \r\n");        
         break;
     case WiFi_DISASSOCIATION:
         printf("[E] Error code WiFi_ DISASSOCIATION  \r\n");
         break;
     case WiFi_JOIN_FAILED:
         printf("[E] Error code WiFi_JOIN_FAILED  \r\n");
         break;
     case WiFi_SCAN_BLEWUP:
         printf("[E] Error code WiFi_ SCAN_BLEWUP \r\n");
         break;
     case WiFi_SCAN_FAILED:
         printf("[E] Error code WiFi_ SCAN_FAILED \r\n");
         break;
     default:
    	 break;
  }
  
  wifi_state = wifi_state_disconnected;
}

/**
  * @brief  Initialize WiFi board
  * @param  None
  * @retval System_Status_t (MODULE_SUCCESS/MODULE_ERROR)
  */
System_Status_t ConfigWiFi(void)
{
  System_Status_t status = MODULE_ERROR;

  /* Config WiFi : disable low power mode */
  config.power=wifi_active;
  config.power_level=high;
  config.dhcp=on;//use DHCP IP address
  config.web_server=WIFI_TRUE;  
  
  status = MODULE_SUCCESS;
  return status;
} 

/**
  * @brief  Configure Access Point parmaters (SSID, PWD, Authenticaation) :
  * @brief  1) Read from FLASH
  * @brief  2) If not available in FLASH or when User Button is pressed : read from NFC
  * @brief  3) If not available in FLASH or when User Button is pressed and if nothing is written in NFC : read from serial terminal
  * @param  None
  * @retval System_Status_t (MODULE_SUCCESS/MODULE_ERROR)
  */
System_Status_t ConfigAPSettings(void)
{
  System_Status_t status = MODULE_ERROR;
  bool b_set_AP_pars = false;
  uint8_t i;
  
  PRINTF("\r\nKeep pressed user button within next 5 seconds to set Wi-Fi Access Point parameters (SSID and PWD) "); 
  PRINTF("\r\nvia serial terminal. Otherwise parameters saved to FLASH will be used.\n");
  for (i=0; i<5; i++) {
    PRINTF("\b\b\b\b...%d", 4-i);
    fflush(stdout);
    HAL_Delay(1000);
  }
  PRINTF("\b\b\b\b");
  fflush(stdout);
  
  /* User Button Pressed --> set parameters */
  if(BSP_PB_GetState(BUTTON_KEY) != GPIO_PIN_RESET) {
    /* Read from FLASH */
    if(ReCallSSIDPasswordFromMemory() > 0) {
      PRINTF("\n\rRead from FLASH:\n\r");
      PRINTF("\tSSID ............ = %s\n\r", WIFITOKEN.NetworkSSID);
      PRINTF("\tKey ............. = %s\n\r", WIFITOKEN.NetworkKey);
      PRINTF("\tAuthentication .. = %s\n\r", WIFITOKEN.AuthenticationType);
      
      status = MODULE_SUCCESS;
    }
    else { 
      PRINTF("\n\rNo data written in FLASH.");
      b_set_AP_pars = true;
    }   
  }  
  else
    b_set_AP_pars = true;
  
  if (b_set_AP_pars) {
    // Blink LED
    /*
    printf("\r\nLED ON....\n\r");
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); 
    BSP_LED_On(LED2);
    HAL_Delay(3000);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
    BSP_LED_Off(LED2);
    */
    status = MODULE_ERROR;
    
    if (status==MODULE_SUCCESS) {
      PRINTF("\r\n[E]. Error in ConfigAPSettings. \r\n");      
    }     
    else {
      PRINTF("\n\rRead parameters from serial terminal."); 
      
      /* Read from Serial.  */
      status = wifi_get_AP_settings();        
      if (status != MODULE_SUCCESS) {
        PRINTF("\r\n[E]. Error in AP Settings\r\n");
        return status;
      }
    }
    /* Save to FLASH */  
    status = SaveSSIDPasswordToMemory();
    if (status != MODULE_SUCCESS) {
      PRINTF("\r\n[E]. Error in AP Settings\r\n");
      return status;
    }
  }
  
  return status;
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
