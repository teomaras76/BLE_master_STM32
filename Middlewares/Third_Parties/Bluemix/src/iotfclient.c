/*******************************************************************************
 * Copyright (c) 2015, 2016 IBM Corp.
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
 *    Lokesh Haralakatta      - Added Logging Feature
 *******************************************************************************/

#include "ctype.h"
#include "iotfclient.h"


unsigned short keepAliveInterval = 360;
//Character strings to hold log header and log message to be dumped.
extern char logHdr[LOG_BUF];
extern char logStr[LOG_BUF];
int sleep(int);
/**
* Function used to initialize the IBM Watson IoT client using the config file which is
* generated when you register your device.
* @param configFilePath - File path to the configuration file
* @Param isGatewayClient - 0 for device client or 1 for gateway client
*
* @return int return code
* error codes
* CONFIG_FILE_ERROR -3 - Config file not present or not in right format
*/

int initialize_configfile(iotfclient  *client, char *configFilePath, int isGatewayClient)
{
       enableLogging();
       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       LOG(logHdr,"entry::");

       Config configstr = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1883,0};

       int rc = 0;

       rc = get_config(configFilePath, &configstr);

       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       sprintf(logStr,"org:%s , domain:%s , type: %s , id:%s , token: %s , useCerts: %d , serverCertPath: %s",
		configstr.org,configstr.domain,configstr.type,configstr.id,configstr.authtoken,configstr.useClientCertificates,
		configstr.serverCertPath);
       LOG(logHdr,logStr);

       if(rc != SUCCESS) {
	       goto exit;
       }

       if(configstr.org == NULL || configstr.type == NULL || configstr.id == NULL ||
	  configstr.authmethod == NULL || configstr.authtoken == NULL) {
	       freeConfig(&configstr);
	       rc = MISSING_INPUT_PARAM;
	       goto exit;
       }

       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       sprintf(logStr,"useCertificates: %d",configstr.useClientCertificates);
       LOG(logHdr,logStr);

       if(configstr.useClientCertificates){
	       if(configstr.rootCACertPath == NULL || configstr.clientCertPath == NULL ||
		  configstr.clientKeyPath == NULL){
		       freeConfig(&configstr);
		       rc = MISSING_INPUT_PARAM;
		       goto exit;
	       }
	       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	       sprintf(logStr,"CACertPath:%s , clientCertPath:%s , clientKeyPath: %s",
			configstr.rootCACertPath,configstr.clientCertPath,configstr.clientKeyPath);
	       LOG(logHdr,logStr);
       }

       if((strcmp(configstr.org,"quickstart") == 0))
	       client->isQuickstart = 1;
       else
	       client->isQuickstart = 0;

       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       sprintf(logStr,"isQuickStart Mode: %d",client->isQuickstart);
       LOG(logHdr,logStr);

       if(isGatewayClient){
	       if(client->isQuickstart) {
		       printf("Quickstart mode is not supported in Gateway Client\n");
		       freeConfig(&configstr);
		       return QUICKSTART_NOT_SUPPORTED;
	       }
	       client->isGateway = 1;
       }
       else
	       client->isGateway = 0;

        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	sprintf(logStr,"isGateway Client: %d",client->isGateway);
	LOG(logHdr,logStr);

       client->cfg = configstr;

 exit:
        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	sprintf(logStr,"rc = %d",rc);
	LOG(logHdr,logStr);
	LOG(logHdr,"exit::");

       return rc;

}

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
*
* @return int return code
*/
int initialize(iotfclient  *client, char *orgId, char* domainName, char *deviceType,
	      char *deviceId, char *authmethod, char *authToken, char *serverCertPath, int useCerts,
	       char *rootCACertPath, char *clientCertPath,char *clientKeyPath, int isGatewayClient)
{
       enableLogging();
       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       LOG(logHdr,"entry::");

       Config configstr = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1883,0};
       int rc = 0;

       	sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	//sprintf(logStr,"org:%s , domain:%s , type: %s , id:%s , token: %s , useCerts: %d , serverCertPath: %s",
	//	       orgId,domainName,deviceType,deviceId,authToken,useCerts,serverCertPath);
	LOG(logHdr,logStr);

       if(orgId == NULL || deviceType == NULL || deviceId == NULL) {
	       rc = MISSING_INPUT_PARAM;
	       goto exit;
       }

       if(useCerts){
	       if(rootCACertPath == NULL || clientCertPath == NULL || clientKeyPath == NULL){
		       rc = MISSING_INPUT_PARAM;
		       goto exit;
	       }
       }

       strCopy(&configstr.org, orgId);
       strCopy(&configstr.domain,"internetofthings.ibmcloud.com");
       if(domainName != NULL)
	       strCopy(&configstr.domain, domainName);
       strCopy(&configstr.type, deviceType);
       strCopy(&configstr.id, deviceId);

        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	sprintf(logStr,"cfgstr.domain:%s , cfgstr.type:%s , cfgstr.id: %s",configstr.domain,configstr.type,configstr.id);
	LOG(logHdr,logStr);

       if((strcmp(orgId,"quickstart") != 0)) {
	       if(authmethod == NULL || authToken == NULL) {
		       freeConfig(&configstr);
		       rc = MISSING_INPUT_PARAM;
		       goto exit;
	       }
	       strCopy(&configstr.authmethod, authmethod);
	       strCopy(&configstr.authtoken, authToken);

	       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	       sprintf(logStr,"cfgstr.authmethod:%s , cfgstr.token:%s ",configstr.authmethod,configstr.authtoken);
	       LOG(logHdr,logStr);

	       if(serverCertPath == NULL)
		       if(isEMBDCHomeDefined())
			       getServerCertPath(&configstr.serverCertPath);
		       else
			       strCopy(&configstr.serverCertPath,"./IoTFoundation.pem");
	       else
		       strCopy(&configstr.serverCertPath,serverCertPath);

	       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	    //   sprintf(logStr,"cfgstr.serverCertPath:%s",configstr.serverCertPath);
	       LOG(logHdr,logStr);

	       if(useCerts){
		       strCopy(&configstr.rootCACertPath,rootCACertPath);
		       strCopy(&configstr.clientCertPath,clientCertPath);
		       strCopy(&configstr.clientKeyPath,clientKeyPath);
		       configstr.useClientCertificates = 1;

		       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
		      // sprintf(logStr,"cfgstr.CACertPath:%s , cfgstr.clientCertPath:%s , cfgstr.clientKeyPath: %s",
			//	configstr.rootCACertPath,configstr.clientCertPath,configstr.clientKeyPath);
		       //LOG(logHdr,logStr);
		       sprintf(logStr,"cfgstr.useCertificates:%d",configstr.useClientCertificates);
		       LOG(logHdr,logStr);
	       }
	       client->isQuickstart = 0;
               configstr.port = 8883;
       }
       else
	       client->isQuickstart = 1;

        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	sprintf(logStr,"isQuickStart Mode: %d Port: %d",client->isQuickstart,configstr.port);
	LOG(logHdr,logStr);

       if(isGatewayClient){
	       if(client->isQuickstart) {
		       printf("Quickstart mode is not supported in Gateway Client\n");
		       freeConfig(&configstr);
		       return QUICKSTART_NOT_SUPPORTED;
	       }
	       client->isGateway = 1;
       }
       else
	       client->isGateway = 0;

        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	sprintf(logStr,"isGateway Client: %d",client->isGateway);
	LOG(logHdr,logStr);

       client->cfg = configstr;

