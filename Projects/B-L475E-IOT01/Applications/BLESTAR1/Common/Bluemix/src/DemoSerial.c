/**
  ******************************************************************************
  * @file    DemoSerial.c
  * @author  CL, MP
  * @version V4.1.0
  * @date    13-Oct-2017
  * @brief   Handle the Serial Protocol
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
#include "DemoSerial.h"
#include "main.h"
#include "com.h"
#include "fft.h"
#include "sensor_def.h"

/** @addtogroup X_NUCLEO_IKS01A2_Examples
  * @{
  */

/** @addtogroup FFT_DEMO
  * @{
  */

/* Extern variables ----------------------------------------------------------*/
extern volatile uint32_t Sensors_Enabled;
extern volatile uint32_t DataTxPeriod;
extern void *ACCELERO_handle;

/* Private variables ---------------------------------------------------------*/
volatile uint8_t DataLoggerActive;
volatile uint8_t SenderInterface = 0;
uint8_t PresentationString[] = {"MEMS shield demo,13,4.1.0,4.1.0,IKS01A2"};
volatile uint8_t DataStreamingDest = 1;

/* Private function prototypes -----------------------------------------------*/
uint8_t Handle_SUBCMD( TMsg *Msg );
void Send_Sensor_Name(TMsg *Msg, uint8_t* sensor_name);
void Send_Avail_Sensor_List(TMsg *Msg, uint8_t* sensor_list);
void Send_Sensor_fs_list(TMsg *Msg, uint32_t* fs_list);
void Send_Sensor_odr_list(TMsg *Msg, float* odr_list);
void Send_Samples_list(TMsg *Msg, uint32_t* samples_list);
void send_ErrorMsg( TMsg *Msg );
uint8_t get_ODR_index( uint8_t value );
void FloatToArray(uint8_t *Dest, float data);
void ArrayToFloat(uint8_t *Source, float *data);
uint8_t getActualFS_index( uint8_t *value );
uint8_t getActualODR_index( uint8_t *value );
uint8_t getActualOpMode_index( uint8_t *value );
uint8_t findODRlistBounds( uint8_t instance, uint8_t *bound );
uint8_t getActualSamples_index( uint8_t *value );
uint8_t getActualSensor_index( uint8_t *value );

/**
  * @brief  Build the reply header
  * @param  Msg the pointer to the message to be built
  * @retval None
  */
void BUILD_REPLY_HEADER(TMsg *Msg)
{
  Msg->Data[0] = Msg->Data[1];
  Msg->Data[1] = DEV_ADDR;
  Msg->Data[2] += CMD_Reply_Add;
}

/**
  * @brief  Initialize the streaming header
  * @param  Msg the pointer to the header to be initialized
  * @retval None
  */
void INIT_STREAMING_HEADER(TMsg *Msg)
{
  Msg->Data[0] = DataStreamingDest;
  Msg->Data[1] = DEV_ADDR;
  Msg->Data[2] = CMD_SF_Data;  
  Msg->Len = 3;
}

/**
  * @brief  Initialize the streaming message
  * @param  Msg the pointer to the message to be initialized
  * @retval None
  */
void INIT_STREAMING_MSG(TMsg *Msg)
{
  uint8_t i;

  Msg->Data[0] = DataStreamingDest;
  Msg->Data[1] = DEV_ADDR;
  Msg->Data[2] = CMD_Start_Data_Streaming;
  for(i = 3; i < STREAMING_MSG_LENGTH + 3; i++)
  {
    Msg->Data[i] = 0;
  }
  Msg->Len = 3;

}

/**
  * @brief  Handle a message
  * @param  Msg the pointer to the message to be handled
  * @retval 1 if the message is correctly handled, 0 otherwise
  */
