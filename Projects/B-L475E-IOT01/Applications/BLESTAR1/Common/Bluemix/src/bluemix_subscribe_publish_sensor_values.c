/**
  ******************************************************************************
  * @file    subscribe_publish_sensor_values.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    04-September-2017
  * @brief   Control of the measurement sampling and MQTT reporting loop.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "iot_flash_config.h"
#include "sensors_data.h"
#include "msg.h"

void MQTTcallbackHandler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData);
int subscribe_publish_sensor_values(void);

/* Private defines ------------------------------------------------------------*/
#define MQTT_CONNECT_MAX_ATTEMPT_COUNT 3
#define TIMER_COUNT_FOR_SENSOR_PUBLISH 10

#define aws_json_pre        "{\"state\":{\"reported\":"
#define aws_json_desired    "{\"state\":{\"desired\":"
#define aws_json_post       "}}"

/* Private variables ---------------------------------------------------------*/
static char ledstate[] = { "Off" };
static char cPTopicName[MAX_SHADOW_TOPIC_LENGTH_BYTES] = "";
static char cSTopicName[MAX_SHADOW_TOPIC_LENGTH_BYTES] = "";
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/**
* @brief This parameter will avoid infinite loop of publish and exit the program after certain number of publishes
*/
static uint32_t publishCount = 60;

/* Functions Definition ------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/**
* @brief MQTT disconnect callback hander
*
* @param pClient: pointer to the AWS client structure
* @param data: 
* @return no return
*/
static void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data)
{
  msg_warning("MQTT Disconnect\n");
  IoT_Error_t rc = FAILURE;
  
  if(NULL == data)
  {
    return;
  }

  AWS_IoT_Client *client = (AWS_IoT_Client *)data;

  if(aws_iot_is_autoreconnect_enabled(client))
  {
    msg_info("Auto Reconnect is enabled, Reconnecting attempt will start now\n");
  }
  else
  {
    msg_warning("Auto Reconnect not enabled. Starting manual reconnect...\n");
    rc = aws_iot_mqtt_attempt_reconnect(client);

    if(NETWORK_RECONNECTED == rc)
    {
      msg_warning("Manual Reconnect Successful\n");
    }
    else
    {
      msg_warning("Manual Reconnect Failed - %d\n", rc);
    }
  }
}

/* Exported functions --------------------------------------------------------*/

/**
* @brief MQTT subscriber callback hander
*
* called when data is received from AWS IoT Thing (message broker)
* @param MQTTCallbackParams type parameter
* @return no return
*/
void MQTTcallbackHandler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData)
{
  const char msg_on[]  = "{\"state\":{\"reported\":{\"LED_value\":\"On\"}}}";
  const char msg_off[] = "{\"state\":{\"reported\":{\"LED_value\":\"Off\"}}}";
  const char *msg = NULL;
  IoT_Publish_Message_Params paramsQOS1;
  paramsQOS1.qos = QOS1;
  
  msg_info("\nMQTT subscribe callback......\n");
  msg_info("%.*s\n", (int)params->payloadLen, (char *)params->payload);
  
  /* If a new desired LED state is received, change the LED state. */
  if (strstr((char *) params->payload, "\"desired\":{\"LED_value\":\"On\"}") != NULL)
  {
    Led_On();
    strcpy(ledstate, "On");    
    msg_info("LED On!\n");
    msg = msg_on;
  }
  else if (strstr((char *) params->payload, "\"desired\":{\"LED_value\":\"Off\"}") != NULL)
  {
    Led_Off();
    strcpy(ledstate, "Off");
    msg_info("LED Off!\n");
    msg = msg_off;
  }
  
  /* Report the new LED state to the MQTT broker. */
  if (msg != NULL)
  { 
    paramsQOS1.payload = (void *) msg;
    paramsQOS1.payloadLen = strlen(msg) + 1;
    IoT_Error_t rc = aws_iot_mqtt_publish(pClient, cPTopicName, strlen(cPTopicName), &paramsQOS1);

    if (rc == AWS_SUCCESS)
    {
      msg_info("\nPublished the new LED status to topic %s:", cPTopicName);
      msg_info("%s\n", msg);
    }
  }
}