exit:
        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	sprintf(logStr,"rc = %d",rc);
	LOG(logHdr,logStr);
	LOG(logHdr,"exit::");

       return rc;
}

// This is the function to read the config from the device.cfg file
int get_config(char * filename, Config * configstr) {
       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       LOG(logHdr,"entry::");

       int rc = 0;
       int linenum = 0;

       FILE* prop;
       prop = fopen(filename, "r");
       if (prop == NULL) {
	      rc = CONFIG_FILE_ERROR;
	      goto exit;
       }
       char line[256];
       strCopy(&configstr->domain,"internetofthings.ibmcloud.com");

       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       sprintf(logStr,"Default domainName: %s",configstr->domain);
       LOG(logHdr,logStr);

       while (fgets(line, 256, prop) != NULL) {
	      char* prop;
	      char* value;
	      linenum++;
	      if (line[0] == '#')
		      continue;

	      prop = strtok(line, "=");
	      prop = trim(prop);
	      value = strtok(NULL, "=");
	      value = trim(value);

	      sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	      sprintf(logStr,"Property: %s , Value: %s",prop,value);
	      LOG(logHdr,logStr);

	      if (strcmp(prop, "org") == 0){
		  if(strlen(value) > 1)
		    strCopy(&(configstr->org), value);

		  if(strcmp(configstr->org,"quickstart") !=0)
		     configstr->port = 8883;

		  sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
		  sprintf(logStr,"cfgstr.org: %s , cfgstr.port: %d",configstr->org,configstr->port);
		  LOG(logHdr,logStr);
	       }
	      else if (strcmp(prop, "domain") == 0){
		  if(strlen(value) <= 1)
		     strCopy(&configstr->domain,"internetofthings.ibmcloud.com");
		  else
		     strCopy(&configstr->domain, value);

		  sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
		  sprintf(logStr,"cfgstr.domain: %s ",configstr->domain);
		  LOG(logHdr,logStr);
	       }
	      else if (strcmp(prop, "type") == 0){
		  if(strlen(value) > 1)
		    strCopy(&configstr->type, value);
	       }
	      else if (strcmp(prop, "id") == 0){
		  if(strlen(value) > 1)
		    strCopy(&configstr->id, value);
	       }
	      else if (strcmp(prop, "auth-token") == 0){
		  if(strlen(value) > 1)
		    strCopy(&configstr->authtoken, value);
	       }
	      else if (strcmp(prop, "auth-method") == 0){
		  if(strlen(value) > 1)
		    strCopy(&configstr->authmethod, value);
	       }
	      else if (strcmp(prop, "serverCertPath") == 0){
		  if(strlen(value) <= 1)
		     getServerCertPath(&configstr->serverCertPath);
		  else
		     strCopy(&configstr->serverCertPath, value);

		  sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
		  sprintf(logStr,"cfgstr.serverCertPath: %s ",configstr->serverCertPath);
		  LOG(logHdr,logStr);
	       }
	      else if (strcmp(prop, "rootCACertPath") == 0){
		  if(strlen(value) > 1)
		    strCopy(&configstr->rootCACertPath, value);
	       }
	      else if (strcmp(prop, "clientCertPath") == 0){
		  if(strlen(value) > 1)
		    strCopy(&configstr->clientCertPath, value);
	       }
	      else if (strcmp(prop, "clientKeyPath") == 0){
		  if(strlen(value) > 1)
		    strCopy(&configstr->clientKeyPath, value);
	       }
	      else if (strcmp(prop,"useClientCertificates") == 0){
		  configstr->useClientCertificates = value[0] - '0';
	       }
       }
 exit:
        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	sprintf(logStr,"rc = %d",rc);
	LOG(logHdr,logStr);
	LOG(logHdr,"exit::");

       return rc;
}

