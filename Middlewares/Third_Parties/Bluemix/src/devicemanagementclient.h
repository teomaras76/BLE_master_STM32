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
 *    Hari hara prasad Viswanathan, - initial implementation and API implementation
 *    Hari prasada Reddy P
 *    Lokesh Haralakatta      - Added SSL/TLS support
 *    Lokesh Haralakatta      - Added Client Side Certificates support
 *******************************************************************************/

#ifndef DEVICEMANAGEMENTCLIENT_H_
#define DEVICEMANAGEMENTCLIENT_H_

#include <stdbool.h>
#include "cJSON.h"
#include "iotfclient.h"
#include "deviceclient.h"

//Macros for the device management requests
#define MANAGE "iotdevice-1/mgmt/manage"
#define UNMANAGE "iotdevice-1/mgmt/unmanage"
#define UPDATE_LOCATION "iotdevice-1/device/update/location"
#define CREATE_DIAG_ERRCODES "iotdevice-1/add/diag/errorCodes"
#define CLEAR_DIAG_ERRCODES "iotdevice-1/clear/diag/errorCodes"
#define ADD_DIAG_LOG "iotdevice-1/add/diag/log"
#define CLEAR_DIAG_LOG "iotdevice-1/clear/diag/log"
#define NOTIFY "iotdevice-1/notify"
#define DMRESPONSE "iotdm-1/response"
#define RESPONSE "iotdevice-1/response"

//Macros for Device actions
#define REBOOT_INITIATED 		  202
#define REBOOT_FAILED	 		  500
#define REBOOT_NOTSUPPORTED 	  501

#define FACTORYRESET_INITIATED 	  202
#define FACTORYRESET_FAILED	 	  500
#define FACTORYRESET_NOTSUPPORTED 501

#define UPDATE_SUCCESS			  204
#define RESPONSE_SUCCESS		  200
#define FIRMWARESTATE_IDLE		  0
#define FIRMWARESTATE_DOWNLOADING 1
#define FIRMWARE_DOWNLOADED		  2

#define FIRMWAREUPDATE_SUCCESS	  	  	  0
#define FIRMWAREUPDATE_INPROGRESS 	  	  1
#define FIRMWAREUPDATE_OUTOFMEMORY 	  	  2
#define FIRMWAREUPDATE_CONNECTIONLOST 	  3
#define FIRMWAREUPDATE_VERIFICATIONFAILED 4
#define FIRMWAREUPDATE_UNSUPPORTEDIMAGE   5
#define FIRMWAREUPDATE_INVALIDURL		  6

#define RESPONSE_ACCEPTED				  202
#define BAD_REQUEST						  400


//structure for device information
//string variables needs to be allocated with enough memory based on the requirement
struct DeviceInfo{
	char serialNumber[20];
	char manufacturer[20];
	char model[20];
	char deviceClass[20];
	char description[30];
	char fwVersion[10];
	char hwVersion[10];
	char descriptiveLocation[20];
};

//structure for device location
struct DeviceLocation{
	double latitude;
	double longitude;
	double elevation;
	time_t measuredDateTime;
	double accuracy;
};

//structure for device actions
struct DeviceAction{
	int status;
	char message[50];
	char typeId[10];
	char deviceId[10];
};

//structure for device firmware attributes
struct DeviceFirmware{
	char version[10];
	char name[20];
	char url[100];
	char verifier[20];
	int state;
	int updateStatus;
	char deviceId[40];
	char typeId[20];
	char updatedDateTime[20];
};

//structure for metadata of device
struct DeviceMetadata{
	char metadata[10];
};

//structure for device management
struct DeviceMgmt{
	struct DeviceFirmware firmware;
};

//structure for device data
struct deviceData{
	struct DeviceInfo deviceInfo;
	struct DeviceLocation deviceLocation;
	struct DeviceMgmt mgmt;
	struct DeviceMetadata metadata;
	struct DeviceAction deviceAction;
};

struct managedDevice{
		bool supportDeviceActions ;
		bool supportFirmwareActions ;
		bool bManaged ;
		bool bObserve;
		char responseSubscription[50];
		struct deviceData DeviceData;
		iotfclient deviceClient;
};
typedef struct managedDevice ManagedDevice;
extern  ManagedDevice dmClient;

//Callback used to process actions
typedef void (*actionCallback)(void);
/**
* Function used to initialize the IBM Watson IoT client using the config file which is generated when you register your device
*
* @param configFilePath - File path to the configuration file
*
* @return int return code
* error codes
* CONFIG_FILE_ERROR -3 - Config file not present or not in right format
*/
int initialize_configfile_dm(char *configFilePath);

