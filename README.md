# BLE_master_STM32
BLE master FW project for STM32F401 Nucleo board (NUCLEO-F401RE) +  X-Nucleo BLE module board (X-NUCLEO-IDB05A1).
https://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-mpu-eval-tools/stm32-mcu-mpu-eval-tools/stm32-nucleo-boards/nucleo-f401re.html
https://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-connect-hw/x-nucleo-idb05a1.html

The FW projects is based on the FP-NET-BLESTAR1 Function Pack FW project for STM32 (https://www.st.com/content/st_com/en/products/embedded-software/mcu-mpu-embedded-software/stm32-embedded-software/stm32-ode-function-pack-sw/fp-net-blestar1.html), to which few modifications have been applied:
- Wifi connectivity not used
- Support connection to SensorTile.box STEVAL-MSKBOX1V1 (https://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mems-motion-sensor-eval-boards/steval-mksbox1v1.html)
- By default the BLE Master is connecting only to a BLE Slave board with manufacturing code equal to SENSOR_VIB1:
  (manuf_data[3] == SENSOR_VIB1)
  If it's needed to connect the BLE master to other boards (ST BlueCoin, Sensortile, ...) need to uncomment line 1602 of ble_master_service.c file (function "GAP_Start_General_Discovery_CB").
- Support of new BLE service for retrieving AI results data. This can be used both with the FP-AI-SENSING1 functional pack (https://www.st.com/content/st_com/en/products/embedded-software/mcu-mpu-embedded-software/stm32-embedded-software/stm32-ode-function-pack-sw/fp-ai-sensing1.html) or with the MLC project for SensorTile.box FP-SNS-STBOX1 (https://www.st.com/content/st_com/en/products/embedded-software/mcu-mpu-embedded-software/stm32-embedded-software/stm32-ode-function-pack-sw/fp-sns-stbox1.html)
- AI Result data are sent to UART1 serial port of STM32F401 (UART1_TX >> PA9, UART1_RX >> PA10)

It's possible to check all modifications applied to original FP-NET-BLESTAR1 project by searching "Matteo" keyword in the comments to the source code.
