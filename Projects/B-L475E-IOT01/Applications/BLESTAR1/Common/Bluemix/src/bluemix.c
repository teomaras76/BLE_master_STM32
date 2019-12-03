/*
 * test_devicemgmt.c
 *
 *  Created on: 03-May-2016
 *      Author: HariPrasadReddy
 */
/* Unit Tests to test source code of devicemanagementclient.c */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <smartfile.h>

#include "msg.h"
#include "devicemanagementclient.h"
#include "sensors_data.h"
#include "iot_flash_config.h"
#include "cloud.h"

/* define for manage device request */
#define LIFE_TIME_BTWN_MANAGE_DEV_REQ   4000
#define FIRMWARE_ACTION_SUPPORT         0
#define DEVICE_ACTION_SUPPORT           1
#define TIME_STAMP_BUF_SIZE             25

#define error(A) printf("%s:%d Error:",__FILE__,__LINE__);printf(A)

/* Global variables ---------------------------------------------------------*/
extern const user_config_t lUserConfig;

/* Private variables ---------------------------------------------------------*/
static bool reboot_callback=false;

/* Private function prototypes -----------------------------------------------*/
int changeState(int);
int getTimestamp(char *, int);

#define PAYLOADSIZE     500
char PayloadBuffer[PAYLOADSIZE];

#define BM_INDIV_DEV_STRING_MAX 32
typedef struct
{
  char   register_mode[BM_INDIV_DEV_STRING_MAX];
  char   orgid[BM_INDIV_DEV_STRING_MAX];
  char   url[BM_INDIV_DEV_STRING_MAX];
  char   device_type[BM_INDIV_DEV_STRING_MAX];
  char   device_id[BM_INDIV_DEV_STRING_MAX];
  char   secmode[BM_INDIV_DEV_STRING_MAX];
  char   token[BM_INDIV_DEV_STRING_MAX];
  int    usecert;
  char * rootCACertString;
  char * interCACertString;
  char * clientCertString;
  char * clientKeyString;
} device_info_t;

int PrepareToggleMqttPayload(char * PayloadBuffer, int PayloadSize, char * deviceID, int toggle);
void myCallback (char* commandName, char* format, void* payload);
void managedCallBack (char* Status, char* requestId, void* payload);
void rebootCallBack (char* reqID, char* action, void* payload);
void factoryResetCallBack (char* reqID, char* action, void* payload);
void firmwareDownloadCallBack(void);
void firmwareUpdateCallBack(void);
void populateMgmtConfig(void);
void testManage(void **state);
void testDeviceActions(void **state);
void testDeviceFirmwareActions(void **state);
char *min_char_ptr( char * ptr1, char * ptr2 );
int32_t comp_left_ms(uint32_t init, uint32_t now, uint32_t timeout);
int setBluemixDevInfo( device_info_t * dev_info );
void connectIoTDev(device_info_t *dev);
void publishJSONPkt(uint8_t* buffer);


int PrepareToggleMqttPayload(char * PayloadBuffer, int PayloadSize, char * deviceID, int toggle)
{
  char * Buff = PayloadBuffer;
  int BuffSize = PayloadSize;
  int snprintfreturn = 0;
  char timestamp[TIME_STAMP_BUF_SIZE];
  memset(&timestamp, 0, sizeof(timestamp));

#ifdef BLUEMIX
  if (0 != getTimestamp(timestamp, TIME_STAMP_BUF_SIZE)) {
    error("Unable to get a timestamp \n");
  }

  snprintfreturn = snprintf( Buff, BuffSize, "{\"d\":{\"Led state\": %d, \"Timestamp\": \"%s\"}}", toggle, timestamp );
  //snprintfreturn = snprintf( Buff, BuffSize, "{\"d\":{\"Led state\": %d }}", toggle );

#else
  if (deviceID != NULL)
  {
    snprintfreturn = snprintf( Buff, BuffSize, "{\"deviceId\":\"%s\",\"PushButton\": %d }",deviceID,toggle);
  }
  else
  {
    snprintfreturn = snprintf( Buff, BuffSize, "{ \"state\": { \"reported\": {\"PushButton\": %d}}}",toggle );
  }
#endif

  if (snprintfreturn >= 0 && snprintfreturn < PayloadSize)
  {
    return 0;
  }

  return 1;
}

