/**
  ******************************************************************************
  * @file    fft.c
  * @author  Martin Polacek
  * @version V4.2.0
  * @date    13-October-2017
  * @brief   FFT demo driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fft.h"
#include <string.h> /* strlen */
#include <stdio.h>  /* snprintf */
#include <math.h>   /* trunc */
#include <stdlib.h> /* atoi */

#include "flat_top_win.h"
#include "sensor_def.h"
void Error_Handler(void);

/* Do not change following defines */
#define MAX_SAMPLES             1024
#define HEADER_LENGTH             82
#define NUM_SENSORS                5

/* Typedefs ------------------------------------------------------------------*/
typedef enum
{
  MAG_200mg = 0,
  MAG_500mg,
  MAG_1g,
  MAG_2g,
  MAG_4g,
  MAG_8g,
  MAG_16g
} magnitudeVal_t;

typedef enum
{
  MENU_MAIN = 0,
  MENU_ODR,
  MENU_MAG,
  MENU_AXIS,
  MENU_FS,
  MENU_OPMODE,
  MENU_SAMPLES,
  MENU_HP_FILTER,
  MENU_PLOT_GRAPH,
  MENU_PLOT_TABLE,
  NUM_MENU,
  MENU_SENSORS
} menu_t;

/* Main menu status */
typedef enum
{
  NONE,
  ODR_SET,
  ODR_SET_ERR,
  MAG_SET,
  AXIS_SET,
  AXIS_SET_ERR,
  FS_SET,
  FS_SET_ERR,
  SAMPLES_SET,
  OPMODE_SET,
  OPMODE_SET_ERR,
  HP_SWITCHED,
  HP_SWITCHED_ERR,
  HP2DC_NULL_SUBST,
  FIRST_ODR_MEAS,
  SENSOR_CHNGD
} MM_status_t;

typedef struct
{
  uint16_t       magnitude;
  uint16_t       samples;
  uint8_t        hp_filter;
  uint8_t        switch_HP_to_DC_null;

} fft_settings_t;

typedef struct
{
  float32_t      bin_max_value;
  uint32_t       bin_max_index;
  float32_t      ftw_max_value;
  uint32_t       ftw_max_index;
  float32_t      max_value;
  uint32_t       max_index;
} fft_out_data_t;

/* Extern variables ----------------------------------------------------------*/
extern char dataOut[256];
extern UART_HandleTypeDef UartHandle;
extern void *ACCELERO_handle;
extern volatile uint32_t Int_Current_Time1;
extern volatile uint32_t Int_Current_Time2;
extern volatile uint8_t AXL_INT_received;


/* Private macros ------------------------------------------------------------*/
#define DISP(...) snprintf(dataOut, sizeof(dataOut), __VA_ARGS__); \
                  HAL_UART_Transmit(&UartHandle, (uint8_t*)dataOut, strlen(dataOut), 5000)


/* Private variables ---------------------------------------------------------*/
float32_t        fft_input[MAX_SAMPLES];
float32_t        fft_output[MAX_SAMPLES];
float32_t        fft_text_plot_data[51];
float32_t        fft_tmp[MAX_SAMPLES];
float32_t        fft_tmp2[MAX_SAMPLES];
int16_t          tmp_array[MAX_SAMPLES];

const uint16_t mag_values_list[]=
{
  [MAG_200mg]    = 200,
  [MAG_500mg]    = 500,
  [MAG_1g]       = 1000,
  [MAG_2g]       = 2000,
  [MAG_4g]       = 4000,
  [MAG_8g]       = 8000,
  [MAG_16g]      = 16000
};

uint8_t menu_list[NUM_MENU];
uint8_t sensor_list[NUM_SENSORS];

arm_rfft_fast_instance_f32 real_fft_Instance;

fft_out_data_t   out_data = {.max_index = 0, .max_value = 0, .bin_max_index = 0, .ftw_max_index = 0, .ftw_max_value = 0};
fft_settings_t   settings = {.magnitude = 1000, .hp_filter = 0, .switch_HP_to_DC_null = 0, .samples = 512 };
float            odr_measured = 0; /* Calculated value of ODR (for ODR calibration) */
MM_status_t      main_menu_status = NONE;
ProgFun          prog_function;
uint16_t         freq_axis[50];
uint8_t          daq_enable;
uint8_t          init_menu;
uint8_t          instance;
uint8_t          table_displayed = 1;
uint8_t          unicleo_in_use = 0;
volatile uint8_t data_collect_EN = 0;
volatile uint8_t fft_data_rdy = 0;


/* Private function prototypes -----------------------------------------------*/
static uint8_t   acquire_data( void );
static uint8_t   disable_DRDY( void );
static uint8_t   disable_FIFO( void );
static uint8_t   enable_DRDY( void );
static uint8_t   enable_FIFO( void );
static uint8_t   get_available_sensors( void );
static uint8_t   get_pos_in_menu_list( menu_t item );
static uint8_t   restart_FIFO( void );
static void      calculate_freq_axis( void );
static void      clear_screen( void );
static void      calculate_fft( void );
static void      En_Dis_HP_or_DCnull( void );
static void      fft_plot_creator( int16_t n, char *c );
static void      HP_DC_changer( void );
static void      plot_fft( void );
static void      plot_table( void );
static void      print_menu( menu_t menu );
static void      print_menu_header( menu_t menu );

/* Menu functions */
static void      term_menu_handler( void );
static uint8_t   main_menu( uint8_t rx_buffer, menu_t *menu );
static uint8_t   odr_menu( uint8_t rx_buffer );
static uint8_t   magnitude_menu( uint8_t rx_buffer );
static uint8_t   axis_menu( uint8_t rx_buffer );
static uint8_t   full_scale_menu( uint8_t rx_buffer );
static uint8_t   samples_menu( uint8_t rx_buffer );
static uint8_t   opmode_menu( uint8_t rx_buffer );
static uint8_t   sensor_menu( uint8_t rx_buffer );

/* ODR measurement functions */
static uint8_t  meas_odr( void );


/* Public function prototypes ------------------------------------------------*/
uint8_t          init_fft( ProgFun func );
uint8_t          reset_INT( void );
void             fft_main( void );

float            *get_fft_data( void );
uint8_t          prepare_fft_data( void );

uint16_t         get_samples( void );
uint32_t         get_fft_max_freq( void );
uint32_t         get_fft_max_freq_amp( void );
uint32_t         get_meas_odr( void );
uint8_t          get_hp_filter( void );
uint8_t *        get_sensor_list( void );
void             get_fft_msg( TMsg *Msg );

uint8_t          set_active_axis( uint8_t value );
uint8_t          set_full_scale( uint8_t value );
uint8_t          set_hp_filter( uint8_t value );
uint8_t          set_odr( uint8_t value );
uint8_t          set_opmode( uint8_t value );
uint8_t          set_samples( uint8_t value );

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initialize FFT demo
 *
 * @param  func Program functionality selector
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
uint8_t init_fft( ProgFun func )
{
  if(BSP_ACCELERO_Get_Instance(ACCELERO_handle, &instance) != COMPONENT_OK)
  {
    return 1;
  }

  prog_function = func;

  if( sensor_setting[instance].hp_filter_available )
  {
    if(BSP_ACCELERO_Disable_HP_Filter_Ext( ACCELERO_handle ) != COMPONENT_OK)
    {
      return 1;
    }
  }
  if(BSP_ACCELERO_Set_Active_Axis_Ext( ACCELERO_handle, Z_AXIS ) != COMPONENT_OK)
  {
    return 1;
  }
  /* Set ODR to 400Hz for FFT demo */
  if(BSP_ACCELERO_Set_ODR_Value( ACCELERO_handle, 400.0f ) != COMPONENT_OK)
  {
    return 1;
  }

  /* Turn-on time delay */
  HAL_Delay(40);

  if( sensor_setting[instance].opmode_list != NULL ) /* NULL = opmode not supported */
  {
    if( BSP_ACCELERO_Set_OpMode_Ext( ACCELERO_handle, NORMAL_MODE ) != COMPONENT_OK )
    {
      return 1;
    }
  }

  if(enable_DRDY())
  {
    return 1;
  }

  settings.switch_HP_to_DC_null = 0;
  settings.samples = 512;
  daq_enable = 1;
  table_displayed = 1;
  fft_data_rdy = 0;
  settings.hp_filter = 0;
  init_menu = 1;

  arm_rfft_fast_init_f32(&real_fft_Instance, settings.samples);

  if( instance == LSM6DSL_X_0 || instance == LSM6DSL_X_1 )
  {
    /* Enable AXL data to FIFO with no decimation */
    if(BSP_ACCELERO_FIFO_Set_Decimation_Ext( ACCELERO_handle, 0x01 ) != COMPONENT_OK)
    {
      return 1;
    }
    /* Set FIFO ODR */
    if(BSP_ACCELERO_FIFO_Set_ODR_Value_Ext( ACCELERO_handle, 6660 ) != COMPONENT_OK)
    {
      return 1;
    }

    /* Set FIFO watermark level */
    if(BSP_ACCELERO_FIFO_Set_Watermark_Level_Ext( ACCELERO_handle, (512+1)*3 ) != COMPONENT_OK) // 512 (16 bit) values, 3 axes
    {
      return 1;
    }
    /* Set FIFO to stop on FIFO threshold */
    if(BSP_ACCELERO_FIFO_Set_Stop_On_Fth_Ext( ACCELERO_handle, 0x80 ) != COMPONENT_OK)
    {
      return 1;
    }
  }

  if( !sensor_setting[instance].hp_filter_available )
  {
    HP_DC_changer();
  }

  /* Measeure and calculate ODR */
  if(meas_odr())
  {
    return 1;
  }

  main_menu_status = FIRST_ODR_MEAS;

  /* Calculate values for graph x axis */
  calculate_freq_axis();

  switch( func )
  {
  case TERMINAL:
    {
      uint8_t i = 1;
      unicleo_in_use = 0;
      settings.magnitude = 1000;

      if( sensor_setting[instance].odr_list != NULL )
      {
        menu_list[i] = MENU_ODR;
        i++;
      }
      menu_list[i] = MENU_MAG;
      i++;
      if( sensor_setting[instance].axis_list != NULL )
      {
        menu_list[i] = MENU_AXIS;
        i++;
      }
      if( sensor_setting[instance].fs_list != NULL )
      {
        menu_list[i] = MENU_FS;
        i++;
      }
      if( sensor_setting[instance].opmode_list != NULL )
      {
        menu_list[i] = MENU_OPMODE;
        i++;
      }
      menu_list[i] = MENU_SAMPLES;
      i++;
      menu_list[i] = MENU_HP_FILTER;
      i++;
      menu_list[i] = MENU_PLOT_GRAPH;
      i++;
      menu_list[i] = MENU_PLOT_TABLE;
      menu_list[0] = i;

    } break;

  case UNICLEO:
    {
      unicleo_in_use = 1;
    } break;

  default:
    return 1;
  }

  return 0;
}


