/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    06-July-2018
  * @brief   Main program body.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
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
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rfu.h"
#ifdef USE_BLUENRG_EXT
  #include "stm32l475e_iot01_bluenrg_ext.h"
#else
  #include "stm32l475e_iot01_bluenrg.h"
#endif
#include "stm32_bluenrg_ble.h"
#include "bluenrg_utils.h"
#include "ble_master_service.h"
#include "ble_slave_service.h"

/** @addtogroup Projects
 *  @{
 */
 
/** @addtogroup B-L475E-IOT01
 *  @{
 */

/** @defgroup B-L475E-IOT01_Applications Applications
 *  @{
 */

/** @defgroup B-L475E-IOT01_BLESTAR1 BLESTAR1
 *  @{
 */
 
/** @defgroup B-L475E-IOT01_MAIN MAIN
 *  @{
 */
/* Global variables ---------------------------------------------------------*/
TargetFeatures_t TargetBoardFeatures;
net_hnd_t        hnet; /* Is initialized by cloud_main(). */ 

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static UART_HandleTypeDef console_uart;
volatile uint8_t button_flags = 0;
static volatile int HCI_ProcessEvent = 0;
static uint8_t bdaddr[6];
#ifdef USE_BLUENRG_EXT
  uint8_t wifi_present = TRUE;
#else
  uint8_t wifi_present = FALSE;
#endif
uint8_t notification_freq;
uint8_t wifi_data[256];
volatile uint8_t new_data = 0;
volatile char *data = "Data not available!";
uint8_t json_buffer[384];

/* Imported variables --------------------------------------------------------*/
extern SPI_HandleTypeDef SpiHandle;
extern PeripheralDevices_t perDevs;

/* Imported functions --------------------------------------------------------*/
extern void publishJSONPkt(uint8_t* buffer);

/** @defgroup B-L475E-IOT01_MAIN_Private_Function_Prototypes Private Function Prototypes
 * @{
 */ 
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Console_UART_Init(void);
static void RTC_Init(void);
static void Button_ISR(void);
static void Print_Pack_Info(void);
static void Init_BlueNRG_BLE(void);
static void Init_BlueNRG_Stack(void);
static HAL_StatusTypeDef BlueNRG_SPI_Init(SPI_HandleTypeDef *hspi);
static void BlueNRG_Process(void);
static void NotifyLEDBlink(unsigned int msPeriod);
static void Init_WIFI_Watson(void);
static void WiFi_Process(void);
static void prepareJSONPkt(uint8_t* buffer, char* data);
/**
 * @}
 */

/** @defgroup B-L475E-IOT01_MAIN_Private_Functions Private Functions
 * @{
 */