void myCallback (char* commandName, char* format, void* payload)
{
  printf("The command received :: %s\n", commandName);
  printf("format : %s\n", format);
  printf("Payload is : %s\n", (char *)payload);
}

void managedCallBack (char* Status, char* requestId, void* payload)
{
  printf("\n--------- Callback following our request to become a managed device ---------\n" );
  printf("Status :: %s\n", Status);
  printf("requestId : %s\n", requestId);
  printf("Payload is : %s\n", (char *)payload);
  printf("-----------------------------------------------------------------------------\n" );
}

void rebootCallBack (char* reqID, char* action, void* payload)
{
  printf("\n----------- REBOOT callback ----------- \n" );
  printf("request Id :: %s\n", reqID);
  printf("action : %s\n", action);
  printf("Payload is : %s\n", (char *)payload);
  
  changeState(REBOOT_INITIATED);
  //Reboot custom code needs to be added based on the platform the application is running
  //After Rebooting the device Manage request needs to be sent to the platform to successfully complete the action
  //So this program needs to be kept in the bashrc so that once the system reboots Manage event will be sent and the action will be successful.
  //#if defined(_WIN32) || defined(_WIN64)
  //	system("C:\\WINDOWS\\System32\\shutdown -r");
  //#else
  //	system("sudo shutdown -r now");
  //#endif
  printf("------------------------------------\n" );
  reboot_callback=true;
}

void factoryResetCallBack (char* reqID, char* action, void* payload)
{
  printf("\n--------------FACTORYRESET----------------------\n" );
  printf("request Id: %s\n", reqID);
  printf("action : %s\n", action);
  printf("Payload is : %s\n", (char *)payload);
  
  // This sample doesn't support factory reset, so respond accordingly
  changeState(FACTORYRESET_NOTSUPPORTED);
  printf("Factory reset is not supported in this sample\n" );
  printf("------------------------------------\n" );
}

void firmwareDownloadCallBack()
{
  printf("\n--------------Firmware Download----------------------\n" );
  //Add the code for downloading the firmware
  changeFirmwareState(FIRMWARESTATE_DOWNLOADING);
  sleep(5);
  changeFirmwareState(FIRMWARE_DOWNLOADED);
}

void firmwareUpdateCallBack()
{
  printf("\n--------------Firmware Update----------------------\n");
  //Add the code for updating the firmware
  changeFirmwareUpdateState(FIRMWAREUPDATE_INPROGRESS);
  sleep(5);
  changeFirmwareUpdateState(FIRMWAREUPDATE_SUCCESS);
  sleep(5);
  changeFirmwareState(FIRMWARESTATE_IDLE);
}

void populateMgmtConfig()
{
  strcpy(dmClient.DeviceData.deviceInfo.serialNumber, "XXX" );
  strcpy(dmClient.DeviceData.deviceInfo.manufacturer , "ST microelectronics");
  strcpy(dmClient.DeviceData.deviceInfo.model , "7865");
  strcpy(dmClient.DeviceData.deviceInfo.deviceClass , "DMA");
  strcpy(dmClient.DeviceData.deviceInfo.description , "Sensor board");
  strcpy(dmClient.DeviceData.deviceInfo.fwVersion , "1.0.0");
  strcpy(dmClient.DeviceData.deviceInfo.hwVersion , "1.0");
  strcpy(dmClient.DeviceData.deviceInfo.descriptiveLocation , "EGL C");
  strcpy(dmClient.DeviceData.metadata.metadata ,"{}");
}

