  /**
  ******************************************************************************
  * @file    main.c
  * @author  Central LAB
  * @version V1.0.0
  * @date    28-Sep-2016
  * @brief   Main program body
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
#include "main.h"
#include "cube_hal.h"

#include "wifi_module.h"
#include "wifi_globals.h"
#include "wifi_support.h"
#include "wifi_interface.h"
#include "IBM_Bluemix_Config.h"

#include "ble_master_service.h"
#include "ble_slave_service.h"
#include "btle_config.h"
#include "debug.h"
#include "bluenrg_utils.h"

#include "stdio.h"
#include "string.h"
#include "math.h"

/**
   *! @mainpage FP-NET-BLESTAR1 firmware package
   * <b>Introduction</b> <br>
   * This firmware package includes Components Device Drivers, Board Support Package and
   * example application for STMicroelectronics X-NUCLEO-IDW01M1 Wi-Fi expansion board and
   * X-NUCLEO-IDB05A1 BlueNRG-MS expansion board.
   * <b>Example applications included in this package:</b><br>
   * <b>BLESTAR1</b><br>
   * In this application the STM32 Nucleo board, equipped with both the WiFi and the BlueNRG
   * expansion boards, acts as a BLE Master connected to several BLE Peripherals (nodes)
   * according to a star-topology network.
   * The device exports to the IBM Watson IoT Cloud platform via WiFi all data coming from the 
   * peripheral nodes.
   * The url where data can be seen is printed in the serial console and is of type:
   * https://quickstart.internetofthings.ibmcloud.com/#/device/<the_device_MAC_address>/sensor/
   * After starting the reading of data from peripherals, the device starts acting also as a
   * slave, accepting connection from a BLE client (tipically an Android or iOS smartphone/tablet).
   * By using an app for the parsing of the BLE characteristics, data from peripherals can be seen.
   * It is also possible to send command to the peripherals from the BLE client.
   * In this sample it is possible to send a command for turning on/off the LED on the STM32 Nucleo
   * boards acting as peripherals.
   * The firmware for BLE peripheral nodes is provided with the package at
   * Utilities\Node_Firmware\
   * Alternatively the firmware in the packages FP-SNS-MOTENV1, FP-SNS-ALLMEMS1, FP-SNS-FLIGHT1 
   * available on www.st.com, can be used.<br>
   * <b>Toolchain Support</b><br>
   * The library has been developed and tested on following toolchains:
   *        - IAR Embedded Workbench for ARM (EWARM) toolchain V7.80.4 + ST-Link
   *        - Keil Microcontroller Development Kit (MDK-ARM) toolchain V5.24.2 + ST-LINK
   *        - System Workbench for STM32 (SW4STM32) V2.4.0 + ST-LINK
*/