/**
 * @brief  Main function of FFT demo library
 *
 * @param  None
 * @retval None
 */
void fft_main(void)
{
  if( !unicleo_in_use )
  {
    term_menu_handler();
  }

  if( !fft_data_rdy && data_collect_EN )
  {
    if( daq_enable )
    {
      daq_enable = acquire_data();
    } else
    {
      daq_enable = 1;
      calculate_fft();
    }
  }
}


/**
 * @brief  Handle pressed button in terminal and display proper menu
 *
 * @param  None
 * @retval None
 */
static void term_menu_handler( void )
{
  uint8_t rx_buffer = '\0';
  static menu_t menu = MENU_MAIN;
  static uint8_t menu_status = 0;

  if(init_menu)
  {
    init_menu = 0;
    menu = MENU_MAIN;
    menu_status = 0;
  }

  HAL_UART_Receive(&UartHandle, &rx_buffer, 1, 1);

  if( menu_status == 0 )
  {
    if( menu != MENU_PLOT_GRAPH && menu != MENU_PLOT_TABLE )
    {
      print_menu( menu );
      fflush(stdout);
    }
  }

  switch( menu )
  {
  case MENU_MAIN:
    menu_status = main_menu( rx_buffer, &menu );
    break;

  case MENU_ODR:
    menu_status = odr_menu( rx_buffer );
    if( menu_status == 0 )
    {
      menu = MENU_MAIN;
    }
    break;

  case MENU_MAG:
    menu_status = magnitude_menu( rx_buffer );
    if( menu_status == 0 )
    {
      menu = MENU_MAIN;
    }
    break;

  case MENU_AXIS:
    menu_status = axis_menu( rx_buffer );
    if( menu_status == 0 )
    {
      menu = MENU_MAIN;
    }
    break;

  case MENU_FS:
    menu_status = full_scale_menu( rx_buffer );
    if( menu_status == 0 )
    {
      menu = MENU_MAIN;
    }
    break;

  case MENU_OPMODE:
    menu_status = opmode_menu( rx_buffer );
    if( menu_status == 0 )
    {
      menu = MENU_MAIN;
    }
    break;

  case MENU_SAMPLES:
    menu_status = samples_menu( rx_buffer );
    if( menu_status == 0 )
    {
      menu = MENU_MAIN;
    }
    break;

  case MENU_PLOT_GRAPH:
    if( fft_data_rdy )
    {
      plot_fft();
      fflush(stdout);
      fft_data_rdy = 0;
    }
    break;

  case MENU_PLOT_TABLE:
    if( fft_data_rdy && !table_displayed  )
    {
      plot_table();
      fflush(stdout);
      fft_data_rdy = 0;
    }
    break;

  case MENU_SENSORS:
    menu_status = sensor_menu( rx_buffer );
    if( menu_status == 0 )
    {
      menu = MENU_MAIN;
    }
    break;

  default:
    break;
  }

  /* Go to main menu after "m" is pressed */
  if( menu != MENU_MAIN && (rx_buffer == 'm' || rx_buffer == 'M') )
  {
    init_menu = 1;
    main_menu_status = NONE;
    data_collect_EN = 0;
  }

  if( rx_buffer == 's' || rx_buffer == 'S' )
  {
    if( get_available_sensors() > 0 )
    {
      menu = MENU_SENSORS;
      menu_status = 0;
    }
  }
}


/**
 * @brief  Main menu handler

 * Main menu include two status bars:
 * 1) top status bar, that shows actual settings
 * 2) bottom status bar, that inform user about recently done changes
 *
 * @param  rx_buffer value received from UART
 * @param  menu actual menu
 * @retval 0 in case of success
 * @retval 1 in case of failure
 * @retval 2 when reset_INT function failed
 */
static uint8_t main_menu( uint8_t rx_buffer, menu_t *menu )
{
  uint8_t ret_val = 1;
  uint8_t tmp = atoi((const char*)&rx_buffer);

  if( rx_buffer == 'h' || rx_buffer == 'H' )
  {
    if(sensor_setting[instance].hp_filter_available)
    {
      HP_DC_changer();
      ret_val = 0;
    }
  } else
  {
    /* Set chosen menu */
    if( (tmp <= menu_list[0]) && tmp != 0 )
    {
      if( menu_list[tmp] == MENU_HP_FILTER )
      {
        En_Dis_HP_or_DCnull();
      } else
      {
        *menu = (menu_t)menu_list[tmp];

        if( menu_list[tmp] == MENU_PLOT_GRAPH || menu_list[tmp] == MENU_PLOT_TABLE )
        {
          if(reset_INT())
          {
            return 2;
          }

          data_collect_EN = 1;
          daq_enable = 1;
          table_displayed = 0;
          fft_data_rdy = 0;
        }
      }
      ret_val = 0;
    }
  }

  return ret_val;
}


/**
 * @brief  ODR menu handler
 *
 * User can set multiple values that depends on sensor in use.
 *
 * ODR menu include top status bar, that shows actually set value
 *
 * @param  rx_buffer value received from UART
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
static uint8_t odr_menu( uint8_t rx_buffer )
{
  uint8_t ret_val = 1;
  uint8_t tmp = atoi((const char*)&rx_buffer);

  /* Set chosen ODR */
  if( (tmp <= sensor_setting[instance].odr_list[0]) && tmp != 0 )
  {
    if( instance == LSM303AGR_X_0 || instance == LIS2DH12_0 )
    {
      OP_MODE_t operatingMode;
      if( BSP_ACCELERO_Get_OpMode_Ext( ACCELERO_handle, &operatingMode ) != COMPONENT_OK )
      {
        main_menu_status = ODR_SET_ERR;
      } else
      {
        if( (operatingMode == LOW_PWR_MODE && tmp != 3) || (operatingMode != LOW_PWR_MODE && tmp != 4 && tmp != 5))
        {
          if( BSP_ACCELERO_Set_ODR_Value(ACCELERO_handle, sensor_setting[instance].odr_list[tmp]) != COMPONENT_OK )
          {
            main_menu_status = ODR_SET_ERR;
          } else
          {
            meas_odr();
            daq_enable = 1;
            main_menu_status = ODR_SET;
          }
          ret_val = 0;
        }
      }
    } else
    {
      if( BSP_ACCELERO_Set_ODR_Value(ACCELERO_handle, sensor_setting[instance].odr_list[tmp]) != COMPONENT_OK )
      {
        main_menu_status = ODR_SET_ERR;
      } else
      {
        meas_odr();
        daq_enable = 1;
        main_menu_status = ODR_SET;
      }
      ret_val = 0;
    }
    if(settings.hp_filter)
    {
      HAL_Delay((uint32_t)(320/odr_measured));
      reset_INT();
    }
  }

  return ret_val;
}

/**
 * @brief  Magnitude menu handler
 *
 * User can set multiple values that depends on sensor in use.
 *
 * Magnitude menu include top status bar, that shows actually set value
 *
 * @param  rx_buffer value received from UART
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
static uint8_t magnitude_menu( uint8_t rx_buffer )
{
  uint8_t ret_val = 1;
  uint8_t tmp = atoi((const char*)&rx_buffer);

  /* Set chosen graph magnitude */
  if( tmp <= 7 && tmp != 0 )
  {
    settings.magnitude = mag_values_list[tmp-1];
    main_menu_status = MAG_SET;
    ret_val = 0;
  }

  return ret_val;
}

/**
 * @brief  Sensing axis menu handler
 *
 * User can set these values:
 * 1) X axis
 * 2) Y axis
 * 3) Z axis
 *
 * Sensing axis menu include top status bar, that shows actually set value
 *
 * @param  rx_buffer value received from UART
 * @retval 0 in case of success
 * @retval 1 in case of failure
 * @retval 2 when reset_INT function failed
 */