int HandleMSG(TMsg *Msg)
//  DestAddr | SouceAddr | CMD | PAYLOAD
//      1          1        1       N
{
  uint32_t i;

  if (Msg->Len < 2) return 0;
  if (Msg->Data[0] != DEV_ADDR) return 0;
  switch (Msg->Data[2])   // CMD
  {   
  case CMD_Enter_DFU_Mode:
    if (Msg->Len != 3) return 0;
    BUILD_REPLY_HEADER(Msg);
    Msg->Len = 3;
    return 1;
    
  case CMD_Read_PresString:
    if (Msg->Len != 3) return 0;
    BUILD_REPLY_HEADER(Msg);
    i = 0;
    while (i < (sizeof(PresentationString) - 1))
    {
      Msg->Data[3 + i] = PresentationString[i];
      i++;
    }
    Msg->Len = 3 + i;
    UART_SendMsg(Msg);
    return 1;
    
  case CMD_CheckModeSupport:
    if (Msg->Len < 3) return 0;
    BUILD_REPLY_HEADER(Msg);
    Serialize_s32(&Msg->Data[3], DATALOG_FUSION_MODE, 4);
    Msg->Len = 3 + 4;
    UART_SendMsg(Msg);
    return 1;
    
  case CMD_Start_Data_Streaming:
    if (Msg->Len < 3) return 0;
    Sensors_Enabled = Deserialize(&Msg->Data[3], 4);
    DataLoggerActive = 1;
    DataStreamingDest = Msg->Data[1];
    BUILD_REPLY_HEADER(Msg);
    Msg->Len = 3;
    if(reset_INT())
    {
      Msg->Data[2] = 0;
      return 0;
    }
    UART_SendMsg(Msg);
    return 1;
    
  case CMD_Stop_Data_Streaming:
    if (Msg->Len < 3) return 0;
    Sensors_Enabled = 0;
    DataLoggerActive = 0;
    BUILD_REPLY_HEADER(Msg);
    UART_SendMsg(Msg);
    return 1;

  case CMD_SUBCMD_SENSOR:
    if (Msg->Len < 3) return 0;
    if (Msg->Data[4] != SUBCMD_SENSOR_ACC) return 0;
    return Handle_SUBCMD(Msg);    
    
  default:
    return 0;
  }
}