/**
* Function used to connect to the IBM Watson IoT client
* @param client - Reference to the Iotfclient
*
* @return int return code
*/
int connectiotf(iotfclient  *client)
{
       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       LOG(logHdr,"entry::");

       int rc = 0;
       int useCerts = client->cfg.useClientCertificates;
       int isGateway = client->isGateway;
       int qsMode = client->isQuickstart;
       int port = client->cfg.port;
       MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       sprintf(logStr,"useCerts:%d , isGateway:%d , qsMode:%d",useCerts,isGateway,qsMode);
       LOG(logHdr,logStr);

       char messagingUrl[120];
       sprintf(messagingUrl, ".messaging.%s",client->cfg.domain);
       
       char hostname[HOSTNAME_STRING_SIZE];
       int n=strlen(client->cfg.org) + strlen(messagingUrl) + 1;
       if (n > HOSTNAME_STRING_SIZE)
       {
         rc = -1;
         LOG(logHdr,"Hostname string is too long , update HOSTNAME_STRING_SIZE macros\n");
         goto exit;
       }
       sprintf(hostname, "%s%s", client->cfg.org, messagingUrl);
       
       char clientId[CLIENTID_STRING_SIZE];
       n=strlen(client->cfg.org) + strlen(client->cfg.type) + strlen(client->cfg.id) + 5;
       if (n > CLIENTID_STRING_SIZE)
       {
         rc = -1;
         LOG(logHdr,"ClientId string is too long , update CLIENTID_STRING_SIZE macros\n");
         goto exit;
       }
       
       if(isGateway)
	  sprintf(clientId, "g:%s:%s:%s", client->cfg.org, client->cfg.type, client->cfg.id);
       else
	  sprintf(clientId, "d:%s:%s:%s", client->cfg.org, client->cfg.type, client->cfg.id);

       	sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	sprintf(logStr,"messagingUrl:%s , hostname:%s , port:%d , clientId:%s",messagingUrl,hostname,port,clientId);
	LOG(logHdr,logStr);

       NewNetwork(&client->n);

       if(!isGateway && qsMode){
	   if((rc = ConnectNetwork(&(client->n),hostname,client->cfg.port)) != 0){
	       goto exit;
	   }

	   sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	   sprintf(logStr,"RC from ConnectNetwork:%d",rc);
           LOG(logHdr,logStr);
       }
       else 
       {
         tls_connect_params tls_params = {NULL,NULL,NULL,NULL,NULL};
         strCopy(&tls_params.pServerCertLocation,client->cfg.serverCertPath);
         strCopy(&tls_params.pDestinationURL,hostname);
         if(useCerts){
           strCopy(&tls_params.pRootCACertLocation,client->cfg.rootCACertPath);
           strCopy(&tls_params.pDeviceCertLocation,client->cfg.clientCertPath);
           strCopy(&tls_params.pDevicePrivateKeyLocation,client->cfg.clientKeyPath);
         }
         
         sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
         if(useCerts){
	   sprintf(logStr,"tls_params: {  , %s }",
                   //                tls_params.pServerCertLocation,
                   //                 tls_params.pRootCACertLocation,tls_params.pDeviceCertLocation,tls_params.pDevicePrivateKeyLocation,
		   tls_params.pDestinationURL);
         }
         LOG(logHdr,logStr);
         
         client->n.TLSConnectData = tls_params;
         if((rc = tls_connect(&(client->n.TLSInitData),&(client->n.TLSConnectData),hostname,port,useCerts))!=0)
         {
           goto exit;
         }
         
         client->n.my_socket = (int32_t*) client->n.TLSInitData.server_fd.fd;
         client->n.mqttwrite = tls_write;
         client->n.mqttread = tls_read;
         
         sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
         sprintf(logStr,"RC from tlsconnect: %d",rc);
         LOG(logHdr,logStr);
       }
       MQTTClient(&client->c, &client->n, 1000, client->buf, BUFFER_SIZE, client->readbuf, BUFFER_SIZE);



       data.willFlag = 0;
       data.MQTTVersion = 3;
       data.clientID.cstring = clientId;

       if(!qsMode) {
	       data.username.cstring = "use-token-auth";
	       data.password.cstring = client->cfg.authtoken;
       }

       data.keepAliveInterval = keepAliveInterval;
       data.cleansession = 1;

       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       if(!qsMode) {
         sprintf(logStr,"MQTT Connect Data: { %d , %d , %s , %s, %s , %d , %d }",data.willFlag,data.MQTTVersion,
                 data.clientID.cstring,data.username.cstring,data.password.cstring,data.keepAliveInterval,data.cleansession);
       }
       else
       {
         sprintf(logStr,"MQTT Connect Data: { %d , %d , %s , %d , %d }",data.willFlag,data.MQTTVersion,
                 data.clientID.cstring,data.keepAliveInterval,data.cleansession);
       }
       LOG(logHdr,logStr);

       if((rc = MQTTConnect(&client->c, &data))==0){
	   if(qsMode)
	      printf("Device Client Connected to %s Platform in QuickStart Mode\n",hostname);
	   else
	   {
	       char *clientType = (isGateway)?"Gateway Client":"Device Client";
	       char *connType = (useCerts)?"Client Side Certificates":"Secure Connection";
	       printf("%s Connected to %s in registered mode using %s\n",clientType,hostname,connType);
	   }

	   sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	   sprintf(logStr,"RC from MQTTConnect: %d",rc);
	   LOG(logHdr,logStr);
       }

exit:
        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	sprintf(logStr,"rc = %d",rc);
	LOG(logHdr,logStr);

        if(rc != 0){
                freeTLSConnectData(&(client->n.TLSConnectData));
             //FIXME   freeConfig(&client->cfg);
        }

        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
	LOG(logHdr,"exit::");

        return rc;
}