static uint8_t axis_menu( uint8_t rx_buffer )
{
  uint8_t ret_val = 1;
  uint8_t tmp = atoi((const char*)&rx_buffer);

  /* Set chosen sensing axis */
  if( tmp <= 3 && tmp != 0 )
  {
    daq_enable = 1;

    if(BSP_ACCELERO_Set_Active_Axis_Ext( ACCELERO_handle, sensor_setting[instance].axis_list[tmp-1] ) != COMPONENT_OK)
    {
      main_menu_status = AXIS_SET_ERR;
    } else
    {
      main_menu_status = AXIS_SET;
    }

    if(reset_INT())
    {
      return 2;
    }

    ret_val = 0;
  }

  return ret_val;
}


/**
 * @brief  Full scale menu handler
 *
 * User can set multiple values that depends on sensor in use.
 *
 * Full scale menu include top status bar, that shows actually set value
 *
 * @param  rx_buffer value received from UART
 * @retval 0 in case of success
 * @retval 1 in case of failure
 * @retval 2 when reset_INT function failed
 */
static uint8_t full_scale_menu( uint8_t rx_buffer )
{
  uint8_t ret_val = 1;
  uint8_t tmp = atoi((const char*)&rx_buffer);

  /* Set chosen AXL Full scale */
  if( tmp <= sensor_setting[instance].fs_list[0] && tmp != 0 )
  {
    daq_enable = 1;

    if(BSP_ACCELERO_Set_FS_Value( ACCELERO_handle, sensor_setting[instance].fs_list[tmp] ) != COMPONENT_OK)
    {
      main_menu_status = FS_SET_ERR;
    } else
    {
      main_menu_status = FS_SET;
    }

    if(reset_INT())
    {
      return 2;
    }

    ret_val = 0;
  }

  return ret_val;
}

/**
 * @brief  Operating mode menu handler
 *
 * User can set these vlaues of Operating Mode:
 * 1) Low power mode
 * 2) Normal mode
 * 3) High resolution mode
 *
 * Operating mode menu include one status bar:
 * 1) top status bar, that shows actually set value
 *
 * @param  rx_buffer value received from UART
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
static uint8_t opmode_menu( uint8_t rx_buffer )
{
  uint8_t ret_val = 1;
  float odr_val;
  uint8_t tmp = atoi((const char*)&rx_buffer);

  /* Set chosen Operating mode */
  if( (tmp <= 3) && tmp != 0 )
  {
    if( instance == LSM303AGR_X_0 || instance == LIS2DH12_0 )
    {
      uint32_t odr_switch = 0;
      daq_enable = 1;

      if( BSP_ACCELERO_Get_ODR( ACCELERO_handle, &odr_val ) != COMPONENT_OK )
      {
        main_menu_status = OPMODE_SET_ERR;
      } else
      {
        odr_switch = (uint32_t)odr_val;
        if( (odr_switch != 1344 && tmp == 1) || ((odr_switch != 1620 && odr_switch != 5376) && (tmp == 2 || tmp == 3)) )
        {
          if(BSP_ACCELERO_Set_OpMode_Ext( ACCELERO_handle, sensor_setting[instance].opmode_list[tmp-1] ) != COMPONENT_OK)
          {
            main_menu_status = OPMODE_SET_ERR;
          } else
          {
            main_menu_status = OPMODE_SET;
          }
          ret_val = 0;
        }
      }
    }
  }

  return ret_val;
}


/**
 * @brief  FFT samples menu handler
 *
 * User can set these vlaues of FFT samples:
 * 1) 256
 * 2) 512
 * 3) 1024
 *
 * Set FFT samples menu include top status bar, that shows actually set value
 *
 * @param  rx_buffer value received from UART
 * @retval 0 in case of success
 * @retval 1 in case of failure
 * @retval 2 when reset_INT function failed
 */
static uint8_t samples_menu( uint8_t rx_buffer )
{
  uint8_t ret_val = 1;
  uint8_t tmp = atoi((const char*)&rx_buffer);

  /* Set chosen FFT Samples and calculate values for graph x axis  */
  if( tmp <= sensor_setting[instance].samples_list[0] && tmp != 0 )
  {
    settings.samples = sensor_setting[instance].samples_list[tmp];
    calculate_freq_axis();
    daq_enable = 1;
    main_menu_status = SAMPLES_SET;

    arm_rfft_fast_init_f32(&real_fft_Instance, settings.samples);

    if( instance == LSM6DSL_X_0 || instance == LSM6DSL_X_1 )
    {
      BSP_ACCELERO_FIFO_Set_Watermark_Level_Ext( ACCELERO_handle, (settings.samples+1)*3 );
      if(reset_INT())
      {
        return 2;
      }
    }

    ret_val = 0;
  }

  return ret_val;
}


/**
 * @brief  Change sensor menu handler
 *
 * @param  rx_buffer value received from UART
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
static uint8_t sensor_menu( uint8_t rx_buffer )
{
  uint8_t ret_val = 1;
  uint8_t tmp = atoi((const char*)&rx_buffer);

  /* Set chosen FFT Samples and calculate values for graph x axis  */
  if( tmp <= sensor_list[0] && tmp != 0 )
  {
    BSP_ACCELERO_DeInit(&ACCELERO_handle);

    if( BSP_ACCELERO_Init( (ACCELERO_ID_t)sensor_list[tmp], &ACCELERO_handle ) != COMPONENT_OK)
    {
      DISP("\033[2J\033[0;0HLoading required accelerometer failed...\n\rPlease (re)insert IKS01A2 sensor expansion board (LIS2DH12 or LSM6DSL DIL24 can be inserted) and restart nucleo board.\n\r");
      Error_Handler();
    }

    BSP_ACCELERO_Sensor_Enable( ACCELERO_handle );
    init_fft( TERMINAL );
    main_menu_status = SENSOR_CHNGD;
    ret_val = 0;
  }

  return ret_val;
}

/**
 * @brief  Calculate FFT and create data array for FFT graph plot
 * @param  None
 * @retval None
 */
static void calculate_fft( void )
{
  uint32_t ifft_flag = 0;
  uint16_t i, j;

  for(i=0; i<51; i++)
  {
    fft_text_plot_data[i] = 0;
  }

  for(i=0; i<settings.samples; i++)
  {
    fft_tmp[i] = fft_input[i];
  }

  /* Calculate FFT */
  arm_rfft_fast_f32(&real_fft_Instance, fft_tmp, fft_tmp2, ifft_flag);

  /* Process the data through the Complex Magnitude Module for
  calculating the magnitude at each bin */
  arm_cmplx_mag_f32(fft_tmp2, fft_output, settings.samples);


  /* Flat Top window is supported only when HP filter is on */
  if( !settings.switch_HP_to_DC_null && settings.hp_filter )
  {

    for(i=0; i<settings.samples; i++)
    {
      fft_tmp[i] = fft_input[i]*flat_top_win[i*(1024/settings.samples)];
    }

    /* Calculate FFT using Flat Top window */
    arm_rfft_fast_f32(&real_fft_Instance, fft_tmp, fft_tmp2, ifft_flag);

    /* Process the data through the Complex Magnitude Module for
    calculating the magnitude at each bin */
    arm_cmplx_mag_f32(fft_tmp2, fft_tmp, settings.samples);
  }


  j = 0;

  for(i=0; i<settings.samples/2; i++)
  {
    if( i == 0 )
    {
      if( settings.switch_HP_to_DC_null && settings.hp_filter )
      {
        fft_output[i] = 0; /* if DC nulling enabled */
      } else
      {
        fft_output[i] = (fft_output[i]/(settings.samples))*1000;
        fft_tmp[i] = ((fft_tmp[i]/(settings.samples))*1000)*scale_factor;
      }
    } else
    {
      fft_output[i] = (fft_output[i]/(settings.samples))*2000; /* output*2 */
      fft_tmp[i] = ((fft_tmp[i]/(settings.samples))*2000)*scale_factor;
    }


    if( i == freq_axis[j] )
    {
      j++;
    }

    /* Consider only max value in frequency range of bin */
    if( fft_output[i] > fft_text_plot_data[j] )
    {
      fft_text_plot_data[j] = fft_output[i];
    }
  }

  /* Calculates max_value and returns corresponding BIN value */
  arm_max_f32(fft_output, settings.samples/2, &out_data.max_value, &out_data.max_index);

  /* Calculates max_value and returns corresponding BIN value */
  arm_max_f32(fft_text_plot_data, 51, &out_data.bin_max_value, &out_data.bin_max_index);


  /* Flat Top window is supported only when HP filter is on */
  if( !settings.switch_HP_to_DC_null && settings.hp_filter )
  {
    /* Calculates max_value and returns corresponding BIN value */
    arm_max_f32(fft_tmp, settings.samples/2, &out_data.ftw_max_value, &out_data.ftw_max_index);
  }

  fft_data_rdy = 1;
}


/**
 * @brief  Plot FFT Table
 * @param  None
 * @retval None
 */
static void plot_table(void)
{
  int16_t i = 0;

  DISP("\r\n\r\nInput [mg]:\tOutput [mg]:\tGraph data [mg]:\tODR [Hz]:\tSamples:\r\n");

  for(i=0; i<settings.samples; i++)
  {
    if( i < 1 )                         /* all data */
    {
      DISP("%.f\t\t%4.2f\t\t%4.2f\t\t\t%d\t\t%d\r\n", fft_input[i]*1000, fft_output[i], fft_text_plot_data[i], (uint16_t)odr_measured, settings.samples);

    } else if( i < 51 )                 /* all data but ODR and Samples */
    {
      DISP("%.f\t\t%4.2f\t\t%4.2f\r\n", fft_input[i]*1000, fft_output[i], fft_text_plot_data[i]);

    } else if( i < settings.samples/2 ) /* Input and Output data only */
    {
      DISP("%.f\t\t%4.2f\r\n", fft_input[i]*1000, fft_output[i]);

    } else                              /* Input data only */
    {
      DISP("%.f\r\n", fft_input[i]*1000);
    }
  }

  table_displayed = 1;
}


