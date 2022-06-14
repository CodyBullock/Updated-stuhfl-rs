/******************************************************************************
  * \attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2022 STMicroelectronics</center></h2>
  *
  * Licensed under ST MYLIBERTY SOFTWARE LICENSE AGREEMENT (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        www.st.com/myliberty
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
  * AND SPECIFICALLY DISCLAIMING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
******************************************************************************/


//
#if !defined __STUHFL_BL_WIN32_H
#define __STUHFL_BL_WIN32_H

#include "stuhfl.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#if defined(WIN32)

//
STUHFL_T_RET_CODE STUHFL_F_Connect_Win32(STUHFL_T_DEVICE_CTX *device, char* port, uint32_t br);
STUHFL_T_RET_CODE STUHFL_F_Reset_Win32(STUHFL_T_DEVICE_CTX *device, STUHFL_T_RESET resetType);
STUHFL_T_RET_CODE STUHFL_F_Disconnect_Win32(STUHFL_T_DEVICE_CTX *device);

STUHFL_T_RET_CODE STUHFL_F_SndRaw_Win32(STUHFL_T_DEVICE_CTX *device, uint8_t *data, uint16_t dataLen);
STUHFL_T_RET_CODE STUHFL_F_RcvRaw_Win32(STUHFL_T_DEVICE_CTX *device, uint8_t *data, uint16_t *dataLen);

STUHFL_T_RET_CODE STUHFL_F_SetDTR_Win32(STUHFL_T_DEVICE_CTX *device, uint8_t dtrValue);
STUHFL_T_RET_CODE STUHFL_F_GetDTR_Win32(STUHFL_T_DEVICE_CTX *device, uint8_t *dtrValue);

STUHFL_T_RET_CODE STUHFL_F_SetRTS_Win32(STUHFL_T_DEVICE_CTX *device, uint8_t rtsValue);
STUHFL_T_RET_CODE STUHFL_F_GetRTS_Win32(STUHFL_T_DEVICE_CTX *device, uint8_t *rtsValue);

STUHFL_T_RET_CODE STUHFL_F_SetTimeouts_Win32(STUHFL_T_DEVICE_CTX *device, uint32_t rdTimeout, uint32_t wrTimeout);
STUHFL_T_RET_CODE STUHFL_F_GetTimeouts_Win32(STUHFL_T_DEVICE_CTX *device, uint32_t *rdTimeout, uint32_t *wrTimeout);

#endif

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __STUHFL_BL_WIN32_H