uint8_t Handle_SUBCMD( TMsg *Msg )
{
  uint8_t retVal = 1;
  
  switch(Msg->Data[3]) /* SUBCMD */
  {    
  case SUBCMD_GET_SENSOR_NAME:
    {
      uint8_t instance;
      
      if( BSP_ACCELERO_Get_Instance( ACCELERO_handle, &instance ) != COMPONENT_OK )
      {
        retVal = 0;
        send_ErrorMsg(Msg);
      } else
      {
        Send_Sensor_Name(Msg, (uint8_t*)sensor_setting[instance].name);
      }
    } break;
    
    
//  case SUBCMD_READ_REGISTER:
//    {
//      uint8_t reg_value;
//      
//      if( BSP_ACCELERO_Read_Reg( ACCELERO_handle, Msg->Data[5], &reg_value ) != COMPONENT_OK )
//      {
//        retVal = 0;
//        break;
//      }
//      
//      BUILD_REPLY_HEADER(Msg);
//      Msg->Data[6] = reg_value;
//      Msg->Len = 7;
//      UART_SendMsg(Msg);
//    } break;
//    
//    
//  case SUBCMD_WRITE_REGISTER:
//    {
//      if( BSP_ACCELERO_Write_Reg( ACCELERO_handle, Msg->Data[5], Msg->Data[6] ) != COMPONENT_OK )
//      {
//        retVal = 0;
//        break;
//      }
//      
//      BUILD_REPLY_HEADER(Msg);
//      Msg->Len = 7;
//      UART_SendMsg(Msg);
//    } break;
    
    
  case SUBCMD_GET_FS_LIST:
    {
      uint8_t instance;
      
      if( BSP_ACCELERO_Get_Instance( ACCELERO_handle, &instance ) != COMPONENT_OK )
      {
        retVal = 0;
        send_ErrorMsg(Msg);
      } else
      {
        Send_Sensor_fs_list(Msg, (uint32_t *)sensor_setting[instance].fs_list);
      }
    } break;
    
    
  case SUBCMD_SET_FS_INDEX:
    {
      if( set_full_scale( Msg->Data[5] ) )
      {
        retVal = 0;
        Msg->Data[5] = 0;
      } else
      {
        if( !getActualFS_index( &Msg->Data[5] ) )
        {
          retVal = 0;
          Msg->Data[5] = 0;
        }
      }
      
      BUILD_REPLY_HEADER(Msg);
      Msg->Len = 6;
      UART_SendMsg(Msg);
    } break;
    
    
  case SUBCMD_GET_ODR_LIST:
    {
      uint8_t instance;
      
      if( BSP_ACCELERO_Get_Instance( ACCELERO_handle, &instance ) != COMPONENT_OK )
      {
        retVal = 0;
        send_ErrorMsg(Msg);
      } else
      {
        Send_Sensor_odr_list(Msg, (float *)sensor_setting[instance].odr_om_list);
      }
    } break;
    
    
  case SUBCMD_SET_ODR_INDEX:
    {
      uint8_t index;
      
      index = get_ODR_index(Msg->Data[5]);
      
      if( index == 0)
      {
        send_ErrorMsg(Msg);
        break;
      }
      
      if( set_odr(index) )
      {
        retVal = 0;
        Msg->Data[5] = 0;
      } else
      {
        if( !getActualODR_index( &Msg->Data[5] ) )
        {
          retVal = 0;
          Msg->Data[5] = 0;
        }
      }
      
      BUILD_REPLY_HEADER(Msg);
      Msg->Len = 6;
      UART_SendMsg(Msg);
    } break;

    
  case SUBCMD_SET_SENSING_AXIS:
    {
      if( set_active_axis(Msg->Data[5]) )
      {
        retVal = 0;
        Msg->Data[5] = 0;
      } else
      {
        ACTIVE_AXIS_t axis_active;
        
        if( BSP_ACCELERO_Get_Active_Axis_Ext( ACCELERO_handle, &axis_active ) )
        {
          retVal = 0;
          Msg->Data[5] = 0;
        } else
        {
          Msg->Data[5] = (uint8_t) axis_active + 1; 
        }
      }
      
      BUILD_REPLY_HEADER(Msg);
      Msg->Len = 6;
      UART_SendMsg(Msg);
    } break;
    
    
  case SUBCMD_GET_OP_MODE:
    {
      uint8_t instance;
      OP_MODE_t opMode;
      
      if( BSP_ACCELERO_Get_Instance( ACCELERO_handle, &instance ) != COMPONENT_OK )
      {
        retVal = 0;
        send_ErrorMsg(Msg);
      } else
      {
        if( sensor_setting[instance].opmode_list != NULL ) // NULL = opMode not supported
        {
          if( BSP_ACCELERO_Get_OpMode_Ext( ACCELERO_handle, &opMode ) != COMPONENT_OK )
          {
            retVal = 0;
            send_ErrorMsg(Msg);
          }        
        } else
        {
          opMode = NORMAL_MODE;
        }
        
        switch( opMode )
        {
        case LOW_PWR_MODE:
          Msg->Data[5] = 1;
          break;
          
        case NORMAL_MODE:
          Msg->Data[5] = 2;
          break;
          
        case HIGH_RES_MODE:
          Msg->Data[5] = 3;
          break;
          
        default:
          Msg->Data[5] = 0;
          break;
        }
        
        BUILD_REPLY_HEADER(Msg);
        Msg->Len = 6;
        UART_SendMsg(Msg);
      }      
    } break;
      
    
  case SUBCMD_SET_OP_MODE:
    {
      if( set_opmode(Msg->Data[5]) )
      {
        retVal = 0;
        Msg->Data[5] = 0;
      } else
      {
        if( !getActualOpMode_index( &Msg->Data[5] ) )
        {
          retVal = 0;
          Msg->Data[5] = 0;
        }
      }
            
      BUILD_REPLY_HEADER(Msg);
      Msg->Len = 6;
      UART_SendMsg(Msg);
    } break;
    
    
  case SUBCMD_GET_FFT_SAMPLES_LIST:
    {
      uint8_t instance;
      
      if( BSP_ACCELERO_Get_Instance( ACCELERO_handle, &instance ) != COMPONENT_OK )
      {
        retVal = 0;
        send_ErrorMsg(Msg);
      } else
      {
        Send_Samples_list(Msg, (uint32_t *)sensor_setting[instance].samples_list);
      }
    } break;

    
  case SUBCMD_SET_FFT_SAMPLES:
    {
      if( set_samples(Msg->Data[5]) )
      {
        retVal = 0;
        Msg->Data[5] = 0;
      } else
      {
        if( !getActualSamples_index( &Msg->Data[5] ) )
        {
          retVal = 0;
          Msg->Data[5] = 0;
        }
      }
         
      BUILD_REPLY_HEADER(Msg);
      Msg->Len = 6;
      UART_SendMsg(Msg);
    } break;
    
    
  case SUBCMD_SET_HP_FILTER:
    {
      if( set_hp_filter(Msg->Data[5]-1) )
      {
        retVal = 0;
        Msg->Data[5] = 0;
      } else
      {
        Msg->Data[5] = get_hp_filter() + 1;
      }
      
      BUILD_REPLY_HEADER(Msg);
      Msg->Len = 6;
      UART_SendMsg(Msg);      
    } break;
    
    
  case SUBCMD_GET_SENSOR_LIST:
    {
      Send_Avail_Sensor_List(Msg, get_sensor_list());
    } break;
    
    
  case SUBCMD_SET_SENSOR_INDEX:
    {
      uint8_t* sensor_list;
      uint8_t isEnabled;
      
      sensor_list = get_sensor_list();
      
      BSP_ACCELERO_IsEnabled( ACCELERO_handle, &isEnabled );
      
      if( Msg->Data[5] > sensor_list[0] )
      {   
        retVal = 0;
        Msg->Data[5] = 0;
      } else
      {
        BSP_ACCELERO_Set_INT1_DRDY_Ext( ACCELERO_handle, INT1_DRDY_DISABLED );
        BSP_ACCELERO_DeInit(&ACCELERO_handle);          
        if( BSP_ACCELERO_Init( (ACCELERO_ID_t) sensor_list[Msg->Data[5]], &ACCELERO_handle ) != COMPONENT_OK )
        {
          retVal = 0;
          Msg->Data[5] = 0;
        } else
        {
          if( !getActualSensor_index( &Msg->Data[5] ) )
          {
            retVal = 0;
            Msg->Data[5] = 0;
          }
          
          BSP_ACCELERO_Sensor_Enable( ACCELERO_handle );
          
          init_fft( UNICLEO );
                  
        }
      }
      
      BUILD_REPLY_HEADER(Msg);
      Msg->Len = 6;
      UART_SendMsg(Msg);
    } break;
    
  default:
    retVal = 0;
    break;
  }
  
  return retVal;
}


