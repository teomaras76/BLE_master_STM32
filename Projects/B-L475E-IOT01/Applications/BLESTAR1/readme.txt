/**
  @page FP-NET-BLESTAR1 functional application for STM32 Nucleo/Discovery Boards
  
  @verbatim
  ******************** (C) COPYRIGHT 2018 STMicroelectronics *******************
  * @file    readme.txt  
  * @version V0.0.2
  * @date    16-07-2018
  * @brief   This application contains an example which shows how to use the WiFi 
             and the BLE devices on the B-L475E-IOT01 board.

  ******************************************************************************
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  @endverbatim

@par Application Description

In this application the B-L475E-IOT01 board acts as a BLE Master connected to 
several (up to seven) BLE Peripherals (nodes) according to a star-topology network.

The BLE Master is able to make connections with three kinds of nodes:
- the ALLMEMS1 node sending environmental sensor data (Pressure, Humidity and Temperature), 
  LED status and Ambient Noise level (see package FP-SNS-ALLMEMS1 and the related 
  documentation on st.com)
- the FLIGHT1 node sending environmental sensor data (Pressure, Humidity and Temperature),
  Ambient Light level and Proximity value (see package FP-SNS-FLIGHT1 and the related
  documentation on st.com)
- the MOTENV1 node sending environmental sensor data (Pressure, Humidity and Temperature), 
  CO Gas Sensor, LED status and Wake Up event (the last only when using X-NUCLEO-IKS01A1 +
  STEVAL-MKI160V1 or X-NUCLEO-IKS01A2 expansion boards) 
  (see package FP-SNS-MOTENV1 and the related documentation on st.com).
  The Wake Up event can be enabled/disabled by setting to 1/0 the #define WUP_EVENT_ENABLED 
  in file ble_master_service.h.
  
All data from peripheral nodes are shown on serial terminal, on the IBM Watson IoT Cloud platform 
and on the ST SensNet app for iOS/Android devices.
  
The CO sensor gas support can be enabled/disabled by setting to 1/0 the #define ENABLE_CO in file 
main.h.
  
WARNING: The CO sensor data will be supported by ST SensNet app V>2.0 (if explicitly reported in the 
release notes).
  
NOTE: The FP-NET-BLESTAR1 V2.1.0 has been aligned to the following package versions:
         - FP-SNS-ALLMEMS1 V3.3.0
         - FP-SNS-FLIGHT1 V3.3.0
         - FP-SNS-MOTENV1 V3.3.0

So the BLE Master reads, periodically, all data values from the connected ALLMEMS1, FLIGHT1 and 
MOTENV1 nodes and receives notifications from these nodes.
The BLE Master is also able to turn LED ON/OFF on the ALLMEMS1 and MOTENV1 nodes.

The BLE Master is also able to connect to a WiFi LAN and to export the data received from 
all nodes to the IBM Watson IoT Cloud platform. 

WARNING: When using the WiFi device mounted on the B-L475E-IOT01 board, the BLE device mounted 
         on the same B-L475E-IOT01 board cannot be used, so the X-NUCLEO-IDB05A1 must be plugged 
		 on top of it. 
		 When using this hardware setup, the B-L475E-IOT01_BLESTAR_IDB05A1 configuration
		 must be used in the IDE projects.
		 If the B-L475E-IOT01_BLESTAR is used, the BlueNRG-MS mounted on the B-L475E-IOT01 is
		 instead used but the WiFi connection is not activated since the WiFi device cannot be used.
         		 
WARNING: X-NUCLEO-IDB05A1 is Arduino compatible with an exception: instead of using pin D13
         for the SPI clock, it uses the pin D3. The default configuration for its library 
		 is having the SPI clock on pin D3.
		 When using the X-NUCLEO-IDB05A1 on top of a B-L475E-IOT01 board, to be fully	 
         Arduino compatible, the X-NUCLEO-IDB05A1 needs a small HW patch. 
         This patch consists in removing zero resistor R4 and instead soldering zero resistor
		 R6. Alternatively, pin D3 and D13 can be bridged.

When the WiFi device is used, this sample application implements also an IBM Bluemix Cloud
IoT client for B-L475E-IOT01 board using the on-board Inventek ISM43362 Wifi module.

2 ways of connecting to the IBM IoT platform are proposed:
* Quickstart registration: no account nor token are required, the connection is
  unsecured.
* Simple registration: an IBM IoT account is required, the device is authenticated 
  by a token, the connection is secured using mbedTLS.
  
The application connects to the IBM BlueMix IoT Cloud platform with the credentials provided 
by the user.

The application is configured by default to run in Quickstart mode for data visualization
only, but can be quickly modified in order to register and control the device in IBM Cloud.
This latter mode requires an account on IBM Watson IoT Cloud platform.

The URL of the web page where sensors data can be visualized is:
https://quickstart.internetofthings.ibmcloud.com/#/device/BLESTAR1_MAC_address/sensor/

At the same time the BLE Master acts also as Slave accepting connection from a BLE Client
(tipically a BLE smartphone/tablet) and sending data from nodes to it.
For easy monitoring and controlling the BLESTAR1 network the STSensNet App for Android/iOS 
can be used. 
This app is available for Android devices on the Google Play Store at 
https://play.google.com/store/apps/details?id=com.st.SensNet 
while for iOS devices on iTunes at 
https://itunes.apple.com/en/app/st-sensnet/id1108449985 
                                        

                                    MOTENV1 #1                  --- BLE connection
                                        /  ALLMEMS1 #2          +++ WIFI connection
                                       /       /       __           
                                      /       /       |  |
       IBM Watson IoT Cloud ++++++++ BLESTAR1 ------- |  | BLE Client
                                          \    ...    |__|
                                           \    ...
                                            \
                                        FLIGHT1 #7
                 
                                        
The BLE nodes firmware for Nucleo-F401RE, Nucleo-L053R8 and Nucleo-L476RG is provided with
the package at Utilities\Node_Firmware\

@par Default Application Settings

This program assumes some default settings in the source code. The default settings
may be changed by the user.

/* Supported peripheral nodes */
...
...
#define MAX_NUM_OF_NODES        7 /* max num of peripheral nodes can be connected to one central device */
(see file Inc\ble_master_service.h)