/**
 * @brief  Plot FFT Graph and max frequency + value
 * @param  None
 * @retval None
 */
static void plot_fft( void )
{
  int16_t i, tmp;
  char plot_data[53];
  int32_t range[2];
  int32_t max_f = 0;
  float tmp2;
  float max = -1e5;
  float min = 1e5;
  float ampl = 0;

  /* Calculate the bin index of max frequency */
  if( out_data.bin_max_index == 0 )
  {
    range[0] = 0;
    range[1] = (int32_t)(((float)(odr_measured/settings.samples)*freq_axis[out_data.bin_max_index]) + 0.5f);
  } else if (out_data.bin_max_index == 50)
  {
    range[0] = (int32_t)(((float)(odr_measured/settings.samples)*freq_axis[out_data.bin_max_index-1]) + 0.5f);
    range[1] = (int32_t)((float)(odr_measured/2)+0.5f);
  } else
  {
    range[0] = (int32_t)(((odr_measured/settings.samples)*freq_axis[out_data.bin_max_index-1]) + 0.5f);
    range[1] = (int32_t)(((odr_measured/settings.samples)*freq_axis[out_data.bin_max_index]) + 0.5f);
  }

  max_f = (int32_t)((out_data.max_index*odr_measured)/(settings.samples));

  for( i = 0; i < settings.samples; i++ )
  {
    if( fft_input[i] > max )
    {
      max = fft_input[i];
    }
    if( fft_input[i] < min )
    {
      min = fft_input[i];
    }
  }

  ampl = ((max - min)*500);


  /* Display: */
  clear_screen();
  DISP("     A[mg]\r\n\r\n       ^\r\n       |\r\n       |\r\n");

  tmp = settings.magnitude;
  for( i = 0; i < 11; i++ )
  {
    fft_plot_creator(tmp, plot_data);
    if( i%2 != 0 )
    {
      DISP("      _%s\r\n", plot_data);
    } else
    {
      if(tmp == 0)
      {
        DISP("   %d __%s__ -> f[Hz]\r\n", tmp, plot_data);
      } else if( tmp < 100 )
      {
        DISP("  %d __%s\r\n", tmp, plot_data);
      } else if( tmp < 1000 )
      {
        DISP(" %d __%s\r\n", tmp, plot_data);
      } else if( tmp < 10000 )
      {
        DISP("%d __%s\r\n", tmp, plot_data);
      } else
      {
        tmp2 = tmp/1000;
        DISP(" %dK __%s\r\n", (int16_t)tmp2, plot_data);
      }

    }
    tmp-=settings.magnitude/10;
  }

  for( i = 0; i < 11; i++ )
  {
    if( i==0 ){
      DISP("       |");
    } else
    {
      DISP("    |");
    }
  }

  tmp2 = odr_measured/20;
  for ( i = 0; i < 11; i++ )
  {
    tmp =(int16_t)((float)tmp2*i+0.5f);

    if(tmp == 0)
    {
      DISP("\r\n       0");
    } else if( tmp < 10 )
    {
      DISP("    %d ", tmp);
    } else if( tmp < 100 )
    {
      DISP("   %d", tmp);
    } else if( tmp < 1000)
    {
      DISP("  %d", tmp);
    } else if( tmp < 10000 )
    {
      DISP(" %d", tmp);
    }
  }

  DISP("\r\n\r\n\r\nMax f = %d Hz\r\nMax value = %4.2f mg\r\n\r\n", (int)max_f, out_data.max_value);

  /* Flat Top window is supported only when HP filter is on */
  if( !settings.switch_HP_to_DC_null && settings.hp_filter )
  {
    DISP("\r\nMax value (Flat Top window used): %4.2f mg\r\n\r\n", out_data.ftw_max_value);
  }
  DISP("\r\nMax f Bin no. %d\r\n[i.e. values %d to %d Hz]\r\n\r\n", (int)out_data.bin_max_index, (int)range[0], (int)range[1]);
  DISP("\r\n(Input signal amplitude = %.2f mg)\r\n", ampl);
}


/**
 * @brief  Create FFT graph for plot
 * @param  n row of graph
 * @param  c line of graph to be displayed
 * @retval None
 */
static void fft_plot_creator(int16_t n, char *c)
{
  uint16_t i;

  for( i = 0; i < 52; i++ )
  {
    c[i]='\0';
  }

  for( i = 0; i < 51; i++ )
  {

    if( fft_text_plot_data[i] > n )
    {
      c[i] = '#';

      if( n == 0 && (fft_text_plot_data[i] < (settings.magnitude/100)) )
      {
        c[i] = '_';
        if(i == 0)
        {
          c[i] = '|';
        }
      }
    } else
    {
      if( i == 0 )
      {
        c[i] = '|';
      } else
      {
        c[i] = ' ';
      }
    }
  }
}


/**
 * @brief  Measure ODR of AXL
 * @param  None
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
static uint8_t meas_odr( void )
{
  uint8_t  odr_meas_enable = 1;
  uint16_t odr_meas_iter = 0;
  uint16_t odr_meas_start_time = 0;
  uint16_t odr_meas_stop_time = 0;
  uint16_t odr_meas_samples = 150; /* number of measured samples for calculating ODR */
  uint32_t start = 0;
  ACTIVE_AXIS_t axis_active;

  if(BSP_ACCELERO_Get_Active_Axis_Ext( ACCELERO_handle, &axis_active ) != COMPONENT_OK )
  {
    return 1;
  }

  if( instance == LSM6DSL_X_0 || instance == LSM6DSL_X_1 )
  {
    if(disable_FIFO())
    {
      return 1;
    }
    /* Set DRDY pulsed mode */
    if(BSP_ACCELERO_Set_DRDY_Mode_Ext( ACCELERO_handle, DRDY_PULSED ) != COMPONENT_OK )
    {
      return 1;
    }
    if(enable_DRDY())
    {
      return 1;
    }
  }

  start = HAL_GetTick();

  while( odr_meas_enable )
  {
    if( ((HAL_GetTick() - start) > 1000) )
    {
      return 1;
    }

    if( AXL_INT_received )
    {
      AXL_INT_received = 0;
      //printf("3. AXL_INT_received: %d\n\r", AXL_INT_received);
      /* Get start time */
      if( odr_meas_iter == 0)
      {
        Int_Current_Time1 = user_currentTimeGetTick();
        odr_meas_start_time = Int_Current_Time1;
      }

      /* Get stop time */
      if( odr_meas_iter == odr_meas_samples - 1)
      {
        Int_Current_Time2 = user_currentTimeGetTick();
        odr_meas_stop_time = Int_Current_Time2;
        odr_meas_enable = 0;
      }

      /* Stop after measuring "odr_meas_samples" values */
      if( odr_meas_iter < odr_meas_samples )
      {
        odr_meas_iter++;
        if( instance != LSM6DSL_X_0 && instance != LSM6DSL_X_1 )
        {
          if( BSP_ACCELERO_ClearDRDY_Ext( ACCELERO_handle, axis_active ) != COMPONENT_OK )
          {
            return 1;
          }
        }
      }
    }
  }

  /* Calculate measured ODR */
  odr_measured = ((float)(1000*odr_meas_samples)/(odr_meas_stop_time - odr_meas_start_time));

  if( instance == LSM6DSL_X_0 || instance == LSM6DSL_X_1 )
  {
    if(disable_DRDY())
    {
      return 1;
    }
    /* Set DRDY latched mode */
    if(BSP_ACCELERO_Set_DRDY_Mode_Ext( ACCELERO_handle, DRDY_LATCHED ) != COMPONENT_OK )
    {
      return 1;
    }
    /* Enable FIFO full flag interrupt */
    if(enable_FIFO())
    {
      return 1;
    }
    if(reset_INT())
    {
      return 1;
    }
  }

  return 0;
}


/**
 * @brief  Calculate bins values for graph x axis
 * @param  None
 * @retval None
 */
static void calculate_freq_axis( void )
{
  int8_t i = 0;

  float delta = ((float)settings.samples/100);   /* (samples/2)/50 */
  float tmp = (delta/2);

  for( i = 0; i < 50; i++ )
  {
    freq_axis[i] = (int16_t)(tmp + 0.5f);
    tmp += delta;
  }

}


/**
 * @brief  Enable/Disable HP or DCnull
 * @param  void
 * @retval void
 */
static void En_Dis_HP_or_DCnull( void )
{
  if( settings.switch_HP_to_DC_null )
  {
    if ( settings.hp_filter )
    {
      settings.hp_filter = 0;
    } else
    {
      settings.hp_filter = 1;
    }

    daq_enable = 1;
    main_menu_status = HP_SWITCHED;

  } else
  {
    if ( settings.hp_filter )
    {
      /* Disable HP filter */
      if(BSP_ACCELERO_Disable_HP_Filter_Ext( ACCELERO_handle ) != COMPONENT_OK)
      {
        main_menu_status = HP_SWITCHED_ERR;
      } else
      {
        settings.hp_filter = 0;
        daq_enable = 1;
        main_menu_status = HP_SWITCHED;
      }

    } else
    {
      /* Enable HP filter */
      if(BSP_ACCELERO_Enable_HP_Filter_Ext( ACCELERO_handle ) != COMPONENT_OK)
      {
        main_menu_status = HP_SWITCHED_ERR;
      } else
      {
        settings.hp_filter = 1;
        daq_enable = 1;
        main_menu_status = HP_SWITCHED;
      }
    }
    HAL_Delay(40);
    reset_INT();
  }
}


