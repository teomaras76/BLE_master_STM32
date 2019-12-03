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
 *                        -  Contains the Utility Functions declarations
 *******************************************************************************/
 #ifndef IOTF_UTILS_H_
 #define IOTF_UTILS_H_

 #include<stdlib.h>
 #include<stdio.h>
 #include<string.h>

 #define LOG(logHeader,msg)       if(logger != NULL) fprintf(logger,"%s:%s\n",logHeader,msg);
 #define LOG_BUF 512

 extern FILE *logger;

//Utility Functions
 void enableLogging(void);
 void disableLogging(void);
 int isEMBDCHomeDefined(void);
 void getServerCertPath(char** path);
 void getSamplesPath(char** path);
 void getTestCfgFilePath(char** cfgFilePath, char* fileName);
 void buildPath(char **ptr, char *path);
 char *trim(char *str);
 void strCopy(char **dest, char *src);
 int reconnect_delay(int i);
 void freePtr(char* p);

 #endif
