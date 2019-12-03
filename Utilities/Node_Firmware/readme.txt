/**
  @page Nodes Firmware for FP-NET-BLESTAR1 Applications
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    readme.txt  
  * @version V2.0.0
  * @date    25-11-2016
  * @brief   This folder contains nodes firmware that can be used by the user
			 while running some of the applications contained within the
			 FP-NET-BLESTAR1 package.

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

-----------
readme.txt
-----------
This readme.txt file describes the contents of this folder.

---------
Contents
---------
ALLMEMS1
  - ALLMEMS1_IKS01A1_F401RE_BL.bin
  - ALLMEMS1_IKS01A1_L476RG_BL.bin
  - ALLMEMS1_IKS01A2_F401RE_BL.bin
  - ALLMEMS1_IKS01A2_L476RG_BL.bin
  - ALLMEMS1_L476RG_SensorTile_BL.bin
  Node firmware for STM32 SensorTile and for STM32 Nucleo-F401RE and Nucleo-L476RG equipped with 
  the following expansion boards:
  - X-NUCLEO-IDB05A1 (for Bluetooth Low Energy connectivity)
  - X-NUCLEO-IKS01A1 or X-NUCLEO-IKS01A2 (for MEMS and Environmental sensors)
  - X-NUCLEO-CCA02M1 (for MEMS microphone).  
  For further details please refers to the Function Pack FP-SNS-ALLMEMS1 freely
  available at www.st.com.
  "AM1VXYZ" is the name used by the node on the BLE network.
FLIGHT1
  - FLIGHT1_53L0A1_F401RE_BL.bin
  - FLIGHT1_53L0A1_L476RG_BL.bin
  - FLIGHT1_6180XA1_F401RE_BL.bin
  - FLIGHT1_6180XA1_L476RG_BL.bin
  Node firmware for STM32 Nucleo-F401RE and Nucleo-L476RG equipped with the following
  expansion boards:
  - X-NUCLEO-IDB05A1 (for Bluetooth Low Energy connectivity)
  - X-NUCLEO-IKS01A1 or X-NUCLEO-IKS01A2 (for MEMS and Environmental sensors)
  - X-NUCLEO-53L0A1 or X-NUCLEO-6180XA1 (for Proximity and Ambient Light sensor).
  For further details please refers to the Function Pack FP-SNS-FLIGHT1 freely
  available at www.st.com.  
  "FL1VXYZ" is the name used by the node on the BLE network.  
MOTENV1
  - MOTENV1_F401RE_BL.bin
  - MOTENV1_IKS01A1_L053R8.bin
  - MOTENV1_IKS01A2_L053R8.bin
  - MOTENV1_L476RG_BL.bin
  - MOTENV1_L476RG_SensorTile_BL.bin
  Node firmware for STM32 SensorTile and for STM32 Nucleo-F401RE, Nucleo-L053R8 and Nucleo-L476RG equipped with the following expansion boards:
  - X-NUCLEO-IDB05A1 (for Bluetooth Low Energy connectivity)
  - X-NUCLEO-IKS01A1 or X-NUCLEO-IKS01A2 (for MEMS and Environmental sensors)
  - STEVAL-MKI160V1 (for MEMS) -- to be used only with X-NUCLEO-IKS01A1 --
  For further details please refers to the Function Pack FP-SNS-MOTENV1 freely
  available at www.st.com.
  "ME1VXYZ" is the name used by the node on the BLE network.   
    
NOTE
  All the binary files for the NUCLEO-F401RE run also on the NUCLEO-F411RE RevC board,
  even if the chip could be not exploited at its best since the projects are
  configured for the NUCLEO-F401RE target board.
  
WARNING
  Be sure all your X-NUCLEO-IDB05A1 BlueNRG expansion boards are the ones
  labeled on the back of the SPBTLE-RF module with the MODEL, the FCCID and the
  IC like in the following write:
  MODEL:SPBTLE-RF 2215
  FCCID:59NSPBTLERF
  IC:8976C-SPBTLERF
  These boards fix an issue discovered with high transmission power levels which may 
  occurr with the included node firmware (built to transmit at +4dBm power level).

------------
Description
------------
After flashing one of these firmware, the board will act as Server-Peripheral.

After establishing the BLE connection with the Master running the BLESTAR1
application in folder Projects\Multi\Applications\BLESTAR1:
 - a ALLMEMS1 node will send the environmental sensors values (temperature, humidity and pressure) 
   from the X-NUCLEO-IKS01A1/2 expansion board and the noise level (in dB) from the X-NUCLEO-CCA02M1.
   This node will export also the status of the LED LD2 on the Nucleo board.
 - a FLIGHT1 node will send the environmental sensors values (temperature, humidity and pressure) 
   from the X-NUCLEO-IKS01A1/2 expansion board and ambient light level (in LUX) or proximity (in mm)
   value from the X-NUCLEO-6180XA1/53L0A1.
 - a MOTENV1 node will send the environmental sensors values (temperature, humidity and pressure)
   and the Wake Up event (detected when the node is moved) from the X-NUCLEO-IKS01A1/2 expansion board.
   By using the X-NUCLEO-IKS01A1, the STEVAL-MKI160V1 must be also used to detect the Wake Up event.
   This node will export also the status of the LED LD2 on the Nucleo board.
 
By means of the app for Android devices included in folder Utilities\Android_Software, all the 
data can be monitored and the LEDs can be turned ON/OFF.
  
-----------
Known-Bugs
-----------
- None

* <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
