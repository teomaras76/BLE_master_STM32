   /**
  ******************************************************************************
  * @file    IBM_Watson_Config.h
  * @author  Central LAB
  * @version V1.0.0
  * @date    17-Oct-2015
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


#ifndef __IBM_Watson_Config_H
#define __IBM_Watson_Config_H

#include "stdio.h"
#include "stdint.h"
//#include "MQTTClient.h"

/* IKS x-cube related section    ---------------------------------------------------*/
#if (defined __IKS01A1)
#include "x_nucleo_iks01a1.h"
#include "x_nucleo_iks01a1_accelero.h"
#include "x_nucleo_iks01a1_gyro.h"
#include "x_nucleo_iks01a1_magneto.h"
#include "x_nucleo_iks01a1_humidity.h"
#include "x_nucleo_iks01a1_temperature.h"
#include "x_nucleo_iks01a1_pressure.h"
#elif (defined IKS01A2)
#include "x_nucleo_iks01a2.h"
#include "x_nucleo_iks01a2_accelero.h"
#include "x_nucleo_iks01a2_gyro.h"
#include "x_nucleo_iks01a2_magneto.h"
#include "x_nucleo_iks01a2_humidity.h"
#include "x_nucleo_iks01a2_temperature.h"
#include "x_nucleo_iks01a2_pressure.h"
#endif  // __IKS01A1

/* define sensors depending on whether we are using X-NUCLEO-IKS01A1 or X-NUCLEO-IKS01A2 */
#ifdef __IKS01A1
#define MAGNETOMETER_SENSOR_AUTO (LIS3MDL_0)
#else
#define MAGNETOMETER_SENSOR_AUTO (LSM303AGR_M_0)
#endif

/* Definition for I2Cx clock resources */
#define I2Cx                             I2C1
#define I2Cx_CLK_ENABLE()                __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2Cx_FORCE_RESET()               __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()             __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_8
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SCL_AF                     GPIO_AF4_I2C1
#define I2Cx_SDA_PIN                    GPIO_PIN_9
#define I2Cx_SDA_GPIO_PORT              GPIOB
#define I2Cx_SDA_AF                     GPIO_AF4_I2C1

/* Definition for I2Cx's NVIC */
#define I2Cx_EV_IRQn                    I2C1_EV_IRQn
#define I2Cx_EV_IRQHandler              I2C1_EV_IRQHandler
#define I2Cx_ER_IRQn                    I2C1_ER_IRQn
#define I2Cx_ER_IRQHandler              I2C1_ER_IRQHandler

#if 0 // <hdd>
/** Use default settings for Wifi SSID and security key. Not to be changed by user. */
#define USE_WIFI_DEFAULT_SETTINGS (0)

/** NFC configuratino related macro for NDEF WIFI type record. Not to be changed by user. */
#define USE_NDEF_WIFI_RECORD (1)  // Macro to enable NFC usage with NDEF Text records
/** NFC configuratino related macro for NDEF WIFI type record. Not to be changed by user. */
#define USE_NDEF_TEXT_RECORD (2)  // Macro to enable NFC usage with NDEF Text records
/**
  macro to configure the NFC usage mode.
  USE_NDEF_WIFI_RECORD: Enable NFC usage with NDEF WIFI records
  USE_NDEF_TEXT_RECORD: Enable NFC usage with NDEF Text records
  Note: If NFC HW is not present, Firmware will automatically disable NFC usage.
*/
#define CONFIG_USE_NFC (USE_NDEF_TEXT_RECORD)

// 1. Enable FFT usage
// 0. Disable FFT usage
#define FFT_ENABLED 0

/* Add versioning information */
#define IBM_WATSON_IOT_DEMO_APP_VERSION_MAJOR (2)
#define IBM_WATSON_IOT_DEMO_APP_VERSION_MINOR (1)
#define IBM_WATSON_IOT_DEMO_APP_VERSION_MICRO (0)

