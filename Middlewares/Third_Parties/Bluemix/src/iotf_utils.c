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
 *                        -  Contains Utility Functions Definitions
 *                        -  Added logging feature
 *******************************************************************************/

 //Include iotf_utils.h
 #include "ctype.h"
 #include "iotf_utils.h"

#include "devicemanagementclient.h"

 //File pointer to log file
 //FILE *logger = stdout;
 FILE *logger = NULL;

 //Character strings to hold log header and log message to be dumped.
 extern char logHdr[LOG_BUF];
 extern char logStr[LOG_BUF];

 /** Function to check whether environemnt variable IOT_EMBDC_LOGGING defined.
 * If defined, then initializes the logger with the logging file if it's not done so.
 * @param - None
 * @return - None
 **/
 void enableLogging(){
        //printf("In enableLogging...\n");
        if(logger == NULL)
        {
                //printf("Initializing the logger...\n");
                char *loggingVar = getenv("IOT_EMBDC_LOGGING");
                int enabled = (loggingVar != NULL) && strcmp(loggingVar,"ON")==0?1:0;
                char *iotfLog = "iotfclient.log";
                char *embdcHome = getenv("IOT_EMBDC_HOME");
                char *logFile;

                if(enabled){
                        //printf("Logging is Enabled\n");
                        if(embdcHome != NULL && strlen(embdcHome)>1){
                                //printf("IOT_EMBDC_HOME is defined to %s\n",embdcHome);
                                logFile = (char*)malloc(strlen(embdcHome)+strlen(iotfLog)+5);
                                strcpy(logFile,embdcHome);
                                strcat(logFile,"/");
                                strcat(logFile,iotfLog);
                        }
                        else{
                                //printf("IOT_EMBDC_HOME is not defined...\n");
                                logFile = (char*)malloc(strlen(iotfLog)+5);
                                strcpy(logFile,"./");
                                strcat(logFile,iotfLog);
                        }
                        if((logger = fopen(logFile,"a"))!= NULL){
                                printf("Logger initialized with log file - %s\n",logFile);
                                sprintf(logStr,"%s","==============  iotfclient.log Entry ==============");
                                LOG("",logStr);
														    time_t t = 0;
														    char updatedDateTime[50];//"2016-03-01T07:07:56.323Z"
	                              strftime(updatedDateTime, sizeof(updatedDateTime), "%Y-%m-%dT%TZ", localtime(&t));
													      sprintf(logStr,"%s",updatedDateTime);
                                //sprintf(logStr,"%s",__TIMESTAMP__);
                                LOG("",logStr);
                        }
                        else{
                                //printf("Logger not initialized...\n");
                                enabled = 0;
                        }
                        //freePtr(logFile);
                }
                //else
                        //printf("Logging is Not Enabled");
        }
        //printf("Returning from enableLogging...\n");

    return;
 }

 /** Function to disable the logging by printing the footer in the log file.
 * @param - None
 *
 * @return - None
 **/
 void disableLogging(){
        if(logger != NULL){
					time_t t = 0;
					 char updatedDateTime[50];//"2016-03-01T07:07:56.323Z"
	         strftime(updatedDateTime, sizeof(updatedDateTime), "%Y-%m-%dT%TZ", localtime(&t));
					 sprintf(logStr,"%s",updatedDateTime);
 // 	       sprintf(logStr,"%s",__TIMESTAMP__);
  	       LOG("",logStr);
  	       LOG("","==============  iotfclient.log Exit ==============");
  	       fclose(logger);
               logger = NULL;
        }
 }
 /** Function to check whether environment variable IOT_EMBDC_HOME defined.
 * @return - 1 if IOT_EMBDC_HOME is defined
 *         - 0 if IOT_EMBDC_HOME is not defined
 **/
 int isEMBDCHomeDefined(){
         enableLogging();
         sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
         LOG(logHdr,"entry::");

         char *embdC_home = getenv("IOT_EMBDC_HOME");
         int rc = 0;

         sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
         sprintf(logStr,"IOT_EMBDC_HOME = %s",embdC_home);
         LOG(logHdr,logStr);

         if(embdC_home != NULL && strlen(embdC_home)>1)
           rc =  1;
	 else
	   rc =  0;

         sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
         sprintf(logStr,"rc = %d",rc);
         LOG(logHdr,logStr);
         LOG(logHdr,"exit::");

         return rc;
 }