void testManage(void **state)
{
  unsigned char *payload1 = (unsigned char*) "{ \"d\" : { \"temp\" : 34.0 }}";
  unsigned char *payload2 = (unsigned char*) "{ \"d\" : { \"temp\" : 14.5 }}";
  
  assert_int_equal(connectiotf_dm(),0);
  
  setCommandHandler_dm(myCallback);
  setManagedHandler_dm(managedCallBack );
  subscribeCommands_dm();
  populateMgmtConfig();
  
  char reqId[40] = {0};
  char test[40] = {0};
  assert_string_equal(reqId, test);
  publishManageEvent(4000,1,1, reqId);
  assert_string_not_equal(reqId, test);
  
  strcpy(reqId, test);
  addErrorCode(121, reqId);
  assert_string_not_equal(reqId, test);
  
  strcpy(reqId, test);
  clearErrorCodes(reqId);
  assert_string_not_equal(reqId, test);
  //strcpy(reqId, test);
  //addLog("test","",1, reqId);
  //assert_string_not_equal(reqId, test);
  
  strcpy(reqId, test);
  clearLogs(reqId);
  assert_string_not_equal(reqId, test);
  
  strcpy(reqId, test);
  time_t t = time(NULL);
  time(&t);
  struct tm* tt = localtime(&t);
  char updatedDateTime[80];	//"2016-03-01T07:07:56.323Z"
  strftime(updatedDateTime, 80, "%Y-%m-%dT%TZ",(struct tm const*) &tt);
  strcpy(updatedDateTime,"2016-03-01T07:07:56.323Z");
  updateLocation(77.5667, 12.9667, 0, updatedDateTime, 0, reqId);
  assert_string_not_equal(reqId, test);
  
  {
    assert_int_equal(publishEvent_dm("status","json", payload1 , QOS0),0);
    sleep(5);
    assert_int_equal(publishEvent_dm("status","json", payload2 , QOS0),0);
    sleep(5);
  }
  
  strcpy(reqId, test);
  publishUnManageEvent(reqId);
  assert_string_not_equal(reqId, test);
  
  assert_int_equal(yield_dm(10),0);
}

void testDeviceActions(void **state)
{
  assert_int_equal(connectiotf_dm(),0);
  
  setCommandHandler_dm(myCallback);
  setManagedHandler_dm(managedCallBack);
  setRebootHandler(rebootCallBack);
  setFactoryResetHandler(factoryResetCallBack);
  subscribeCommands_dm();
  populateMgmtConfig();
  
  char reqId[40] = {0};
  char test[40] = {0};
  
  publishManageEvent(4000, 1, 1, reqId);
  assert_string_not_equal(reqId, test);
#define AUTOTEST
#ifdef AUTOTEST
  char action[15];
  char payload[100];
  
  strcpy(reqId, "9c982454-3b44-4160-b577-cd6df67be560");
  strcpy(action,"reboot");
  
  strcpy(payload,"{\"reqId\":\"9c982454-3b44-4160-b577-cd6df67be560\"}");
  
  //Construct message data for Reboot
  MessageData md;
  int topicLen = 50;
  int loadLen = 100;
  
  MQTTString topicName;
  topicName.lenstring.data = "iotdm-1/mgmt/initiate/device/reboot";
  topicName.lenstring.len = topicLen;
  
  MQTTMessage msg;
  msg.payload = "{\"reqId\":\"9c982454-3b44-4160-b577-cd6df67be560\"}";
  msg.payloadlen = loadLen;
  md.message = &msg;
  md.topicName = &topicName;
  
  
  messageForAction(&md,1);
#if 0
  //Construct Factory reset request
  //rebootCallBack(reqId,action,payload);
  
  topicName.lenstring.data = "iotdm-1/mgmt/initiate/device/factory_reset";
  topicName.lenstring.len = topicLen;
  
  msg.payload = "{\"reqId\":\"8c982454-3b44-4160-b577-cd6df67be5702\"}";
  msg.payloadlen = loadLen;
  md.message = &msg;
  md.topicName = &topicName;
  
  messageForAction(&md,0);
#endif
  
#endif
  while(!reboot_callback) yield_dm(100);
  
}

