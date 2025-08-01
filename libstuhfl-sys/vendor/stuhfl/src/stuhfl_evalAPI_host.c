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

/** @file
 *
 *  @author ST Microelectronics
 *
 *  @brief Main dispatcher for streaming protocol uses a stream_driver for the Host
 *
 */

/** @addtogroup Application
  * @{
  */
/** @addtogroup PC_Communication
  * @{
  */

#include "stuhfl.h"
#include "stuhfl_sl.h"
#include "stuhfl_sl_gen2.h"
#include "stuhfl_sl_gb29768.h"
#include "stuhfl_sl_iso6b.h"
#include "stuhfl_al.h"
#include "stuhfl_pl.h"
#include "stuhfl_dl.h"
#include "stuhfl_dl_ST25RU3993.h"
#include "stuhfl_evalAPI.h"
#include "stuhfl_err.h"
#include "stuhfl_helpers.h"
#include "stuhfl_log.h"

//
#define TRACE_EVAL_API_CLEAR()    { STUHFL_F_LogClear(STUHFL_D_LOG_LEVEL_TRACE_EVAL_API); }
#define TRACE_EVAL_API_APPEND(...){ STUHFL_F_LogAppend(STUHFL_D_LOG_LEVEL_TRACE_EVAL_API, __VA_ARGS__); }
#define TRACE_EVAL_API_FLUSH()    { STUHFL_F_LogFlush(STUHFL_D_LOG_LEVEL_TRACE_EVAL_API); }
//
#define TRACE_EVAL_API_START()      { STUHFL_F_LogClear(STUHFL_D_LOG_LEVEL_TRACE_EVAL_API); }
#define TRACE_EVAL_API(...)         { STUHFL_F_LogAppend(STUHFL_D_LOG_LEVEL_TRACE_EVAL_API, __VA_ARGS__); STUHFL_F_LogFlush(STUHFL_D_LOG_LEVEL_TRACE_EVAL_API); }

//
STUHFL_T_DEVICE_CTX device = 0;
#define SND_BUFFER_SIZE         (STUHFL_D_UART_RX_BUFFER_SIZE)   /* HOST SND buffer based on FW RCV buffer */
#define RCV_BUFFER_SIZE         (STUHFL_D_UART_TX_BUFFER_SIZE)   /* HOST RCV buffer based on FW SND buffer */
uint8_t sndData[SND_BUFFER_SIZE];
uint16_t sndDataLen = SND_BUFFER_SIZE;
uint8_t rcvData[RCV_BUFFER_SIZE];
uint16_t rcvDataLen = RCV_BUFFER_SIZE;

STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Connect(char *szComPort)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_PORT, (STUHFL_T_PARAM_VALUE)szComPort);
    retCode |= STUHFL_F_Connect(&device, sndData, SND_BUFFER_SIZE, rcvData, RCV_BUFFER_SIZE);
    // enable data line
    uint8_t on = TRUE;
    retCode |= STUHFL_F_SetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_DTR, (STUHFL_T_PARAM_VALUE)&on);
    // toogle reset line
    on = TRUE;
    retCode |= STUHFL_F_SetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_RTS, (STUHFL_T_PARAM_VALUE)&on);
    on = FALSE;
    retCode |= STUHFL_F_SetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_RTS, (STUHFL_T_PARAM_VALUE)&on);
    TRACE_EVAL_API("Connect(Port: %s) = %d", szComPort, retCode);
    return retCode;
}

STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Disconnect()
{
    return STUHFL_F_Disconnect();
}

// ---- Getter Generic ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_BoardVersion(STUHFL_T_Version *swVersion, STUHFL_T_Version *hwVersion)
{
    memset(swVersion, 0, sizeof(STUHFL_T_Version));
    memset(hwVersion, 0, sizeof(STUHFL_T_Version));
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetVersion(&swVersion->major, &swVersion->minor, &swVersion->micro, &swVersion->nano,
                                &hwVersion->major, &hwVersion->minor, &hwVersion->micro, &hwVersion->nano);

    TRACE_EVAL_API("Get_BoardVersion(swVersion: %d.%d.%d.%d, HW_Version: %d.%d.%d.%d) = %d", swVersion->major, swVersion->minor, swVersion->micro, swVersion->nano,
                   hwVersion->major, hwVersion->minor, hwVersion->micro, hwVersion->nano, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_BoardInfo(STUHFL_T_VersionInfo *swInfo, STUHFL_T_VersionInfo *hwInfo)
{
    memset(swInfo, 0, sizeof(STUHFL_T_VersionInfo));
    memset(hwInfo, 0, sizeof(STUHFL_T_VersionInfo));
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetInfo((char*)swInfo, (char*)hwInfo);
    swInfo->info[swInfo->infoLength] = 0;
    hwInfo->info[hwInfo->infoLength] = 0;
    TRACE_EVAL_API("Get_BoardInfo(SW Info: %s, HW Info: %s) = %d", swInfo->info, hwInfo->info, retCode);
    return retCode;
}

STUHFL_DLL_API void CALL_CONV Reboot()
{
    TRACE_EVAL_API_START();
    STUHFL_F_Reboot();
    TRACE_EVAL_API("Reboot()");
}
STUHFL_DLL_API void CALL_CONV EnterBootloader()
{
    TRACE_EVAL_API_START();
    STUHFL_F_EnterBootloader();
    TRACE_EVAL_API("EnterBootloader()");
}

// ---- Setter Register ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Register(STUHFL_T_ST25RU3993_Register *reg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_REGISTER, (STUHFL_T_PARAM_VALUE)reg);
    TRACE_EVAL_API("Set_Register(addr: 0x%02x, data: 0x%02x) = %d", reg->addr, reg->data, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_RegisterMultiple(STUHFL_T_ST25RU3993_Register **reg, uint8_t numReg)
{
    STUHFL_T_PARAM params[256];
    for (int i = 0; i < numReg; i++) {
        params[i] = STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_REGISTER;
    }
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetMultipleParams(numReg, params, (STUHFL_T_PARAM_VALUE *)*reg);
#define TB_SIZE    1024U
    char tb[TB_SIZE];
    int j = 0;
    for (int i = 0; i < numReg; i++) {
        j += snprintf(tb + j, (uint32_t)(TB_SIZE - (uint32_t)j), "[%02X, %02X] ", (*reg)[i].addr, (*reg)[i].data);
    }
    TRACE_EVAL_API("Set_Register([addr, data]: %s) = %d", tb, retCode);
#undef TB_SIZE
    return retCode;
}

// ---- Getter Register ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Register(STUHFL_T_ST25RU3993_Register *reg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_REGISTER, (STUHFL_T_PARAM_VALUE)reg);
    TRACE_EVAL_API("Get_Register(addr: 0x%02x, data: 0x%02x) = %d", reg->addr, reg->data, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_RegisterMultiple(uint8_t numReg, STUHFL_T_ST25RU3993_Register **reg)
{
    STUHFL_T_PARAM params[256];
    for (int i = 0; i < numReg; i++) {
        params[i] = STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_REGISTER;
    }
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetMultipleParams(numReg, params, (STUHFL_T_PARAM_VALUE *)*reg);
#define TB_SIZE    1024U
    char tb[TB_SIZE];
    int j = 0;
    for (int i = 0; i < numReg; i++) {
        j += snprintf(tb + j, (uint32_t)(TB_SIZE - (uint32_t)j), "[%02X, %02X] ", (*reg)[i].addr, (*reg)[i].data);
    }
    TRACE_EVAL_API("Get_Register([addr, data]: %s) = %d", tb, retCode);
#undef TB_SIZE
    return retCode;
}



// ---- Setter RwdCfg ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_RwdCfg(STUHFL_T_ST25RU3993_RwdConfig *rwdCfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_CONFIG, (STUHFL_T_PARAM_VALUE)rwdCfg);
    TRACE_EVAL_API("Set_RwdCfg(id: 0x%02x, value: 0x%02x) = %d", rwdCfg->id, rwdCfg->value, retCode);
    return retCode;
}

// ---- Getter RwdCfg ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_RwdCfg(STUHFL_T_ST25RU3993_RwdConfig *rwdCfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_CONFIG, (STUHFL_T_PARAM_VALUE)rwdCfg);
#ifdef CHECK_API_INTEGRITY
    if ((retCode == STUHFL_ERR_NONE)
            && (   (rwdCfg->id > STUHFL_D_RWD_CFG_ID_HARDWARE_ID_NUM)
                   || ((rwdCfg->id == STUHFL_D_RWD_CFG_ID_EXTVCO) && (rwdCfg->value > 1))
                   || ((rwdCfg->id == STUHFL_D_RWD_CFG_ID_INPUT) && (rwdCfg->value > 1))
                   || ((rwdCfg->id == STUHFL_D_RWD_CFG_ID_ANTENNA_SWITCH) && (rwdCfg->value > 1))
                   || ((rwdCfg->id == STUHFL_D_RWD_CFG_ID_TUNER) && (rwdCfg->value & ~(STUHFL_D_TUNER_CIN | STUHFL_D_TUNER_CLEN | STUHFL_D_TUNER_COUT)))
                   || ((rwdCfg->id == STUHFL_D_RWD_CFG_ID_HARDWARE_ID_NUM) && (rwdCfg->value > STUHFL_HWID_ELANCE))
               )) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif
    TRACE_EVAL_API("Get_RwdCfg(id: 0x%02x, value: 0x%02x) = %d", rwdCfg->id, rwdCfg->value, retCode);
    return retCode;
}

// ---- Setter RxFilter & Calibration ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Gen2_RxFilter(STUHFL_T_ST25RU3993_RxFilter *rxFilter)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_GEN2_RX_FILTER, (STUHFL_T_PARAM_VALUE)rxFilter);
    TRACE_EVAL_API("Set_Gen2_RxFilter(blf: 0x%02x, coding: 0x%02x, rxFilter: 0x%02x) = %d", rxFilter->blf, rxFilter->coding, rxFilter->value, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Gen2_RxFilterMultiple(uint8_t numRxFilter, STUHFL_T_ST25RU3993_RxFilter **rxFilter)
{
    STUHFL_T_PARAM params[256];
    for (int i = 0; i < numRxFilter; i++) {
        params[i] = STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_GEN2_RX_FILTER;
    }
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetMultipleParams(numRxFilter, params, (STUHFL_T_PARAM_VALUE *)*rxFilter);
#define TB_SIZE    1024U
    char tb[TB_SIZE];
    int j = 0;
    for (int i = 0; i < numRxFilter; i++) {
        j += snprintf(tb + j, (uint32_t)(TB_SIZE - (uint32_t)j), "[%02X, %02X, %02X] ", (*rxFilter)[i].blf, (*rxFilter)[i].coding, (*rxFilter)[i].value);
    }
    TRACE_EVAL_API("Set_Gen2_RxFilter([blf, coding, rxFilter]: %s) = %d", tb, retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Gb29768_RxFilter(STUHFL_T_ST25RU3993_RxFilter *rxFilter)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_GB29768_RX_FILTER, (STUHFL_T_PARAM_VALUE)rxFilter);
    TRACE_EVAL_API("Set_Gb29768_RxFilter(blf: 0x%02x, coding: 0x%02x, rxFilter: 0x%02x) = %d", rxFilter->blf, rxFilter->coding, rxFilter->value, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Gen2_FilterCalibration(STUHFL_T_ST25RU3993_FilterCalibration *fCal)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_GEN2_FILTER_CALIBRATION, (STUHFL_T_PARAM_VALUE)fCal);
    TRACE_EVAL_API("Set_Gen2_FilterCalibration(blf: 0x%02x, coding: 0x%02x, highPass: 0x%02x, lowPass: 0x%02x) = %d", fCal->blf, fCal->coding, fCal->highPass, fCal->lowPass, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Gen2_FilterCalibrationMultiple(uint8_t numFCal, STUHFL_T_ST25RU3993_FilterCalibration **fCal)
{
    STUHFL_T_PARAM params[256];
    for (int i = 0; i < numFCal; i++) {
        params[i] = STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_GEN2_FILTER_CALIBRATION;
    }
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetMultipleParams(numFCal, params, (STUHFL_T_PARAM_VALUE *)*fCal);
#define TB_SIZE    1024U
    char tb[TB_SIZE];
    int j = 0;
    for (int i = 0; i < numFCal; i++) {
        j += snprintf(tb + j, (uint32_t)(TB_SIZE - (uint32_t)j), "[%02X, %02X, %02X, %02X] ", (*fCal)[i].blf, (*fCal)[i].coding, (*fCal)[i].highPass, (*fCal)[i].lowPass);
    }
    TRACE_EVAL_API("Set_Gen2_FilterCalibration([blf, coding, highPass, lowPass]: %s) = %d", tb, retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Gb29768_FilterCalibration(STUHFL_T_ST25RU3993_FilterCalibration *fCal)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_GB29768_FILTER_CALIBRATION, (STUHFL_T_PARAM_VALUE)fCal);
    TRACE_EVAL_API("Set_Gb29768_FilterCalibration(blf: 0x%02x, coding: 0x%02x, highPass: 0x%02x, lowPass: 0x%02x) = %d", fCal->blf, fCal->coding, fCal->highPass, fCal->lowPass, retCode);
    return retCode;
}

// ---- Getter RxFilter & Calibration ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Gen2_RxFilter(STUHFL_T_ST25RU3993_RxFilter *rxFilter)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_GEN2_RX_FILTER, (STUHFL_T_PARAM_VALUE)rxFilter);
    TRACE_EVAL_API("Get_Gen2_RxFilter(blf: 0x%02x, coding: 0x%02x, rxFilter: 0x%02x) = %d", rxFilter->blf, rxFilter->coding, rxFilter->value, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Gen2_RxFilterMultiple(uint8_t numRxFilter, STUHFL_T_ST25RU3993_RxFilter **rxFilter)
{
    STUHFL_T_PARAM params[256];
    for (int i = 0; i < numRxFilter; i++) {
        params[i] = STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_GEN2_RX_FILTER;
    }
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetMultipleParams(numRxFilter, params, (STUHFL_T_PARAM_VALUE *)*rxFilter);
#define TB_SIZE    1024U
    char tb[TB_SIZE];
    int j = 0;
    for (int i = 0; i < numRxFilter; i++) {
        j += snprintf(tb + j, (uint32_t)(TB_SIZE - (uint32_t)j), "[%02X, %02X, %02X] ", (*rxFilter)[i].blf, (*rxFilter)[i].coding, (*rxFilter)[i].value);
    }
    TRACE_EVAL_API("Get_Gen2_RxFilter([blf, coding, rxFilter]: %s) = %d", tb, retCode);
#undef TB_SIZE
    return retCode;
}

STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Gb29768_RxFilter(STUHFL_T_ST25RU3993_RxFilter *rxFilter)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_GB29768_RX_FILTER, (STUHFL_T_PARAM_VALUE)rxFilter);
    TRACE_EVAL_API("Get_Gb29768_RxFilter(blf: 0x%02x, coding: 0x%02x, rxFilter: 0x%02x) = %d", rxFilter->blf, rxFilter->coding, rxFilter->value, retCode);
    return retCode;
}

STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Gen2_FilterCalibration(STUHFL_T_ST25RU3993_FilterCalibration *fCal)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_GEN2_FILTER_CALIBRATION, (STUHFL_T_PARAM_VALUE)fCal);
    TRACE_EVAL_API("Get_Gen2_FilterCalibration(blf: 0x%02x, coding: 0x%02x, highPass: 0x%02x, lowPass: 0x%02x) = %d", fCal->blf, fCal->coding, fCal->highPass, fCal->lowPass, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Gen2_FilterCalibrationMultiple(uint8_t numFCal, STUHFL_T_ST25RU3993_FilterCalibration **fCal)
{
    STUHFL_T_PARAM params[256];
    for (int i = 0; i < numFCal; i++) {
        params[i] = STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_GEN2_FILTER_CALIBRATION;
    }
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetMultipleParams(numFCal, params, (STUHFL_T_PARAM_VALUE *)*fCal);
#define TB_SIZE    1024U
    char tb[TB_SIZE];
    int j = 0;
    for (int i = 0; i < numFCal; i++) {
        j += snprintf(tb + j, (uint32_t)(TB_SIZE - (uint32_t)j), "[%02X, %02X, %02X, %02X] ", (*fCal)[i].blf, (*fCal)[i].coding, (*fCal)[i].highPass, (*fCal)[i].lowPass);
    }
    TRACE_EVAL_API("Get_Gen2_FilterCalibration([blf, coding, highPass, lowPass]: %s) = %d", tb, retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Gb29768_FilterCalibration(STUHFL_T_ST25RU3993_FilterCalibration *fCal)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_GB29768_FILTER_CALIBRATION, (STUHFL_T_PARAM_VALUE)fCal);
    TRACE_EVAL_API("Get_Gb29768_FilterCalibration(blf: 0x%02x, coding: 0x%02x, highPass: 0x%02x, lowPass: 0x%02x) = %d", fCal->blf, fCal->coding, fCal->highPass, fCal->lowPass, retCode);
    return retCode;
}

// ---- Setter Antenna Power ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_AntennaPower(STUHFL_T_ST25RU3993_AntennaPower *antPwr)
{
    // depending on the timeout it might be longer as the communication timeout..
    uint32_t rdTimeOut = 4000;
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_RD_TIMEOUT_MS, (STUHFL_T_PARAM_VALUE)&rdTimeOut);
    uint32_t tmpRdTimeOut = antPwr->timeout + 4000U;
    if (STUHFL_ERR_NONE == STUHFL_F_SetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_RD_TIMEOUT_MS, (STUHFL_T_PARAM_VALUE)&tmpRdTimeOut)) {
        // set power..
        retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_ANTENNA_POWER, (STUHFL_T_PARAM_VALUE)antPwr);
        // revert max timeout
        retCode |= STUHFL_F_SetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_RD_TIMEOUT_MS, (STUHFL_T_PARAM_VALUE)&rdTimeOut);
    }
    TRACE_EVAL_API("Set_AntennaPower(mode: 0x%02x timeout: %d, frequency: %d) = %d", antPwr->mode, antPwr->timeout, antPwr->frequency, retCode);
    return retCode;
}