/**
 * @brief  Changes HP to DCnull and vice versa in main menu options
 * @param  void
 * @retval void
 */
static void HP_DC_changer( void )
{
  uint8_t retErr = 0;

  if( settings.switch_HP_to_DC_null )
  {
    settings.switch_HP_to_DC_null = 0;
    settings.hp_filter = 0;
    main_menu_status = HP2DC_NULL_SUBST;

  } else
  {
    /* Disable HP filter */
    if( sensor_setting[instance].hp_filter_available )
    {
      if(BSP_ACCELERO_Disable_HP_Filter_Ext( ACCELERO_handle ) != COMPONENT_OK)
      {
        main_menu_status = HP_SWITCHED_ERR;
        retErr = 1;
      }
    }

    if( retErr == 0 )
    {
      settings.switch_HP_to_DC_null = 1;
      settings.hp_filter = 0;
      daq_enable = 1;
      main_menu_status = HP2DC_NULL_SUBST;
    }
  }
}


/**
 * @brief  Acquire Data from accelerometer
 * @param  void
 * @retval 0 in case of success
 * @retval 1 in case of failure
 * @retval 2 when reset_INT function failed
 * @retval 3 when the time of sampling exceeds the timeout
 * @retval 4 when number of samples in FIFO is not equal to required number of samples
 */
static uint8_t acquire_data( void )
{
  ACTIVE_AXIS_t axis_active;
  float sensitivity = 0.0f;
  int16_t data;
  OP_MODE_t opmode;
  uint16_t amount_of_data = 0;
  uint16_t i;
  uint32_t start = HAL_GetTick();
  uint8_t n_bit_num = 16;
  uint8_t ret_val = 1;

  BSP_ACCELERO_Get_Active_Axis_Ext( ACCELERO_handle, &axis_active );

  if( instance == LSM6DSL_X_0 || instance == LSM6DSL_X_1 )
  {
    if(AXL_INT_received)
    {
      int32_t acceleration;
      uint16_t pattern;
      uint16_t samples_in_fifo = 0;
      AXL_INT_received = 0;
      //printf("4. AXL_INT_received: %d\n\r", AXL_INT_received);
      BSP_ACCELERO_FIFO_Get_Num_Of_Samples_Ext( ACCELERO_handle, &samples_in_fifo );

      if( ((float)samples_in_fifo/3) < settings.samples )
      {
        reset_INT();
        return 4;
      }

      while( amount_of_data < settings.samples )
      {
        if(((HAL_GetTick() - start) > 6000))
        {
          reset_INT();
          return 3;
        }

        BSP_ACCELERO_FIFO_Get_Pattern_Ext( ACCELERO_handle, &pattern );
        BSP_ACCELERO_FIFO_Get_Axis_Ext( ACCELERO_handle, &acceleration );       /* get data from FIFO*/

        if( pattern == axis_active )
        {
          fft_input[amount_of_data] = (float)acceleration/1000;                 /* convert from [mg] to [g] */
          amount_of_data++;
        }

      }

      if( amount_of_data == settings.samples )
      {
        ret_val = 0;
      }

      if(reset_INT())
      {
        return 2;
      }
    }
  } else
  {
    if( sensor_setting[instance].opmode_list != NULL ) /* NULL = opmode not supported */
    {
      BSP_ACCELERO_Get_OpMode_Ext( ACCELERO_handle, &opmode );

      switch(opmode)
      {
      default:
      case LOW_PWR_MODE:
        n_bit_num = 8;
        break;
      case NORMAL_MODE:
        n_bit_num = 10;
        break;
      case HIGH_RES_MODE:
        n_bit_num = 12;
        break;
      }
    }
    BSP_ACCELERO_Get_Sensitivity( ACCELERO_handle, &sensitivity );

    while( amount_of_data != settings.samples )
    {
      if(((HAL_GetTick() - start) > 10000))
      {
        return 3;
      }

      if(AXL_INT_received)
      {
        AXL_INT_received = 0;
        //printf("5. AXL_INT_received: %d\n\r", AXL_INT_received);
        BSP_ACCELERO_Get_SuperRawAxes_Ext( ACCELERO_handle, &data, axis_active );      /* get data */

        tmp_array[amount_of_data] = data;

        amount_of_data++;
      }
    }

    if( amount_of_data == settings.samples )
    {
      for( i=0; i<settings.samples; i++ )
      {
        if( n_bit_num == 8)
        {
          tmp_array[i] >>= 8;

          /* convert the 2's complement 8 bit to 2's complement 16 bit */
          if (tmp_array[i] & 0x0080)
          {
            tmp_array[i] |= 0xFF00;
          }
        } else if( n_bit_num == 10)
        {
          tmp_array[i] >>= 6;

          /* convert the 2's complement 10 bit to 2's complement 16 bit */
          if (tmp_array[i] & 0x0200)
          {
            tmp_array[i] |= 0xFC00;
          }
        } else if( n_bit_num == 12)
        {
          tmp_array[i] >>= 4;

          /* convert the 2's complement 12 bit to 2's complement 16 bit */
          if (tmp_array[i] & 0x0800)
          {
            tmp_array[i] |= 0xF000;
          }
        }

        fft_input[i] = (float)((tmp_array[i] * sensitivity)/1000);
      }

      ret_val = 0;
    }
  }

  return ret_val;
}



/**
 * @brief  Print menu header to terminal
 *
 * @param  menu the menu to be printed to terminal
 * @retval void
 */
static void print_menu_header( menu_t menu )
{
  char stars[HEADER_LENGTH+1] = {'\0'};
  char spaces[(HEADER_LENGTH+1)/2] = {'\0'};
  uint8_t menuNameLength = 0;
  uint8_t i = 0;

  const char* const menuHeaderList[]=
  {
    [MENU_MAIN]    = "Main Menu",
    [MENU_ODR]     = "ODR Menu",
    [MENU_MAG]     = "Magnitude Menu",
    [MENU_AXIS]    = "Sensing axis Menu",
    [MENU_FS]      = "Full scale Menu",
    [MENU_SAMPLES] = "FFT samples Menu",
    [MENU_OPMODE]  = "Operating mode Menu",
    [MENU_SENSORS] = "Sensor Menu"
  };

  menuNameLength = strlen(menuHeaderList[menu]);

  for( i=0; i<HEADER_LENGTH; i++ )
  {
    stars[i] = '*';
  }

  if( ((menuNameLength%2 != 0) && (HEADER_LENGTH%2 == 0)) ||  ((menuNameLength%2 == 0) && (HEADER_LENGTH%2 != 0)) )
  {
    for( i=0; i<((HEADER_LENGTH - (menuNameLength + 1) - 4)/2); i++ )
    {
      spaces[i] = ' ';
    }
  } else
  {
    for( i=0; i<((HEADER_LENGTH - menuNameLength - 4)/2); i++ )
    {
      spaces[i] = ' ';
    }
  }

  clear_screen();
  DISP("%s\r\n", stars);

  if( ((menuNameLength%2 != 0) && (HEADER_LENGTH%2 == 0)) ||  ((menuNameLength%2 == 0) && (HEADER_LENGTH%2 != 0)) )
  {
    DISP("** %s%s%s**\r\n", spaces, menuHeaderList[menu], spaces);

  } else
  {
    DISP("**%s%s%s**\r\n", spaces, menuHeaderList[menu], spaces);
  }
  DISP("%s\r\n\r\n", stars);
}


