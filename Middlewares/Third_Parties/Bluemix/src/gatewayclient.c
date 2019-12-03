/*******************************************************************************
 * Copyright (c) 2016 IBM Corp.
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
 *    Jeffrey Dare            - initial implementation
 *    Lokesh Haralakatta      - Added SSL/TLS support
 *    Lokesh Haralakatta      - Added Client Side Certificates support
 *    Lokesh Haralakatta      - Separated out device client and gateway client specific code.
 *                            - Retained gateway specific code here.
 *                            - Added logging feature
 *******************************************************************************/

#include "gatewayclient.h"

//Command Callback
commandCallback cb;

//Character strings to hold log header and log message to be dumped.
char logHdr[LOG_BUF];
char logStr[LOG_BUF];

//Subscription details storage
char* subscribeTopics[5];
int subscribeCount = 0;

/**
* Function used to Publish events from the device to the Watson IoT
* @param client - Reference to the GatewayClient
* @param deviceType - The type of your device
* @param deviceId - The ID of your deviceId
* @param eventType - Type of event to be published e.g status, gps
* @param eventFormat - Format of the event e.g json
* @param data - Payload of the event
* @param QoS - qos for the publish event. Supported values : QOS0, QOS1, QOS2
*
* @return int return code from the publish
*/
int publishDeviceEvent(iotfclient  *client, char *deviceType, char *deviceId, char *eventType, char *eventFormat, char* data, enum QoS qos)
{
	sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

	int rc = -1;

	char publishTopic[strlen(eventType) + strlen(eventFormat) + strlen(deviceType) + strlen(deviceId)+25];

	sprintf(publishTopic, "iot-2/type/%s/id/%s/evt/%s/fmt/%s", deviceType, deviceId, eventType, eventFormat);

	sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"Calling publishData to publish to topic - %s",publishTopic);
        LOG(logHdr,logStr);

	rc = publishData(&client->c, publishTopic , data, qos);

	if(rc != SUCCESS) {
		printf("connection lost.. %d \n",rc);
		retry_connection(client);
		rc = publishData(&client->c, publishTopic, data, qos);
	}

	sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"rc = %d",rc);
        LOG(logHdr,logStr);
        LOG(logHdr,"exit::");

	return rc;

}

/**
* Function used to Publish events from the device to the Watson IoT
* @param client - Reference to the GatewayClient
* @param eventType - Type of event to be published e.g status, gps
* @param eventFormat - Format of the event e.g json
* @param data - Payload of the event
* @param QoS - qos for the publish event. Supported values : QOS0, QOS1, QOS2
*
* @return int return code from the publish
*/
int publishGatewayEvent(iotfclient  *client, char *eventType, char *eventFormat, char* data, enum QoS qos)
{
	sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

	int rc = -1;

	char publishTopic[strlen(eventType) + strlen(eventFormat) + strlen(client->cfg.id) + strlen(client->cfg.type)+25];

	sprintf(publishTopic, "iot-2/type/%s/id/%s/evt/%s/fmt/%s", client->cfg.type, client->cfg.id, eventType, eventFormat);

	sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"Calling publishData to publish to topic - %s",publishTopic);
        LOG(logHdr,logStr);

	rc = publishData(&client->c, publishTopic , data, qos);

	if(rc != SUCCESS) {
		printf("connection lost.. \n");
		retry_connection(client);
		rc = publishData(&client->c, publishTopic , data, qos);
	}

	sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"rc = %d",rc);
        LOG(logHdr,logStr);
        LOG(logHdr,"exit::");

	return rc;

}

/**
* Function used to subscribe to all commands for gateway.
*
* @return int return code
*/
int subscribeToGatewayCommands(iotfclient  *client)
{
	sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

	int rc = -1;

	char* subscribeTopic = NULL;

	subscribeTopic = (char*) malloc(strlen(client->cfg.id) + strlen(client->cfg.type) + 28);

	sprintf(subscribeTopic, "iot-2/type/%s/id/%s/cmd/+/fmt/+", client->cfg.type, client->cfg.id);

	sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"Calling MQTTSubscribe for subscribing to gateway commands");
        LOG(logHdr,logStr);

	rc = MQTTSubscribe(&client->c, subscribeTopic, QOS2, gatewayMessageArrived);

	subscribeTopics[subscribeCount++] = subscribeTopic;

	sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"RC from MQTTSubscribe - %d",rc);
        LOG(logHdr,logStr);
        LOG(logHdr,"exit::");

	return rc;
}