// ---- Getter Antenna Power ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_AntennaPower(STUHFL_T_ST25RU3993_AntennaPower *antPwr)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_ANTENNA_POWER, (STUHFL_T_PARAM_VALUE)antPwr);
#ifdef CHECK_API_INTEGRITY
    if ((retCode == STUHFL_ERR_NONE)
            && (   ((antPwr->mode != STUHFL_D_ANTENNA_POWER_MODE_OFF) && (antPwr->mode != STUHFL_D_ANTENNA_POWER_MODE_ON))
                   || (antPwr->frequency > STUHFL_D_FREQUENCY_MAX_VALUE)
               )) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif
    TRACE_EVAL_API("Get_AntennaPower(mode: 0x%02x timeout: %d, frequency: %d) = %d", antPwr->mode, antPwr->timeout, antPwr->frequency, retCode);
    return retCode;
}




// ---- Setter Frequency ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_ChannelList(STUHFL_T_ST25RU3993_ChannelList *channelList)
{
    TRACE_EVAL_API_CLEAR();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_CHANNEL_LIST, (STUHFL_T_PARAM_VALUE)channelList);
    TRACE_EVAL_API_APPEND("Set_ChannelList(numFrequencies: %d, channelListIdx: %d, persistent: %d, idx:{Freq, ANTENNA 1(cin,clen,cout), ANTENNA 2(cin,clen,cout)} = ", channelList->numFrequencies, channelList->channelListIdx, channelList->persistent);
    for (int i = 0; i < channelList->numFrequencies; i++) {
        TRACE_EVAL_API_APPEND("%d:{%d, (%d,%d,%d), (%d,%d,%d)}, ", i, channelList->itemList[i].frequency,
            channelList->itemList[i].caps[0].cin, channelList->itemList[i].caps[0].clen, channelList->itemList[i].caps[0].cout,
            channelList->itemList[i].caps[1].cin, channelList->itemList[i].caps[1].clen, channelList->itemList[i].caps[1].cout);
    }
    TRACE_EVAL_API_APPEND(") = %d", retCode);
    TRACE_EVAL_API_FLUSH();
    return retCode;
}
#if OLD_FREQUENCY_TUNING
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_FreqProfile(STUHFL_T_ST25RU3993_FreqProfile *freqProfile)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_FREQ_PROFILE, (STUHFL_T_PARAM_VALUE)freqProfile);
    TRACE_EVAL_API("Set_FreqProfile(profile: %d) = %d", freqProfile->profile, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_FreqProfileAddToCustom(STUHFL_T_ST25RU3993_FreqProfileAddToCustom *freqProfileAdd)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_FREQ_PROFILE_ADDTOCUSTOM, (STUHFL_T_PARAM_VALUE)freqProfileAdd);
    TRACE_EVAL_API("Set_FreqProfileAddToCustom(clearList: %d, frequency: %d) = %d", freqProfileAdd->clearList, freqProfileAdd->frequency, retCode);
    return retCode;
}
#endif
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_FreqHop(STUHFL_T_ST25RU3993_FreqHop *freqHop)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_FREQ_HOP, (STUHFL_T_PARAM_VALUE)freqHop);
    TRACE_EVAL_API("Set_FreqHop(maxSendingTime: %d, minSendingTime: %d, mode: %d) = %d", freqHop->maxSendingTime, freqHop->minSendingTime, freqHop->mode, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_FreqLBT(STUHFL_T_ST25RU3993_FreqLBT *freqLBT)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_FREQ_LBT, (STUHFL_T_PARAM_VALUE)freqLBT);
    TRACE_EVAL_API("Set_FreqLBT(listeningTime: %d, idleTime: %d, rssiLogThreshold: %d, skipLBTcheck: %d) = %d", freqLBT->listeningTime, freqLBT->idleTime, freqLBT->rssiLogThreshold, freqLBT->skipLBTcheck, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_FreqContinuousModulation(STUHFL_T_ST25RU3993_FreqContinuousModulation *freqContMod)
{
    uint32_t rdTimeOut = 4000;
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_RD_TIMEOUT_MS, (STUHFL_T_PARAM_VALUE)&rdTimeOut);
    uint32_t tmpRdTimeOut = freqContMod->maxSendingTime + 4000U;
    retCode |= STUHFL_F_SetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_RD_TIMEOUT_MS, (STUHFL_T_PARAM_VALUE)&tmpRdTimeOut);
    //
    retCode |= STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_FREQ_CONTINUOUS_MODULATION, (STUHFL_T_PARAM_VALUE)freqContMod);
    // revert max timeout
    retCode |= STUHFL_F_SetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_RD_TIMEOUT_MS, (STUHFL_T_PARAM_VALUE)&rdTimeOut);
    TRACE_EVAL_API("Set_FreqContinuousModulation(frequency: %d, enable : %d, maxSendingTime : %d, mode : %d) = %d", freqContMod->frequency, freqContMod->enable, freqContMod->maxSendingTime, freqContMod->mode, retCode);
    return retCode;
}

// ---- Getter Frequency ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_ChannelList(STUHFL_T_ST25RU3993_ChannelList *channelList)
{
    TRACE_EVAL_API_CLEAR();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_CHANNEL_LIST, (STUHFL_T_PARAM_VALUE)channelList);
#ifdef CHECK_API_INTEGRITY
    if (retCode == STUHFL_ERR_NONE) {
        bool isOk = (channelList->numFrequencies <= STUHFL_D_MAX_FREQUENCY) ? true : false;
        for (uint32_t i=0 ; (i<channelList->numFrequencies) && (isOk) ; i++) {
            isOk = (channelList->itemList[i].frequency <= STUHFL_D_FREQUENCY_MAX_VALUE) ? true : false;
        }
        if (   (!isOk)
            || (channelList->channelListIdx > STUHFL_D_MAX_FREQUENCY)
#if ELANCE
            || ((channelList->antenna != STUHFL_D_ANTENNA_ALT) && (channelList->antenna > STUHFL_D_ANTENNA_4))
#else
            || ((channelList->antenna != STUHFL_D_ANTENNA_ALT) && (channelList->antenna > STUHFL_D_ANTENNA_2))
#endif
           ) {
            return (STUHFL_T_RET_CODE)STUHFL_ERR_PROTO;
        }
    }