static void print_menu( menu_t menu )
{
  float tmp;
  uint32_t odr_switch = 0;
  uint32_t fullScale = 0;
  OP_MODE_t operatingMode;
  ACTIVE_AXIS_t axis_active;
  uint8_t pos[NUM_MENU];
  uint8_t i;
  char tmpStr[32];
  char tmpStr2[256];

  BSP_ACCELERO_Get_OpMode_Ext( ACCELERO_handle, &operatingMode );
  BSP_ACCELERO_Get_ODR( ACCELERO_handle, &tmp );
  odr_switch = (uint32_t)tmp;
  BSP_ACCELERO_Get_FS( ACCELERO_handle, &tmp);
  fullScale = (uint32_t)tmp;
  BSP_ACCELERO_Get_Active_Axis_Ext( ACCELERO_handle, &axis_active );

  for( i = MENU_ODR; i < NUM_MENU; i++ )
  {
    pos[i] = get_pos_in_menu_list((menu_t)i);
  }

  /* Print menu header to terminal */
  print_menu_header( menu );

  if(menu == MENU_MAIN)
  {
    /* Actual settings */
    DISP("Actual settings:\r\n");
  } else
  {
    if(menu != MENU_SENSORS)
    {
      /* Actual settings */
      DISP("Actually set value:\r\n");
    } else
    {
      DISP("Actual sensor in use:\r\n");
    }
  }

  memset(tmpStr2, 0, sizeof(tmpStr2));

  /* ODR */
  if((menu == MENU_MAIN && pos[MENU_ODR] != 0) || menu == MENU_ODR)
  {
    snprintf(tmpStr, sizeof(tmpStr), "ODR = %d Hz", (int)odr_switch);
    strcat(tmpStr2, tmpStr);
  }

  /* MAG */
  if(menu == MENU_MAIN || menu == MENU_MAG)
  {
    if(menu == MENU_MAIN)
    {
      strcat(tmpStr2, ", ");
    }

    if( settings.magnitude < mag_values_list[MAG_1g] )
    {
      snprintf(tmpStr, sizeof(tmpStr), "Mag. = %d mg", settings.magnitude);
      strcat(tmpStr2, tmpStr);
    } else
    {
      snprintf(tmpStr, sizeof(tmpStr), "Mag. = %d g", settings.magnitude/mag_values_list[MAG_1g]);
      strcat(tmpStr2, tmpStr);
    }
  }

  /* AXIS */
  if((menu == MENU_MAIN && pos[MENU_AXIS] != 0) || menu == MENU_AXIS)
  {
    if(menu == MENU_MAIN)
    {
      strcat(tmpStr2, ", ");
    }

    switch( axis_active )
    {
    case X_AXIS:
      strcat(tmpStr2, "Axis = X");
      break;

    case Y_AXIS:
      strcat(tmpStr2, "Axis = Y");
      break;

    case Z_AXIS:
      strcat(tmpStr2, "Axis = Z");
      break;

    default:
      break;
    }
  }

  /* FS */
  if((menu == MENU_MAIN && pos[MENU_FS] != 0) || menu == MENU_FS)
  {
    if(menu == MENU_MAIN)
    {
      strcat(tmpStr2, ", ");
    }

    snprintf(tmpStr, sizeof(tmpStr), "FS = +-%d g", (int)fullScale);
    strcat(tmpStr2, tmpStr);
  }

  /* OPMODE */
  if((menu == MENU_MAIN && pos[MENU_OPMODE] != 0) || menu == MENU_OPMODE)
  {
    if(menu == MENU_MAIN)
    {
      strcat(tmpStr2, ", ");
    }

    switch( operatingMode )
    {
    case LOW_PWR_MODE:
      strcat(tmpStr2, "OpMode = LPM");
      break;

    case NORMAL_MODE:
      strcat(tmpStr2, "OpMode = NM");
      break;

    case HIGH_RES_MODE:
      strcat(tmpStr2, "OpMode = HRM");
      break;

    default:
      break;
    }
  }

  /* SAMPLES */
  if(menu == MENU_MAIN || menu == MENU_SAMPLES)
  {
    if(menu == MENU_MAIN)
    {
      strcat(tmpStr2, ", ");
    }

    snprintf(tmpStr, sizeof(tmpStr), "Sam. = %d", settings.samples);
    strcat(tmpStr2, tmpStr);
  }

  /* HP FILTER/DC NULL */
  if(menu == MENU_MAIN && pos[MENU_HP_FILTER] != 0)
  {
    char hpStatus[20];
    memset(hpStatus, 0, sizeof(hpStatus));
    uint8_t tmp123 = 0;
    uint8_t tmp1234 = 0;

    if( settings.switch_HP_to_DC_null )
    {
      if( settings.hp_filter )
      {
        strcpy(hpStatus, "DCnull = Enabled,");
      } else
      {
        strcpy(hpStatus, "DCnull = Disabled,");
      }
    } else
    {
      if( settings.hp_filter )
      {
        strcpy(hpStatus, "HPF = Enabled,");
      } else
      {
        strcpy(hpStatus, "HPF = Disabled,");
      }
    }

    tmp123 = strlen(tmpStr2);
    tmp1234 = strlen(hpStatus);

    if( tmp123 > (83 - tmp1234) ) /* terminal size - length of string */
    {
      strcat(tmpStr2, ",\r\n");
      strcat(tmpStr2, hpStatus);
      strcat(tmpStr2, " ");
    } else
    {
      strcat(tmpStr2, ", ");
      strcat(tmpStr2, hpStatus);
      strcat(tmpStr2, "\r\n");
    }
  }


  /* SENSORS */
  if(menu == MENU_MAIN || menu == MENU_SENSORS)
  {
    if(menu == MENU_MAIN)
    {
      strcat(tmpStr2, "Sensor = ");
    }

    if( instance != LSM6DSL_X_0 && instance != LSM303AGR_X_0 && instance != LIS2DH12_0 && instance != LSM6DSL_X_1 )
    {
      strcat(tmpStr2, "Unknown");
    } else
    {
      snprintf(tmpStr, sizeof(tmpStr), "%s", sensor_setting[instance].name );
      strcat(tmpStr2, tmpStr);
    }

    if(menu == MENU_MAIN)
    {
      strcat(tmpStr2, "\r\n\r\n");
    }
  }


  DISP("%s", tmpStr2);


  /* Options to be choosed */
  switch( menu )
  {
  case MENU_MAIN:
    {
      DISP("Options:\t\tNo. to be pressed:\r\n");
      DISP("------------------------------------------\r\n");

      if( pos[MENU_ODR] != 0 )
      {
        DISP("\r\nSet ODR\t\t\t\"%d\"\r\n", pos[MENU_ODR] );
      }

      DISP("\r\nSet Magnitude\t\t\"%d\"\r\n", pos[MENU_MAG] );

      if( pos[MENU_AXIS] != 0 )
      {
        DISP("\r\nSet Sensing axis\t\"%d\"\r\n", pos[MENU_AXIS] );
      }

      if( pos[MENU_FS] != 0 )
      {
        DISP("\r\nSet Full scale\t\t\"%d\"\r\n", pos[MENU_FS] );
      }

      if( pos[MENU_OPMODE] != 0 )
      {
        DISP("\r\nSet Operating mode\t\"%d\"\r\n", pos[MENU_OPMODE] );
      }

      DISP("\r\nSet FFT Samples\t\t\"%d\"\r\n", pos[MENU_SAMPLES] );

      if( pos[MENU_HP_FILTER] != 0 )
      {
        if( settings.switch_HP_to_DC_null )
        {
          if( settings.hp_filter )
          {
            DISP("\r\nDisable DC nulling\t\"%d\"\r\n", pos[MENU_HP_FILTER] );
          } else
          {
            DISP("\r\nEnable DC nulling\t\"%d\"\r\n", pos[MENU_HP_FILTER] );
          }
        } else
        {
          if( settings.hp_filter )
          {
            DISP("\r\nDisable HP Filter\t\"%d\"\r\n", pos[MENU_HP_FILTER] );
          } else
          {
            DISP("\r\nEnable HP Filter\t\"%d\"\r\n", pos[MENU_HP_FILTER] );
          }
        }
      }

      DISP("\r\nPlot FFT Graph\t\t\"%d\"\r\n", pos[MENU_PLOT_GRAPH] );

      DISP("\r\nPlot FFT Table\t\t\"%d\"\r\n", pos[MENU_PLOT_TABLE] );

    } break;


  case MENU_ODR:
    {
      DISP("\r\n\r\n\r\nODR:\t\t\tNo. to be pressed:\r\n");
      DISP("------------------------------------------\r\n");

      for( i = 0; i < sensor_setting[instance].odr_list[0]; i++ )
      {
        memset(tmpStr2, 0, sizeof(tmpStr2));

        snprintf(tmpStr2, sizeof(tmpStr2), "\r\n%d Hz", (int)sensor_setting[instance].odr_list[i+1]);

        if( instance == LSM303AGR_X_0 || instance == LIS2DH12_0 )
        {
          if( (operatingMode == LOW_PWR_MODE && (i+1) == 3) || ( operatingMode != LOW_PWR_MODE && (((i+1) == 4) || (i+1) == 5 )))
          {
           snprintf(tmpStr2, sizeof(tmpStr2), "\r\n*%d Hz", (int)sensor_setting[instance].odr_list[i+1]);
          }
        }

        while( strlen(tmpStr2) < 22 )
        {
          strcat(tmpStr2, " ");
        }
        snprintf(tmpStr, sizeof(tmpStr), "\t\"%d\"\r\n", i+1);
        strcat(tmpStr2, tmpStr);
        DISP("%s", tmpStr2);
      }

      DISP("\r\n\r\nReturn to Main menu\t\"m\"\r\n");

      if( instance == LSM303AGR_X_0 || instance == LIS2DH12_0 )
      {
        if(operatingMode == LOW_PWR_MODE)
        {
          DISP("\r\n\r\n\r\n\r\n\r\n\r\n* You have to set Operating Mode to High resolution mode or Normal mode\r\n  to use this option\r\n");
        } else
        {
          DISP("\r\n\r\n\r\n\r\n\r\n\r\n* You have to set Operating Mode to Low power mode to use this option\r\n");
        }
      }
    } break;


  case MENU_MAG:
    {
      DISP("\r\n\r\n\r\nMagnitude:\t\tNo. to be pressed:\r\n");
      DISP("------------------------------------------\r\n");
      DISP("\r\n200 mg\t\t\t\"1\"\r\n");
      DISP("\r\n500 mg\t\t\t\"2\"\r\n");
      DISP("\r\n1 g\t\t\t\"3\"\r\n");
      DISP("\r\n2 g\t\t\t\"4\"\r\n");
      DISP("\r\n4 g\t\t\t\"5\"\r\n");
      DISP("\r\n8 g\t\t\t\"6\"\r\n");
      DISP("\r\n16 g\t\t\t\"7\"\r\n");
      DISP("\r\n\r\nReturn to Main menu\t\"m\"\r\n");

    } break;


  case MENU_AXIS:
    {
      DISP("\r\n\r\n\r\nSensing axis:\t\tNo. to be pressed:\r\n");
      DISP("------------------------------------------\r\n");
      DISP("\r\nX-axis\t\t\t\"1\"\r\n");
      DISP("\r\nY-axis\t\t\t\"2\"\r\n");
      DISP("\r\nZ-axis\t\t\t\"3\"\r\n");
      DISP("\r\n\r\nReturn to Main menu\t\"m\"\r\n");

    } break;


  case MENU_FS:
    {
      DISP("\r\n\r\n\r\nFull scale:\t\tNo. to be pressed:\r\n");
      DISP("------------------------------------------\r\n");

      for( i = 0; i < sensor_setting[instance].fs_list[0]; i++ )
      {
        memset(tmpStr2, 0, sizeof(tmpStr2));

        snprintf(tmpStr2, sizeof(tmpStr2), "\r\n+-%d g", (int)sensor_setting[instance].fs_list[i+1]);

        while( strlen(tmpStr2) < 22 )
        {
          strcat(tmpStr2, " ");
        }
        snprintf(tmpStr, sizeof(tmpStr), "\t\"%d\"\r\n", i+1);
        strcat(tmpStr2, tmpStr);
        DISP("%s", tmpStr2);
      }

      DISP("\r\n\r\nReturn to Main menu\t\"m\"\r\n");

    } break;


  case MENU_OPMODE:
    {
      DISP("\r\n\r\n\r\nOperating mode:\t\tNo. to be pressed:\r\n");
      DISP("------------------------------------------\r\n");

      if( instance == LSM303AGR_X_0 || instance == LIS2DH12_0 )
      {
        if( odr_switch == 1344 )
        {
          DISP("\r\n*OpMode = Low power\t\"1\"\r\n");
        } else
        {
          DISP("\r\nOpMode = Low power\t\"1\"\r\n");
        }
        if( odr_switch == 1620 || odr_switch == 5376 )
        {
          DISP("\r\n*OpMode = Normal\t\"2\"\r\n");
          DISP("\r\n*OpMode = High res\t\"3\"\r\n");
        } else
        {
          DISP("\r\nOpMode = Normal\t\t\"2\"\r\n");
          DISP("\r\nOpMode = High res\t\"3\"\r\n");
        }

        DISP("\r\n\r\nReturn to Main menu\t\"m\"\r\n");

        if( odr_switch == 1344 || odr_switch == 1620 || odr_switch == 5376 )
        {
          DISP("\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n* You have to set a common ODR for all OpModes (i.e. 200 Hz or 400 Hz) \r\n  to use this option\r\n");
        }
      } else
      {
        DISP("\r\nOpMode = Normal\t\t\"2\"\r\n");
        DISP("\r\nOpMode = High res\t\"3\"\r\n");
        DISP("\r\nOpMode = Low power\t\"1\"\r\n");
      }
    } break;


  case MENU_SAMPLES:
    {
      DISP("\r\n\r\n\r\nNumber of samples:\tNo. to be pressed:\r\n");
      DISP("------------------------------------------\r\n");

      for( i = 0; i < sensor_setting[instance].samples_list[0]; i++ )
      {
        memset(tmpStr2, 0, sizeof(tmpStr2));

        snprintf(tmpStr2, sizeof(tmpStr2), "\r\n%d", (int)sensor_setting[instance].samples_list[i+1]);

        while( strlen(tmpStr2) < 22 )
        {
          strcat(tmpStr2, " ");
        }
        snprintf(tmpStr, sizeof(tmpStr), "\t\"%d\"\r\n", i+1);
        strcat(tmpStr2, tmpStr);
        DISP("%s", tmpStr2);
      }

      DISP("\r\n\r\nReturn to Main menu\t\"m\"\r\n");
    } break;

  case MENU_SENSORS:
    {
      DISP("\r\n\r\n\r\nSensor\t\t\tNo. to be pressed:\r\n");
      DISP("------------------------------------------\r\n");

      for( i = 0; i < sensor_list[0]; i++ )
      {
        memset(tmpStr2, 0, sizeof(tmpStr2));

        snprintf(tmpStr2, sizeof(tmpStr2), "\r\n%s", sensor_setting[sensor_list[i+1]].name);

        while( strlen(tmpStr2) < 22 )
        {
          strcat(tmpStr2, " ");
        }
        snprintf(tmpStr, sizeof(tmpStr), "\t\"%d\"\r\n", i+1);
        strcat(tmpStr2, tmpStr);
        DISP("%s", tmpStr2);
      }

      DISP("\r\n\r\nReturn to Main menu\t\"m\"\r\n");
    } break;

  default:
    break;
  }

  /* Status message of Main menu */
  if( menu == MENU_MAIN )
  {
    switch(main_menu_status)
    {
    case ODR_SET:
      {
        DISP("\r\n\r\nODR was successfully set to %d Hz!\r\n(Measured ODR = %d Hz)\r\n", (int)odr_switch, (int)odr_measured);
      } break;

    case ODR_SET_ERR:
      {
        DISP("Error occurred during ODR set! Please restart nucleo board...\r\n");
        Error_Handler();
      } break;

    case MAG_SET:
      {
        if( settings.magnitude < mag_values_list[MAG_1g] )
        {
          DISP("\r\n\r\nMagnitude was successfully set to %d mg!\r\n", settings.magnitude);
        } else
        {
          DISP("\r\n\r\nMagnitude was successfully set to %d g!\r\n", settings.magnitude/mag_values_list[MAG_1g]);
        }
      } break;

    case AXIS_SET:
      {
        switch( axis_active )
        {
        case X_AXIS:
          DISP("\r\n\r\nSensing axis was successfully set to X!\r\n");
          break;

        case Y_AXIS:
          DISP("\r\n\r\nSensing axis was successfully set to Y!\r\n");
          break;

        case Z_AXIS:
          DISP("\r\n\r\nSensing axis was successfully set to Z!\r\n");
          break;

        default:
          break;
        }
      } break;

    case AXIS_SET_ERR:
      {
        DISP("Error occurred during Active Axis set! Please restart nucleo board...\r\n");
        Error_Handler();
      } break;

    case FS_SET:
      {
        DISP("\r\n\r\nFull scale was successfully set to +-%d g!\r\n", (int)fullScale);
      } break;

    case FS_SET_ERR:
      {
        DISP("Error occurred during FS set! Please restart nucleo board...\r\n");
        Error_Handler();
      } break;

    case SAMPLES_SET:
      {
        DISP("\r\n\r\nFFT Samples was successfully set to %d!\r\n", settings.samples);
      } break;

    case OPMODE_SET:
      {
        switch( operatingMode )
        {
        case LOW_PWR_MODE:
          DISP("\r\n\r\nOperating mode was successfully set to Low power mode!\r\n");
          break;

        case NORMAL_MODE:
          DISP("\r\n\r\nOperating mode was successfully set to Normal mode!\r\n");
          break;

        case HIGH_RES_MODE:
          DISP("\r\n\r\nOperating mode was successfully set to High resolution mode!\r\n");
          break;

        default:
          break;
        }
      } break;

    case OPMODE_SET_ERR:
      {
        DISP("Error occurred during OpMode set! Please restart nucleo board...\r\n");
        Error_Handler();
      } break;

    case HP_SWITCHED:
      {
        if( settings.switch_HP_to_DC_null )
        {
          if( settings.hp_filter )
          {
            DISP("\r\n\r\nDC nulling was successfully Enabled!\r\n");
          } else
          {
            DISP("\r\n\r\nDC nulling was successfully Disabled!\r\n");
          }
        } else
        {
          if( settings.hp_filter )
          {
            DISP("\r\n\r\nHP filter was successfully Enabled!\r\n");
          } else
          {
            DISP("\r\n\r\nHP filter was successfully Disabled!\r\n");
          }
        }
      } break;

    case HP_SWITCHED_ERR:
      {
        DISP("\r\n\r\nError occurred during HP filter switch! Please restart nucleo board...\r\n");
        Error_Handler();
      } break;

    case HP2DC_NULL_SUBST:
      {
        if( settings.switch_HP_to_DC_null )
        {
          DISP("\r\n\r\nOption HP Filter was successfully changed to DC nulling in main menu!\r\n");
        } else
        {
          DISP("\r\n\r\nOption DC nulling was successfully changed to HP Filter in main menu!\r\n");
        }
      } break;

    case FIRST_ODR_MEAS:
      {
        DISP("\r\n\r\n(Initially measured ODR = %d Hz)\r\n", (int)odr_measured);
      } break;

    case SENSOR_CHNGD:
      {
        DISP("\r\n\r\nSensor in use successfully changed to %s!\r\n", sensor_setting[instance].name);
      } break;

    default:
      break;
    }
  }
}