uint8_t getActualFS_index( uint8_t *value )
{
  float fullScale;
  uint8_t instance;
  uint8_t i;
  
  if( BSP_ACCELERO_Get_FS(ACCELERO_handle, &fullScale) != COMPONENT_OK )
  {
    return 0;
  }  
  if( BSP_ACCELERO_Get_Instance( ACCELERO_handle, &instance ) != COMPONENT_OK )
  {
    return 0;
  }
  
  for( i=0; i<=sensor_setting[instance].fs_list[0]; i++)
  {
    if( sensor_setting[instance].fs_list[i] == fullScale )
    {
      *value = i;
      return 1;
    }
  }
  
  return 0;
  
}


uint8_t getActualODR_index( uint8_t *value )
{
  float odr;
  uint8_t instance;
  OP_MODE_t opMode;
  uint8_t i;
  uint8_t bStart, bStop;
  uint8_t bound[3];
    
  if( BSP_ACCELERO_Get_ODR(ACCELERO_handle, &odr) != COMPONENT_OK )
  {
    return 0;
  }  
  if( BSP_ACCELERO_Get_Instance( ACCELERO_handle, &instance ) != COMPONENT_OK )
  {
    return 0;
  }
    
  if( sensor_setting[instance].opmode_list != NULL ) // NULL = opMode not supported
  {
    if( BSP_ACCELERO_Get_OpMode_Ext( ACCELERO_handle, &opMode ) != COMPONENT_OK )
    {
      return 0;
    }        
  } else 
  {
    opMode = NORMAL_MODE;
  }
  
  if( !findODRlistBounds(instance, bound) )
  {
    return 0;
  }
  
  switch( opMode )
  {
  case LOW_PWR_MODE:
    bStart = 0;
    bStop = bound[0];
    break;
  
  case NORMAL_MODE:
    bStart = bound[0];
    bStop = bound[1];
    break;
    
  case HIGH_RES_MODE:
    bStart = bound[1];
    bStop = (uint8_t)sensor_setting[instance].odr_om_list[0] + 1;
    break;
    
  default:
    return 0;
  }
    
  for(i=bStart+1; i<bStop; i++)
  {
    if(sensor_setting[instance].odr_om_list[i] == odr)
    {
      *value = i - bStart;
      return 1;
    }
  }
  
  return 0;
}