#endif
    TRACE_EVAL_API_APPEND("Get_ChannelList(numFrequencies: %d, channelListIdx: %d, persistent: %d, idx:{Freq, ANTENNA 1(cin,clen,cout), ANTENNA 2(cin,clen,cout)} = ", channelList->numFrequencies, channelList->channelListIdx, channelList->persistent);
    for (int i = 0; i < channelList->numFrequencies; i++) {
        TRACE_EVAL_API_APPEND("%d:{%d, (%d,%d,%d), (%d,%d,%d)}, ", i, channelList->itemList[i].frequency,
            channelList->itemList[i].caps[0].cin, channelList->itemList[i].caps[0].clen, channelList->itemList[i].caps[0].cout,
            channelList->itemList[i].caps[1].cin, channelList->itemList[i].caps[1].clen, channelList->itemList[i].caps[1].cout);
    }
    TRACE_EVAL_API_APPEND(") = %d", retCode);
    TRACE_EVAL_API_FLUSH();
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_FreqRSSI(STUHFL_T_ST25RU3993_FreqRssi *freqRSSI)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_FREQ_RSSI, (STUHFL_T_PARAM_VALUE)freqRSSI);
#ifdef CHECK_API_INTEGRITY
    if ((retCode == STUHFL_ERR_NONE)
            && (   (freqRSSI->frequency > STUHFL_D_FREQUENCY_MAX_VALUE)
                   || (freqRSSI->rssiLogI > 15)
                   || (freqRSSI->rssiLogQ > 15)
               )) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif
    TRACE_EVAL_API("Get_FreqRSSI(frequency: %d, rssiLogI: %d, rssiLogQ: %d) = %d", freqRSSI->frequency, freqRSSI->rssiLogI, freqRSSI->rssiLogQ, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_FreqReflectedPower(STUHFL_T_ST25RU3993_FreqReflectedPowerInfo *freqReflectedPower)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_FREQ_REFLECTED, (STUHFL_T_PARAM_VALUE)freqReflectedPower);
#ifdef CHECK_API_INTEGRITY
    if ((retCode == STUHFL_ERR_NONE)
            && ((freqReflectedPower->frequency > STUHFL_D_FREQUENCY_MAX_VALUE)
               )) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif
    TRACE_EVAL_API("Get_FreqReflectedPower(frequency: %d, applyTunerSetting: %d, reflectedI: %d, reflectedQ: %d) = %d", freqReflectedPower->frequency, freqReflectedPower->applyTunerSetting, freqReflectedPower->reflectedI, freqReflectedPower->reflectedQ, retCode);
    return retCode;
}
#if OLD_FREQUENCY_TUNING
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_FreqProfileInfo(STUHFL_T_ST25RU3993_FreqProfileInfo *freqProfileInfo)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_FREQ_PROFILE_INFO, (STUHFL_T_PARAM_VALUE)freqProfileInfo);
#ifdef CHECK_API_INTEGRITY
    if ((retCode == STUHFL_ERR_NONE)
            && (   (freqProfileInfo->profile >= STUHFL_D_NUM_SAVED_PROFILES)
                   || (freqProfileInfo->minFrequency > STUHFL_D_FREQUENCY_MAX_VALUE)
                   || (freqProfileInfo->maxFrequency > STUHFL_D_FREQUENCY_MAX_VALUE)
                   || (freqProfileInfo->numFrequencies > STUHFL_D_MAX_FREQUENCY)
               )) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif
    TRACE_EVAL_API("Get_FreqProfileInfo(profile: %d, minFreq : %d, maxFreq: %d, numFrequencies : %d) = %d", freqProfileInfo->profile, freqProfileInfo->minFrequency, freqProfileInfo->maxFrequency, freqProfileInfo->numFrequencies, retCode);
    return retCode;
}
#endif
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_FreqHop(STUHFL_T_ST25RU3993_FreqHop *freqHop)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_FREQ_HOP, (STUHFL_T_PARAM_VALUE)freqHop);
#ifdef CHECK_API_INTEGRITY
    if ((retCode == STUHFL_ERR_NONE)
            && ((freqHop->maxSendingTime < 40)
               )) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif
    TRACE_EVAL_API("Get_FreqHop(maxSendingTime: %d, minSendingTime: %d, mode: %d) = %d", freqHop->maxSendingTime, freqHop->minSendingTime, freqHop->mode, retCode);
    return retCode;
}

STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_FreqLBT(STUHFL_T_ST25RU3993_FreqLBT *freqLBT)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_FREQ_LBT, (STUHFL_T_PARAM_VALUE)freqLBT);
    TRACE_EVAL_API("Get_FreqLBT(listeningTime: %d, idleTime: %d, rssiLogThreshold: %d, skipLBTcheck: %d) = %d", freqLBT->listeningTime, freqLBT->idleTime, freqLBT->rssiLogThreshold, freqLBT->skipLBTcheck, retCode);
    return retCode;
}



