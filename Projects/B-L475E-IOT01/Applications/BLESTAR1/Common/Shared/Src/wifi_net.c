/**
  ******************************************************************************
  * @file    wifi_net.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    26-October-2017
  * @brief   Wifi-specific NET initialization.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "wifi.h"
#include "iot_flash_config.h"

#ifdef USE_WIFI_IOT01


/* Private defines -----------------------------------------------------------*/
#define  WIFI_CONNECT_MAX_ATTEMPT_COUNT  3

#ifdef ES_WIFI_MAX_SSID_NAME_SIZE
#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_MAX_SSID_NAME_SIZE
#define WIFI_PAYLOAD_SIZE                           ES_WIFI_PAYLOAD_SIZE
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
int net_if_init(void * if_ctxt);
int net_if_deinit(void * if_ctxt);

/* Functions Definition ------------------------------------------------------*/
int net_if_init(void * if_ctxt)
{
  const char *ssid;
  const char  *psk;
  WIFI_Ecn_t security_mode;
  char moduleinfo[WIFI_PRODUCT_INFO_SIZE];
  WIFI_Status_t wifiRes;
  uint8_t macAddress[6];
  int wifiConnectCounter = 0;
  bool skip_reconf = false;

  printf("\n*** WIFI connection ***\n\n");

  skip_reconf = (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) == HAL_OK);

  if (skip_reconf == true)
  {
    printf("Push the User Button (blue) within the next 5 seconds if you want to update"
           " the WiFi network configuration.\n\n");

    skip_reconf = (Button_WaitForPush(5000) == BP_NOT_PUSHED);
  }
  
  if (skip_reconf == false)
  {
    printf("Your WiFi parameters need to be entered to proceed.\n");
    do
    {
      updateWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode);
    } while (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) != HAL_OK);
  }
  
  /*  Wifi Module initialization */
  printf("Initializing the WiFi module\n");
  
  wifiRes = WIFI_Init();
  if ( WIFI_STATUS_OK != wifiRes )
  {
    printf("Failed to initialize WIFI module\n");
    return -1;
  }
    
  /* Retrieve the WiFi module mac address to confirm that it is detected and communicating. */
  WIFI_GetModuleName(moduleinfo);
  printf("Module initialized successfully: %s",moduleinfo);
  
  WIFI_GetModuleID(moduleinfo);
  printf(" %s",moduleinfo);
  
  WIFI_GetModuleFwRevision(moduleinfo);
  printf(" %s\n",moduleinfo);
  
  printf("Retrieving the WiFi module MAC address:");
  wifiRes = WIFI_GetMAC_Address( (uint8_t*)macAddress);
  if ( WIFI_STATUS_OK == wifiRes)
  {
    printf(" %02x:%02x:%02x:%02x:%02x:%02x\n",
         macAddress[0], macAddress[1], macAddress[2],
         macAddress[3], macAddress[4], macAddress[5]);
  }
  else
  {
       printf("Failed to get MAC address\n");
  }
  /* Connect to the specified SSID. */

  printf("\n");
  do 
  {
    printf("\rConnecting to AP: %s  Attempt %d/%d ...",ssid, ++wifiConnectCounter,WIFI_CONNECT_MAX_ATTEMPT_COUNT);
    wifiRes = WIFI_Connect(ssid, psk, security_mode);
    if (wifiRes == WIFI_STATUS_OK) break;
  } 
  while (wifiConnectCounter < WIFI_CONNECT_MAX_ATTEMPT_COUNT);
  
  if (wifiRes == WIFI_STATUS_OK)
  {
    printf("\nConnected to AP %s\n",ssid);
  }
  else
  {
    printf("\nFailed to connect to AP %s\n",ssid);
  }
  
  return (wifiRes == WIFI_STATUS_OK)?0:-1;
}


int net_if_deinit(void * if_ctxt)
{
  return 0;
}

#endif

#ifdef USE_WIFI_SPWF04

#include "wifi_interface.h"

#define ES_WIFI_MAX_SSID_NAME_SIZE                  50    
#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_MAX_SSID_NAME_SIZE
wifi_config config;

typedef enum {
  wifi_state_reset = 0,
  wifi_state_ready,
  wifi_state_idle,
  wifi_state_connected,
  wifi_state_connecting,
  wifi_state_socket,
  wifi_state_socket_write,
  wifi_state_disconnected,
  wifi_state_activity,
  wifi_state_inter,
  wifi_state_print_data,
  wifi_state_input_buffer,
  wifi_state_error,
  wifi_undefine_state       = 0xFF,
} wifi_state_t;

wifi_state_t wifi_state;

