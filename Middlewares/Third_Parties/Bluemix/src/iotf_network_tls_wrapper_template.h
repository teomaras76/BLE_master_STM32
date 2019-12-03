/*******************************************************************************
 * Copyright (c) 2017 IBM Corp.
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
 *    Lokesh Haralakatta  -  Initial implementation
 *                        -  Contains the required header files inclusion,
 *                        -  structures definitions and functions declarations.
 *******************************************************************************/

#ifndef IOTF_NETWORK_TLS_WRAPPER_H_
#define IOTF_NETWORK_TLS_WRAPPER_H_

//Include OS Specific Header Files
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>

//Include mbedtls specific header files
#include "mbedtls/config.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#include "iotf_utils.h"

//tls initialization paramaters
typedef struct
{
	mbedtls_net_context server_fd;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	mbedtls_x509_crt cacert;
	mbedtls_x509_crt clicert;
	mbedtls_pk_context pkey;
        char clientName[20];
} tls_init_params;

//Structure for storing certificates location
typedef struct
{
	char *pServerCertLocation;
        char *pRootCACertLocation;
	char *pDeviceCertLocation;
	char *pDevicePrivateKeyLocation;
	char *pDestinationURL;
} tls_connect_params;

typedef struct Timer Timer;

struct Timer {
	struct timeval end_time;
};

//Strcture definition to store network related information
typedef struct Network Network;
struct Network
{
	int my_socket;
	tls_connect_params TLSConnectData;
	tls_init_params TLSInitData;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*, int);
};

//Functions declaration related to network activity
void NewNetwork(Network*);
int ConnectNetwork(Network* n, char* addr, int port);
int network_read(Network* n, unsigned char* buffer, int len, int timeout_ms);
int network_write(Network* n, unsigned char* buffer, int len, int timeout_ms);
void network_disconnect(Network* n, int qsMode);

//Functions declaration related to timing
char expired(Timer*);
void countdown_ms(Timer*, unsigned int);
void countdown(Timer*, unsigned int);
int left_ms(Timer*);
void InitTimer(Timer*);

//Functions declaration to interact with mbedtls library
int initialize_tls(tls_init_params *tlsInitData, int useCerts);
void tls_debug(void *ctx, int level,const char *file, int line, const char *str);
int tls_connect(tls_init_params *tlsInitData,tls_connect_params *tlsConnectData,
	                                    const char *server, const int port, int useCerts);
int tls_write(Network* n, unsigned char* buffer, int len, int timeout_ms);
int tls_read(Network* n, unsigned char* buffer, int len, int timeout_ms);
void teardown_tls(tls_init_params* tlsInitData, tls_connect_params* tlsConnectData);
void freeTLSConnectData(tls_connect_params* tlsConnectData);
#endif
