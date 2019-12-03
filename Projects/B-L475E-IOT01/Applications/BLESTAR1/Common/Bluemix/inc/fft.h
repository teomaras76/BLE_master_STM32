/**
  *******************************************************************************
  * @file    fft.h
  * @author  Martin Polacek
  * @version V4.2.0
  * @date    13-October-2017
  * @brief   header for fft.c.
  *******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
  ********************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FFT_H
#define __FFT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "serial_protocol.h"
#include "arm_math.h"

/* Defines -------------------------------------------------------------------*/

typedef enum
{
  TERMINAL,
  UNICLEO
} ProgFun;

/* Function prototypes */
extern uint8_t  init_fft( ProgFun func );
extern uint8_t  reset_INT( void );
extern void     fft_main( void );

extern float    *get_fft_data( void );
extern uint8_t  prepare_fft_data( void );

extern uint16_t get_samples( void );
extern uint32_t get_fft_max_freq( void );
extern uint32_t get_fft_max_freq_amp( void );
extern uint32_t get_meas_odr( void );
extern uint8_t  get_hp_filter( void );
extern uint8_t *get_sensor_list( void );
extern void     get_fft_msg( TMsg *Msg );

extern uint8_t  set_active_axis( uint8_t value );
extern uint8_t  set_full_scale( uint8_t value );
extern uint8_t  set_hp_filter( uint8_t value );
extern uint8_t  set_odr( uint8_t value );
extern uint8_t  set_opmode( uint8_t value );
extern uint8_t  set_samples( uint8_t value );


#ifdef __cplusplus
}
#endif

#endif /* __FFT_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