/**
* Function used to initialize the IBM Watson IoT client
*
* @param org - Your organization ID
*
* @param domain - Your domain Name
*
* @param type - The type of your device
*
* @param id - The ID of your device
*
* @param auth-method - Method of authentication (the only value currently supported is â€œtokenâ€�)
*
* @param auth-token - API key token (required if auth-method is â€œtokenâ€�)
*
* @Param serverCertPath - Custom Server Certificate Path
*
* @Param useCerts - Flag to indicate whether to use client side certificates for authentication
*
* @Param rootCAPath - if useCerts is 1, Root CA certificate Path
* @Param clientCertPath - if useCerts is 1, Client Certificate Path
* @Param clientKeyPath - if useCerts is 1, Client Key Path
*
* @return int return code
*/
int initialize_dm(char *orgId, char *domianName, char *deviceType, char *deviceId,
		char *authmethod,char *authToken,char *serverCertPath, int useCerts,
		char *rootCACertPath, char *clientCertPath, char *clientKeyPath);

/**
* Function used to connect the device to IBM Watson IoT client
*
* @return int return code
*/

int connectiotf_dm(void);
/**
* Function used to Publish events from the device to the IBM Watson IoT service
*
* @param eventType - Type of event to be published e.g status, gps
*
* @param eventFormat - Format of the event e.g json
*
* @param data - Payload of the event
*
* @param QoS - qos for the publish event. Supported values : QOS0, QOS1, QOS2
*
* @return int return code from the publish
*/
int publishEvent_dm(char *eventType, char *eventFormat, unsigned char* data, enum QoS qos);

/**
* Function used to set the Command Callback function. This must be set if you want to receive commands.
*
* @param client - Reference to the ManagedDevice
*
* @param cb - A Function pointer to the commandCallback. Its signature - void (*commandCallback)(char* commandName, char* format, void*     payload)
*
*/
void setCommandHandler_dm(commandCallback cb);

/**
* Function used to subscribe to all commands. This function is by default called when in registered mode.
*
* @return int return code
*/
int subscribeCommands_dm(void);

/**
* Function used to check if the client is connected
*
* @return int return code
*/
int isConnected_dm(void);

/**
* Function used to Yield for commands.
*
* @param time_ms - Time in milliseconds
*
* @return int return code
*/
int yield_dm(int time_ms);

/**
* Function used to disconnect from the IBM Watson IoT service
*
* @return int return code
*/
int disconnect_dm(void);

/**
* <p>Send a device manage request to Watson IoT Platform</p>
*
* <p>A Device uses this request to become a managed device.
* It should be the first device management request sent by the
* Device after connecting to the IBM Watson IoT Platform.
* It would be usual for a device management agent to send this
* whenever is starts or restarts.</p>
*
* <p>This method connects the device to Watson IoT Platform connect if its not connected already</p>
*
* @param client reference to the ManagedDevice
*
* @param lifetime The length of time in seconds within
*        which the device must send another Manage device request.
*        if set to 0, the managed device will not become dormant.
*        When set, the minimum supported setting is 3600 (1 hour).
*
* @param supportFirmwareActions Tells whether the device supports firmware actions or not.
*        The device must add a firmware handler to handle the firmware requests.
*
* @param supportDeviceActions Tells whether the device supports Device actions or not.
*        The device must add a Device action handler to handle the reboot and factory reset requests.
*
* @param reqId Function returns the reqId if the publish Manage request is successful.
*
* @return
*/
void publishManageEvent(long lifetime, int supportFirmwareActions,
	int supportDeviceActions, char* reqId);
/**
 * Moves the device from managed state to unmanaged state
 *
 * A device uses this request when it no longer needs to be managed.
 * This means Watson IoT Platform will no longer send new device management requests
 * to this device and device management requests from this device will
 * be rejected apart from a Manage device request
 *
 * @param reqId Function returns the reqId if the Unmanage request is successful.
 *
 */
void publishUnManageEvent(char* reqId);

/**
 * Update the location.
 *
 * @param latitude	Latitude in decimal degrees using WGS84
 *
 * @param longitude Longitude in decimal degrees using WGS84
 *
 * @param elevation	Elevation in meters using WGS84
 *
 * @param measuredDateTime	Date of location measurement in ISO8601 format
 *
 * @param accuracy	Accuracy of the position in meters
 *
 * @param reqId Function returns the reqId if the updatelocation request is successful.
 *
 * @return code indicating whether the update is successful or not
 *        (200 means success, otherwise unsuccessful)
 */
void updateLocation(double latitude, double longitude, double elevation, char* measuredDateTime, double accuracy, char* reqId) ;