void testDeviceFirmwareActions(void **state)
{
  assert_int_equal(connectiotf_dm(),0);
  
  setCommandHandler_dm(myCallback);
  setManagedHandler_dm(managedCallBack);
  setFirmwareDownloadHandler(firmwareDownloadCallBack);
  setFirmwareUpdateHandler(firmwareUpdateCallBack);
  subscribeCommands_dm();
  
  populateMgmtConfig();
  
  char reqId[40] = {0};
  char test[40] = {0};
  //reqId[0] = '\0';
  //assert_null(reqId);
  assert_string_equal(reqId, test);
  
  publishManageEvent(4000,1,1, reqId);
  
  assert_string_not_equal(reqId, test);
  
  //Construct message data for location update
  MessageData md;
  MQTTMessage msg;
  
  int loadLen = 400;
  int topicLen = 100;
  
  MQTTString topicName;
  topicName.lenstring.data = "iotdevice-1/device/update/location";
  topicName.lenstring.len = topicLen;
  
  msg.payload =
    "{\"reqId\" : \"b38faafc-53de-47a8-a940-e697552c3194\",\"d\" : {\"fields\" : [{\"field\" : \"location\",\"value\" : {\"latitude\" : \"latitude value\",\"longitude\" : \"longitude val\",\"elevation\" : \"some elevation\",\"accuracy\" : \"accuracy val\",\"measuredDateTime\" : \"date and time\",\"updatedDateTime\" : \"updated date\"}}]}}";
  msg.payloadlen = loadLen;
  md.message = &msg;
  md.topicName = &topicName;
  messageUpdate(&md);
  
  //Construct message data for firmware update
  //	MessageData md;
  //	MQTTMessage msg;
  //
  //	int loadLen = 400;
  //	int topicLen = 100;
  
  //	MQTTString topicName;
  topicName.lenstring.data = "iotdm-1/device/update";
  topicName.lenstring.len = topicLen;
  
  msg.payload = "{\"reqId\" : \"f38faafc-53de-47a8-a940-e697552c3194\",\"d\" : {\"fields\" : [{\"field\" : \"mgmt.firmware\",\"value\" : {\"version\" : \"some firmware version\",\"name\" : \"some firmware name\",\"uri\" : \"some uri for firmware location\",\"verifier\" : \"some validation code\",\"state\" : 0,\"updateStatus\" : 0,\"updatedDateTime\" : \"\"}}]}}";
  msg.payloadlen = loadLen;
  md.message = &msg;
  md.topicName = &topicName;
  messageUpdate(&md);
  
  //Construct request for observe for Firmware state
  topicName.lenstring.data =  "iotdm-1/observe";
   
  char payLoad[] = "{\"reqId\" : \"909b477c-cd37-4bee-83fa-1d568664fbe8\",\"d\" : {\"fields\" : [{\"field\":\"mgmt.firmware\"}]}}";
  msg.payload = payLoad;
  
  md.message = &msg;
  md.topicName = &topicName;
  messageObserve(&md);
  
  //Construct message request for firmware download
  topicName.lenstring.data = "iotdm-1/mgmt/initiate/firmware/download";
  msg.payload = "{\"reqId\" : \"7b244053-c08e-4d89-9ed6-6eb2618a8734\"}";
  md.message = &msg;
  md.topicName = &topicName;
  messageFirmwareDownload(&md);
  messageFirmwareDownload(&md);// Negative  test case where to test download after the state change
  
  //Construct message request for cancel
  topicName.lenstring.data = "iotdm-1/cancel";
  msg.payload = "{\"reqId\" : \"d9ca3635-64d5-46e2-93ee-7d1b573fb20f\",\"d\" : {\"fields\" : [{\"field\":\"mgmt.firmware\"}]}}";
  md.message = &msg;
  md.topicName = &topicName;
  messageCancel(&md);
  
  //Construct request for observe for Firmware state
  topicName.lenstring.data = "iotdm-1/observe";
  msg.payload =
    "{\"reqId\" : \"909b477c-cd37-4bee-83fa-1d568664fbe8\",\"d\" : {\"fields\" : [{\"field\":\"mgmt.firmware\"}]}}";
  md.message = &msg;
  md.topicName = &topicName;
  messageObserve(&md);
  
  //Construct message request for firmware update
  topicName.lenstring.data = "iotdm-1/mgmt/initiate/firmware/update";
  msg.payload = "{\"reqId\" : \"7b244053-c08e-4d89-9ed6-6eb2618a8734\"}";
  md.message = &msg;
  md.topicName = &topicName;
  messageFirmwareUpdate(&md);
  
  //Construct message request for cancel
  topicName.lenstring.data = "iotdm-1/cancel";
  msg.payload =
    "{\"reqId\" : \"d9ca3635-64d5-46e2-93ee-7d1b573fb20f\",\"d\" : {\"fields\" : [{\"field\":\"mgmt.firmware\"}]}}";
  md.message = &msg;
  md.topicName = &topicName;
  messageCancel(&md);
}

#define min(a, b) ((a < b) ? a : b)

/**
  * @brief  returns the pointer having the minimum address
  * @param  In: ptr1, ptr2      the 2 pointers to compare
  * @retval minimum pointer or NULL if none input pointer is defined
  */