/**
 * @brief  Search menu item in the menu list
 *
 * @param  none
 * @retval position in menu list if found, 0 otherwise
 */
static uint8_t get_pos_in_menu_list( menu_t item )
{
  uint8_t i;
  for( i = 1; i < NUM_MENU; i++ )
  {
    if(item == menu_list[i])
    {
      return i;
    }
  }

  return 0;
}


/**
 * @brief  Clear terminal screen
 *
 * @param  none
 * @retval None
 */
static void clear_screen(void)
{
  DISP("\033[2J\033[0;0H");
}


/**
 * @brief  Enable DRDY
 * @param  None
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
static uint8_t enable_DRDY( void )
{
  ACTIVE_AXIS_t axis_active;
  AXL_INT_received = 0;
      //printf("6. AXL_INT_received: %d\n\r", AXL_INT_received);
  /* Get Sensing axis */
  if(BSP_ACCELERO_Get_Active_Axis_Ext( ACCELERO_handle, &axis_active ) != COMPONENT_OK)
  {
    return 1;
  }
  /* Enable DRDY */
  if(BSP_ACCELERO_Set_INT1_DRDY_Ext( ACCELERO_handle, INT1_DRDY_ENABLED ) != COMPONENT_OK)
  {
    return 1;
  }
  /* Clear DRDY */
  if(BSP_ACCELERO_ClearDRDY_Ext( ACCELERO_handle, axis_active ) != COMPONENT_OK)
  {
    return 1;
  }

  return 0;
}