/**
 * Update the location.
 *
 * @param latitude	Latitude in decimal degrees using WGS84
 *
 * @param longitude Longitude in decimal degrees using WGS84
 *
 * @param elevation	Elevation in meters using WGS84
 *
 * @param measuredDateTime	Date of location measurement in ISO8601 format
 *
 * @param updatedDateTime	Date of the update to the device information in ISO8601 format
 *
 * @param accuracy	Accuracy of the position in meters
 *
 * @param reqId Function returns the reqId if the UpdateLocation request is successful.
 *
 * @return code indicating whether the update is successful or not
 *        (200 means success, otherwise unsuccessful)
 */
void updateLocationEx(double latitude, double longitude, double elevation, char* measuredDateTime,char* updatedDateTime, double accuracy, char* reqId);
/**
 * Adds the current errorcode to IBM Watson IoT Platform.
 *
 * @param errorCode The "errorCode" is a current device error code that
 * needs to be added to the Watson IoT Platform.
 *
 * @param reqId Function returns the reqId if the addErrorCode request is successful.
 *
 * @return code indicating whether the update is successful or not
 *        (200 means success, otherwise unsuccessful)
 */
void addErrorCode(int errNum, char* reqId);
/**
 * Clear the Error Codes from IBM Watson IoT Platform for this device
 *
 * @param reqId Function returns the reqId if the clearErrorCodes request is successful.
 *
 * @return code indicating whether the clear operation is successful or not
 *        (200 means success, otherwise unsuccessful)
 */
void clearErrorCodes(char* reqId);
/**
 * The Log message that needs to be added to the Watson IoT Platform.
 *
 * @param message The Log message that needs to be added to the Watson IoT Platform.
 *
 * @param timestamp The Log timestamp
 *
 * @param data The optional diagnostic string data
 *
 * @param severity The Log severity
 *
 * @param reqId Function returns the reqId if the addLog request is successful.
 *
 * @return code indicating whether the update is successful or not
 *        (200 means success, otherwise unsuccessful)
 */
void addLog(char* message, char* data ,int severity, char* reqId);
/**
 * Clear the Logs from IBM Watson IoT Platform for this device
 *
 * @param reqId Function returns the reqId if the clearLogs request is successful.
 *
 * @return code indicating whether the clear operation is successful or not
 *        (200 means success, otherwise unsuccessful)
 */
void clearLogs(char* reqId);

/**
 * Register Callback function to managed request response
 *
 * @param cb - A Function pointer to the commandCallback. Its signature - void (*commandCallback)(char* Status, char* requestId,            void*       payload)
 *
*/
void setManagedHandler_dm(commandCallback cb);

/**
 * Register Callback function to Factory reset request
 *
 * @param client reference to the ManagedDevice
 *
 * @param cb - A Function pointer to the commandCallback. Its signature - void (*commandCallback)(char* Status, char* requestId,            void*       payload)
 *
*/
void setFactoryResetHandler(commandCallback cb);

/**
 * Register Callback function to Reboot request
 *
 * @param cb - A Function pointer to the commandCallback. Its signature - void (*commandCallback)(char* Status, char* requestId,            void*       payload)
 *
*/
void setRebootHandler(commandCallback cb);
/**
 * Register Callback function to Firmware Download request
 *
 * @param cb - A Function pointer to the actionCallback. Its signature - void (*actionCallback)()
 *
*/
void setFirmwareDownloadHandler(actionCallback cb);
/**
 * Register Callback function to Firmware Update request
 *
 * @param cb - A Function pointer to the actionCallback. Its signature - void (*actionCallback)()
 *
*/
void setFirmwareUpdateHandler(actionCallback cb);
/**
 * Update the firmware state while downloading firmware and
 * Notifies the IBM Watson IoT Platform with the updated state
 *
 * @param state Download state update received from the device
 *
 * @return int return code
 *
 */
int changeFirmwareState(int state);
/**
 * Update the firmware state while updating firmware and
 * Notifies the IBM Watson IoT Platform with the updated state
 *
 * @param state update state update received from the device
 *
 * @return int return code
 *
 */
int changeFirmwareUpdateState(int state);

//util functions
void onMessage(MessageData* md);
void messageResponse(MessageData* md);
void messageUpdate(MessageData* md);
void messageObserve(MessageData* md);
void messageCancel(MessageData* md);
void messageForAction(MessageData* md, bool isReboot);
void generateUUID(char* uuid_str);
int publish(char* publishTopic, char* data);
void getMessageFromReturnCode(int rc, char* msg);
void messageFirmwareDownload(MessageData* md);
void messageFirmwareUpdate(MessageData* md);
int changeState(int rc);
void updateLocationHandler(double latitude, double longitude, double elevation, char* measuredDateTime,char* updatedDateTime, double accuracy);
void updateLocationRequest(cJSON* value);
void updateFirmwareRequest(cJSON* value);


#endif /* DEVICEMANAGEMENTCLIENT_H_ */