/**
* Function used to publish the given data to the topic with the given QoS
* @Param client - Address of MQTT Client
* @Param topic - Topic to publish
* @Param payload - Message payload
* @Param qos - quality of service either of 0,1,2
*
* @return int - Return code from MQTT Publish
**/
int publishData(Client *mqttClient, char *topic, char *payload, int qos){
       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       LOG(logHdr,"entry::");

       int rc = -1;
       MQTTMessage pub;

       if (qos==0) pub.qos=QOS0;
       if (qos==1) pub.qos=QOS1;
       if (qos==2) pub.qos=QOS2;
       
       pub.retained = '0';
       pub.payload = payload;
       pub.payloadlen = strlen(payload);

       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       sprintf(logStr,"MQTTMessage = { qos: %d  retained: %d  payload: %s  payloadLen: %d}",
                        pub.qos,pub.retained,(char*) pub.payload,pub.payloadlen);
       LOG(logHdr,logStr);

       rc = MQTTPublish(mqttClient, topic , &pub);

       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       sprintf(logStr,"rc = %d",rc);
       LOG(logHdr,logStr);
       LOG(logHdr,"exit::");

       return(rc);
}

/**
* Function used to Yield for commands.
* @param time_ms - Time in milliseconds
* @return int return code
*/
int yield(iotfclient  *client, int time_ms)
{
       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       LOG(logHdr,"entry::");

       int rc = 0;
       rc = MQTTYield(&client->c, time_ms);

       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       sprintf(logStr,"rc = %d",rc);
       LOG(logHdr,logStr);
       LOG(logHdr,"exit::");

       return rc;
}

