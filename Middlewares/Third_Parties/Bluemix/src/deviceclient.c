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
 *                        -  Contains the device client specific functions
 *                        -  Added logging feature
 *******************************************************************************/

 #include "deviceclient.h"

 //Command Callback
 commandCallback cb;

//Character strings to hold log header and log message to be dumped.
 extern char logHdr[LOG_BUF];
 extern char logStr[LOG_BUF];

 /**
 * Function used to Publish events from the device to the IBM Watson IoT service
 * @param eventType - Type of event to be published e.g status, gps
 * @param eventFormat - Format of the event e.g json
 * @param data - Payload of the event
 * @param QoS - qos for the publish event. Supported values : QOS0, QOS1, QOS2
 *
 * @return int return code from the publish
 */

 int publishEvent(iotfclient  *client, char *eventType, char *eventFormat, char* data, enum QoS qos)
 {
        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

        int rc = -1;
        char publishTopic[PUBLISH_STRING_SIZE];
        int len = strlen(eventType) + strlen(eventFormat) + 1;
        if (len < PUBLISH_STRING_SIZE )
        {
          sprintf(publishTopic, "iot-2/evt/%s/fmt/%s", eventType, eventFormat);
        }
        else
        {
          printf("\nPublish string too long , increase PUBLISH_STRING_SIZE define %d\n",PUBLISH_STRING_SIZE);
          return rc;
        }
        sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"Calling publishData to publish to topic - %s",publishTopic);
        LOG(logHdr,logStr);

 	rc = publishData(&(client->c),publishTopic,data,qos);

 	if(rc != SUCCESS) {
 		printf("\nConnection lost, retry the connection \n");
 		retry_connection(client);
 		rc = publishData(&(client->c),publishTopic,data,qos);
 	}

        sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"rc = %d",rc);
        LOG(logHdr,logStr);
        LOG(logHdr,"exit::");

 	return rc;

 }

 /**
 * Function used to subscribe to all device commands.
 *
 * @return int return code
 */
 int subscribeCommands(iotfclient  *client)
 {
        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

        int rc = -1;

        sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"Calling MQTTSubscribe for subscribing to device commands");
        LOG(logHdr,logStr);

        rc = MQTTSubscribe(&client->c, "iot-2/cmd/+/fmt/+", QOS0, messageArrived);
       // rc = MQTTSubscribe(&client->c, "iotdm-1/response/json", QOS0, messageArrived);

        sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
        sprintf(logStr,"RC from MQTTSubscribe - %d",rc);
        LOG(logHdr,logStr);
        LOG(logHdr,"exit::");

        return rc;
 }

 //Handler for all commands. Invoke the callback.
 void messageArrived(MessageData* md)
 {
        sprintf(logHdr,"%s:%d:%s:",__FILE__,__LINE__,__func__);
        LOG(logHdr,"entry::");

 	if(cb != 0) {
 		MQTTMessage* message = md->message;

 		char *topic = malloc(md->topicName->lenstring.len+1);

 		sprintf(topic,"%.*s",md->topicName->lenstring.len,md->topicName->lenstring.data);

 		void *payload = message->payload;

 		strtok(topic, "/");
 		strtok(NULL, "/");

 		char *commandName = strtok(NULL, "/");
 		strtok(NULL, "/");
 		char *format = strtok(NULL, "/");

                sprintf(logHdr,"%s:%d:%s",__FILE__,__LINE__,__func__);
                sprintf(logStr,"Calling registered callabck to process the arrived message");
                LOG(logHdr,logStr);

 		(*cb)(commandName, format, payload);

 		free(topic);

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
 void setCommandHandler(iotfclient  *client, commandCallback handler)
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