/* Functions Definition ------------------------------------------------------*/
int net_if_init(void * if_ctxt)
{
  const char *ssid;
  const char  *psk;
  WIFI_Ecn_t security_mode;
  char moduleinfo[WIFI_PRODUCT_INFO_SIZE];
  WIFI_Status_t wifiRes = WIFI_STATUS_ERROR;
//  uint8_t macAddress[6];
  int wifiConnectCounter = 0;
  bool skip_reconf = false;

  printf("\n*** WIFI connection ***\n\n");
  
  skip_reconf = (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) == HAL_OK);

  if (skip_reconf == true)
  {
    printf("Push the User Button (blue) within the next 5 seconds if you want to update"
           " the WiFi network configuration.\n\n");

    skip_reconf = (Button_WaitForPush(5000) == BP_NOT_PUSHED);
  }
  
  if (skip_reconf == false)
  {
    printf("Your WiFi parameters need to be entered to proceed.\n");
    do
    {
      updateWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode);
    } while (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) != HAL_OK);
  }
  
  /*  Wifi Module initialization */
  printf("Initializing the WiFi module\n");
  
  {
    
  config.power=wifi_active;
  config.power_level=high;
  config.dhcp=on;//use DHCP IP address
  config.web_server=WIFI_TRUE;
  config.mcu_baud_rate = 115200;
  UART_Configuration(115200); 
  
  wifiRes = wifi_init(&config);
  if ( WIFI_STATUS_OK != wifiRes )
  {
    printf("Failed to initialize WIFI module\n");
    return -1;
  }

    
  }
  
  /* wait here to be ready */
  while(wifi_state != wifi_state_ready)
  {
        HAL_Delay(500);
  }
  
  
  if((wifiRes = wifi_connect( (char *)ssid, (char *)psk, security_mode))!= WiFi_MODULE_SUCCESS)
  {
        printf("\r\nError in AP Connection");
        return (wifiRes == WIFI_STATUS_OK)?0:-1;
  }
  
    
    /* wait here to be connected */
  while(wifi_state != wifi_state_connected)
  {
        HAL_Delay(500);
  }

  
  return (wifiRes == WIFI_STATUS_OK)?0:-1;
}

void ind_wifi_on() 
{
    /*Critical. Never change it*/
    wifi_state = wifi_state_ready; 
}


/**
* @brief Called asychronously by wifi driver when wifi device is connected to AP
*
* 
* @param no parameter
* @return no return
*/

void ind_wifi_connected()
{
    printf("\r\nwifi connected to AP\r\n");
    wifi_state = wifi_state_connected;
}

int net_if_deinit(void * if_ctxt)
{
  return 0;
}

#endif

#ifdef USE_WIFI_SPWF01

#include "wifi_interface.h"

#define ES_WIFI_MAX_SSID_NAME_SIZE                  50    
#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_MAX_SSID_NAME_SIZE
wifi_config config;

typedef enum {
  wifi_state_reset = 0,
  wifi_state_ready,
  wifi_state_idle,
  wifi_state_connected,
  wifi_state_connecting,
  wifi_state_socket,
  wifi_state_socket_write,
  wifi_state_disconnected,
  wifi_state_activity,
  wifi_state_inter,
  wifi_state_print_data,
  wifi_state_input_buffer,
  wifi_state_error,
  wifi_undefine_state       = 0xFF,
} wifi_state_t;

wifi_state_t wifi_state;
/* Functions Definition ------------------------------------------------------*/
int net_if_init(void * if_ctxt)
{
  const char *ssid;
  const char  *psk;
  WIFI_Ecn_t security_mode;
  char moduleinfo[WIFI_PRODUCT_INFO_SIZE];
  WIFI_Status_t wifiRes = WIFI_STATUS_ERROR;
//  uint8_t macAddress[6];
  int wifiConnectCounter = 0;
  bool skip_reconf = false;

  printf("\n*** WIFI connection ***\n\n");
  
  skip_reconf = (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) == HAL_OK);

  if (skip_reconf == true)
  {
    printf("Push the User Button (blue) within the next 5 seconds if you want to update"
           " the WiFi network configuration.\n\n");

    skip_reconf = (Button_WaitForPush(5000) == BP_NOT_PUSHED);
  }
  
  if (skip_reconf == false)
  {
    printf("Your WiFi parameters need to be entered to proceed.\n");
    do
    {
      updateWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode);
    } while (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) != HAL_OK);
  }
  
  /*  Wifi Module initialization */
  printf("Initializing the WiFi module\n");
  
  {
    
    config.power=wifi_active;
    config.power_level=high;
    config.dhcp=on;//use DHCP IP address
    config.web_server=WIFI_TRUE;
    config.mcu_baud_rate = 115200;
    UART_Configuration(115200); 

    
    wifiRes = wifi_init(&config);
    if ( WIFI_STATUS_OK != wifiRes )
    {
      printf("Failed to initialize WIFI module\n");
      return -1;
    }
    
    /* wait here to be ready */
    while(wifi_state != wifi_state_ready)
    {
          HAL_Delay(500);
    }
    
    if((wifiRes = wifi_connect( (char *)ssid, (char *)psk, security_mode))!= WiFi_MODULE_SUCCESS)
    {
          printf("\r\nError in AP Connection");
          return (wifiRes == WIFI_STATUS_OK)?0:-1;
    }
    
      
      /* wait here to be connected */
    while(wifi_state != wifi_state_connected)
    {
          HAL_Delay(500);
    }


    
  }
  
  
  return (wifiRes == WIFI_STATUS_OK)?0:-1;
}

void ind_wifi_on() 
{
    /*Critical. Never change it*/
    wifi_state = wifi_state_ready; 
}


/**
* @brief Called asychronously by wifi driver when wifi device is connected to AP
*
* 
* @param no parameter
* @return no return
*/

void ind_wifi_connected()
{
    printf("\r\nwifi connected to AP\r\n");
    wifi_state = wifi_state_connected;
}


int net_if_deinit(void * if_ctxt)
{
  return 0;
}

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