char *min_char_ptr( char * ptr1, char * ptr2 )
{
  char * ret = NULL;
  
  if (ptr1 != NULL)
  {
    if (ptr2 != NULL)
    {
      ret = min(ptr1,ptr2);
    }
    else
    {
      ret = ptr1;
    }
  }
  else
  {
    if (ptr2 != NULL)
    {
      ret = ptr2;
    }
    else
    {
      ret = NULL;
    }
  }
  
  return ret;
}

/**
 * @brief   Return the integer difference between 'init + timeout' and 'now'.
 *          The implementation is robust to uint32_t overflows.
 * @param   In:   init      Reference index.
 * @param   In:   now       Current index.
 * @param   In:   timeout   Target index.
 * @retval  Number of units from now to target.
 */
int32_t comp_left_ms(uint32_t init, uint32_t now, uint32_t timeout)
{
  int32_t ret = 0;
  uint32_t wrap_end = 0;
  
  if (now < init)
  { // Timer wrap-around detected
    // printf("Timer: wrap-around detected from %d to %d\n", init, now);
    wrap_end = UINT32_MAX - init;
  }
  ret = wrap_end - (now - init) + timeout;
  
  return ret;
}

/**
  * @brief  Set the Bluemix device info structure according to the connection string in the FLASH memory.
  * @param  Out: dev_info       pointer to the bluemix device info structure
  * @retval 0:    the connection string in Flash has been parsed succesfully and the output structure fields have been set
  *        -1:    in case of failure
  */
int setBluemixDevInfo( device_info_t * dev_info )
{
  char * devInfoPtr = NULL;           /* pointer toward the values in the output structure */
  const char * devName=NULL;          /* pointer to device name in Flash */
  char workDevName[USER_CONF_DEVICE_NAME_LENGTH]; /* local variable receiving the flash device name copy */
  const char s[] = ";=";              /* delimiter used in the string to parse */
  char * smallerStr;                  /* pointer to smaller strings that are included in the device name string */
  bool is_nextAToken = false;
  size_t length;
  char * strPtr = NULL;
  char * strPtr2 = NULL;
  int ret = 0;
  
  /* initialising the output structure */
  memset(dev_info, 0, sizeof(device_info_t));
  strcpy(dev_info->url,IBM_IOT_URL);
  
  /* initialising the local array */
  memset(&workDevName, 0, sizeof(workDevName));
  
  /* retrieve the device name address in flash */
  getDeviceName(&devName);
  
  /* copy its content into the local variable */
  strcpy(workDevName,devName);
  
  /* get the first smaller string */
  smallerStr = strtok(workDevName, s);
  
  /* walk through other smaller strings */
  while( (smallerStr != NULL) && (ret == 0) )
  {
    msg_debug( "connection string part: %s\n", smallerStr );
    
    if (strcmp(smallerStr, QUICK_START_REG_NAME) == 0)
    {
      strcpy(dev_info->register_mode,QUICK_START_REG_NAME);
      strcpy(dev_info->orgid,"quickstart");
      dev_info->usecert = 0;
    } else
      if (strcmp(smallerStr, SIMPLE_REG_NAME) == 0)
      {
        strcpy(dev_info->register_mode,SIMPLE_REG_NAME);
        dev_info->usecert = 0;
        strcpy(dev_info->secmode,"token");
        dev_info->rootCACertString = (char*)&lUserConfig.tls_root_ca_cert;
      }
      else
        if (strcmp(smallerStr, ORG_ID_KEY) == 0) { devInfoPtr = dev_info->orgid; } else
          if (strcmp(smallerStr, DEVICE_TYPE_KEY) == 0) { devInfoPtr = dev_info->device_type; } else
            if (strcmp(smallerStr, DEVICE_ID_KEY) == 0) { devInfoPtr = dev_info->device_id; } else
              if (strcmp(smallerStr, TOKEN_KEY) == 0) { is_nextAToken = true; } else
                if (devInfoPtr != NULL)
                {
                  strcpy(devInfoPtr,smallerStr);
                  
                  /* this pointer shall be set again when a key word is found */
                  devInfoPtr = NULL;
                }
                else {
                  ret = -1;
                  msg_info( "Your Bluemix connection string cannot be parsed correctly.\nPlease check its syntax and reprogram it pressing the User Button (blue) during the initialisation phase.\n" );
                }
    
    /* Exception in case the next smaller string is the Bluemix token, we avoid to breaks it with delimiters as those could be part of the token */
    if (ret == 0)
    {
      if (is_nextAToken == true)
      {
        /* smallerStr points to the token key word */
        /* go to the token value */
        length = strlen(smallerStr);
        smallerStr += (length+1);
        
        /* look for occurence of other key words after smallerStr */
        strPtr = strstr(smallerStr, ORG_ID_KEY);
        
        strPtr2 = strstr(smallerStr, DEVICE_TYPE_KEY);
        strPtr = min_char_ptr(strPtr,strPtr2);
        
        strPtr2 = strstr(smallerStr, DEVICE_ID_KEY);
        strPtr = min_char_ptr(strPtr,strPtr2);
        
        strPtr2 = strstr(smallerStr, TOKEN_KEY);
        strPtr = min_char_ptr(strPtr,strPtr2);
        
        if (strPtr != NULL)
        {
          /* split in case a key word is put after the token */
          *(strPtr -1) = '\0';
        }
        
        /* copy the token value in the dev_info struct */
        devInfoPtr = dev_info->token;
        strcpy(devInfoPtr,smallerStr);
        
        /* this pointer shall be set again when a key word is found */
        devInfoPtr = NULL;
        
        /* print the token value */
        msg_debug( "connection string part (token): %s\n", smallerStr );
        
        /* reinit strtok(), force it to point after the token value */
        smallerStr=strtok(smallerStr+strlen(smallerStr)+1, s);
        
        /* the token case has been handled, setting the boolean to false in case some other info are to be parsed after the token */
        is_nextAToken = false;
      }
      else
      {
        smallerStr=strtok(NULL, s);
      }
    }
  } /* while end */
 
  return ret;
}