/** @defgroup Projects Projects
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

/** @defgroup STM32F401RE-Nucleo_MAIN MAIN
 * @{
 */
  
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup STM32F401RE-Nucleo_MAIN_Private_Function_Prototypes Function Prototypes
 * @{
 */ 
/* Private function prototypes -----------------------------------------------*/
/* MQTT Callback for messages arrived ----------------------------------------*/
void messageArrived(MessageData* md);

void WiFi_Init(WiFi_Status_t* status);
void WiFi_Process(WiFi_Status_t* status);
static System_Status_t Get_MAC_Add (char *macadd);

//static void floatToInt(float in, int32_t *out_int, int32_t *out_dec, int32_t dec_prec);
static void NotifyLEDOn(void);
static void NotifyLEDOff(void);
static void NotifyLEDBlink(unsigned int msPeriod);
static void prepare_json_pkt(uint8_t* buffer);

static void BlueNRG_Process(void);

/* Matteo - UART1*/
static void MX_USART1_UART_Init(void);

/**
 * @}
 */

/** @defgroup STM32F401RE-Nucleo_MAIN_Private_Variables Private Variables
 * @{
 */ 

/* Private Variables ---------------------------------------------------------*/
/* WiFi */
extern wifi_state_t wifi_state;
extern wifi_config config;
wifi_scan net_scan[WIFI_SCAN_BUFFER_LIST];
uint8_t wifi_present;

uint8_t wifi_data[256];
volatile uint8_t new_data = 0;
volatile char *data = "Data not available!";
char *protocol = "t";//t -> tcp , s-> secure tcp
uint8_t macadd[17];
uint8_t json_buffer[512];

extern uWifiTokenInfo UnionWifiToken;
extern WiFi_Priv_Mode mode;
extern uint8_t console_input[1];
extern uint8_t console_count;
extern wifi_bool set_AP_config;
extern wifi_bool SSID_found;
extern char console_ssid[40];
extern char console_psk[20];
extern char console_host[20];
#if 0
  /* Default configuration SSID/PWD */ 
  char * ssid = "STM";
  char * seckey = "STMdemoPWD";
#else
  char * ssid = NULL;
  char * seckey = NULL;
#endif
WiFi_Priv_Mode mode = WPA_Personal;

/* MQTT */
unsigned char MQTT_read_buf[512];
unsigned char MQTT_write_buf[512];
extern Network n;
Client  c;
MQTTMessage  MQTT_msg;
uint8_t url_ibm[80]; 
MQTT_vars mqtt_ibm_setup;
ibm_mode_t ibm_mode;  //This could be moved to IBM struct. 
MQTTPacket_connectData options = MQTTPacket_connectData_initializer;

extern TIM_HandleTypeDef  MQTTtimHandle;
TIM_IC_InitTypeDef sConfig;

/* UART */
extern UART_HandleTypeDef UartHandle;
extern UART_HandleTypeDef UartMsgHandle;
extern char print_msg_buff[512];

/* Matteo - UART1 */
extern UART_HandleTypeDef huart1;
extern char print_msg_buff_UART1[512];

/* BlueNRG */
extern PeripheralDevices_t perDevs;
extern SlaveDevice_t slaveDev;
extern uint8_t bnrg_expansion_board_type;
uint8_t start_bnrg;
uint8_t bnrg_initialized;

/**
 * @}
 */

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{   
  WiFi_Status_t status_spwf = WiFi_MODULE_SUCCESS;
  uint8_t DisplayName[32];
  
  __GPIOA_CLK_ENABLE();
  HAL_Init();
  
  /* Configure the system clock */
  SystemClock_Config();
  
  /* Configure the timers */
  Timer_Config();
  
  /* Configure UART and USART */
  UART_Configuration(WiFi_USART_BAUD_RATE); 
#ifdef USART_PRINT_MSG
  UART_Msg_Gpio_Init();
  USART_PRINT_MSG_Configuration(USART_SPEED_MSG);
#endif  
  
  /* Matteo - UART1*/
  MX_USART1_UART_Init();
  PRINTF_UART1("\rTest UART1\n");
  
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
  BSP_LED_Init(LED2);  
  
  start_bnrg = 0;
  bnrg_initialized = 0;
  
  HAL_Delay(1000);
  
  PRINTF("\r\n\n/*******************************************************\n");
  PRINTF("\r*                                                      *\n");
  PRINTF("\r* FP-NET-BLESTAR1 Expansion Software v2.0.0            *\n");
  PRINTF("\r*                                                      *\n");
  PRINTF("\r*******************************************************/\n");
  
  /*
   * Start first WiFi and then BlueNRG to avoid interferences between the processes.
   * The BlueNRG waits for the WiFi to start succesfully, otherwise BlueNRG starts
   * anyway.
   * The BlueNRG start is triggered by the variable start_bnrg.
   */  
  PRINTF("\n\n***** Starting Wi-Fi module *****\n\n");
  
  if (check_wifi_hw() == WiFi_MODULE_SUCCESS) {    
    PRINTF("Wi-Fi module detected");
#if ENABLE_WIFI
    PRINTF("!\n\n");
#else 
    PRINTF(" but disabled!\n\n");
#endif
    PRINTF("BE SURE THE JUMPERS ARE AT THE RIGHT POSITION ON JP3 and JP4\n");
    PRINTF("(see the application README file for more info).\n");
    wifi_present = (ENABLE_WIFI);
  }
  else {
    PRINTF("WARNING: no Wi-Fi module detected!!!\n");    
    /* LED blinking to signal that the Wi-Fi module detection has failed */
    BSP_LED_On(LED2);
    HAL_Delay(1000);
    BSP_LED_Off(LED2);
    wifi_present = (FALSE);    
  }
  
  if (wifi_present) {    
    if ((ssid!=NULL) && (seckey!=NULL)) {
      PRINTF("\r\nUsing SSID and PWD written in the code. "); 
      
      memcpy(WIFITOKEN.NetworkSSID, ssid, strlen(ssid));
      memcpy(WIFITOKEN.NetworkKey, seckey, strlen(seckey));
      memcpy(WIFITOKEN.AuthenticationType, "WPA2", strlen("WPA2"));
    }
    else {
      PRINTF("\r\nStarting configure procedure for SSID and PWD....  "); 
      HAL_Delay(3000);
      
      if (ConfigAPSettings() < 0) {  
        PRINTF("\n\rFailed to set AP settings."); 
        return 0; 
      }   
      else
        PRINTF("\n\rAP settings set.");         
    }      
    
    /* WiFi Init */
    if (ConfigWiFi() < 0) {  
      PRINTF("\n\rFailed to Initialize Wi-Fi."); 
      return 0; 
    }   
    else
      PRINTF("\n\rWi-Fi initialized.");   
    
    /* Init MQTT intferface */
    if (InitMQTT () < 0) {   
      PRINTF("\n\rFailed to Initialize MQTT interface"); 
      return 0; 
    }   
    else {
      PRINTF("\n\rMQTT initialized.");   
    }
    
    /* Initialize WiFi module */
    wifi_state = wifi_state_idle;
    if (wifi_init(&config) != WiFi_MODULE_SUCCESS) {
      PRINTF("\r\n[E].Error in Wi-Fi init \r\n");
      return 0;
    }    
    
    /*  Get MAC Address */  
    if (Get_MAC_Add((char *)DisplayName) < 0) { 
      PRINTF("\r\n[E]. Error while retrieving MAC address \r\n");  
      return 0; 
    }
    else {
      LOC_DEBUG("\r\n [D]. Wi-Fi MAC address: \r\n");
      LOC_DEBUG((char *)DisplayName);
    }  
    
    /* Config MQTT Infrastructure */
    Config_MQTT_IBM (&mqtt_ibm_setup, DisplayName); 
    /* Compose Quickstart URL */  
    Compose_Quickstart_URL (url_ibm, DisplayName); 
    
    PRINTF("\r\nIBM Quickstart URL (https://+)  \r\n");
    PRINTF((char *)url_ibm); 
    PRINTF("\r\n");
  } 
  else {
#if ENABLE_WIFI
    PRINTF("Please, check that the Wi-Fi module is correctly plugged.\n");
#else 
    PRINTF("\nTo enable Wi-Fi, please, set the define ENABLE_WIFI to 1 (file main.h).\n"); 
#endif
  }

  while(1) {   
    if (wifi_present && (perDevs.status!=NOTIFICATIONS_DATA_READ)) {    
      /* Start WiFi process */
      WiFi_Process(&status_spwf);
    }
    else {
      start_bnrg = 1;    
      if (perDevs.status!=NOTIFICATIONS_DATA_READ) {
        HAL_Delay(1000);
      }
    }
    
    if (start_bnrg) {
      if (!bnrg_initialized) {        
        PRINTF("\n\n***** Starting BlueNRG module *****\n\n");
        /* Initialize BlueNRG module and scanning */      
        BlueNRG_Init();
        bnrg_initialized = 1;
      }
      /* Start BlueNRG connection, characteristics discovering and enable notifications */
      BlueNRG_Process();
    }
  }   
}

/**
  * @brief  Start the WiFi process
  * @param  The WiFi status
  * @retval None
  */
void WiFi_Process(WiFi_Status_t* status_spwf)
{
  //  uint16_t len;
  uint8_t i = 0;
  uint8_t wifi_channelnum[4];
  int8_t ret_code;
  
  switch (wifi_state) 
  { 
  case wifi_state_reconnect:
    PRINTF("\r\n [E] Wi-Fi connection lost. Wait 10 sec then reconnect with parameters:  \r\n");
    PRINTF((char *)WIFITOKEN.NetworkSSID);
    PRINTF("\r\n");
    PRINTF((char *)WIFITOKEN.NetworkKey);
    PRINTF("\r\n");
    HAL_Delay(10000);
    
    *status_spwf = wifi_connect(WIFITOKEN.NetworkSSID, WIFITOKEN.NetworkKey, mode);
    
    if(*status_spwf!=WiFi_MODULE_SUCCESS){
      PRINTF("\r\n[E].Error cannot connect with Wi-Fi \r\n");
      wifi_state = wifi_state_reconnect;
      start_bnrg = 1; /* start BlueNRG anyway */
    } else{
      PRINTF("\r\n [D] Reconnecting....  \r\n");
      PRINTF((char *)WIFITOKEN.NetworkSSID);
      wifi_state = wifi_state_idle;
    }
    break;      
      
  case wifi_state_reset:
    break;
    
  case wifi_state_ready: 
    HAL_Delay(20);
    *status_spwf = wifi_network_scan(net_scan, WIFI_SCAN_BUFFER_LIST);
    if(*status_spwf==WiFi_MODULE_SUCCESS)
    {   
      if (strcmp(WIFITOKEN.AuthenticationType, "NONE") == 0)
        mode = None;
      else if (strcmp(WIFITOKEN.AuthenticationType, "WEP") == 0)
        mode = WEP;
      else
        mode = WPA_Personal;
      
      for ( i=0; i<WIFI_SCAN_BUFFER_LIST; i++ )
      {
        if (((char *)strstr((const char *)net_scan[i].ssid,(const char *)WIFITOKEN.NetworkSSID)) !=NULL) {
          SSID_found = WIFI_TRUE;
          memcpy(WIFITOKEN.AuthenticationType, "WPA2", strlen("WPA2"));                                      
          *status_spwf = wifi_connect(WIFITOKEN.NetworkSSID, WIFITOKEN.NetworkKey, mode);
          
          if(*status_spwf!=WiFi_MODULE_SUCCESS) {
            PRINTF("\r\n[E].Error cannot connect to Wi-Fi network \r\n");
            wifi_present = FALSE; //return 0;
          } 
          else {
            PRINTF("\r\nConnected to network with SSID: ");
            PRINTF((char *)WIFITOKEN.NetworkSSID);                        
          }
          break;
        }
      }
      
      if ((!SSID_found)) 
      {
        /* Can happen in crowdy environments */ 
        PRINTF("\r\n[E]. Error, given SSID not found! Trying to force connection with SSID: ");
        PRINTF((char *)WIFITOKEN.NetworkSSID); 
        *status_spwf = wifi_connect(WIFITOKEN.NetworkSSID, WIFITOKEN.NetworkKey, mode);
        if (*status_spwf!=WiFi_MODULE_SUCCESS) {
          PRINTF("\r\n[E].Error cannot connect with Wi-Fi \r\n");
          wifi_present = FALSE; //return 0;
        } 
        else {
          PRINTF("\r\nConnected to network with SSID: ");
          PRINTF((char *)WIFITOKEN.NetworkSSID);            
        }
      }       
      memset(net_scan, 0x00, sizeof(net_scan));
    }
    else
      PRINTF("\r\n[E]. Error, network AP not found! \r\n");
    
    wifi_state = wifi_state_idle; 
    break;
    
  case wifi_state_connected:
    // Low power mode not used
    break;
    
  case mqtt_socket_create:
    if (spwf_socket_create (&n,mqtt_ibm_setup.hostname, mqtt_ibm_setup.port, &mqtt_ibm_setup.protocol)<0) {
      PRINTF("\r\n [E]. Socket opening with IBM MQTT broker failed. Please check MQTT configuration. Trying reconnection.... \r\n");
      PRINTF((char*)mqtt_ibm_setup.hostname);
      PRINTF("\r\n");
      PRINTF((char*)(&mqtt_ibm_setup.protocol));
      PRINTF("\r\n");
      wifi_state=mqtt_socket_create;
      HAL_Delay(2000);
    } 
    else {   
      /* Initialize MQTT client structure */
      MQTTClient(&c,&n, 4000, MQTT_write_buf, sizeof(MQTT_write_buf), MQTT_read_buf, sizeof(MQTT_read_buf));
      wifi_state=mqtt_connect;
      
      PRINTF("\r\nCreated socket with MQTT broker. \r\n");
    }    
    break;
    
  case mqtt_connect:     
    options.MQTTVersion = 3;
    options.clientID.cstring = (char*)mqtt_ibm_setup.clientid;
    options.username.cstring = (char*)mqtt_ibm_setup.username;
    options.password.cstring = (char*)mqtt_ibm_setup.password;   
    
    if ((ret_code = MQTTConnect(&c, &options)) != 0) {  
      PRINTF("\r\n [E]. Client connection with IBM MQTT broker failed with Error Code %d \r\n", (int)ret_code);
      switch (ret_code)
      {
      case C_REFUSED_PROT_V:
        PRINTF("Connection Refused, unacceptable protocol version \r\n");
        break;
      case C_REFUSED_ID:  
        PRINTF("Connection Refused, identifier rejected \r\n");
        break;
      case C_REFUSED_SERVER:
        PRINTF("Connection Refused, Server unavailable \r\n");
        break;
      case C_REFUSED_UID_PWD:
        PRINTF("Connection Refused, bad user name or password \r\n");
        break;
      case C_REFUSED_UNAUTHORIZED:   
        PRINTF("Connection Refused, not authorized \r\n");
        break;
      }  
      spwf_socket_close (&n);
      wifi_state = mqtt_socket_create;
      HAL_Delay(2000);
      /* Turn LED2 Off to indicate that we are now disconnected from IBM MQTT broker */          
      NotifyLEDOff();
    } 
    else {  
      if (mqtt_ibm_setup.ibm_mode == QUICKSTART) {
        PRINTF("Connected with IBM MQTT broker for Quickstart mode (only MQTT publish supported) \r\n");
        /* Quickstart mode. We only publish data. */
        wifi_state = mqtt_pub;
      } else {
        PRINTF("Connected with IBM MQTT broker for Registered devices mode (requires account on IBM Bluemix; publish/subscribe supported) \r\n");
        /* Registered device mode. */
        wifi_state = mqtt_sub;
      }
      
      /* Turn LED2 ON to indicate that we are now connected to IBM MQTT broker */
      NotifyLEDOn();
    }
    
    while (((uint8_t)atoi((char const *)wifi_channelnum) == 0) || ((uint8_t)atoi((char const *)wifi_channelnum) > 16))
    {
      HAL_Delay(1500);    
      if (GET_Configuration_Value("wifi_channelnum",(uint32_t *)wifi_channelnum) != WiFi_MODULE_SUCCESS) {
        PRINTF("\nError retrieving Wi-Fi channel num \r\n");       
      }
      else {
        PRINTF("\nWi-Fi channel: %d\n", (uint8_t)atoi((char const *)wifi_channelnum));
      }    
      HAL_Delay(1500);
    }
    start_bnrg = 1; /* start BlueNRG */
        
    break;
    
  case mqtt_sub: 
    PRINTF("\r\n [D] mqtt sub  \r\n");
    
    /* Subscribe to topic */ 
    PRINTF("\r\n [D] Subscribing topic:  \r\n");
    PRINTF((char *)mqtt_ibm_setup.sub_topic);     
    
    if( MQTTSubscribe(&c, (char*)mqtt_ibm_setup.sub_topic, mqtt_ibm_setup.qos, messageArrived) < 0) {
      PRINTF("\r\n [E]. Subscribe with IBM MQTT broker failed. Please check MQTT configuration. \r\n");
    } else {
      PRINTF("\r\n [D] Subscribed to topic:  \r\n");
      PRINTF((char *)mqtt_ibm_setup.sub_topic);             
    }
    
    HAL_Delay(1500);
    wifi_state=mqtt_pub; 
    break;
    
  case mqtt_pub:
    if (new_data)
    {
      /* Prepare MQTT message */
      prepare_json_pkt(json_buffer);
      
      MQTT_msg.qos=QOS0;
      MQTT_msg.dup=0;
      MQTT_msg.retained=1;
      MQTT_msg.payload= (char *) json_buffer;
      MQTT_msg.payloadlen=strlen( (char *) json_buffer);  
      
      /* Publish MQTT message */
      if (MQTTPublish(&c,(char*)mqtt_ibm_setup.pub_topic,&MQTT_msg) < 0) {  
        PRINTF("\r\n [E]. Failed to publish data. Reconnecting to MQTT broker.... \r\n");
        wifi_state=mqtt_connect;
      } 
      else {
        if (mqtt_ibm_setup.ibm_mode == REGISTERED) { 
          PRINTF("\r\n [D]. Wait %d msec to see if any data is received \r\n", MQTT_PUBLISH_DELAY); 
          /* Wait few seconds to see if data is received */ 
          if (MQTTYield(&c, MQTT_PUBLISH_DELAY) < 0)
          {
            PRINTF("\r\n [D]. Failed MQTTYield. \r\n");
            HAL_Delay(MQTT_PUBLISH_DELAY);            
          }  
        } 
        else {
          HAL_Delay(700); //reduced to 700 from MQTT_PUBLISH_DELAY
        }
        //PRINTF("\r\n [D]. Sensor data are published to IBM cloud \r\n");
        //PRINTF(MQTT_msg.payload);
        /* Blink LED2 to indicate that sensor data is published */
        NotifyLEDBlink(250); //reduced to 250 from 500
      }
      new_data = 0;
    }
    else {      
      HAL_Delay(600);
    }
    break;
    
  case wifi_state_idle:
    PRINTF(".");
    HAL_Delay(500);
    break;  
    
  case wifi_state_disconnected:        
    NotifyLEDOff();
    wifi_state=wifi_state_reconnect;
    break;
    
  default:
    break;
  }
}

/**
  * @brief  Start the BlueNRG connections, characteristics discovering and enable
  *         notifications
  * @param  None
  * @retval None
  */
static void BlueNRG_Process(void)
{   
  HCI_Process();
  Connection_Process();
  Reading_Process();
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: PRINTF("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @brief  MQTT Callback function
  * @param  MessageData (MQTT message received)
  * @retval WiFi_Status_t
  */
void messageArrived(MessageData* md)
{
  MQTTMessage* message = md->message;
  
  PRINTF("\r\n [D]. MQTT payload received is: %s (len %d)\r\n", (char*)message->payload, message->payloadlen);
  
  /*
   * The message must arrive in string format.
   * E.g. if the arrived message is 665b0400:
   * characters from 0 to 3 indicate the peripheral node_id (665b in the example)
   * characters from 4 to 5 indicate the node type id (04 in the example is the LED Type id)
   * characters from 6 on indicate the command value (00 in the example is the LED OFF command)
   * 
   */  
  Forward_Command_To_BlueNRG((uint8_t*)message->payload, message->payloadlen);
}

/**
 * @brief  Prepare JSON packet with sensors data
 * @param  Buffer that will contain sensor data in JSON format 
 * @retval None
 */
void prepare_json_pkt (uint8_t * buffer)
{
  char tempbuff[256];

  strcpy((char *)buffer,"{\"d\":{\"myName\":\"MyBLEStar\"");     
  sprintf(tempbuff, ",%s", data);
  strcat((char *)buffer,tempbuff); 
  strcat((char *)buffer,"}}");
  
  return;
}

/**
  * @brief  GET MAC Address from WiFi
  * @param  char* macadd : string containing MAC address
  * @retval None
  */
static System_Status_t Get_MAC_Add (char *macadd)
{
  uint8_t macaddstart[32];
  System_Status_t status = MODULE_ERROR; // 0 success
  int i,j;
  
  if (GET_Configuration_Value("nv_wifi_macaddr",(uint32_t *)macaddstart) != WiFi_MODULE_SUCCESS) {
    PRINTF("Error retrieving MAC address \r\n");  
    return status; 
  }
  else
    status = MODULE_SUCCESS;
  
  macaddstart[17]='\0';
  PRINTF("\nMAC orig: ");  
  PRINTF((char *)macaddstart);
  PRINTF("\r\n");
  
  if (status == MODULE_SUCCESS) {  
    for (i=0,j=0;i<17;i++) {
      if (macaddstart[i]!=':') {
        macadd[j]=macaddstart[i];
        j++;  
      } 
    }
    macadd[j]='\0';
  }
  
  return status;
}

/**
  * @brief  Turn on notificaion LED (LED2)
  * @param  None
  * @retval None
  */
static void NotifyLEDOn(void)
{
 BSP_LED_On(LED2);
}

 /**
  * @brief  Turn off notificaion LED (LED2)
  * @param  None
  * @retval None
  */
static void NotifyLEDOff(void)
{
 BSP_LED_Off(LED2);
}

 /**
  * @brief  Turn on notificaion LED (LED2)
  * @param  msPeriod time delay in milli seconds
  * @retval None
  */
static void NotifyLEDBlink(unsigned int msPeriod) 
{
   BSP_LED_Off(LED2);
   HAL_Delay(msPeriod);
   BSP_LED_On(LED2);
}

/**
 * @brief  Splits a float into two integer values.
 * @param
 * @retval None
 */
//static void floatToInt(float in, int32_t *out_int, int32_t *out_dec, int32_t dec_prec)
//{
//  *out_int = (int32_t)in;
//  in = in - (float)(*out_int);
//  *out_dec = (int32_t)trunc(in*pow(10,dec_prec));
//}


/* Matteo - UART1*/
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