In folder Utilities\Node_Firmware the firmware for "AM1VXYZ", "FL1VXYZ" and "ME1VXYZ" nodes is
provided (further details are reported in Utilities\Node_Firmware\readme.txt)

Application first launch
  - The board must be connected to a PC through USB (ST-LINK USB port).
    Open the console through serial terminal emulator (e.g. TeraTerm), select the ST-LINK COM port of your
    board and configure it with:
    - 8N1, 115200 bauds, no HW flow control;
    - line endings set to LF.

    - Set the TLS root CA certificates:
      Copy-paste the content of Projects\B-L475E-IOT01\Applications\BLESTAR1\Common\Bluemix\comodo_bluemix.pem.
      The device uses it to authenticate the remote hosts through TLS.

      Note: The sample application requires that a concatenation of 2 CA certificates is provided
          1) for the HTTPS server which is used to retrieve the current time and date at boot time.
             (the "Comodo" certificate).
          2) for the IBM IoT Platform. This one is used with the simple way of registering not with the quickstart way.
  
  - Select the way of registering between "quickstart" and "simple" and enter the connection string as prompted.
  
  - After the parameters are configured, it is possible to change them by restarting the board
    and pushing the User Button (blue button) when prompted.


Application runtime
  - Makes an HTTPS request to retrieve the current time and date, and configures the RTC.

      Note: HTTPS has the advantage over NTP that the server can be authenticated by the board, preventing
            a possible man-in-the-middle attack.
            However, the first time the board is switched on (and each time it is powered down and up, if
			the RTC is not backed up), the verification of the server certificate will fail as its validity
			period will not match the RTC value.
            The following log will be printed to the console: "x509_verify_cert() returned -9984 (-0x2700)"

            If the CLOUD_TIMEDATE_TLS_VERIFICATION_IGNORE switch is defined in cloud.c (it is the case by
			default), this error is ignored and the RTC is updated from the "Date:" field of the HTTP
			response header.
            Else, a few more error log lines are printed, and the application retries to connect without
			verifying the server certificate.
            If/once ok, the RTC is updated.
            
  - Device to cloud connection
    The device connects to the IBM Bluemix platform.
    One a data from a peripheral node arrives, the application publishes such data on the IBM Bluemix
	platform.
      
    The user can see the reported value at the url as indicated in the serial terminal emulator.
    At https://quickstart.internetofthings.ibmcloud.com with quickstart or
	at https://<Organisation Id>.internetofthings.ibmcloud.com with the
	simple registration mode
    The device Id to use is also given in the serial terminal emulator

  - Cloud to device connection
    In simple register mode only.