/**
 * @brief  Disable DRDY
 * @param  None
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
static uint8_t disable_DRDY( void )
{
  /* Disable DRDY */
  if(BSP_ACCELERO_Set_INT1_DRDY_Ext( ACCELERO_handle, INT1_DRDY_DISABLED ) != COMPONENT_OK)
  {
    return 1;
  }

  return 0;
}


/**
 * @brief  Enable FIFO measuring
 * @param  None
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
static uint8_t enable_FIFO( void )
{
  /* Enable FIFO full flag interrupt */
  if(BSP_ACCELERO_FIFO_Set_INT1_FIFO_Full_Ext( ACCELERO_handle, 0x20 ) != COMPONENT_OK)
  {
    return 1;
  }

  return 0;
}


/**
 * @brief  Disable FIFO measuring
 * @param  None
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
static uint8_t disable_FIFO( void )
{
  /* Set FIFO to bypass mode */
  if(BSP_ACCELERO_FIFO_Set_Mode_Ext( ACCELERO_handle, 0x00 ) != COMPONENT_OK)
  {
    return 1;
  }
  /* Disable FIFO full flag interrupt */
  if(BSP_ACCELERO_FIFO_Set_INT1_FIFO_Full_Ext( ACCELERO_handle, 0x00 ) != COMPONENT_OK)
  {
    return 1;
  }

  return 0;
}


/**
 * @brief  Restart FIFO
 * @param  None
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
static uint8_t restart_FIFO( void )
{
  AXL_INT_received = 0;
     // printf("7. AXL_INT_received: %d\n\r", AXL_INT_received);
  /* FIFO bypass */
  if(BSP_ACCELERO_FIFO_Set_Mode_Ext( ACCELERO_handle, 0x00 ) != COMPONENT_OK)
  {
    return 1;
  }
  /* FIFO Mode*/
  if(BSP_ACCELERO_FIFO_Set_Mode_Ext( ACCELERO_handle, 0x01 ) != COMPONENT_OK)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Reset interrupts
 * @param  None
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
uint8_t reset_INT( void )
{
  ACTIVE_AXIS_t axis_active;

  if( instance == LSM6DSL_X_0 || instance == LSM6DSL_X_1 )
  {
    if(restart_FIFO())
    {
      return 1;
    }
  } else
  {
    /* Get Sensing axis */
    if(BSP_ACCELERO_Get_Active_Axis_Ext( ACCELERO_handle, &axis_active ) != COMPONENT_OK)
    {
      return 1;
    }
    /* Clear DRDY */
    if(BSP_ACCELERO_ClearDRDY_Ext( ACCELERO_handle, axis_active ) != COMPONENT_OK)
    {
      return 1;
    }
    AXL_INT_received = 0;
     // printf("8. AXL_INT_received: %d\n\r", AXL_INT_received);
  }

  return 0;
}


/**
 * @brief  Find available accelerometers that can be used for FFT demo
 * @param  None
 * @retval Number of found sensors
 */
uint8_t get_available_sensors( void )
{
  uint8_t sensorsAvailable = 1;
  void* test_handle;

  if( BSP_LSM6DSL_AVAIL_Ext( &test_handle ) == COMPONENT_OK )
  {
    sensor_list[sensorsAvailable] = LSM6DSL_X_0;
    sensorsAvailable++;
  }
  if( BSP_LSM303AGR_AVAIL_Ext( &test_handle ) == COMPONENT_OK )
  {
    sensor_list[sensorsAvailable] = LSM303AGR_X_0;
    sensorsAvailable++;
  }
  if( BSP_LIS2DH12_AVAIL_Ext( &test_handle ) == COMPONENT_OK )
  {
    sensor_list[sensorsAvailable] = LIS2DH12_0;
    sensorsAvailable++;
  }
  if( BSP_LSM6DSL_DIL24_AVAIL_Ext( &test_handle ) == COMPONENT_OK )
  {
    sensor_list[sensorsAvailable] = LSM6DSL_X_1;
    sensorsAvailable++;
  }

  sensor_list[0] = sensorsAvailable-1;

  return sensor_list[0];
}


/**
 * @brief  Get max frequency value.
 *
 * @param  none
 * @retval calculated max frequency
 */
uint32_t get_fft_max_freq( void )
{
  return (uint32_t)((out_data.max_index*odr_measured)/(settings.samples));
}


/**
 * @brief  Get amplitude of max frequency using Flat Top window.
 *
 * @param  none
 * @retval calculated max frequency
 */
uint32_t get_fft_max_freq_amp( void )
{
  uint32_t max_val = 0;
  
  /* use calculated max value from Flat Top window when HP filter is on */
  if( !settings.switch_HP_to_DC_null && settings.hp_filter )
  {
    max_val = (uint32_t)out_data.ftw_max_value;
  } else
  {
    max_val = (uint32_t)out_data.max_value;
  }
  
  return max_val;
}



/**
 * @brief  Get HP filter status
 *
 * @param none
 * @retval HP filter status
 */
uint8_t get_hp_filter( void )
{
  return settings.hp_filter;
}


/**
 * @brief  Get number of FFT samples
 *
 * @param none
 * @retval FFT samples
 */
uint16_t get_samples( void )
{
  return settings.samples;
}


/**
 * @brief  Get list of available sensors
 *
 * @param none
 * @retval FFT samples
 */
uint8_t * get_sensor_list( void )
{
  get_available_sensors();

  return sensor_list;
}



/**
 * @brief  Get FFT message
 *
 * @param  Msg the pointer to the message to be handled
 * @retval none
 */
void get_fft_msg( TMsg *Msg )
{
  uint32_t k;

  memcpy(&Msg->Data[3], (void *) &odr_measured, 4);

  for(k = 0; k < settings.samples/2; k++)
  {
    memcpy(&Msg->Data[7+k*4], (void *) &fft_output[k], 4);
  }

  Msg->Len = 7 + k*sizeof(float);
}


/**
 * @brief  Get measured FFT
 *
 * @param  none
 * @retval measured ODR of the sensor
 */
uint32_t get_meas_odr( void )
{
  return (uint32_t)(odr_measured);
}

/**
 * @brief  Set ODR of the accelerometer
 *
 * @param value the index of ODR to be set
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
uint8_t set_odr( uint8_t value )
{
  uint8_t ret_val = 1;
  char odr[5];

  snprintf(odr, sizeof(odr), "%d", value);
  ret_val = odr_menu(odr[0]);

  return ret_val;
}


/**
 * @brief  Set active axis of the accelerometer
 *
 * @param value the index of active axis to be set
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
uint8_t set_active_axis( uint8_t value )
{
  uint8_t ret_val = 1;
  char axis_active[5];

  snprintf(axis_active, sizeof(axis_active), "%d", value);
  ret_val = axis_menu(axis_active[0]);

  return ret_val;
}


/**
 * @brief  Set full scale of the accelerometer
 *
 * @param value the index of full scale to be set
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
uint8_t set_full_scale( uint8_t value )
{
  uint8_t ret_val = 1;
  char fullScale[5];

  snprintf(fullScale, sizeof(fullScale), "%d", value);
  ret_val = full_scale_menu(fullScale[0]);

  return ret_val;
}


/**
 * @brief  Set operating mode of the accelerometer
 *
 * @param value the index of operating mode to be set
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
uint8_t set_opmode( uint8_t value )
{
  uint8_t ret_val = 1;
  char opmode[5];

  snprintf(opmode, sizeof(opmode), "%d", value);
  ret_val = opmode_menu(opmode[0]);

  return ret_val;
}


/**
 * @brief  Set number of FFT samples
 *
 * @param value the index of FFT samples to be set
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
uint8_t set_samples( uint8_t value )
{
  uint8_t ret_val = 1;
  char samples[5];

  snprintf(samples, sizeof(samples), "%d", value);
  ret_val = samples_menu(samples[0]);

  return ret_val;
}


/**
 * @brief  Set HP filter
 *
 * @param value the value of FFT samples to be set
 * @retval 0 in case of success
 * @retval 1 in case of failure
 * @retval 2 when HP filter setting is not available for sensor in use
 */
uint8_t set_hp_filter( uint8_t value )
{
  if( !sensor_setting[instance].hp_filter_available )
  {
    return 2;
  }

  if( value != settings.hp_filter )
  {
    En_Dis_HP_or_DCnull();
  }

  /* check if the variable settings.hp_filter is properly changed */
  if( value != settings.hp_filter )
  {
    return 1;
  }

  return 0;

}


/**
 * @brief  Prepare FFT data for get_fft_data(), get_fft_max_freq() and get_fft_max_freq_amp() functions
 *
 * @param none
 * @retval 0 in case of success
 * @retval 1 in case of failure
 * @retval 2 when reset_INT function failed
 * @retval 3 when the time of sampling exceeds the timeout
 */
uint8_t prepare_fft_data( void )
{
  uint8_t ret_val;
  uint32_t start = HAL_GetTick();

  if(reset_INT())
  {
    return 2;
  }

  do
  {
    if((HAL_GetTick() - start) > 4000)
    {
      return 3;
    }

    ret_val = acquire_data();

  } while(ret_val);

  calculate_fft();
  if( !fft_data_rdy )
  {
    return 1;
  }

  return 0;
}


/**
 * @brief  Get FFT output data
 *
 * @param value the value of FFT samples to be set
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
float *get_fft_data( void )
{
  return (float*)fft_output;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