void connectIoTDev(device_info_t *dev)
{ 
  char reqId[40] = {0};
  
  bool is_quickstart;
  
  if (0 == strcmp(dev->register_mode,QUICK_START_REG_NAME))
  {
    is_quickstart = true;
  }
  else
  {
    is_quickstart = false;
  }
  
  initialize_dm(dev->orgid, dev->url, dev->device_type, dev->device_id, dev->secmode,dev->token,dev->rootCACertString,dev->usecert,
                dev->interCACertString, dev->clientCertString,dev->clientKeyString);
  
  if (connectiotf_dm())
  {
    error("Unable to connect \n");
  }
  
  if (!is_quickstart)
  {
    setCommandHandler_dm(myCallback);
    setManagedHandler_dm(managedCallBack);
    setRebootHandler(rebootCallBack);
    //setFactoryResetHandler(factoryResetCallBack);
    subscribeCommands_dm();
    populateMgmtConfig();
    publishManageEvent(LIFE_TIME_BTWN_MANAGE_DEV_REQ, FIRMWARE_ACTION_SUPPORT, DEVICE_ACTION_SUPPORT, reqId);
  }
  
  /* Info to the user in order to access to the device data via IBM Watson IoT Platform */
  printf("\nYou can see your published data at https://%s.internetofthings.ibmcloud.com\n\r", dev->orgid);
  if (is_quickstart) {
    printf("Device Id is %s\n\n\r", dev->device_id);
  }
  else {
    printf("Sign in. ");
    printf("Open a session with the \"%s\" organization (next to your account name)\n",dev->orgid);
    printf("Select the device dashboard and ");
    printf("click on the \"%s\" device to get its information and access to the \"Reboot\" button\n\n", dev->device_id);
  } 
}
  
/**
 * @brief  Upload data in JSON format to the cloud platform
 * @param  Buffer containing data in JSON format
 * @retval None
 *       
 */
void publishJSONPkt(uint8_t* buffer)
{  
  publishEvent_dm("status", "json", (unsigned char*)buffer, QOS0);
}

/**
  * @brief  Run the Bluemix test. 
  *         The devide credentials including the registration mode are 
  *         retrieved from flash
  * @param  None
  * @retval  0: the connection string in Flash has been parsed succesfully and 
  *             the output structure fields have been set
  *         -1: in case of failure
  */
int bluemix_init(void)
{
  device_info_t device;
  int ret = 0;

  ret = setBluemixDevInfo(&device); 

  if (ret == 0)
  {
    connectIoTDev(&device);
  }

  return ret;
}