@par Hardware and Software environment

  - MCU board: B-L475E-IOT01 (MB1297 rev D), with FW "Inventek eS-WiFi ISM43362-M3G-L44-SPI C3.5.2.3.BETA9"
      NOTE: the FW version is displayed on the board console at boot time.
  
  - WiFi access point.
      * with a transparent Internet connectivity: no proxy, no firewall blocking the outgoing traffic
      * running a DHCP server delivering the IP and DNS configuration to the board

  - Computer for running Node-RED application.

  - Bluemix IoT account
      * An IBM Bluemix account is required to use an IBM Watson IoT platform application
        or to use the simple way of registering the device.
        An IBM Bluemix account is not required to use the Quickstart way of registering the device.
        To register, go to this url: https://console.bluemix.net/registration/
        You will receive an email asking to confirm the account creation

  - An IBM Watson IoT platform application
      * This provides a way to interact from the cloud to the device and is not required for the
        device to publish data

  - A development PC for building the application, programming through ST-Link, and running the virtual console.
  
  
@par X-CUBE and Function Pack

This Function Pack uses the following X-CUBE-* and Function Pack SW
 - X-CUBE-BLE1 V3.3.0
 - FP-CLD-WATSON1 V3.0.0

 
@par How to use it ?

In order to make the program work, you must follow these steps:

Device creation (for the simple registering)
  - Follow the steps as indicated at: https://developer.ibm.com/recipes/tutorials/how-to-register-devices-in-ibm-iot-foundation/


Application build and flash
  
  - WARNING: before opening the project with any toolchain be sure your folder
    installation path is not too in-depth since the toolchain may report errors
    after building.

  - Open and build the project with one of the supported development toolchains (see the release note
    for detailed information about the version requirements).

  - Program the firmware on the STM32 board: you can copy (or drag and drop) the
    binary file under Projects\B-L475E-IOT01\Applications\Cloud\Bluemix\Binary
    to the USB mass storage location created when you plug the STM32
    board to your PC. If the host is a Linux PC, the STM32 device can be found in
    the /media folder with the name "DIS_L475". For example, if the created mass
    storage location is "/media/DIS_L475", then the command to program the board
    with a binary file named "my_firmware.bin" is simply: cp my_firmware.bin
    /media/DIS_L475.

  Alternatively, you can program the STM32 board directly through one of the supported development
  toolchains.


For IAR:
 - Open IAR toolchain (this firmware has been successfully tested with Embedded Workbench
   V8.20.1).
 - Open the IAR project file EWARM\Project.eww
 - Rebuild all files and load your image into target memory.
 - Run the example.

For µVision:
 - Open µVision toolchain (this firmware has been successfully tested with MDK-ARM
   Professional Version: 5.24.2).
 - Open the µVision project files MDK-ARM\Project.uvprojx
 - Rebuild all files and load your image into target memory.
 - Run the example.
 
For System Workbench:
 - Open System Workbench for STM32 (this firmware has been successfully tested with System
   Workbench for STM32 Version 2.4.0).
 - Open the SW4STM32 project files SW4STM32\BLESTAR\.project
 - Rebuild all files and load your image into target memory.
 - Run the example.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