/**
* @brief main entry function to AWS IoT code
*
* @param no parameter
* @return AWS_SUCCESS: 0 
          FAILURE: -1
*/
int subscribe_publish_sensor_values(void)
{
  bool infinitePublishFlag = true;
  const char *serverAddress = NULL;
  const char *pCaCert;
  const char *pClientCert;
  const char *pClientPrivateKey;
  const char *pDeviceName;
  char cPayload[AWS_IOT_MQTT_TX_BUF_LEN];
  char const * deviceName;
  int i = 0;
  int connectCounter;
  IoT_Error_t rc = FAILURE;
#ifdef SENSOR
  int timeCounter = 0;
#endif
  uint8_t bp_pushed;

  AWS_IoT_Client client;
  memset(&client, 0, sizeof(AWS_IoT_Client));
  IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
  IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

  getDeviceName(&deviceName);
  if (strlen(deviceName) >= MAX_SIZE_OF_THING_NAME)
  {
    msg_error("The length of the device name stored in the iot user configuration is larger than the AWS client MAX_SIZE_OF_THING_NAME.\n");
    return -1;
  }
  
  snprintf(cPTopicName, sizeof(cPTopicName), AWS_DEVICE_SHADOW_PRE "%s" AWS_DEVICE_SHADOW_UPDATE_TOPIC, deviceName);
  snprintf(cSTopicName, sizeof(cSTopicName), AWS_DEVICE_SHADOW_PRE "%s" AWS_DEVICE_SHADOW_UPDATE_ACCEPTED_TOPIC, deviceName);
  
  /*
  IoT_Publish_Message_Params paramsQOS0;
  IoT_Publish_Message_Params paramsQOS1;
  */

  msg_info("AWS IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

  getServerAddress(&serverAddress);
  getTLSKeys(&pCaCert, &pClientCert, &pClientPrivateKey);
  mqttInitParams.enableAutoReconnect = false; /* We enable this later below */
  mqttInitParams.pHostURL = (char *) serverAddress;
  mqttInitParams.port = AWS_IOT_MQTT_PORT;
  mqttInitParams.pRootCALocation = (char *) pCaCert;
  mqttInitParams.pDeviceCertLocation = (char *) pClientCert;
  mqttInitParams.pDevicePrivateKeyLocation = (char *) pClientPrivateKey;
  mqttInitParams.mqttCommandTimeout_ms = 20000;
  mqttInitParams.tlsHandshakeTimeout_ms = 5000;
  mqttInitParams.isSSLHostnameVerify = true;
  mqttInitParams.disconnectHandler = disconnectCallbackHandler;
  mqttInitParams.disconnectHandlerData = NULL;

  rc = aws_iot_mqtt_init(&client, &mqttInitParams);

  if(AWS_SUCCESS != rc)
  {
    msg_error("aws_iot_mqtt_init returned error : %d\n", rc);
    return -1;
  }

  getDeviceName(&pDeviceName);
  connectParams.keepAliveIntervalInSec = 30;
  connectParams.isCleanSession = true;
  connectParams.MQTTVersion = MQTT_3_1_1;
  connectParams.pClientID = (char *) pDeviceName;
  connectParams.clientIDLen = (uint16_t) strlen(pDeviceName);
  connectParams.isWillMsgPresent = false;


  connectCounter = 0;
  
  do 
  {
    connectCounter++;
    printf("MQTT connection in progress:   Attempt %d/%d ...\n",connectCounter,MQTT_CONNECT_MAX_ATTEMPT_COUNT);
    rc = aws_iot_mqtt_connect(&client, &connectParams);
  } while((rc != AWS_SUCCESS) && (connectCounter < MQTT_CONNECT_MAX_ATTEMPT_COUNT));  

  if(AWS_SUCCESS != rc) 
  {
    msg_error("Error(%d) connecting to %s:%d\n", rc, mqttInitParams.pHostURL, mqttInitParams.port);
    return -1;
  }
  else
  {
    printf("Connected to %s:%d\n", mqttInitParams.pHostURL, mqttInitParams.port);
  }

  /*
  * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
  *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
  *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
  */
  rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
  
  if(AWS_SUCCESS != rc)
  {
    msg_error("Unable to set Auto Reconnect to true - %d\n", rc);

    if (aws_iot_mqtt_is_client_connected(&client)) 
    {
      aws_iot_mqtt_disconnect(&client);
    }

    return -1;
  }
  
  rc = aws_iot_mqtt_subscribe(&client, cSTopicName, strlen(cSTopicName), QOS0, MQTTcallbackHandler, NULL);

  if(AWS_SUCCESS != rc)
  {
    msg_error("Error subscribing : %d\n", rc);
    return -1;
  } 
  else
  {
    msg_info("Subscribed to topic %s\n", cSTopicName);
  }

  sprintf(cPayload, "%s : %d ", "hello from STM", i);

  IoT_Publish_Message_Params paramsQOS1;
  paramsQOS1.qos = QOS1;
  paramsQOS1.payload = (void *) cPayload;

  if(publishCount != 0)
  {
    infinitePublishFlag = false;
  }
  
  
  printf("Press the User button (Blue) to publish the LED desired value on the %s topic\n", cPTopicName);
  
#ifdef SENSOR
  timeCounter = TIMER_COUNT_FOR_SENSOR_PUBLISH;
#endif
   
  while((NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || AWS_SUCCESS == rc) && (publishCount > 0 || infinitePublishFlag))
  {
    /* Max time the yield function will wait for read messages */
    rc = aws_iot_mqtt_yield(&client, 10);

    if(NETWORK_ATTEMPTING_RECONNECT == rc)
    {
      /* Delay to let the client reconnect */
      HAL_Delay(1000); 
      msg_info("Attempting to reconnect\n");
      /* If the client is attempting to reconnect we will skip the rest of the loop */
      continue; 
    }
    if(NETWORK_RECONNECTED == rc)
    {
      msg_info("Reconnected.\n");
    }

    bp_pushed = Button_WaitForPush(1000);
    
    /* exit loop on long push  */
    if (bp_pushed == BP_MULTIPLE_PUSH ) 
    {
      msg_info("\nPushed button perceived as a *double push*. Terminates the application.\n");
      infinitePublishFlag = false;
      publishCount = 0;
      break;
    }
    
    if (bp_pushed == BP_SINGLE_PUSH)
    {
      if(strstr(ledstate, "Off")!= NULL)
      {
        strcpy(ledstate, "On");
      }
      else
      {
        strcpy(ledstate, "Off");
      }
      
      printf("Sending the desired LED state to AWS.\n");
      
      /* create desired message */
      memset(cPayload, 0, sizeof(cPayload));
      strcat(cPayload, aws_json_desired);
      strcat(cPayload, "{\"LED_value\":\"");
      strcat(cPayload, ledstate);
      strcat(cPayload, "\"}");
      strcat(cPayload, aws_json_post);
      
      paramsQOS1.payloadLen = strlen(cPayload) + 1;

      do 
      {
        rc = aws_iot_mqtt_publish(&client, cPTopicName, strlen(cPTopicName), &paramsQOS1);

        if (rc == AWS_SUCCESS)
        {
          printf("\nPublished to topic %s:", cPTopicName);
          printf("%s\n", cPayload);
        }

        if (publishCount > 0)
        {
          publishCount--;
        }
      } while(MQTT_REQUEST_TIMEOUT_ERROR == rc && (publishCount > 0 || infinitePublishFlag));      
    }
    
#ifdef  SENSOR
    timeCounter ++;
    if (timeCounter >= TIMER_COUNT_FOR_SENSOR_PUBLISH)  
    {
      timeCounter = 0;
            
      PrepareMqttPayload(cPayload, sizeof(cPayload), NULL);           
            
      paramsQOS1.payloadLen = strlen(cPayload) + 1;
            
      do
      {
        rc = aws_iot_mqtt_publish(&client, cPTopicName, strlen(cPTopicName), &paramsQOS1);

        if (rc == AWS_SUCCESS)
        {
          printf("\nPublished to topic %s:\n", cPTopicName);
          printf("%s\n", cPayload);
        }

        if (publishCount > 0)
        {
          publishCount--;
        }
      } while((MQTT_REQUEST_TIMEOUT_ERROR == rc) && (publishCount > 0 || infinitePublishFlag));
    }
#endif


    
  } /* End of while */

  rc = aws_iot_mqtt_disconnect(&client);


  return rc;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
