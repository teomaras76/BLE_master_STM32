/*******************************************************************************
 * Copyright (c) 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Jeffrey Dare            - initial implementation and API implementation
 *    Sathiskumar Palaniappan - Added support to create multiple Iotfclient
 *                              instances within a single process
 *    Lokesh Haralakatta      - Added SSL/TLS support
 *    Lokesh Haralakatta      - Added Client Side Certificates support
 *    Lokesh Haralakatta      - Separated out device client and gateway client specific code.
 *                            - Retained back common code here.
 *******************************************************************************/

#ifndef IOTCLIENT_H_
#define IOTCLIENT_H_

#include "iotf_utils.h"
#include "MQTTClient.h"
#include "iotf_network_tls_wrapper.h"

#define BUFFER_SIZE 1024
#define HOSTNAME_STRING_SIZE    64
#define CLIENTID_STRING_SIZE    128

enum errorCodes { CONFIG_FILE_ERROR = -3, MISSING_INPUT_PARAM = -4, QUICKSTART_NOT_SUPPORTED = -5 };

extern unsigned short keepAliveInterval;
extern char *sourceFile;

//configuration file structure
struct iotf_config
{
       char* org;
       char* domain;
       char* type;
       char* id;
       char* authmethod;
       char* authtoken;
       char* serverCertPath;
       char* rootCACertPath;
       char* clientCertPath;
       char* clientKeyPath;
       int port;
       int useClientCertificates;
};

typedef struct iotf_config Config;

//iotfclient
typedef struct
{
       Network n;
       Client c;
       Config cfg;
       unsigned char buf[BUFFER_SIZE];
       unsigned char readbuf[BUFFER_SIZE];
       int isQuickstart;
       int isGateway;
} iotfclient;

/**
* Function used to initialize the Watson IoT client
* @param client - Reference to the Iotfclient
* @param org - Your organization ID
* @param domain - Your domain Name
* @param type - The type of your device
* @param id - The ID of your device
* @param auth-method - Method of authentication (the only value currently supported is â€œtokenâ€�)
* @param auth-token - API key token (required if auth-method is â€œtokenâ€�)
* @Param serverCertPath - Custom Server Certificate Path
* @Param useCerts - Flag to indicate whether to use client side certificates for authentication
* @Param rootCAPath - if useCerts is 1, Root CA certificate Path
* @Param clientCertPath - if useCerts is 1, Client Certificate Path
* @Param clientKeyPath - if useCerts is 1, Client Key Path
* @Param isGatewayClient - 0 for device client or 1 for gateway client
*
* @return int return code
*/
int initialize(iotfclient *client, char *orgId, char *domain, char *deviceType, char *deviceId,
              char *authmethod, char *authtoken, char *serverCertPath, int useCerts,
              char *rootCAPath, char *clientCertPath,char *clientKeyPath, int isGatewayClient);
/**
* Function used to initialize the IBM Watson IoT client using the config file which is generated when you register your device
* @param client - Reference to the Iotfclient
* @param configFilePath - File path to the configuration file
* @Param isGatewayClient - 0 for device client or 1 for gateway client
*
* @return int return code
* error codes
* CONFIG_FILE_ERROR -3 - Config file not present or not in right format
*/
int initialize_configfile(iotfclient *client, char *configFilePath, int isGatewayClient);

/**
* Function used to initialize the IBM Watson IoT client
* @param client - Reference to the Iotfclient
*
* @return int return code
*/
int connectiotf(iotfclient *client);

/**
* Function used to publish the given data to the topic with the given QoS
* @Param client - Address of MQTT Client
* @Param topic - Topic to publish
* @Param payload - Message payload
* @Param qos - quality of service either of 0,1,2
*
* @return int - Return code from MQTT Publish Call
**/
int publishData(Client *mqttClient, char *topic, char *payload, int qos);

/**
* Function used to check if the client is connected
* @param client - Reference to the Iotfclient
*
* @return int return code
*/
int isConnected(iotfclient *client);

/**
* Function used to Yield for commands.
* @param client - Reference to the Iotfclient
* @param time_ms - Time in milliseconds
* @return int return code
*/
int yield(iotfclient *client, int time_ms);

/**
* Function used to disconnect from the IBM Watson IoT service
* @param client - Reference to the Iotfclient
*
* @return int return code
*/
int disconnect(iotfclient *client);

/**
* Function used to set the time to keep the connection alive with IBM Watson IoT service
* @param keepAlive - time in secs
*
*/
void setKeepAliveInterval(unsigned int keepAlive);

int retry_connection(iotfclient *client);

int get_config(char * filename, Config * configstr);

void freeConfig(Config *cfg);

#endif /* IOTCLIENT_H_ */