uint8_t getActualOpMode_index( uint8_t *value )
{
  OP_MODE_t opMode;
  uint8_t instance;
  uint8_t i;
  
  if( BSP_ACCELERO_Get_OpMode_Ext(ACCELERO_handle, &opMode) != COMPONENT_OK )
  {
    return 0;
  }  
  if( BSP_ACCELERO_Get_Instance( ACCELERO_handle, &instance ) != COMPONENT_OK )
  {
    return 0;
  }
  
  for( i=0; i<3; i++)
  {
    if( sensor_setting[instance].opmode_list[i] == opMode )
    {
      *value = i + 1;
      return 1;
    }
  }
  
  return 0;
}


uint8_t getActualSamples_index( uint8_t *value )
{
  uint16_t samples;
  uint8_t instance;
  uint8_t i;
  
  if( BSP_ACCELERO_Get_Instance( ACCELERO_handle, &instance ) != COMPONENT_OK )
  {
    return 0;
  }
  
  samples = get_samples();
  
  for(i=1; i<=sensor_setting[instance].samples_list[0]; i++)
  {
    if(samples == sensor_setting[instance].samples_list[i])
    {
      *value = i;
      return 1;
    }
  }
  
  return 0;
}


uint8_t getActualSensor_index( uint8_t *value )
{
  uint8_t instance;
  uint8_t i;
  uint8_t* sensor_list;
  
  sensor_list = get_sensor_list();
  
  if( BSP_ACCELERO_Get_Instance( ACCELERO_handle, &instance ) != COMPONENT_OK )
  {
    return 0;
  }
  
  for(i=1; i<=sensor_list[0]; i++)
  {
    if(instance == sensor_list[i])
    {
      *value = i;
      return 1;
    }
  }
  
  return 0;
}


void Send_Sensor_Name(TMsg *Msg, uint8_t* sensor_name)
{
  int i = 0;
  BUILD_REPLY_HEADER(Msg);

  while (i < strlen((char const*)sensor_name))
  {
    Msg->Data[5 + i] = sensor_name[i];
    i++;
  }

  Msg->Len = 5 + i;
  UART_SendMsg(Msg);
}


void Send_Avail_Sensor_List(TMsg *Msg, uint8_t* sensor_list)
{
  int i = 0;
  char sensorNames[64];
  char tmp[20];
  BUILD_REPLY_HEADER(Msg);
  
  memset(sensorNames, 0, sizeof(sensorNames));

  for( i = 0; i<sensor_list[0]; i++ )
  {
    snprintf(tmp, sizeof(tmp), "%s, ", sensor_setting[sensor_list[i+1]].name);
    strcat(sensorNames, tmp);
  }

  i = 0;
  
  while (i < strlen((char const*)sensorNames)-2) // -2 -> delete ", " after last sensor name
  {
    Msg->Data[5 + i] = sensorNames[i];
    i++;
  }
  
  Msg->Len = 5 + i;
  UART_SendMsg(Msg);
}


void Send_Sensor_fs_list(TMsg *Msg, uint32_t* fs_list)
{
  int i = 0;
  BUILD_REPLY_HEADER(Msg);

  Serialize(&Msg->Data[5], fs_list[0], 4);

  for (i = 0; i < fs_list[0]; i++)
  {
    Serialize(&Msg->Data[9 + i * 4], fs_list[i + 1], 4);
  }

  Msg->Len = 9 + i * 4;
  UART_SendMsg(Msg);
}


void Send_Sensor_odr_list(TMsg *Msg, float* odr_list)
{
  int i = 0;
  BUILD_REPLY_HEADER(Msg);

  Serialize(&Msg->Data[5], (int) odr_list[0], 4);

  for (i = 0; i < odr_list[0]; i++)
  {
    FloatToArray(&Msg->Data[9 + i * 4], odr_list[i + 1]);
  }

  Msg->Len = 9 + i * 4;
  UART_SendMsg(Msg);
}


