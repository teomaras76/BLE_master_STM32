/**
  @page FP-NET-BLESTAR1 functional application for STM32 Nucleo/Discovery Boards
  
  @verbatim
  ******************** (C) COPYRIGHT 2015 STMicroelectronics *******************
  * @file    readme.txt  
  * @version V0.0.2
  * @date    13-12-2016
  * @brief   This application contains an example which shows how to use the WiFi 
             and the BlueNRG expansion boards on top of a STM32 Nucleo board.

  ******************************************************************************
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  @endverbatim

  
@par Application Description 

In this application the STM32 Nucleo board, equipped with both the WiFi and the BlueNRG
expansion boards, acts as a BLE Master connected to several (up to seven) BLE Peripherals (nodes)
according to a star-topology network.

The BLE Master is able to make connections with three kinds of nodes:
- the ALLMEMS1 node sending environmental sensor data (Pressure, Humidity and Temperature), LED status 
  and Ambient Noise level (see package FP-SNS-ALLMEMS1 and the related documentation on st.com)
- the FLIGHT1 node sending environmental sensor data (Pressure, Humidity and Temperature), Ambient Light
  level and Proximity value (see package FP-SNS-FLIGHT1 and the related documentation on st.com)
- the MOTENV1 node sending environmental sensor data (Pressure, Humidity and Temperature), CO Gas Sensor, LED status 
  and Wake Up event (the last only when using X-NUCLEO-IKS01A1 + STEVAL-MKI160V1 or X-NUCLEO-IKS01A2 expansion boards) 
  (see package FP-SNS-MOTENV1 and the related documentation on st.com).
  The Wake Up event can be enabled/disabled by setting to 1/0 the #define WUP_EVENT_ENABLED in file ble_master_service.h.
  
  All data from peripheral nodes are shown on serial terminal, on the IBM Watson IoT Cloud platform and on the ST SensNet 
  app for iOS/Android devices.
  
  The CO sensor gas support can be enabled/disabled by setting to 1/0 the #define ENABLE_CO in file main.h.
  
WARNING: The CO sensor data will be supported by ST SensNet app V>2.0 (if explicitly reported in the release notes).
  
NOTE: The FP-NET-BLESTAR1 V2.1.0 has been aligned to the following package versions:
         - FP-SNS-ALLMEMS1 V3.3.0
         - FP-SNS-FLIGHT1 V3.3.0
         - FP-SNS-MOTENV1 V3.3.0

So the BLE Master reads, periodically, all data values from the connected ALLMEMS1, FLIGHT1 and MOTENV1
nodes and receives notifications from these nodes.
The BLE Master is also able to turn LED ON/OFF on the ALLMEMS1 and MOTENV1 nodes.

The Ble Master is able to connect to a WiFi LAN and to export the data received from all 
nodes to the IBM Watson IoT Cloud platform. 
The application is configured by default to run in Quickstart mode for data visualization only, 
but can be quickly modified in order to register and control the device in IBM Cloud.
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

It is important to highlight that to act simultaneously as Master and Slave, the X-NUCLEO-IDB05A1
BlueNRG-MS expansion board is required.
The firmware running on the STM32 Nucleo board will check if the X-NUCLEO-IDB05A1 is plugged.
If no X-NUCLEO-IDB05A1 is plugged, a message will be printed on the serial console and the 
STM32 Nucleo board will stop any operation. This stop condition will be signaled by the blinking
LED LD2.
                                           


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

This program assumes also some default settings in the source code which are important
to make the wifi module search, connect and communicate. The default settings 
may be changed by the user in order for him/her to use the program in a correct
manner and to get the desired output. The settings used in this program and which 
need to be changed by the user are (see file Src\main.c):

  - Access-Point to connect, ssid = "STM"
  - security key, seckey = "STMdemoPWD"
  - privacy mode, mode = WPA_Personal

The user may replace the above settings with his/her own specific details directly in the
source code.
Otherwise there is also the option to enter the settings during run-time of the program
(through a serial terminal connection with Baud Rate set to 115200).
The user is asked by the program if he/she wants to enter any new settings for the Wifi.
To enter the new settings, the user must keep the USER button pressed for 5 seconds.
The user is prompted by the program to enter the above described details at the start of the
execution of the program. The user needs to correctly enter his/her wifi settings and host
name details (delete is recognized as an ASCII key). Here is a snapshot of the output of the
program at the outset:

/*******************************************************
*                                                      *
* FP-NET-BLESTAR1 Expansion Software vX.Y.Z            *
*                                                      *
*******************************************************/