/** Function used to build path by appending the given file path to IOT_EMBDC_HOME path.
*   If IOT_EMBDC_HOME is not defined, then builds only the given path with ./
* @param - Pointer to character string to hold the final path
* @param - Filepath to append to IOT_EMBDC_HOME path
*
* @returns - None
*
**/
 void buildPath(char **ptr, char *filePath){
         sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
         LOG(logHdr,"entry::");

         int pathLen;
         if(isEMBDCHomeDefined()){
            char *embdC_home = getenv("IOT_EMBDC_HOME");
            pathLen = strlen(embdC_home) + strlen(filePath);
            *ptr = (char*)malloc(sizeof(char)*(pathLen+3));
            strcpy(*ptr,embdC_home);
            strcat(*ptr,filePath);
         }
         else{
            pathLen = strlen(filePath);
            *ptr = (char*)malloc(sizeof(char)*(pathLen+3));
            strcpy(*ptr,".");
            strcat(*ptr,filePath);
         }
         (*ptr)[pathLen]='\0';

         sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
         sprintf(logStr,"Built Path = %s",*ptr);
         LOG(logHdr,logStr);
         LOG(logHdr,"exit::");
 }
 /** Function to store server certificate path using environment variable IOT_EMBDC_HOME.
 * @param - Address of character pointer to store the server certificate path
 * @return - void
 **/
 void getServerCertPath(char** path){
        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

	buildPath(path,"/IoTFoundation.pem");

        sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"Server Certificate Path = %s",*path);
        LOG(logHdr,logStr);
        LOG(logHdr,"exit::");
 }

 /** Function to store samples path using environment variable IOT_EMBDC_HOME.
 * @param - Address of character pointer to store the samples path
 * @return - void
 **/
 void getSamplesPath(char** path){
        enableLogging();
        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

	buildPath(path,"/samples/");

        sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"Samples Path = %s",*path);
        LOG(logHdr,logStr);
        LOG(logHdr,"exit::");

 }

 /** Function to store proper path for config file using environment variable IOT_EMBDC_HOME.
 * @param - Character string to store the path
 *        - Config file name to be appended to path
 * @return - void
 **/
 void getTestCfgFilePath(char** path, char* fileName){
        enableLogging();
        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

        buildPath(path,"/test/");

        *path = (char*)realloc(*path,strlen(*path)+strlen(fileName)+1);
        strcat(*path,fileName);

        sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"Test Config File Path = %s",*path);
        LOG(logHdr,logStr);
        LOG(logHdr,"exit::");
 }

//Trimming characters
 char *trim(char *str) {
        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

 	size_t len = 0;
 	char *frontp = str - 1;
 	char *endp = NULL;

 	if (str == NULL)
 		return NULL;

 	if (str[0] == '\0')
 		return str;

 	len = strlen(str);
 	endp = str + len;

 	while (isspace(*(++frontp)))
 		;
 	while (isspace(*(--endp)) && endp != frontp)
 		;

 	if (str + len - 1 != endp)
 		*(endp + 1) = '\0';
 	else if (frontp != str && endp == frontp)
 		*str = '\0';

 	endp = str;
 	if (frontp != str) {
 		while (*frontp)
 			*endp++ = *frontp++;
 		*endp = '\0';
 	}

        sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"String After trimming = %s",str);
        LOG(logHdr,logStr);
        LOG(logHdr,"exit::");

 	return str;
 }

 /** Function to copy source to destination string after allocating required memory.
 * @param - Address of character pointer as destination
 *        - Contents of source string
 * @return - void
 **/
 void strCopy(char **dest, char *src){
         sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
         LOG(logHdr,"entry::");
         if (src == NULL) 
         { 
           *dest = NULL;
           return;
         }
         if(strlen(src) >= 1){

                 *dest = (char*)malloc(sizeof(char)*(strlen(src)+1));
                 strcpy(*dest,src);

                 sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
            //     sprintf(logStr,"Destination String = %s",*dest);
                 LOG(logHdr,logStr);
         }
         else{
                 sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
                 sprintf(logStr,"Source String is empty");
                 LOG(logHdr,logStr);
         }

         sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
         LOG(logHdr,"exit::");
 }

 /** Function to free the allocated memory for character string.
 * @param - Character pointer pointing to allocated memory
 * @return - void
 **/
 void freePtr(char* p){
         sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
         LOG(logHdr,"entry::");

         if(p != NULL)
            free(p);
         else {
                 sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
                 sprintf(logStr,"NULL Pointer cannot be freed");
                 LOG(logHdr,logStr);
         }

         sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
         LOG(logHdr,"exit::");
 }

 /* Reconnect delay time
  * depends on the number of failed attempts
  */
 int reconnect_delay(int i)
 {
 	if (i < 10) {
 		return 3; // first 10 attempts try every 3 seconds
 	}
 	if (i < 20)
 		return 60; // next 10 attempts retry after every 1 minute

 	return 600;	// after 20 attempts, retry every 10 minutes
 }
