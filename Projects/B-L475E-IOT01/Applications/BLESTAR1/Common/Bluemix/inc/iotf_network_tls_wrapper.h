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
#include "paho_mqtt_platform.h"
#include "mbedtls/platform.h"

//Include OS Specific Header Files
#if 0
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <stdio.h>
#if 0
#include <unistd.h>
#endif
#include <errno.h>
#if 0
#include <fcntl.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>


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