#define MAX_CLIENT_ID_LEN (100)

/* define sensors depending on whether we are using X-NUCLEO-IKS01A1 or X-NUCLEO-IKS01A2 */
#ifdef __IKS01A1
#define MAGNETOMETER_SENSOR_AUTO (LIS3MDL_0)
#else
#define MAGNETOMETER_SENSOR_AUTO (LSM303AGR_M_0)
#endif

typedef enum {
  QUICKSTART = 0,
  REGISTERED = 1,
} ibm_mode_t;

enum { C_ACCEPTED = 0, C_REFUSED_PROT_V = 1, C_REFUSED_ID = 2, C_REFUSED_SERVER = 3, C_REFUSED_UID_PWD = 4, C_REFUSED_UNAUTHORIZED = 5 };

/**
  * @brief  Parameters for MQTT connection
  */
typedef struct mqtt_vars
{
        uint8_t pub_topic[128];  /**< topic to publish */
        uint8_t sub_topic[128];  /**< topic to subscribe */
        uint8_t clientid[64];    /**< topic to publish */
        enum QoS qos;            /**< Quality of service parameter for MQTT connection to IBM Watson IOT platform service */
	uint8_t username[64];    /**< "use-token-auth"*/
	uint8_t password[64];    /**< secret token generated (or the one you passed) while registering your device wih “IBM Watson IOT Platform” service  */
	uint8_t hostname[64];    /**< your_ibm_org_id."messaging.internetofthings.ibmcloud.com"  */
        uint8_t device_type[64]; /**< device type in IBM Watson IOT platform service */
        uint8_t org_id[64];        /**< org id in IBM Watson */
	uint32_t port;                 /**< TCP port */
        uint8_t protocol; /**<t -> tcp , s-> secure tcp, c-> secure tcp + certificates */
        ibm_mode_t ibm_mode; /**< QUICKSTART, REGISTERED */
} MQTT_vars;


/**
  * @brief  This structure contains all the parameters which user can pass to firmware
  */
typedef struct user_vars
{
        char NetworkSSID[32];         /**< SSID of the WIFI network that your using for internet connectivity.  */
        char AuthenticationType[6];   /**< Authentication type for Wifi (e.g. WEP/WPA2 etc.) */
        char EncryptionType[6];       /**< Wifi encryption. Not to be passed by user as it is set internally in the FW. */
        char NetworkKey[32];          /**< Wifi security key corresponding to your Wifi SSID */
        ibm_mode_t ibm_mode;          /**< mode of connection to IBM Watson platform service. It can be either "quickstart" or "registered" */  	
        enum QoS qos;                 /**< Quality of service parameter for MQTT connection to IBM Watson IOT platform service */
        char device_type[64];      /**< device type in IBM Watson IOT platform service */
        char org_id[64];           /**< org id in IBM Watson */
        char username[64];         /**< "use-token-auth"*/
        char password[64];         /**< secret token generated (or the one you passed) while registering your device wih “IBM Watson IOT Platform” service  */
        char hostname[64];         /**< your_ibm_org_id."messaging.internetofthings.ibmcloud.com"  */		
        char clientid[64];         /**< : IBM Watson device id corresponding to your device */
} User_NFC_vars;

#define UNION_DATA_SIZE_TEXT (6*64+32*2+8*2+4*1)

/**
  * @brief  This union contains all the parameters which user can pass to firmware
  */
typedef union
{
  uint32_t Data[UNION_DATA_SIZE_TEXT];            /**< : data buffer */
  User_NFC_vars configParams;                     /**< : structure for holding user configuration parameters */
}uNfcTokenInfo;

extern uNfcTokenInfo UnionNFCToken;

/* MQTT IBM Functions */
void Config_MQTT_IBM ( MQTT_vars *mqtt_ibm_setup, uint8_t *macadd ); 
void Compose_Quickstart_URL ( uint8_t *url_ibm, uint8_t *macadd ); 


#endif // hdd
#endif 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