// ---- Setter SW Cfg ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Gen2_Timings(STUHFL_T_ST25RU3993_Gen2_Timings *gen2Timings)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_GEN2_TIMINGS, (STUHFL_T_PARAM_VALUE)gen2Timings);
    TRACE_EVAL_API("Set_Gen2_Timings(T4Min: %d) = %d", gen2Timings->T4Min, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Gen2_ProtocolCfg(STUHFL_T_ST25RU3993_Gen2_ProtocolCfg *gen2ProtocolCfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_GEN2_PROTOCOL_CFG, (STUHFL_T_PARAM_VALUE)gen2ProtocolCfg);
    TRACE_EVAL_API("Set_Gen2_ProtocolCfg(tari: %d, blf: %d, coding: %d, trext: %d) = %d",
                   gen2ProtocolCfg->tari, gen2ProtocolCfg->blf, gen2ProtocolCfg->coding, gen2ProtocolCfg->trext, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Gb29768_ProtocolCfg(STUHFL_T_ST25RU3993_Gb29768_ProtocolCfg *gb29768ProtocolCfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_GB29768_PROTOCOL_CFG, (STUHFL_T_PARAM_VALUE)gb29768ProtocolCfg);
    TRACE_EVAL_API("Set_Gb29768_ProtocolCfg(tc: %d, blf: %d, coding: %d, trext: %d) = %d",
                   gb29768ProtocolCfg->tc, gb29768ProtocolCfg->blf, gb29768ProtocolCfg->coding, gb29768ProtocolCfg->trext, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Iso6b_ProtocolCfg(STUHFL_T_ST25RU3993_Iso6b_ProtocolCfg *iso6bProtocolCfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_ISO6B_PROTOCOL_CFG, (STUHFL_T_PARAM_VALUE)iso6bProtocolCfg);
    TRACE_EVAL_API("Set_Iso6b_ProtocolCfg(blf: %d) = %d", iso6bProtocolCfg->blf, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_TxRxCfg(STUHFL_T_ST25RU3993_TxRxCfg *txRxCfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_TXRX_CFG, (STUHFL_T_PARAM_VALUE)txRxCfg);
    TRACE_EVAL_API("Set_TxRxCfg(txOutputLevel: %d, rxSensitivity: %d, usedAntenna: %d, alternateAntennaInterval: %d) = %d", txRxCfg->txOutputLevel, txRxCfg->rxSensitivity, txRxCfg->usedAntenna, txRxCfg->alternateAntennaInterval, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_PowerAmplifierCfg(STUHFL_T_ST25RU3993_PowerAmplifierCfg *paCfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_POWER_AMPLIFIER_CFG, (STUHFL_T_PARAM_VALUE)paCfg);
    TRACE_EVAL_API("Set_PowerAmplifierCfg(external: %d) = %d", paCfg->external, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Gen2_InventoryCfg(STUHFL_T_ST25RU3993_Gen2_InventoryCfg *invGen2Cfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_GEN2_INVENTORY_CFG, (STUHFL_T_PARAM_VALUE)invGen2Cfg);

#define TB_SIZE    256
    char tb_c1[TB_SIZE];
    char tb_c2[TB_SIZE];
    char tb_sI[TB_SIZE];
    char tb_s1[TB_SIZE];
    char tb_s2[TB_SIZE];
    int j1, j2;

    // C
    j1 = j2 = 0;
    for (int i = 0; i < STUHFL_D_NB_C_VALUES; i++) {
        j1 += snprintf(tb_c1 + j1, TB_SIZE - j1, "%d,", invGen2Cfg->antiCollision.C1[i]);
        j2 += snprintf(tb_c2 + j2, TB_SIZE - j2, "%d,", invGen2Cfg->antiCollision.C2[i]);
    }
    // S
    j1 = j2 = 0;
    for (int i = 0; i < STUHFL_D_NB_RX_VALUES; i++) {
        j1 += snprintf(tb_s1 + j1, TB_SIZE - j1, "%d,", invGen2Cfg->adaptiveSensitivity.incThreshold[i]);
        j2 += snprintf(tb_s2 + j2, TB_SIZE - j2, "%d,", invGen2Cfg->adaptiveSensitivity.decThreshold[i]);
    }
    snprintf(tb_sI, TB_SIZE, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", 
        invGen2Cfg->adaptiveSensitivity.ivTag,
        invGen2Cfg->adaptiveSensitivity.ivEmpty,
        invGen2Cfg->adaptiveSensitivity.ivColllision,
        invGen2Cfg->adaptiveSensitivity.ivPreample,
        invGen2Cfg->adaptiveSensitivity.ivCrc,
        invGen2Cfg->adaptiveSensitivity.ivHeader,
        invGen2Cfg->adaptiveSensitivity.ivRxCount,
        invGen2Cfg->adaptiveSensitivity.ivStopBit,
        invGen2Cfg->adaptiveSensitivity.ivResendAck,
        invGen2Cfg->adaptiveSensitivity.ivNoiseSuspicion);
#undef TB_SIZE
    TRACE_EVAL_API_APPEND("Set_Gen2_InventoryCfg(InventoryOption[fast: %d, autoAck: %d, readTID: %d], ", invGen2Cfg->inventoryOption.fast, invGen2Cfg->inventoryOption.autoAck, invGen2Cfg->inventoryOption.readTID);
    TRACE_EVAL_API_APPEND("Anticollision[adaptiveQ : %d, startQ: %d, minQ : %d, maxQ : %d, options : %d, C1 : {%s}, C2 : {%s}], ", invGen2Cfg->antiCollision.adaptiveQ, invGen2Cfg->antiCollision.startQ, invGen2Cfg->antiCollision.minQ, invGen2Cfg->antiCollision.maxQ, invGen2Cfg->antiCollision.options, tb_c1, tb_c2);
    TRACE_EVAL_API_APPEND("Auto-Tuning[interval: %d, level : %d, algo : %d, autoTuningFalsePositiveDetection : %d], ", invGen2Cfg->autoTuning.interval, invGen2Cfg->autoTuning.level, invGen2Cfg->autoTuning.algorithm, invGen2Cfg->autoTuning.falsePositiveDetection);
    TRACE_EVAL_API_APPEND("Query-Params[sel: %d, session : %d, target : %d, toggleTarget : %d, targetDepletionMode : %d], ", invGen2Cfg->queryParams.sel, invGen2Cfg->queryParams.session, invGen2Cfg->queryParams.target, invGen2Cfg->queryParams.toggleTarget, invGen2Cfg->queryParams.targetDepletionMode);
    TRACE_EVAL_API_APPEND("Adaptive-Sensitivity[adaptiveRx : %d, startRx: %d, minRx : %d, maxRx : %d, incValues : {%s}, incThreshold : {%s}, decThreshold : {%s}], ", invGen2Cfg->adaptiveSensitivity.adaptiveRx, invGen2Cfg->adaptiveSensitivity.startRx, invGen2Cfg->adaptiveSensitivity.minRx, invGen2Cfg->adaptiveSensitivity.maxRx, tb_sI, tb_s1, tb_s2);
    TRACE_EVAL_API_APPEND("Adaptive-OutputPower[adaptiveTx : %d, startTx: %d, minTx : %d, maxTx : %d]", invGen2Cfg->adaptiveOutputPower.adaptiveTx, invGen2Cfg->adaptiveOutputPower.startTx, invGen2Cfg->adaptiveOutputPower.minTx, invGen2Cfg->adaptiveOutputPower.maxTx);
    TRACE_EVAL_API_APPEND(") = %d", retCode);
    TRACE_EVAL_API_FLUSH();

    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Gb29768_InventoryCfg(STUHFL_T_ST25RU3993_Gb29768_InventoryCfg *invGb29768Cfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_GB29768_INVENTORY_CFG, (STUHFL_T_PARAM_VALUE)invGb29768Cfg);
    TRACE_EVAL_API("Set_Gb29768_InventoryCfg(readTID: %d, interval: %d, level: %d, algo: %d, autoTuningFalsePositiveDetection: %d, condition: %d, session: %d, target: %d, toggleTarget: %d, targetDepletionMode: %d, endThreshold:%d, ccnThreshold:%d, cinThreshold:%d) = %d",
                   invGb29768Cfg->inventoryOption.readTID,
                   invGb29768Cfg->autoTuning.interval, invGb29768Cfg->autoTuning.level, invGb29768Cfg->autoTuning.algorithm, invGb29768Cfg->autoTuning.falsePositiveDetection,
                   invGb29768Cfg->queryParams.condition, invGb29768Cfg->queryParams.session, invGb29768Cfg->queryParams.target, invGb29768Cfg->queryParams.toggleTarget, invGb29768Cfg->queryParams.targetDepletionMode,
                   invGb29768Cfg->antiCollision.endThreshold, invGb29768Cfg->antiCollision.ccnThreshold, invGb29768Cfg->antiCollision.cinThreshold,
                   retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Iso6b_InventoryCfg(STUHFL_T_ST25RU3993_Iso6b_InventoryCfg *invIso6bCfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_ISO6B_INVENTORY_CFG, (STUHFL_T_PARAM_VALUE)invIso6bCfg);

#define TB_SIZE    256
    //char tb_c1[TB_SIZE];
    //char tb_c2[TB_SIZE];
    //char tb_sI[TB_SIZE];
    //char tb_s1[TB_SIZE];
    //char tb_s2[TB_SIZE];

    //int j1, j2;

    //// C
    //j1 = j2 = 0;
    //for (int i = 0; i < STUHFL_D_NB_C_VALUES; i++) {
    //    j1 += snprintf(tb_c1 + j1, TB_SIZE - j1, "%d,", invGen2Cfg->antiCollision.C1[i]);
    //    j2 += snprintf(tb_c2 + j2, TB_SIZE - j2, "%d,", invGen2Cfg->antiCollision.C2[i]);
    //}
    // S
    //j1 = j2 = 0;
    //for (int i = 0; i < STUHFL_D_NB_RX_VALUES; i++) {
    //    j1 += snprintf(tb_s1 + j1, TB_SIZE - j1, "%d,", invIso6bCfg->adaptiveSensitivity.incThreshold[i]);
    //    j2 += snprintf(tb_s2 + j2, TB_SIZE - j2, "%d,", invIso6bCfg->adaptiveSensitivity.decThreshold[i]);
    //}
    //snprintf(tb_sI, TB_SIZE, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
    //    invIso6bCfg->adaptiveSensitivity.ivTag,
    //    invIso6bCfg->adaptiveSensitivity.ivEmpty,
    //    invIso6bCfg->adaptiveSensitivity.ivColllision,
    //    invIso6bCfg->adaptiveSensitivity.ivPreample,
    //    invIso6bCfg->adaptiveSensitivity.ivCrc,
    //    invIso6bCfg->adaptiveSensitivity.ivHeader,
    //    invIso6bCfg->adaptiveSensitivity.ivRxCount,
    //    invIso6bCfg->adaptiveSensitivity.ivStopBit,
    //    invIso6bCfg->adaptiveSensitivity.ivResendAck,
    //    invIso6bCfg->adaptiveSensitivity.ivNoiseSuspicion);
#undef TB_SIZE
    TRACE_EVAL_API_APPEND("Set_Iso6b_InventoryCfg(");
    //TRACE_EVAL_API_APPEND("InventoryOption[fast: %d, autoAck: %d, readTID: %d], ", invIso6bCfg->inventoryOption.fast, invIso6bCfg->inventoryOption.autoAck, invIso6bCfg->inventoryOption.readTID);
    //TRACE_EVAL_API_APPEND("Anticollision[adaptiveQ : %d, startQ: %d, minQ : %d, maxQ : %d, options : %d, C1 : {%s}, C2 : {%s}], ", invIso6bCfg->antiCollision.adaptiveQ, invIso6bCfg->antiCollision.startQ, invIso6bCfg->antiCollision.minQ, invIso6bCfg->antiCollision.maxQ, invIso6bCfg->antiCollision.options, tb_c1, tb_c2);
    TRACE_EVAL_API_APPEND("Auto-Tuning[interval: %d, level : %d, algo : %d, autoTuningFalsePositiveDetection : %d]", invIso6bCfg->autoTuning.interval, invIso6bCfg->autoTuning.level, invIso6bCfg->autoTuning.algorithm, invIso6bCfg->autoTuning.falsePositiveDetection);
    //TRACE_EVAL_API_APPEND("Query-Params[sel: %d, session : %d, target : %d, toggleTarget : %d, targetDepletionMode : %d], ", invIso6bCfg->queryParams.sel, invIso6bCfg->queryParams.session, invIso6bCfg->queryParams.target, invIso6bCfg->queryParams.toggleTarget, invIso6bCfg->queryParams.targetDepletionMode);
    //TRACE_EVAL_API_APPEND("Adaptive-Sensitivity[adaptiveRx : %d, startRx: %d, minRx : %d, maxRx : %d, incValues : {%s}, incThreshold : {%s}, decThreshold : {%s}], ", invIso6bCfg->adaptiveSensitivity.adaptiveRx, invIso6bCfg->adaptiveSensitivity.startRx, invIso6bCfg->adaptiveSensitivity.minRx, invIso6bCfg->adaptiveSensitivity.maxRx, tb_sI, tb_s1, tb_s2);
    //TRACE_EVAL_API_APPEND("Adaptive-OutputPower[adaptiveTx : %d, startTx: %d, minTx : %d, maxTx : %d]", invIso6bCfg->adaptiveOutputPower.adaptiveTx, invIso6bCfg->adaptiveOutputPower.startTx, invIso6bCfg->adaptiveOutputPower.minTx, invIso6bCfg->adaptiveOutputPower.maxTx);
    TRACE_EVAL_API_APPEND(") = %d", retCode);
    TRACE_EVAL_API_FLUSH();
    //TRACE_EVAL_API("Set_Iso6b_InventoryCfg(interval: %d, level: %d, algo: %d, autoTuningFalsePositiveDetection: %d, enable: %d, interval: %d) = %d",
    //               invIso6bCfg->autoTuning.interval, invIso6bCfg->autoTuning.level, invIso6bCfg->autoTuning.algorithm, invIso6bCfg->autoTuning.falsePositiveDetection,
    //               invIso6bCfg->adaptiveSensitivity.enable, invIso6bCfg->adaptiveSensitivity.interval,
    //               retCode);
    return retCode;
}

// ---- Getter SW Cfg ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Gen2_Timings(STUHFL_T_ST25RU3993_Gen2_Timings *gen2Timings)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_GEN2_TIMINGS, (STUHFL_T_PARAM_VALUE)gen2Timings);
    TRACE_EVAL_API("Get_Gen2_Timings(T4Min: %d) = %d", gen2Timings->T4Min, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Gen2_ProtocolCfg(STUHFL_T_ST25RU3993_Gen2_ProtocolCfg *gen2ProtocolCfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_GEN2_PROTOCOL_CFG, (STUHFL_T_PARAM_VALUE)gen2ProtocolCfg);
#ifdef CHECK_API_INTEGRITY
    if ((retCode == STUHFL_ERR_NONE)
            && (   (gen2ProtocolCfg->tari > STUHFL_D_GEN2_TARI_25_00)
                   || (gen2ProtocolCfg->coding >= STUHFL_D_NB_GEN2_CODING)
                   || (gen2ProtocolCfg->blf > STUHFL_D_GEN2_BLF_640)
               )) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif
    TRACE_EVAL_API("Get_Gen2_ProtocolCfg(tari: %d, blf: %d, coding: %d, trext: %d) = %d",
                   gen2ProtocolCfg->tari, gen2ProtocolCfg->blf, gen2ProtocolCfg->coding, gen2ProtocolCfg->trext, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Gb29768_ProtocolCfg(STUHFL_T_ST25RU3993_Gb29768_ProtocolCfg *gb29768ProtocolCfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_GB29768_PROTOCOL_CFG, (STUHFL_T_PARAM_VALUE)gb29768ProtocolCfg);
#ifdef CHECK_API_INTEGRITY
    if (    (retCode == STUHFL_ERR_NONE)
        && (   (gb29768ProtocolCfg->blf >= STUHFL_D_NB_GB29768_BLF)
               || (gb29768ProtocolCfg->coding >= STUHFL_D_NB_GB29768_CODING)
               || (gb29768ProtocolCfg->tc > STUHFL_D_GB29768_TC_12_5)
           )) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif
    TRACE_EVAL_API("Get_Gb29768_ProtocolCfg(tc: %d, blf: %d, coding: %d, trext: %d) = %d",
                   gb29768ProtocolCfg->tc, gb29768ProtocolCfg->blf, gb29768ProtocolCfg->coding, gb29768ProtocolCfg->trext, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Iso6b_ProtocolCfg(STUHFL_T_ST25RU3993_Iso6b_ProtocolCfg *iso6bProtocolCfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_ISO6B_PROTOCOL_CFG, (STUHFL_T_PARAM_VALUE)iso6bProtocolCfg);
#ifdef CHECK_API_INTEGRITY
    if (   (retCode == STUHFL_ERR_NONE)
        && (iso6bProtocolCfg->blf >= STUHFL_D_NB_ISO6B_BLF)
           ) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif
    TRACE_EVAL_API("Get_Iso6b_ProtocolCfg(blf: %d) = %d", iso6bProtocolCfg->blf, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_TxRxCfg(STUHFL_T_ST25RU3993_TxRxCfg *txRxCfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_TXRX_CFG, (STUHFL_T_PARAM_VALUE)txRxCfg);
#ifdef CHECK_API_INTEGRITY
    if (   (retCode == STUHFL_ERR_NONE)
        && (
#if ELANCE
            ((txRxCfg->usedAntenna != STUHFL_D_ANTENNA_ALT) && (txRxCfg->usedAntenna > STUHFL_D_ANTENNA_4))
#else
            ((txRxCfg->usedAntenna != STUHFL_D_ANTENNA_ALT) && (txRxCfg->usedAntenna > STUHFL_D_ANTENNA_2))
#endif
        )) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif
    TRACE_EVAL_API("Get_TxRxCfg(txOutputLevel: %d, rxSensitivity: %d, usedAntenna: %d, alternateAntennaInterval: %d) = %d", txRxCfg->txOutputLevel, txRxCfg->rxSensitivity, txRxCfg->usedAntenna, txRxCfg->alternateAntennaInterval, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_PowerAmplifierCfg(STUHFL_T_ST25RU3993_PowerAmplifierCfg *paCfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_POWER_AMPLIFIER_CFG, (STUHFL_T_PARAM_VALUE)paCfg);
    TRACE_EVAL_API("Get_PowerAmplifierCfg(external: %d) = %d", paCfg->external, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Gen2_InventoryCfg(STUHFL_T_ST25RU3993_Gen2_InventoryCfg *invGen2Cfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_GEN2_INVENTORY_CFG, (STUHFL_T_PARAM_VALUE)invGen2Cfg);
#ifdef CHECK_API_INTEGRITY
    if ((retCode == STUHFL_ERR_NONE)
            && (   (invGen2Cfg->queryParams.session > STUHFL_D_GEN2_SESSION_S3)
                   || (invGen2Cfg->queryParams.target > STUHFL_D_GEN2_TARGET_B)
                   || (invGen2Cfg->autoTuning.algorithm > STUHFL_D_TUNING_ALGO_LAST_IDX)
                   || (invGen2Cfg->antiCollision.startQ > STUHFL_D_GEN2_MAXQ)
               )) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif

#define TB_SIZE    256
    char tb_c1[TB_SIZE];
    char tb_c2[TB_SIZE];
    char tb_sI[TB_SIZE];
    char tb_s1[TB_SIZE];
    char tb_s2[TB_SIZE];
    int j1, j2;

    // C
    j1 = j2 = 0;
    for (int i = 0; i < STUHFL_D_NB_C_VALUES; i++) {
        j1 += snprintf(tb_c1 + j1, TB_SIZE - j1, "%d,", invGen2Cfg->antiCollision.C1[i]);
        j2 += snprintf(tb_c2 + j2, TB_SIZE - j2, "%d,", invGen2Cfg->antiCollision.C2[i]);
    }
    // S
    j1 = j2 = 0;
    for (int i = 0; i < STUHFL_D_NB_RX_VALUES; i++) {
        j1 += snprintf(tb_s1 + j1, TB_SIZE - j1, "%d,", invGen2Cfg->adaptiveSensitivity.incThreshold[i]);
        j2 += snprintf(tb_s2 + j2, TB_SIZE - j2, "%d,", invGen2Cfg->adaptiveSensitivity.decThreshold[i]);
    }
    snprintf(tb_sI, TB_SIZE, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
        invGen2Cfg->adaptiveSensitivity.ivTag,
        invGen2Cfg->adaptiveSensitivity.ivEmpty,
        invGen2Cfg->adaptiveSensitivity.ivColllision,
        invGen2Cfg->adaptiveSensitivity.ivPreample,
        invGen2Cfg->adaptiveSensitivity.ivCrc,
        invGen2Cfg->adaptiveSensitivity.ivHeader,
        invGen2Cfg->adaptiveSensitivity.ivRxCount,
        invGen2Cfg->adaptiveSensitivity.ivStopBit,
        invGen2Cfg->adaptiveSensitivity.ivResendAck,
        invGen2Cfg->adaptiveSensitivity.ivNoiseSuspicion);
#undef TB_SIZE
    TRACE_EVAL_API_APPEND("Get_Gen2_InventoryCfg(InventoryOption[fast: %d, autoAck: %d, readTID: %d], ", invGen2Cfg->inventoryOption.fast, invGen2Cfg->inventoryOption.autoAck, invGen2Cfg->inventoryOption.readTID);
    TRACE_EVAL_API_APPEND("Anticollision[adaptiveQ : %d, startQ: %d, minQ : %d, maxQ : %d, options : %d, C1 : {%s}, C2 : {%s}], ", invGen2Cfg->antiCollision.adaptiveQ, invGen2Cfg->antiCollision.startQ, invGen2Cfg->antiCollision.minQ, invGen2Cfg->antiCollision.maxQ, invGen2Cfg->antiCollision.options, tb_c1, tb_c2);
    TRACE_EVAL_API_APPEND("Auto-Tuning[interval: %d, level : %d, algo : %d, autoTuningFalsePositiveDetection : %d], ", invGen2Cfg->autoTuning.interval, invGen2Cfg->autoTuning.level, invGen2Cfg->autoTuning.algorithm, invGen2Cfg->autoTuning.falsePositiveDetection);
    TRACE_EVAL_API_APPEND("Query-Params[sel: %d, session : %d, target : %d, toggleTarget : %d, targetDepletionMode : %d], ", invGen2Cfg->queryParams.sel, invGen2Cfg->queryParams.session, invGen2Cfg->queryParams.target, invGen2Cfg->queryParams.toggleTarget, invGen2Cfg->queryParams.targetDepletionMode);
    TRACE_EVAL_API_APPEND("Adaptive-Sensitivity[adaptiveRx : %d, startRx: %d, minRx : %d, maxRx : %d, incValues : {%s}, incThreshold : {%s}, decThreshold : {%s}], ", invGen2Cfg->adaptiveSensitivity.adaptiveRx, invGen2Cfg->adaptiveSensitivity.startRx, invGen2Cfg->adaptiveSensitivity.minRx, invGen2Cfg->adaptiveSensitivity.maxRx, tb_sI, tb_s1, tb_s2);
    TRACE_EVAL_API_APPEND("Adaptive-OutputPower[adaptiveTx : %d, startTx: %d, minTx : %d, maxTx : %d]", invGen2Cfg->adaptiveOutputPower.adaptiveTx, invGen2Cfg->adaptiveOutputPower.startTx, invGen2Cfg->adaptiveOutputPower.minTx, invGen2Cfg->adaptiveOutputPower.maxTx);
    TRACE_EVAL_API_APPEND(") = %d", retCode);
    TRACE_EVAL_API_FLUSH();

    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Gb29768_InventoryCfg(STUHFL_T_ST25RU3993_Gb29768_InventoryCfg *invGb29768Cfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_GB29768_INVENTORY_CFG, (STUHFL_T_PARAM_VALUE)invGb29768Cfg);
#ifdef CHECK_API_INTEGRITY
    if ((retCode == STUHFL_ERR_NONE)
            && (  (invGb29768Cfg->queryParams.condition > STUHFL_D_GB29768_CONDITION_FLAG0)
               || (invGb29768Cfg->queryParams.session > STUHFL_D_GB29768_SESSION_S3)
               || (invGb29768Cfg->queryParams.target > STUHFL_D_GB29768_TARGET_1)
               || (invGb29768Cfg->autoTuning.algorithm > STUHFL_D_TUNING_ALGO_LAST_IDX)
               )) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif
    TRACE_EVAL_API("Get_Gb29768_InventoryCfg(readTID: %d, interval: %d, level: %d, algo: %d, autoTuningFalsePositiveDetection: %d, condition: %d, session: %d, target: %d, toggleTarget: %d, targetDepletionMode: %d, endThreshold:%d, ccnThreshold:%d, cinThreshold:%d) = %d",
                   invGb29768Cfg->inventoryOption.readTID,
                   invGb29768Cfg->autoTuning.interval, invGb29768Cfg->autoTuning.level, invGb29768Cfg->autoTuning.algorithm, invGb29768Cfg->autoTuning.falsePositiveDetection,
                   invGb29768Cfg->queryParams.condition, invGb29768Cfg->queryParams.session, invGb29768Cfg->queryParams.target, invGb29768Cfg->queryParams.toggleTarget, invGb29768Cfg->queryParams.targetDepletionMode,
                   invGb29768Cfg->antiCollision.endThreshold, invGb29768Cfg->antiCollision.ccnThreshold, invGb29768Cfg->antiCollision.cinThreshold,
                   retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Iso6b_InventoryCfg(STUHFL_T_ST25RU3993_Iso6b_InventoryCfg *invIso6bCfg)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_RWD_ISO6B_INVENTORY_CFG, (STUHFL_T_PARAM_VALUE)invIso6bCfg);
#ifdef CHECK_API_INTEGRITY
    if (   (retCode == STUHFL_ERR_NONE)
        && (invIso6bCfg->autoTuning.algorithm > STUHFL_D_TUNING_ALGO_LAST_IDX)
               ) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif

#define TB_SIZE    256
    //char tb_c1[TB_SIZE];
    //char tb_c2[TB_SIZE];
    //char tb_sI[TB_SIZE];
    //char tb_s1[TB_SIZE];
    //char tb_s2[TB_SIZE];
    //int j1, j2;

    //// C
    //j1 = j2 = 0;
    //for (int i = 0; i < STUHFL_D_NB_C_VALUES; i++) {
    //    j1 += snprintf(tb_c1 + j1, TB_SIZE - j1, "%d,", invGen2Cfg->antiCollision.C1[i]);
    //    j2 += snprintf(tb_c2 + j2, TB_SIZE - j2, "%d,", invGen2Cfg->antiCollision.C2[i]);
    //}
    // S
    //j1 = j2 = 0;
    //for (int i = 0; i < STUHFL_D_NB_RX_VALUES; i++) {
    //    j1 += snprintf(tb_s1 + j1, TB_SIZE - j1, "%d,", invIso6bCfg->adaptiveSensitivity.incThreshold[i]);
    //    j2 += snprintf(tb_s2 + j2, TB_SIZE - j2, "%d,", invIso6bCfg->adaptiveSensitivity.decThreshold[i]);
    //}
    //snprintf(tb_sI, TB_SIZE, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
    //    invIso6bCfg->adaptiveSensitivity.ivTag,
    //    invIso6bCfg->adaptiveSensitivity.ivEmpty,
    //    invIso6bCfg->adaptiveSensitivity.ivColllision,
    //    invIso6bCfg->adaptiveSensitivity.ivPreample,
    //    invIso6bCfg->adaptiveSensitivity.ivCrc,
    //    invIso6bCfg->adaptiveSensitivity.ivHeader,
    //    invIso6bCfg->adaptiveSensitivity.ivRxCount,
    //    invIso6bCfg->adaptiveSensitivity.ivStopBit,
    //    invIso6bCfg->adaptiveSensitivity.ivResendAck,
    //    invIso6bCfg->adaptiveSensitivity.ivNoiseSuspicion);
#undef TB_SIZE
    TRACE_EVAL_API_APPEND("Get_Iso6b_InventoryCfg(");
    //TRACE_EVAL_API_APPEND("Get_Iso6b_InventoryCfg(InventoryOption[fast: %d, autoAck: %d, readTID: %d], ", invIso6bCfg->inventoryOption.fast, invIso6bCfg->inventoryOption.autoAck, invIso6bCfg->inventoryOption.readTID);
    //TRACE_EVAL_API_APPEND("Anticollision[adaptiveQ : %d, startQ: %d, minQ : %d, maxQ : %d, options : %d, C1 : {%s}, C2 : {%s}], ", invIso6bCfg->antiCollision.adaptiveQ, invIso6bCfg->antiCollision.startQ, invIso6bCfg->antiCollision.minQ, invIso6bCfg->antiCollision.maxQ, invIso6bCfg->antiCollision.options, tb_c1, tb_c2);
    TRACE_EVAL_API_APPEND("Auto-Tuning[interval: %d, level : %d, algo : %d, autoTuningFalsePositiveDetection : %d]", invIso6bCfg->autoTuning.interval, invIso6bCfg->autoTuning.level, invIso6bCfg->autoTuning.algorithm, invIso6bCfg->autoTuning.falsePositiveDetection);
    //TRACE_EVAL_API_APPEND("Query-Params[sel: %d, session : %d, target : %d, toggleTarget : %d, targetDepletionMode : %d], ", invIso6bCfg->queryParams.sel, invIso6bCfg->queryParams.session, invIso6bCfg->queryParams.target, invIso6bCfg->queryParams.toggleTarget, invIso6bCfg->queryParams.targetDepletionMode);
    //TRACE_EVAL_API_APPEND("Adaptive-Sensitivity[adaptiveRx : %d, startRx: %d, minRx : %d, maxRx : %d, incValues : {%s}, incThreshold : {%s}, decThreshold : {%s}], ", invIso6bCfg->adaptiveSensitivity.adaptiveRx, invIso6bCfg->adaptiveSensitivity.startRx, invIso6bCfg->adaptiveSensitivity.minRx, invIso6bCfg->adaptiveSensitivity.maxRx, tb_sI, tb_s1, tb_s2);
    //TRACE_EVAL_API_APPEND("Adaptive-OutputPower[adaptiveTx : %d, startTx: %d, minTx : %d, maxTx : %d])", invIso6bCfg->adaptiveOutputPower.adaptiveTx, invIso6bCfg->adaptiveOutputPower.startTx, invIso6bCfg->adaptiveOutputPower.minTx, invIso6bCfg->adaptiveOutputPower.maxTx);
    TRACE_EVAL_API_APPEND(") = %d", retCode);
    TRACE_EVAL_API_FLUSH();

    //TRACE_EVAL_API("Get_Iso6b_InventoryCfg(interval: %d, level: %d, algo: %d, autoTuningFalsePositiveDetection: %d, enable: %d, interval: %d) = %d",
    //               invIso6bCfg->autoTuning.interval, invIso6bCfg->autoTuning.level, invIso6bCfg->autoTuning.algorithm, invIso6bCfg->autoTuning.falsePositiveDetection,
    //               invIso6bCfg->adaptiveSensitivity.enable, invIso6bCfg->adaptiveSensitivity.interval,
    //               retCode);
    return retCode;
}



// ---- Setter Tuning ----
#if OLD_FREQUENCY_TUNING
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_Tuning(STUHFL_T_ST25RU3993_Tuning *tuning)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_TUNING, (STUHFL_T_PARAM_VALUE)tuning);
    TRACE_EVAL_API("Set_Tuning(antenna: %d, cin: %d, clen: %d, cout: %d) = %d", tuning->antenna, tuning->cin, tuning->clen, tuning->cout, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_TuningTableEntry(STUHFL_T_ST25RU3993_TuningTableEntry *tuningTableEntry)
{
#define TB_SIZE    256U
    char tb[5][TB_SIZE];
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_TUNING_TABLE_ENTRY, (STUHFL_T_PARAM_VALUE)tuningTableEntry);
    TRACE_EVAL_API("Set_TuningTableEntry(entry: %d, freq: %d, applyCapValues: 0x%s, cin: 0x%s, clen: 0x%s, cout: 0x%s, IQ: 0x%s) = %d",
                   tuningTableEntry->entry, tuningTableEntry->freq, byteArray2HexString(tb[0], TB_SIZE, tuningTableEntry->applyCapValues, STUHFL_D_MAX_ANTENNA), byteArray2HexString(tb[1], TB_SIZE, tuningTableEntry->cin, STUHFL_D_MAX_ANTENNA), byteArray2HexString(tb[2], TB_SIZE, tuningTableEntry->clen, STUHFL_D_MAX_ANTENNA), byteArray2HexString(tb[3], TB_SIZE, tuningTableEntry->cout, STUHFL_D_MAX_ANTENNA), byteArray2HexString(tb[4], TB_SIZE, (uint8_t *)tuningTableEntry->IQ, STUHFL_D_MAX_ANTENNA * sizeof(uint16_t)), retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_TuningTableDefault(STUHFL_T_ST25RU3993_TunerTableSet *set)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_TUNING_TABLE_DEFAULT, (STUHFL_T_PARAM_VALUE)set);
    TRACE_EVAL_API("Set_TuningTableDefault(profile: %d, freq: %d) = %d", set->profile, set->freq, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_TuningTableSave2Flash(void)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_TUNING_TABLE_SAVE, (STUHFL_T_PARAM_VALUE)NULL);
    TRACE_EVAL_API("Set_TuningTableSave2Flash() = %d", retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_TuningTableEmpty(void)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_TUNING_TABLE_EMPTY, (STUHFL_T_PARAM_VALUE)NULL);
    TRACE_EVAL_API("Set_TuningTableEmpty() = %d", retCode);
    return retCode;
}
#endif
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Set_TuningCaps(STUHFL_T_ST25RU3993_TuningCaps *tuning)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_SetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_TUNING_CAPS, (STUHFL_T_PARAM_VALUE)tuning);
    TRACE_EVAL_API("Set_TuningCaps(channelListIdx: %d, Caps ANTENNA 1:(%d,%d,%d), Caps ANTENNA 2:(%d,%d,%d))", tuning->channelListIdx,
        tuning->caps[0].cin, tuning->caps[0].clen, tuning->caps[0].cout,
        tuning->caps[1].cin, tuning->caps[1].clen, tuning->caps[1].cout);
    return retCode;
}

// ---- Getter Tuning ----
#if OLD_FREQUENCY_TUNING
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_Tuning(STUHFL_T_ST25RU3993_Tuning *tuning)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_TUNING, (STUHFL_T_PARAM_VALUE)tuning);
#ifdef CHECK_API_INTEGRITY
    if (   (retCode == STUHFL_ERR_NONE)
        && (
#if ELANCE
            ((tuning->antenna != STUHFL_D_ANTENNA_ALT) && (tuning->antenna > STUHFL_D_ANTENNA_4))
#else
            ((tuning->antenna != STUHFL_D_ANTENNA_ALT) && (tuning->antenna > STUHFL_D_ANTENNA_2))
#endif
        )) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif
    TRACE_EVAL_API("Get_Tuning(antenna: %d, cin: %d, clen: %d, cout: %d) = %d", tuning->antenna, tuning->cin, tuning->clen, tuning->cout, retCode);
    return retCode;
}

STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_TuningTableEntry(STUHFL_T_ST25RU3993_TuningTableEntry *tuningTableEntry)
{
#define TB_SIZE    256U
    char tb[5][TB_SIZE];
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_TUNING_TABLE_ENTRY, (STUHFL_T_PARAM_VALUE)tuningTableEntry);
#ifdef CHECK_API_INTEGRITY
    uint8_t disabledCapValues[STUHFL_D_MAX_ANTENNA] = {0};
    if ((retCode == STUHFL_ERR_NONE)
            && (   (tuningTableEntry->entry > STUHFL_D_MAX_FREQUENCY)
                   || (tuningTableEntry->freq > STUHFL_D_FREQUENCY_MAX_VALUE)
                   || (memcmp(tuningTableEntry->applyCapValues, disabledCapValues, STUHFL_D_MAX_ANTENNA) != 0)
               )) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif
    TRACE_EVAL_API("Get_TuningTableEntry(entry: %d, freq: %d, applyCapValues: 0x%s,  cin: 0x%s, clen: 0x%s, cout: 0x%s, IQ: 0x%s) = %d",
                   tuningTableEntry->entry, tuningTableEntry->freq, byteArray2HexString(tb[0], TB_SIZE, tuningTableEntry->applyCapValues, STUHFL_D_MAX_ANTENNA), byteArray2HexString(tb[1], TB_SIZE, tuningTableEntry->cin, STUHFL_D_MAX_ANTENNA), byteArray2HexString(tb[2], TB_SIZE, tuningTableEntry->clen, STUHFL_D_MAX_ANTENNA), byteArray2HexString(tb[3], TB_SIZE, tuningTableEntry->cout, STUHFL_D_MAX_ANTENNA), byteArray2HexString(tb[4], TB_SIZE, (uint8_t *)tuningTableEntry->IQ, STUHFL_D_MAX_ANTENNA * sizeof(uint16_t)), retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_TuningTableInfo(STUHFL_T_ST25RU3993_TuningTableInfo *tuningTableInfo)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_TUNING_TABLE_INFO, (STUHFL_T_PARAM_VALUE)tuningTableInfo);
#ifdef CHECK_API_INTEGRITY
    if ((retCode == STUHFL_ERR_NONE)
            && (   (tuningTableInfo->profile >= STUHFL_D_NUM_SAVED_PROFILES)
                   || (tuningTableInfo->numEntries > STUHFL_D_MAX_FREQUENCY)
               )) {
        retCode = STUHFL_ERR_PROTO;
    }
#endif
    TRACE_EVAL_API("Get_TuningTableInfo(profile: %d, numEntries: %d) = %d", tuningTableInfo->profile, tuningTableInfo->numEntries, retCode);
    return retCode;
}
#endif
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Get_TuningCaps(STUHFL_T_ST25RU3993_TuningCaps *tuning)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_ST25RU3993 | STUHFL_KEY_TUNING_CAPS, (STUHFL_T_PARAM_VALUE)tuning);
#ifdef CHECK_API_INTEGRITY
    if (retCode == STUHFL_ERR_NONE) {
        if (   (tuning->channelListIdx > STUHFL_D_MAX_FREQUENCY)
#if ELANCE
            || ((tuning->antenna != STUHFL_D_ANTENNA_ALT) && (tuning->antenna > STUHFL_D_ANTENNA_4))
#else
            || ((tuning->antenna != STUHFL_D_ANTENNA_ALT) && (tuning->antenna > STUHFL_D_ANTENNA_2))
#endif
           ) {
            retCode = STUHFL_ERR_PROTO;
        }
    }
#endif
    TRACE_EVAL_API("Get_TuningCaps(channelListIdx: %d, Caps ANTENNA 1:(%d,%d,%d), Caps ANTENNA 2:(%d,%d,%d))", tuning->channelListIdx,
        tuning->caps[0].cin, tuning->caps[0].clen, tuning->caps[0].cout,
        tuning->caps[1].cin, tuning->caps[1].clen, tuning->caps[1].cout);
    return retCode;
}

// ---- Tuning ----
#if OLD_FREQUENCY_TUNING
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Tune(STUHFL_T_ST25RU3993_Tune *tune)
{
    // As tuning may take a while we increase the communication timeout to be
    uint32_t rdTimeOut = 4000;
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_RD_TIMEOUT_MS, (STUHFL_T_PARAM_VALUE)&rdTimeOut);
    uint32_t tmpRdTimeOut = 60000;   // to be on the safe side use 60 sec
    if (STUHFL_ERR_NONE == STUHFL_F_SetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_RD_TIMEOUT_MS, (STUHFL_T_PARAM_VALUE)&tmpRdTimeOut)) {
        // start tuning..
        retCode |= STUHFL_F_ExecuteCmd((STUHFL_CG_DL << 8) | STUHFL_CC_TUNE, (STUHFL_T_PARAM_VALUE)tune, (STUHFL_T_PARAM_VALUE)tune);
        // revert max timeout
        retCode |= STUHFL_F_SetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_RD_TIMEOUT_MS, (STUHFL_T_PARAM_VALUE)&rdTimeOut);
    }
    TRACE_EVAL_API("Tune(algo: %d, doFalsePositiveDetection: %d) = %d", tune->algo & ~STUHFL_D_TUNING_ALGO_ENABLE_FPD, (tune->algo & STUHFL_D_TUNING_ALGO_ENABLE_FPD) ? true : false, retCode);
    return retCode;
}
#endif
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV TuneChannel(STUHFL_T_ST25RU3993_TuneCfg *tuneCfg)
{
    // As tuning may take a while we increase the communication timeout to be
    uint32_t rdTimeOut = 4000;
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_GetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_RD_TIMEOUT_MS, (STUHFL_T_PARAM_VALUE)&rdTimeOut);
    uint32_t tmpRdTimeOut = 60000;   // to be on the safe side use 60 sec
    if (STUHFL_ERR_NONE == STUHFL_F_SetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_RD_TIMEOUT_MS, (STUHFL_T_PARAM_VALUE)&tmpRdTimeOut)) {
        // start tuning..
        retCode |= STUHFL_F_ExecuteCmd((STUHFL_CG_DL << 8) | STUHFL_CC_TUNE_CHANNEL, (STUHFL_T_PARAM_VALUE)tuneCfg, (STUHFL_T_PARAM_VALUE)tuneCfg);
        // revert max timeout
        retCode |= STUHFL_F_SetParam(STUHFL_PARAM_TYPE_CONNECTION | STUHFL_KEY_RD_TIMEOUT_MS, (STUHFL_T_PARAM_VALUE)&rdTimeOut);
    }
    TRACE_EVAL_API("TuneChannel(falsePositiveDetection: %d, persistent: %d, channelListIdx: %d, antenna: %d, algo: %d, tuneAll: %d) = %d", tuneCfg->falsePositiveDetection, tuneCfg->persistent, tuneCfg->channelListIdx, tuneCfg->antenna, tuneCfg->algorithm, tuneCfg->tuneAll, retCode);
    return retCode;
}