***** Starting Wi-Fi module *****

Wi-Fi module detected!
BE SURE THE JUMPERS ARE AT THE RIGHT POSITION ON JP3 and JP4
(see the application README file for more info).

Starting configure procedure for SSID and PWD....
Keep pressed user button within next 5 seconds to set Wi-Fi Access Point parameters (SSID and PWD)
via serial terminal. Otherwise parameters saved to FLASH will be used.
...0
Read parameters from serial terminal.
Enter the SSID:
STM

Enter the password:
STMdemoPWD

Enter the authentication mode(0:Open, 1:WEP, 2:WPA2/WPA2-Personal):
2


SaveSSIDPasswordToMemory OK
AP settings set.
Wi-Fi initialized.
MQTT initialized.
MAC orig: 00:80:E1:B4:F8:C7

IBM Quickstart URL (https://+)
quickstart.internetofthings.ibmcloud.com/#/device/0080E1B4F8C7/sensor/

...............
  
@par Hardware and Software environment

  - This example runs on STM32 Nucleo devices with WiFi (X-NUCLEO-IDW01M1) and BlueNRG
    (X-NUCLEO-IDB05A1) expansion boards.
    
    WARNING: It is required to upgrade the BLE radio stack running on the X-NUCLEO-IDB05A1
             expansion board to the latest version 7.2c. The tool and the instructions on how to upgrade
             the BLE firmware can be found in package X-CUBE-BLE1 (available on st.com) in folder
             Utilities\PC_Software\FlashUpdaterTool.
    
    WARNING: Be sure all your X-NUCLEO-IDB05A1 BlueNRG expansion boards are the ones
             labeled on the back of the SPBTLE-RF module with the MODEL, the FCCID and the IC like
             in the following write:
               MODEL:SPBTLE-RF 2215
               FCCID:59NSPBTLERF
               IC:8976C-SPBTLERF
             These boards fix an issue discovered with high transmission power levels which may occurr
             with the included node firmware (built to transmit at +4dBm power level).
    
    WARNING: On your X-NUCLEO-IDW01M1 Wi-Fi expansion board
             - Jumper on position JP3 (middle and bottom) is required
             - Jumper on position JP4 (middle and left) is required
             - To achieve best results while debugging, it is highly recommended to remove 
               the R21 resistor
        
  - This example has been tested with STMicroelectronics STM32F401RE-Nucleo RevC board and
    can be easily tailored to any other supported device and development board.
    This example runs also on the NUCLEO-F411RE RevC board, even if the chip could be not
    exploited at its best since the projects are configured for the NUCLEO-F401RE target board.
    
    
@par X-CUBE and Function Pack

This Function Pack uses the following X-CUBE-* and Function Pack SW
 - X-CUBE-BLE1 V3.3.0
 - FP-CLD-WATSON1 V3.0.0

 
@par How to use it ? 

WARNING: before opening the project with any toolchain be sure your folder installation
         path is not too in-depth since the toolchain may report errors when building.

This package contains projects for 3 IDEs: IAR, µVision and System Workbench. 
In order to make the  program work, you must do the following:

For IAR:
 - Open IAR toolchain (this firmware has been successfully tested with Embedded Workbench
   V7.80.4).
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
 - Open the SW4STM32 project files SW4STM32\BLESTAR1\.project
 - Rebuild all files and load your image into target memory.
 - Run the example.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