/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
void main(void)
{
  HAL_Init();
  
  /* Configure the system clock */
  SystemClock_Config();
  
  PeriphClk_Config();
  
  BSP_LED_Init(LED_GREEN);
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* RNG init function */
  TargetBoardFeatures.RngHandle.Instance = RNG;
  if (HAL_RNG_Init(&TargetBoardFeatures.RngHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /* RTC init */
  RTC_Init();
  
  /* UART console init */
  Console_UART_Init();
  
  /* Print fw package information on the serial terminal */
  Print_Pack_Info();
    
  /**
   * Initialize the WiFi device and the connection to the IBM Watson 
   * IoT platform 
   */
  if (wifi_present) {
    Init_WIFI_Watson();
  }
  
  /**
   * Initialize the BlueNRG-MS device and the BLE stack
   */
  Init_BlueNRG_BLE();
  
  /**
   * Infinite loop 
   */
  while(1) 
  {    
    if (perDevs.status!=NOTIFICATIONS_DATA_READ) {    
      /* Handle WiFi processes */
      if (wifi_present) {
        WiFi_Process();        
      }
      else {
        HAL_Delay(1000);  
      }
    }

    /* Handle BlueNRG processes */
    BlueNRG_Process();
  }

}

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Print fw package information on serial terminal
 * @param  None
 * @retval None
 */
static void Print_Pack_Info(void)
{
  const firmware_version_t *fw_version;
  
  rfu_getVersion(&fw_version, false);
  
  STAR_PRINTF("\033[2J\033[0;0H\r\n");
  STAR_PRINTF("**************************************************************************\r\n");
  STAR_PRINTF("***                FP-NET-BLESTAR1 for B-L475E-IOT01A                  ***\r\n");
  STAR_PRINTF("**************************************************************************\n\n\r");
  
  STAR_PRINTF("*** FW version %d.%d.%d - %s, %s ***\n",
              fw_version->major, fw_version->minor, fw_version->patch, fw_version->build_date, 
              fw_version->build_time);  
  
  return;
}

/**
 * @brief  Function for the WiFi and Watson initialization
 * @param  None
 * @retval None
 */
static void Init_WIFI_Watson(void)
{   
  platform_init();  
  
  bluemix_init();
      
  return;
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follows :
 *            System Clock source            = PLL (MSI MSE)
 *            SYSCLK(Hz)                     = 80000000
 *            HCLK(Hz)                       = 80000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 1
 *            APB2 Prescaler                 = 1
 *            MSI Frequency(Hz)              = 48000000
 *            PLL_M                          = 6
 *            PLL_N                          = 20
 *            PLL_R                          = 2
 *            PLL_P                          = 7
 *            PLL_Q                          = 2
 *            Flash Latency(WS)              = 4
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Enable MSI PLL mode */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
 * @brief  Set GREEN LED On 
 * @param  None
 * @retval None
 */
void Led_On(void)
{
  BSP_LED_On(LED_GREEN);
}

/**
 * @brief  Set GREEN LED Off
 * @param  None
 * @retval None
 */
void Led_Off(void)
{
  BSP_LED_Off(LED_GREEN);
}

/**
 * @brief  Toggle LED GREEN
 * @param  None
 * @retval None
 */
void Led_Toggle(void)
{
  BSP_LED_Toggle(LED_GREEN);
}

/**
 * @brief  Update button ISR status 
 * @param  None
 * @retval None
 */
static void Button_ISR(void)
{
  button_flags++;
}

/**
 * @brief  Waiting for button to be pushed 
 * @param  Waiting time
 * @retval None
 */
uint8_t Button_WaitForPush(uint32_t delay)
{
  uint32_t time_out = HAL_GetTick()+delay;
  do
  {
    if (button_flags > 1)  
    {   
      button_flags = 0;
      return BP_MULTIPLE_PUSH;
    }
   
    if (button_flags == 1)  
    {   
      button_flags = 0;
      return BP_SINGLE_PUSH;
    }
  }
  while( HAL_GetTick() < time_out);
  
  return BP_NOT_PUSHED;
}

/**
 * @brief  UART console init function 
 * @param  None
 * @retval None
 */
static void Console_UART_Init(void)
{
  console_uart.Instance = USART1;
  console_uart.Init.BaudRate = 115200;
  console_uart.Init.WordLength = UART_WORDLENGTH_8B;
  console_uart.Init.StopBits = UART_STOPBITS_1;
  console_uart.Init.Parity = UART_PARITY_NONE;
  console_uart.Init.Mode = UART_MODE_TX_RX;
  console_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  console_uart.Init.OverSampling = UART_OVERSAMPLING_16;
#ifdef UART_ONE_BIT_SAMPLE_DISABLE
  console_uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  console_uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
#endif
  BSP_COM_Init(COM1,&console_uart);
}

#if (defined(__GNUC__) && !defined(__CC_ARM))
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */

/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  while (HAL_OK != HAL_UART_Transmit(&console_uart, (uint8_t *) &ch, 1, 30000))
  {
    ;
  }
  return ch;
}

/**
 * @brief  Retargets the C library scanf function to the USART.
 * @param  None
 * @retval None
 */
GETCHAR_PROTOTYPE
{
  /* Place your implementation of fgetc here */
  /* e.g. readwrite a character to the USART2 and Loop until the end of transmission */
  uint8_t ch = 0;
  while (HAL_OK != HAL_UART_Receive(&console_uart, (uint8_t *)&ch, 1, 30000))
  {
    ;
  }
  return ch;
}

/**
 * @brief  RTC init function 
 * @param  None
 * @retval None
 */
static void RTC_Init(void)
{
  RTC_HandleTypeDef *RtcHandle = &TargetBoardFeatures.RtcHandle;  
    /**Initialize RTC Only */
  RtcHandle->Instance = RTC;
  RtcHandle->Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle->Init.AsynchPrediv = 127;
  RtcHandle->Init.SynchPrediv = 255;
  RtcHandle->Init.OutPut = RTC_OUTPUT_DISABLE;
#ifdef RTC_OUTPUT_REMAP_NONE
  RtcHandle->Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
#endif
  RtcHandle->Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle->Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&TargetBoardFeatures.RtcHandle) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief  Function for the BlueNRG-MS device and BLE initialization
 * @param  None
 * @retval None
 */
static void Init_BlueNRG_BLE(void)
{
  STAR_PRINTF("*** BLE connection ***\n\n");  
  /* Initialize SPI */
  if (BlueNRG_SPI_Init(&SpiHandle)!=HAL_OK) {
    STAR_PRINTF("Error Initializing BlueNRG SPI\r\n");
  } else {
    STAR_PRINTF("BlueNRG SPI Initialized\r\n");
  }  

  /* Initialize the BlueNRG */
  Init_BlueNRG_Stack();
}

/**
 * @brief  Initializes the SPI communication with the BlueNRG Shield
 * @param  hspi: SPI Handle
 * @retval None
 */
static HAL_StatusTypeDef BlueNRG_SPI_Init(SPI_HandleTypeDef *hspi)
{
  HAL_StatusTypeDef ret_val    = HAL_OK;
  hspi->Instance               = BNRG_SPI_INSTANCE;
  hspi->Init.Mode              = BNRG_SPI_MODE;
  hspi->Init.Direction         = BNRG_SPI_DIRECTION;
  hspi->Init.DataSize          = BNRG_SPI_DATASIZE;
  hspi->Init.CLKPolarity       = BNRG_SPI_CLKPOLARITY;
  hspi->Init.CLKPhase          = BNRG_SPI_CLKPHASE;
  hspi->Init.NSS               = BNRG_SPI_NSS;
  hspi->Init.FirstBit          = BNRG_SPI_FIRSTBIT;
  hspi->Init.TIMode            = BNRG_SPI_TIMODE;
  hspi->Init.CRCPolynomial     = BNRG_SPI_CRCPOLYNOMIAL;
  hspi->Init.BaudRatePrescaler = BNRG_SPI_BAUDRATEPRESCALER;
  hspi->Init.CRCCalculation    = BNRG_SPI_CRCCALCULATION;

  ret_val = HAL_SPI_Init(hspi);

  return ret_val;
}

/**
 * @brief  This function is used for low level initialization of the SPI 
 *         communication with the BlueNRG Expansion Board.
 * @param  Pointer to the handle of the STM32Cube HAL SPI interface.
 * @retval None
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance==BNRG_SPI_INSTANCE)
  {
    GPIO_InitTypeDef GPIO_InitStruct;
    /* Enable peripherals clock */

    /* Enable GPIO Ports Clock */  
    BNRG_SPI_SCLK_CLK_ENABLE();
    BNRG_SPI_MISO_CLK_ENABLE();
    BNRG_SPI_MOSI_CLK_ENABLE();
    BNRG_SPI_CS_CLK_ENABLE();
    BNRG_SPI_IRQ_CLK_ENABLE();
    BNRG_SPI_RESET_CLK_ENABLE();

    /* Enable SPI clock */
    BNRG_SPI_CLK_ENABLE();
    
    /* Reset */
    GPIO_InitStruct.Pin       = BNRG_SPI_RESET_PIN;
    GPIO_InitStruct.Mode      = BNRG_SPI_RESET_MODE;
    GPIO_InitStruct.Pull      = BNRG_SPI_RESET_PULL;
    GPIO_InitStruct.Speed     = BNRG_SPI_RESET_SPEED;
    GPIO_InitStruct.Alternate = BNRG_SPI_RESET_ALTERNATE;    
    HAL_GPIO_Init(BNRG_SPI_RESET_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(BNRG_SPI_RESET_PORT, BNRG_SPI_RESET_PIN, GPIO_PIN_RESET);	/*Added to avoid spurious interrupt from the BlueNRG */

    /* SCLK */
    GPIO_InitStruct.Pin       = BNRG_SPI_SCLK_PIN;
    GPIO_InitStruct.Mode      = BNRG_SPI_SCLK_MODE;
    GPIO_InitStruct.Pull      = BNRG_SPI_SCLK_PULL;
    GPIO_InitStruct.Speed     = BNRG_SPI_SCLK_SPEED;
    GPIO_InitStruct.Alternate = BNRG_SPI_SCLK_ALTERNATE;
    HAL_GPIO_Init(BNRG_SPI_SCLK_PORT, &GPIO_InitStruct); 

    /* MISO */
    GPIO_InitStruct.Pin       = BNRG_SPI_MISO_PIN;
    GPIO_InitStruct.Mode      = BNRG_SPI_MISO_MODE;
    GPIO_InitStruct.Pull      = BNRG_SPI_MISO_PULL;
    GPIO_InitStruct.Speed     = BNRG_SPI_MISO_SPEED;
    GPIO_InitStruct.Alternate = BNRG_SPI_MISO_ALTERNATE;
    HAL_GPIO_Init(BNRG_SPI_MISO_PORT, &GPIO_InitStruct);

    /* MOSI */
    GPIO_InitStruct.Pin       = BNRG_SPI_MOSI_PIN;
    GPIO_InitStruct.Mode      = BNRG_SPI_MOSI_MODE;
    GPIO_InitStruct.Pull      = BNRG_SPI_MOSI_PULL;
    GPIO_InitStruct.Speed     = BNRG_SPI_MOSI_SPEED;
    GPIO_InitStruct.Alternate = BNRG_SPI_MOSI_ALTERNATE;
    HAL_GPIO_Init(BNRG_SPI_MOSI_PORT, &GPIO_InitStruct);

    /* NSS/CSN/CS */
    GPIO_InitStruct.Pin       = BNRG_SPI_CS_PIN;
    GPIO_InitStruct.Mode      = BNRG_SPI_CS_MODE;
    GPIO_InitStruct.Pull      = BNRG_SPI_CS_PULL;
    GPIO_InitStruct.Speed     = BNRG_SPI_CS_SPEED;
    GPIO_InitStruct.Alternate = BNRG_SPI_CS_ALTERNATE;
    HAL_GPIO_Init(BNRG_SPI_CS_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_SET);

    /* IRQ -- INPUT */
    GPIO_InitStruct.Pin       = BNRG_SPI_IRQ_PIN;
    GPIO_InitStruct.Mode      = BNRG_SPI_IRQ_MODE;
    GPIO_InitStruct.Pull      = BNRG_SPI_IRQ_PULL;
    GPIO_InitStruct.Speed     = BNRG_SPI_IRQ_SPEED;
    GPIO_InitStruct.Alternate = BNRG_SPI_IRQ_ALTERNATE;
    HAL_GPIO_Init(BNRG_SPI_IRQ_PORT, &GPIO_InitStruct);

    /* Configure the NVIC for SPI */
    HAL_NVIC_SetPriority(BNRG_SPI_EXTI_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(BNRG_SPI_EXTI_IRQn);
  }
  return;
}

/** 
 * @brief  Initialize the BlueNRG Stack
 * @param  None
 * @retval None
 */
static void Init_BlueNRG_Stack(void)
{
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  
  uint8_t  hwVersion;
  uint16_t fwVersion;
  uint8_t  role;
  
  int ret;
 
  /* Initialize the BlueNRG HCI */
  HCI_Init();
  
  /* Reset BlueNRG hardware */
  BlueNRG_RST();
  
  /* Get the BlueNRG HW and FW versions */  
  BlueNRG_GetVersion(&hwVersion, &fwVersion);
     
  /* 
   * Reset BlueNRG again otherwise we are not
   * be able to change its MAC address.
   * aci_hal_write_config_data() must be the first
   * command after reset otherwise it will fail.
   */
  BlueNRG_RST();
    
  /* Set the BlueNRG-MS in master mode */
  role = MASTER_MODE;
  ret = aci_hal_write_config_data(CONFIG_DATA_MODE_OFFSET, 0x01, &role);
  if (ret) {
    STAR_PRINTF("Setting Mode %d failed 0x%02x.\n", role, ret);
  }

#ifndef MAC_STAR
  /* Create a Unique BLE MAC */
  Set_Random_Address(bdaddr, hwVersion, fwVersion);
#else /* MAC_STAR */
  {
    uint8_t tmp_bdaddr[6] = {MAC_STAR};
    int32_t i;
    for (i=0; i<6; i++) {
      bdaddr[i] = tmp_bdaddr[i];
    }
  }
#endif /* MAC_STAR */
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
                                  CONFIG_DATA_PUBADDR_LEN,
                                  bdaddr);

  if (ret) {
    STAR_PRINTF("\r\nSetting Pubblic BD_ADDR failed\r\n");
    goto fail;
  }
  
  ret = aci_gatt_init();    
  if (ret) {
    STAR_PRINTF("\r\nGATT_Init failed\r\n");
    goto fail;
  }
  
  ret = aci_gap_init_IDB05A1(GAP_CENTRAL_ROLE_IDB05A1|GAP_PERIPHERAL_ROLE_IDB05A1, 
                             0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  
  if (ret != BLE_STATUS_SUCCESS) {
    STAR_PRINTF("\r\nGAP_Init failed\r\n");
    goto fail;
  }
  
  Add_All_Services();

  /* Set output power level */
  //ret = aci_hal_set_tx_power_level(1,6); /* +4 dBm  */
  //ret = aci_hal_set_tx_power_level(1,5); /* +2 dBm  */
  //ret = aci_hal_set_tx_power_level(0,6); /*  0 dBm  */
  ret = aci_hal_set_tx_power_level(1,4); /* -2 dBm  */
  
  /**
   * 5 sec is the suggested timeout value when the WiFi is enabled.
   * This is because the data upload to the Cloud slows down the system,
   * causing some losses (e.g. the loss of the command from the Master which  
   * enable/disable the Mic/Prox notifications).
   * If the WiFi is disabled the timeout can be set to a lower value (1 sec).
   */
  if (wifi_present) {
    notification_freq = NOTIFICATION_FREQ_WIFI_ON;
  }
  else {
    notification_freq = NOTIFICATION_FREQ_WIFI_OFF;
  }
  
  Init_Processes();

  return;
  
fail:
  return;
}

/**
 * @brief  Start the BlueNRG connections, characteristics discovering and enable
 *         notifications
 * @param  None
 * @retval None
 */
static void BlueNRG_Process(void)
{ 
  if (HCI_ProcessEvent) {  
    HCI_Process();
    HCI_ProcessEvent = 0;
  }
  Connection_Process();
  Reading_Process();
} 

/**
 * @brief  Upload data in JSON format on the IBM Watson IoT Cloud Platform
 * @param  None
 * @retval None
 */
static void WiFi_Process(void)
{
  uint32_t time_out = HAL_GetTick() + 1000;
    
  if (new_data) {    
    prepareJSONPkt(json_buffer, (char*)data);
    publishJSONPkt(json_buffer);
    new_data = 0;
    NotifyLEDBlink(250);
  }

  while (HAL_GetTick() < time_out);
}

/**
 * @brief  Prepare JSON packet with data from peripheral nodes
 * @param  Buffer that will contain data in JSON format
 * @param  Data from peripheral nodes
 * @retval None
 */
static void prepareJSONPkt(uint8_t* buffer, char* data)
{  
  char tempbuff[256];

  strcpy((char *)buffer, "{\"d\":{\"Name\":\"ST BLESTAR\"");     
  sprintf(tempbuff, ",%s", data);
  strcat((char *)buffer, tempbuff); 
  strcat((char *)buffer, "}}");
  
  return;
}

/**
  * @brief  Turn on notificaion LED (LED2)
  * @param  msPeriod time delay in milli seconds
  * @retval None
  */
static void NotifyLEDBlink(unsigned int msPeriod) 
{
  BSP_LED_Off(LED2);
  HAL_Delay(msPeriod);
  BSP_LED_On(LED2);
}

/**
 * @brief  EXTI line detection callback.
 * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin)
  {
  case (USER_BUTTON_PIN):
    Button_ISR();
    break;
  case (BNRG_SPI_IRQ_PIN):
    HCI_Isr();
    HCI_ProcessEvent=1;
    break;      
  default:    
    break;    
  }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void Error_Handler(void)
{
  while(1)
  {
    BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(200);
  }
}
/**
 * @}
 */
 
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