/**
* Function used to subscribe to device commands for gateway.
*
* @return int return code
*/
int subscribeToDeviceCommands(iotfclient  *client, char* deviceType, char* deviceId, char* command, char* format, int qos)
{
	sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

	int rc = -1;

	char* subscribeTopic = NULL;

	subscribeTopic = (char*)malloc(strlen(deviceType) + strlen(deviceId) + strlen(command) + strlen(format) + 26);

	sprintf(subscribeTopic, "iot-2/type/%s/id/%s/cmd/%s/fmt/%s", deviceType, deviceId, command, format);

	sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"Calling MQTTSubscribe for subscribing to device commands");
        LOG(logHdr,logStr);

	rc = MQTTSubscribe(&client->c, subscribeTopic, qos, gatewayMessageArrived);

	subscribeTopics[subscribeCount++] = subscribeTopic;

	sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"RC from MQTTSubscribe - %d",rc);
        LOG(logHdr,logStr);
        LOG(logHdr,"exit::");

	return rc;
}

/**
* Function used to disconnect from the IoTF service
*
* @return int return code
*/

int disconnectGateway(iotfclient  *client)
{
	sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

	int rc = 0;
	int count;

	//Disconnect from IoT Service
	rc = disconnect(client);

	//free memory for subscriptions
	for(count = 0; count < subscribeCount ; count++)
		free(subscribeTopics[count]);

	sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
	sprintf(logStr,"RC from iotf disconnect function - %d",rc);
	LOG(logHdr,logStr);
	LOG(logHdr,"exit::");

	return rc;
}

//Handler for all commands. Invoke the callback.
void gatewayMessageArrived(MessageData* md)
{
       sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
       LOG(logHdr,"entry::");

       if(cb != 0) {
	       MQTTMessage* message = md->message;

	       char *topic = malloc(md->topicName->lenstring.len+1);

	       sprintf(topic,"%.*s",md->topicName->lenstring.len,md->topicName->lenstring.data);

	       void *payload = message->payload;

	       size_t payloadlen = message->payloadlen;

	       strtok(topic, "/");
	       strtok(NULL, "/");

	       char *type = strtok(NULL, "/");
	       strtok(NULL, "/");
	       char *id = strtok(NULL, "/");
	       strtok(NULL, "/");
	       char *commandName = strtok(NULL, "/");
	       strtok(NULL, "/");
	       char *format = strtok(NULL, "/");

	       free(topic);

	       sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
	       sprintf(logStr,"Calling registered callabck to process the arrived message");
	       LOG(logHdr,logStr);

	       (*cb)(type,id,commandName, format, payload,payloadlen);
       }
       else{
	       sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
	       sprintf(logStr,"No registered callback function to process the arrived message");
	       LOG(logHdr,logStr);
       }

       sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
       sprintf(logStr,"Returning from %s",__func__);
       LOG(logHdr,logStr);
       LOG(logHdr,"exit::");
}

/**
* Function used to set the Command Callback function. This must be set if you to recieve commands.
*
* @param cb - A Function pointer to the commandCallback. Its signature - void (*commandCallback)(char* commandName, char* payload)
* @return int return code
*/
void setGatewayCommandHandler(iotfclient *client, commandCallback handler)
{
	sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

	cb = handler;

	if(cb != NULL){
                sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
                sprintf(logStr,"Registered callabck to process the arrived message");
                LOG(logHdr,logStr);
        }
        else{
                sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
                sprintf(logStr,"Callabck not registered to process the arrived message");
                LOG(logHdr,logStr);
        }

        sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"Returning from %s",__func__);
        LOG(logHdr,logStr);
        LOG(logHdr,"exit::");
}
