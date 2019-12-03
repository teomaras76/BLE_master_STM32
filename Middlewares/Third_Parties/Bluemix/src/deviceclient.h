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
 *                        -  Contains the abstract declarations for device client
 *******************************************************************************/
 #ifndef DEVICECLIENT_H_
 #define DEVICECLIENT_H_

 #include "iotfclient.h"

#define PUBLISH_STRING_SIZE     256


 //Callback used to process device commands
 typedef void (*commandCallback)(char* commandName, char *format, void* payload);

 /**
 * Function used to subscribe to all commands.
 * @param client - Reference to the Iotfclient
 *
 * @return int return code
 */
 int subscribeCommands(iotfclient *client);

 /**
 * Function used to Publish events from the device to the IBM Watson IoT service
 * @param client - Reference to the Iotfclient
 * @param eventType - Type of event to be published e.g status, gps
 * @param eventFormat - Format of the event e.g json
 * @param data - Payload of the event
 * @param QoS - qos for the publish event. Supported values : QOS0, QOS1, QOS2
 *
 * @return int return code from the publish
 */
 int publishEvent(iotfclient *client, char *eventType, char *eventFormat, char* data, enum QoS qos);

 /* Function called upon device receiving the message from platform.
 **/
 void messageArrived(MessageData* md);

 /**
 * Function used to set the Command Callback function. This must be set if you to recieve commands.
 * @param client - Reference to the Iotfclient
 *
 * @param cb - A Function pointer to the commandCallback. Its signature - void (*commandCallback)(char* commandName, char* payload)
 * @return int return code
 */
 void setCommandHandler(iotfclient *client, commandCallback cb);

 #endif /* End od DEVICECLIENT_H_ */