// ---- Gen2 ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_Inventory(STUHFL_T_InventoryOption *invOption, STUHFL_T_InventoryData *invData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_Inventory(invOption, invData);
    TRACE_EVAL_API("Gen2_Inventory(rssiMode: %d, roundCnt: %d, inventoryDelay: %d, options: %d, tagListSizeMax: %d, tagListSize: %d, STATISTICS: tuningStatus: %d, roundCnt: %d, sensitivity: %d, Q: %d, adc: %d, frequency: %d, tagCnt: %d, emptySlotCnt: %d, collisionCnt: %d, slotCnt: %d, preambleErrCnt: %d, crcErrCnt: %d, TAGLIST: ..) = %d",
                   invOption->rssiMode, invOption->roundCnt, invOption->inventoryDelay, invOption->options,
                   invData->tagListSizeMax, invData->tagListSize,
                   invData->statistics.tuningStatus, invData->statistics.roundCnt, invData->statistics.sensitivity, invData->statistics.Q, invData->statistics.adc, invData->statistics.frequency, invData->statistics.tagCnt, invData->statistics.emptySlotCnt, invData->statistics.collisionCnt, invData->statistics.slotCnt, invData->statistics.preambleErrCnt, invData->statistics.crcErrCnt, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_Select(STUHFL_T_Gen2_Select *selData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_Select(selData);
    TRACE_EVAL_API("Gen2_Select(mode: %d, target: %d, action: %d, memoryBank: %d, mask[32]: 0x%02x.., maskBitPointer: %d, maskBitLength: %d, truncation: %d) = %d",
                   selData->mode, selData->target, selData->action, selData->memoryBank, selData->mask[0], selData->maskBitPointer, selData->maskBitLength, selData->truncation, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_Read(STUHFL_T_Read *readData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_Read(readData);
#define TB_SIZE    256U
    char tb[2][TB_SIZE];
    TRACE_EVAL_API("Gen2_Read(memoryBank: %d, wordPtr: %d, numBytesToRead: %d, numReadBytes: %d, pwd: 0x%s, data: 0x%s) = %d", readData->memoryBank, readData->wordPtr, readData->numBytesToRead, readData->numReadBytes, byteArray2HexString(tb[0], TB_SIZE, readData->pwd, STUHFL_D_PASSWORD_LEN), byteArray2HexString(tb[1], TB_SIZE, readData->data, STUHFL_D_MAX_READ_DATA_LEN), retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_Write(STUHFL_T_Write *writeData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_Write(writeData);
#define TB_SIZE    256U
    char tb[TB_SIZE];
    TRACE_EVAL_API("Gen2_Write(memoryBank: %d, wordPtr: %d, pwd: 0x%s, data: 0x%02x%02x, tagReply: 0x%02x) = %d", writeData->memoryBank, writeData->wordPtr, byteArray2HexString(tb, TB_SIZE, writeData->pwd, STUHFL_D_PASSWORD_LEN), writeData->data[0], writeData->data[1], writeData->tagReply, retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_BlockWrite(STUHFL_T_BlockWrite *blockWrite)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_BlockWrite(blockWrite);
#define TB_SIZE    256U
    char tb[2][TB_SIZE];
    TRACE_EVAL_API("Gen2_BlockWrite(memoryBank: %d, wordPtr: %d, pwd: 0x%s, numBytesToWrite: %d, data: 0x%s, tagReply: 0x%02x) = %d", blockWrite->memoryBank, blockWrite->wordPtr, byteArray2HexString(tb[0], TB_SIZE, blockWrite->pwd, STUHFL_D_PASSWORD_LEN), blockWrite->numBytesToWrite, byteArray2HexString(tb[1], TB_SIZE, blockWrite->data, STUHFL_D_MAX_BLOCKWRITE_DATA_LEN), blockWrite->tagReply, retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_Lock(STUHFL_T_Gen2_Lock *lockData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_Lock(lockData);
#define TB_SIZE    256U
    char tb[2][TB_SIZE];
    TRACE_EVAL_API("Gen2_Lock(mask: 0x%s, pwd: 0x%s, tagReply: 0x%02x) = %d", byteArray2HexString(tb[0], TB_SIZE, lockData->mask, 3), byteArray2HexString(tb[1], TB_SIZE, lockData->pwd, STUHFL_D_PASSWORD_LEN), lockData->tagReply, retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_Kill(STUHFL_T_Kill *killData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_Kill(killData);
#define TB_SIZE    256U
    char tb[2][TB_SIZE];
    TRACE_EVAL_API("Gen2_Kill(pwd: 0x%s, killPwd: 0x%s, recommission: %d, tagReply: 0x%02x) = %d", 
        byteArray2HexString(tb[0], TB_SIZE, killData->pwd, STUHFL_D_PASSWORD_LEN),  
        byteArray2HexString(tb[1], TB_SIZE, killData->killPwd, STUHFL_D_PASSWORD_LEN), 
        killData->recommission, killData->tagReply, retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_GenericCmd(STUHFL_T_Gen2_GenericCmd *genericCmd)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_GenericCmd(genericCmd);
#define TB_SIZE    256U
    char tb[3][TB_SIZE];
    TRACE_EVAL_API("Gen2_GenericCmd(pwd: 0x%s, cmd: 0x%02x, noResponseTime: %d, sndDataBitLength: %d, sndData: 0x%s.., expectedRcvDataBitLength: %d, rcvDataLength: %d, rcvData: 0x%s..) = %d",
                   byteArray2HexString(tb[0], TB_SIZE, genericCmd->pwd, STUHFL_D_PASSWORD_LEN), genericCmd->cmd, genericCmd->noResponseTime, genericCmd->sndDataBitLength, byteArray2HexString(tb[1], TB_SIZE, genericCmd->sndData, 4), genericCmd->expectedRcvDataBitLength, genericCmd->rcvDataLength, byteArray2HexString(tb[2], TB_SIZE, genericCmd->rcvData, 4), retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_QueryMeasureRssi(STUHFL_T_Gen2_QueryMeasureRssi *queryMeasureRssi)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_QueryMeasureRssi(queryMeasureRssi);
#define TB_SIZE    256U
    char tb[5][TB_SIZE];
    TRACE_EVAL_API("STUHFL_F_Gen2_QueryMeasureRssi(frequency: %d, measureCnt: %d, agc: 0x%s.., rssiLogI: 0x%s.., rssiLogQ: 0x%s.., rssiLinI: 0x%s.., rssiLinQ: 0x%s..) = %d", queryMeasureRssi->frequency, queryMeasureRssi->measureCnt, byteArray2HexString(tb[0], TB_SIZE, queryMeasureRssi->agc, 4), byteArray2HexString(tb[1], TB_SIZE, queryMeasureRssi->rssiLogI, 4), byteArray2HexString(tb[2], TB_SIZE, queryMeasureRssi->rssiLogQ, 4), byteArray2HexString(tb[3], TB_SIZE, queryMeasureRssi->rssiLinI, 4), byteArray2HexString(tb[4], TB_SIZE, queryMeasureRssi->rssiLinQ, 4), retCode);
#undef TB_SIZE
    return retCode;
}
//
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_Challenge(STUHFL_T_Gen2_Challenge *challenge)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_Challenge(challenge);
#define TB_SIZE    256U
    char tb[2][TB_SIZE];
    TRACE_EVAL_API("Gen2_Challenge(pwd: 0x%s, incRepLen: %d, immed: %d, CSI: %d, message: 0x%s, messageBitLength: %d) = %d", 
        byteArray2HexString(tb[0], TB_SIZE, challenge->pwd, STUHFL_D_PASSWORD_LEN),
        challenge->incRepLen, challenge->immed, challenge->CSI, 
        byteArray2HexString(tb[1], TB_SIZE, challenge->message, 16),
        challenge->messageBitLength, retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_Authenticate(STUHFL_T_Gen2_Authenticate *authenticate)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_Authenticate(authenticate);
#define TB_SIZE    256U
    char tb[3][TB_SIZE];
    TRACE_EVAL_API("Gen2_Authenticate(pwd: 0x%s, senRep: %d, incRepLen: %d, CSI: %d, message: 0x%s, messageBitLength: %d, response: 0x%s, responseBitLength: %d, responseHeaderBit: %d) = %d",
        byteArray2HexString(tb[0], TB_SIZE, authenticate->pwd, STUHFL_D_PASSWORD_LEN),
        authenticate->senRep, authenticate->incRepLen, authenticate->CSI,
        byteArray2HexString(tb[1], TB_SIZE, authenticate->message, STUHFL_D_MAX_MESSAGE_LENGTH), authenticate->messageBitLength,
        byteArray2HexString(tb[2], TB_SIZE, authenticate->response, STUHFL_D_MAX_RESPONSE_LENGTH), authenticate->responseBitLength,
        authenticate->responseHeaderBit,
        retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_AuthComm(STUHFL_T_Gen2_AuthComm *authComm)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_AuthComm(authComm);
#define TB_SIZE    256U
    char tb[3][TB_SIZE];
    TRACE_EVAL_API("Gen2_AuthComm(pwd: 0x%s, incRepLen: %d, msg: 0x%s, messageBitLength: %d, res: 0x%s, responseBitLength: %d, responseHeaderBit: %d) = %d",
        byteArray2HexString(tb[0], TB_SIZE, authComm->pwd, STUHFL_D_PASSWORD_LEN),
        authComm->incRepLen,
        byteArray2HexString(tb[1], TB_SIZE, authComm->message, STUHFL_D_MAX_MESSAGE_LENGTH), authComm->messageBitLength,
        byteArray2HexString(tb[2], TB_SIZE, authComm->response, STUHFL_D_MAX_RESPONSE_LENGTH), authComm->responseBitLength,
        authComm->responseHeaderBit,
        retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_SecureComm(STUHFL_T_Gen2_SecureComm *secureComm)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_SecureComm(secureComm);
#define TB_SIZE    256U
    char tb[3][TB_SIZE];
    TRACE_EVAL_API("Gen2_SecureComm(pwd: 0x%s, senRep: %d, incRepLen: %d, msg: 0x%s, messageBitLength: %d, res: 0x%s, responseBitLength: %d, responseHeaderBit: %d) = %d",
        byteArray2HexString(tb[0], TB_SIZE, secureComm->pwd, STUHFL_D_PASSWORD_LEN),
        secureComm->senRep, secureComm->incRepLen,
        byteArray2HexString(tb[1], TB_SIZE, secureComm->message, STUHFL_D_MAX_MESSAGE_LENGTH), secureComm->messageBitLength,
        byteArray2HexString(tb[2], TB_SIZE, secureComm->response, STUHFL_D_MAX_RESPONSE_LENGTH), secureComm->responseBitLength,
        secureComm->responseHeaderBit,
        retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_KeyUpdate(STUHFL_T_Gen2_KeyUpdate *keyUpdate)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_KeyUpdate(keyUpdate);
#define TB_SIZE    256U
    char tb[3][TB_SIZE];
    TRACE_EVAL_API("Gen2_KeyUpdate(pwd: 0x%s, senRep: %d, incRepLen: %d, keyID: %d, msg: 0x%s, messageBitLength: %d, res: 0x%s, responseBitLength: %d, responseHeaderBit: %d) = %d",
        byteArray2HexString(tb[0], TB_SIZE, keyUpdate->pwd, STUHFL_D_PASSWORD_LEN),
        keyUpdate->senRep, keyUpdate->incRepLen, keyUpdate->keyID,
        byteArray2HexString(tb[1], TB_SIZE, keyUpdate->message, STUHFL_D_MAX_MESSAGE_LENGTH), keyUpdate->messageBitLength,
        byteArray2HexString(tb[2], TB_SIZE, keyUpdate->response, STUHFL_D_MAX_RESPONSE_LENGTH), keyUpdate->responseBitLength,
        keyUpdate->responseHeaderBit,
        retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_TagPrivilege(STUHFL_T_Gen2_TagPrivilege *tagPrivilege)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_TagPrivilege(tagPrivilege);
#define TB_SIZE    256U
    char tb[2][TB_SIZE];
    TRACE_EVAL_API("Gen2_TagPrivilege(pwd: 0x%s, senRep: %d, incRepLen: %d, action: %d, target: %d, keyID: %d, res: 0x%s, responseBitLength: %d, responseHeaderBit: %d) = %d",
        byteArray2HexString(tb[0], TB_SIZE, tagPrivilege->pwd, STUHFL_D_PASSWORD_LEN),
        tagPrivilege->senRep, tagPrivilege->incRepLen, tagPrivilege->action, tagPrivilege->target, tagPrivilege->keyID,
        byteArray2HexString(tb[1], TB_SIZE, tagPrivilege->response, STUHFL_D_MAX_RESPONSE_LENGTH), tagPrivilege->responseBitLength,
        tagPrivilege->responseHeaderBit,
        retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_ReadBuffer(STUHFL_T_Gen2_ReadBuffer *readBuffer)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_ReadBuffer(readBuffer);
#define TB_SIZE    256U
    char tb[2][TB_SIZE];
    TRACE_EVAL_API("Gen2_ReadBuffer(pwd: 0x%s, wordPtr: %d, bitCount: %d, res: 0x%s, responseBitLength: %d, responseHeaderBit: %d) = %d",
        byteArray2HexString(tb[0], TB_SIZE, readBuffer->pwd, STUHFL_D_PASSWORD_LEN),
        readBuffer->wordPtr, readBuffer->bitCount,
        byteArray2HexString(tb[1], TB_SIZE, readBuffer->response, STUHFL_D_MAX_RESPONSE_LENGTH), readBuffer->responseBitLength,
        readBuffer->responseHeaderBit,
        retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_Untraceable(STUHFL_T_Gen2_Untraceable *untraceable)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gen2_Untraceable(untraceable);
#define TB_SIZE    256U    
    char tb[TB_SIZE];
    TRACE_EVAL_API("Gen2_Untraceable(pwd: 0x%s, assertU: %d, hideEPC: %d, newEPC_L: %d, hideTID: %d, hideUser: %d, range: %d, tagErrorCode : %d) = %d",
        byteArray2HexString(tb, TB_SIZE, untraceable->pwd, STUHFL_D_PASSWORD_LEN),
        untraceable->assertU, untraceable->hideEPC, untraceable->newEPC_L,
        untraceable->hideTID, untraceable->hideUser, untraceable->range,
        untraceable->tagErrorCode,
        retCode);
#undef TB_SIZE
    return retCode;
}
#if 0 // RFU
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_FileOpen(STUHFL_T_Gen2_FileOpen *fileOpen)
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_FileList(STUHFL_T_Gen2_FileList *fileList)
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_FilePrivilege(STUHFL_T_Gen2_FilePrivilege *filePrivilege)
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gen2_FileSetup(STUHFL_T_Gen2_FileSetup *fileSetup)
#endif

// ---- Gb29768 ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gb29768_Inventory(STUHFL_T_InventoryOption *invOption, STUHFL_T_InventoryData *invData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gb29768_Inventory(invOption, invData);
    TRACE_EVAL_API("Gb29768_Inventory(rssiMode: %d, roundCnt: %d, inventoryDelay: %d, options: %d, tagListSizeMax: %d, tagListSize: %d, STATISTICS: tuningStatus: %d, roundCnt: %d, sensitivity: %d, adc: %d, frequency: %d, tagCnt: %d, emptySlotCnt: %d, collisionCnt: %d, slotCnt: %d, preambleErrCnt: %d, crcErrCnt: %d, TAGLIST: ..) = %d",
                   invOption->rssiMode, invOption->roundCnt, invOption->inventoryDelay, invOption->options,
                   invData->tagListSizeMax, invData->tagListSize,
                   invData->statistics.tuningStatus, invData->statistics.roundCnt, invData->statistics.sensitivity, invData->statistics.adc, invData->statistics.frequency, invData->statistics.tagCnt, invData->statistics.emptySlotCnt, invData->statistics.collisionCnt, invData->statistics.slotCnt, invData->statistics.preambleErrCnt, invData->statistics.crcErrCnt, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gb29768_Sort(STUHFL_T_Gb29768_Sort *sortData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gb29768_Sort(sortData);
    TRACE_EVAL_API("Gb29768_Sort(mode: %d, target: %d, rule: %d, memoryBank: %d, mask[32]: 0x%02x.., maskBitPointer: %d, maskBitLength: %d) = %d",
                   sortData->mode, sortData->target, sortData->rule, sortData->memoryBank, sortData->mask[0], sortData->maskBitPointer, sortData->maskBitLength, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gb29768_Read(STUHFL_T_Read *readData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gb29768_Read(readData);
#define TB_SIZE    256U
    char tb[2][TB_SIZE];
    TRACE_EVAL_API("Gb29768_Read(memoryBank: %d, wordPtr: %d, numBytesToRead: %d, numReadBytes: %d, pwd: 0x%s, data: 0x%s) = %d", readData->memoryBank, readData->wordPtr, readData->numBytesToRead, readData->numReadBytes, byteArray2HexString(tb[0], TB_SIZE, readData->pwd, STUHFL_D_PASSWORD_LEN), byteArray2HexString(tb[1], TB_SIZE, readData->data, STUHFL_D_MAX_READ_DATA_LEN), retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gb29768_Write(STUHFL_T_Write *writeData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gb29768_Write(writeData);
#define TB_SIZE    256U
    char tb[TB_SIZE];
    TRACE_EVAL_API("Gb29768_Write(memoryBank: %d, wordPtr: %d, pwd: 0x%s, data: 0x%02x%02x) = %d", writeData->memoryBank, writeData->wordPtr, byteArray2HexString(tb, TB_SIZE, writeData->pwd, STUHFL_D_PASSWORD_LEN), writeData->data[0], writeData->data[1], retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gb29768_Lock(STUHFL_T_Gb29768_Lock *lockData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gb29768_Lock(lockData);
#define TB_SIZE    256U
    char tb[TB_SIZE];
    TRACE_EVAL_API("Gb29768_Lock(memoryBank: 0x%02x, configuration: 0x%02x, action: 0x%02x, pwd: 0x%s) = %d", lockData->memoryBank, lockData->configuration, lockData->action, byteArray2HexString(tb, TB_SIZE, lockData->pwd, STUHFL_D_PASSWORD_LEN), retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gb29768_Kill(STUHFL_T_Kill *killData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gb29768_Kill(killData);
#define TB_SIZE    256U
    char tb[TB_SIZE];
    TRACE_EVAL_API("Gb29768_Kill(pwd: 0x%s) = %d", byteArray2HexString(tb, TB_SIZE, killData->pwd, STUHFL_D_PASSWORD_LEN), retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Gb29768_Erase(STUHFL_T_Gb29768_Erase *eraseData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Gb29768_Erase(eraseData);
#define TB_SIZE    256U
    char tb[TB_SIZE];
    TRACE_EVAL_API("Gb29768_Erase(memoryBank: %d, bytePtr: %d, numBytesToErase: %d, pwd: 0x%s) = %d", eraseData->memoryBank, eraseData->bytePtr, eraseData->numBytesToErase, byteArray2HexString(tb, TB_SIZE, eraseData->pwd, STUHFL_D_PASSWORD_LEN), retCode);
#undef TB_SIZE
    return retCode;
}



// ---- Iso6b ----
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Iso6b_Inventory(STUHFL_T_InventoryOption *invOption, STUHFL_T_InventoryData *invData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Iso6b_Inventory(invOption, invData);
    TRACE_EVAL_API("Iso6b_Inventory(rssiMode: %d, roundCnt: %d, inventoryDelay: %d, options: %d, tagListSizeMax: %d, tagListSize: %d, STATISTICS: tuningStatus: %d, roundCnt: %d, sensitivity: %d, adc: %d, frequency: %d, tagCnt: %d, emptySlotCnt: %d, collisionCnt: %d, slotCnt: %d, preambleErrCnt: %d, crcErrCnt: %d, TAGLIST: ..) = %d",
                   invOption->rssiMode, invOption->roundCnt, invOption->inventoryDelay, invOption->options,
                   invData->tagListSizeMax, invData->tagListSize,
                   invData->statistics.tuningStatus, invData->statistics.roundCnt, invData->statistics.sensitivity, invData->statistics.adc, invData->statistics.frequency, invData->statistics.tagCnt, invData->statistics.emptySlotCnt, invData->statistics.collisionCnt, invData->statistics.slotCnt, invData->statistics.preambleErrCnt, invData->statistics.crcErrCnt, retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Iso6b_Select(STUHFL_T_Iso6b_Select *selData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Iso6b_Select(selData);
    TRACE_EVAL_API("Iso6b_Select(mode: %d, group: %d, address: %d, bitMask: %d, filter[8]: 0x%02x..) = %d",
                   selData->mode, selData->group, selData->address, selData->bitMask, selData->filter[0], retCode);
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Iso6b_Read(STUHFL_T_Iso6b_Read *readData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Iso6b_Read(readData);
#define TB_SIZE    256U
    char tb[1][TB_SIZE];
    TRACE_EVAL_API("Iso6b_Read(address: %d, data: 0x%s) = %d", readData->address, byteArray2HexString(tb[0], TB_SIZE, readData->data, STUHFL_D_ISO6B_MAX_READ_DATA_LEN), retCode);
#undef TB_SIZE
    return retCode;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Iso6b_Write(STUHFL_T_Iso6b_Write *writeData)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Iso6b_Write(writeData);
    TRACE_EVAL_API("Iso6b_Write(address: %d, data: 0x%02x, tagReply: 0x%02x) = %d", writeData->address, writeData->data, writeData->tagReply, retCode);
    return retCode;
}



// ---- Inventory Runner ----
STUHFL_T_ActionId invRunnerId = 0;
typedef STUHFL_T_RET_CODE(*STUHFL_T_InventoryFinished)(STUHFL_T_InventoryData *data); // Finished Callback definition
STUHFL_T_RET_CODE _finishedCallback(STUHFL_T_InventoryData *data);
STUHFL_T_InventoryFinished callerFinishedCallback = NULL;

STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Inventory_RunnerStart(STUHFL_T_InventoryOption *option, STUHFL_T_InventoryCycle cycleCallback, STUHFL_T_InventoryFinished finishedCallback, STUHFL_T_InventoryData *data)
{
    // allow only one instance
    if (invRunnerId) {
        return STUHFL_ERR_BUSY;
    }

    // remember the finish callback
    callerFinishedCallback = finishedCallback;

    // start the runner
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Start(STUHFL_D_ACTION_INVENTORY, option, (STUHFL_T_ActionCycle)cycleCallback, data, (STUHFL_T_ActionFinished)_finishedCallback, &invRunnerId);

    TRACE_EVAL_API("Inventory_RunnerStart(rssiMode: %d, roundCnt: %d, inventoryDelay: %d, options: %d, tagListSizeMax: %d, tagListSize: %d) = %d",
                   option->rssiMode, option->roundCnt, option->inventoryDelay, option->options,
                   data->tagListSizeMax, data->tagListSize,
                   retCode);

    //
    if (retCode == STUHFL_ERR_NONE) {
        // block execution until thread has finished
#if defined(WIN32) || defined(WIN64)
        WaitForSingleObject((HANDLE)invRunnerId, INFINITE);
#elif defined(POSIX)
        pthread_join((pthread_t)invRunnerId, NULL);
#endif
    }
    return retCode;
}

#ifdef USE_INVENTORY_EXT
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Inventory_RunnerStartExt(STUHFL_T_InventoryOption *option, STUHFL_T_InventoryCycle cycleCallback, STUHFL_T_InventoryFinished finishedCallback, STUHFL_T_InventoryDataExt *data)
{
    // allow only one instance
    if (invRunnerId) {
        return STUHFL_ERR_BUSY;
    }

    // remember the finish callback
    callerFinishedCallback = finishedCallback;

    // start the runner
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Start(STUHFL_D_ACTION_INVENTORY_W_SLOT_STATISTICS, option, (STUHFL_T_ActionCycle)cycleCallback, data, (STUHFL_T_ActionFinished)_finishedCallback, &invRunnerId);

    TRACE_EVAL_API("Inventory_RunnerStartExt(rssiMode: %d, roundCnt: %d, inventoryDelay: %d, options: %d, tagListSizeMax: %d, tagListSize: %d, slotInfoList..) = %d",
                   option->rssiMode, option->roundCnt, option->inventoryDelay, option->options,
                   data->invData.tagListSizeMax, data->invData.tagListSize,
                   retCode);

    //
    if (retCode == STUHFL_ERR_NONE) {
        // block execution until thread has finished
#if defined(WIN32) || defined(WIN64)
        WaitForSingleObject((HANDLE)invRunnerId, INFINITE);
#elif defined(POSIX)
        pthread_join((pthread_t)invRunnerId, NULL);
#endif
    }
    return retCode;
}
#endif  // USE_INVENTORY_EXT

STUHFL_T_RET_CODE _finishedCallback(STUHFL_T_InventoryData *data)
{
    // callback caller if callback is provided
    if (callerFinishedCallback) {
        callerFinishedCallback(data);
    }
    // Reset ID to allow new runner to start
    invRunnerId = 0;
    return STUHFL_ERR_NONE;
}
STUHFL_DLL_API STUHFL_T_RET_CODE CALL_CONV Inventory_RunnerStop(void)
{
    TRACE_EVAL_API_START();
    STUHFL_T_RET_CODE retCode = STUHFL_F_Stop(invRunnerId);
    TRACE_EVAL_API("Inventory_RunnerStop() = %d", retCode);
    return retCode;
}

/**
  * @}
  */
/**
  * @}
  */