/**
* Function used to check if the client is connected
*
* @return int return code
*/
int isConnected(iotfclient  *client)
{
        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

       int result = client->c.isconnected;

       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       sprintf(logStr,"isConnected = %d",result);
       LOG(logHdr,logStr);
       LOG(logHdr,"exit::");

       return result;
}

/**
* Function used to disconnect from the IBM Watson IoT service
*
* @return int return code
*/

int disconnect(iotfclient  *client)
{
        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

       int rc = 0;
       if(isConnected(client))
	  rc = MQTTDisconnect(&client->c);
       client->n.disconnect(&(client->n),client->isQuickstart);
       freeConfig(&(client->cfg));

       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       sprintf(logStr,"rc = %d",rc);
       LOG(logHdr,logStr);
       LOG(logHdr,"exit::");

       return rc;

}

/**
* Function used to set the time to keep the connection alive with IBM Watson IoT service
* @param keepAlive - time in secs
*
*/
void setKeepAliveInterval(unsigned int keepAlive)
{
       keepAliveInterval = keepAlive;

}

//Staggered retry
int retry_connection(iotfclient  *client)
{
       int retry = 1;
       int rc = -1;

       while((rc = connectiotf(client)) != SUCCESS)
       {
	       printf("Retry Attempt #%d ", retry);
	       int delay = reconnect_delay(retry++);
	       printf(" next attempt in %d seconds\n", delay);
	       sleep(delay);
       }
       return rc;
}

void freeConfig(Config *cfg){
       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       LOG(logHdr,"entry::");

       freePtr(cfg->org);
       freePtr(cfg->domain);
       freePtr(cfg->type);
       freePtr(cfg->id);
       freePtr(cfg->authmethod);
       freePtr(cfg->authtoken);
       freePtr(cfg->rootCACertPath);
       freePtr(cfg->clientCertPath);
       freePtr(cfg->clientKeyPath);

       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       LOG(logHdr,"exit::");

       disableLogging();
}