void Send_Samples_list(TMsg *Msg, uint32_t* samples_list)
{
  int i = 0;
  BUILD_REPLY_HEADER(Msg);

  Serialize(&Msg->Data[5], (int) samples_list[0], 4);

  for (i = 0; i < samples_list[0]; i++)
  {
    FloatToArray(&Msg->Data[9 + i * 4], samples_list[i + 1]);
  }

  Msg->Len = 9 + i * 4;
  UART_SendMsg(Msg);
}


void send_ErrorMsg( TMsg *Msg )
{
  Msg->Data[5] = 0;
  BUILD_REPLY_HEADER(Msg);
  Msg->Len = 6;
  UART_SendMsg(Msg);
}


void SEND_INIT_ERR_MSG( TMsg *Msg )
{
  Msg->Data[0] = 1;
  Msg->Data[1] = DEV_ADDR;
  Msg->Data[2] = CMD_SUBCMD_SENSOR + CMD_Reply_Add;
  Msg->Data[3] = 0;
  Msg->Len = 4;
  UART_SendMsg(Msg);
}

void SEND_BOARD_RESTARTED_MSG( TMsg *Msg )
{
  Msg->Data[0] = 1;
  Msg->Data[1] = DEV_ADDR;
  Msg->Data[2] = CMD_SUBCMD_SENSOR + CMD_Reply_Add;
  Msg->Data[3] = SUBCMD_RESTART;
  Msg->Len = 4;
  UART_SendMsg(Msg);
}


uint8_t get_ODR_index( uint8_t value )
{
  uint8_t instance;
  OP_MODE_t opMode;
  uint8_t i;
  uint8_t bound[3];
  uint8_t tmp;
     
  if( BSP_ACCELERO_Get_Instance( ACCELERO_handle, &instance ) != COMPONENT_OK )
  {
    return 0;
  }
  
  if( sensor_setting[instance].opmode_list != NULL ) // NULL = opMode not supported
  {
    if( BSP_ACCELERO_Get_OpMode_Ext( ACCELERO_handle, &opMode ) != COMPONENT_OK )
    {
      return 0;
    }        
  } else 
  {
    opMode = NORMAL_MODE;
  }
  
  if( !findODRlistBounds(instance, bound) )
  {
    return 0;
  }
  
  switch( opMode )
  {
  case LOW_PWR_MODE:
    tmp = value;
    break;
  
  case NORMAL_MODE:
    tmp = value + bound[0];
    break;
    
  case HIGH_RES_MODE:
    tmp = value + bound[1];
    break;
    
  default:
    return 0;
  }
    
  if( tmp > sensor_setting[instance].odr_om_list[0] )
  {
    return 0;
  }
    
  for(i=0; i<=sensor_setting[instance].odr_list[0]; i++)
  {
    if(sensor_setting[instance].odr_om_list[tmp] == sensor_setting[instance].odr_list[i])
    {
      return i;
    }
  }
  
  return 0;
}


uint8_t findODRlistBounds( uint8_t instance, uint8_t *bound )
{
  uint8_t i, j;
  uint8_t tmpBound[3];
  
  memset(tmpBound, 0, sizeof(tmpBound));
  
  for(i = 0, j = 0; i < sensor_setting[instance].odr_om_list[0]; i++)
  {
    if( sensor_setting[instance].odr_om_list[i+1] == 0 )
    {
      tmpBound[j] = i+1;
      j++;
    }
  }
  
  if( tmpBound[0] == 0 || tmpBound[1] == 0 ) // not found in list
  {
    return 0;
  }
  
  for(i=0; i<2; i++)
  {
    bound[i] = tmpBound[i];
  }
  
  return 1;
}


void FloatToArray(uint8_t *Dest, float data)
{
  memcpy(Dest, (void *) &data, 4);
}


void ArrayToFloat(uint8_t *Source, float *data)
{
  memcpy((void *) data, Source, 4);
}

void UART_SendMsg(TMsg *Msg) {
}


/**
 * @}
 */

/**
 * @}
 */
