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
#if !defined __STUHFL_ERR_H
#define __STUHFL_ERR_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

// --------------------------------------------------------------------------
#if defined(POSIX)
#define STUHFL_D_APPLI_ERROR_TYPE   (unsigned)    
#else
#define STUHFL_D_APPLI_ERROR_TYPE
#endif

#define STUHFL_ERR_GENERIC                      ( 1)    /**< generic error */
#define STUHFL_ERR_NONE                         ( 0)    /**< no error occured */
#define STUHFL_ERR_NOMEM                        (-1)    /**< not enough memory to perform the requested operation */
#define STUHFL_ERR_BUSY                         (-2)    /**< device or resource busy */
#define STUHFL_ERR_IO                           (-3)    /**< generic IO error */
#define STUHFL_ERR_TIMEOUT                      (-4)    /**< error due to timeout */
#define STUHFL_ERR_REQUEST                      (-5)    /**< invalid request or requested function can't be executed at the moment */
#define STUHFL_ERR_NOMSG                        (-6)    /**< No message of desired type */
#define STUHFL_ERR_PARAM                        (-7)    /**< Parameter error */
#define STUHFL_ERR_PROTO                        (-8)    /**< Protocol error */


#define STUHFL_ERR_LAST_ERROR                  (-32)  /**< last error number reserved for general errors */
#define STUHFL_ERR_FIRST_ST25RU3993_ERROR      (STUHFL_ERR_LAST_ERROR-1)   /**< first error number reserved for specific chip errors */

/* Errors primarily raised by ST25RU3993 itself, however codes like STUHFL_ERR_CHIP_CRCERROR can be rereused by other protocols */
#define STUHFL_ERR_CHIP_NORESP                      (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 0)    /* -33 */
#define STUHFL_ERR_CHIP_HEADER                      (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 1)
#define STUHFL_ERR_CHIP_PREAMBLE                    (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 2)
#define STUHFL_ERR_CHIP_RXCOUNT                     (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 3)
#define STUHFL_ERR_CHIP_CRCERROR                    (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 4)
#define STUHFL_ERR_CHIP_FIFO                        (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 5)
#define STUHFL_ERR_CHIP_COLL                        (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 6)
    
#define STUHFL_ERR_REFLECTED_POWER                  (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 16)
    
/* Upper level protocol errors for GEN2, e.g. a Write can fail when access command has failed... */
#define STUHFL_ERR_GEN2_SELECT                      (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 32)
#define STUHFL_ERR_GEN2_ACCESS                      (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 33)
#define STUHFL_ERR_GEN2_REQRN                       (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 34)
#define STUHFL_ERR_GEN2_CHANNEL_TIMEOUT             (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 35)
                                                                                               
/* Gen2V2 error codes */                                                                       
#define STUHFL_ERR_GEN2_ERRORCODE_OTHER             (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 36)
#define STUHFL_ERR_GEN2_ERRORCODE_NOTSUPPORTED      (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 37)
#define STUHFL_ERR_GEN2_ERRORCODE_PRIVILEGES        (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 38)
#define STUHFL_ERR_GEN2_ERRORCODE_MEMOVERRUN        (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 39)
#define STUHFL_ERR_GEN2_ERRORCODE_MEMLOCKED         (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 40)
#define STUHFL_ERR_GEN2_ERRORCODE_CRYPTO            (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 41)
#define STUHFL_ERR_GEN2_ERRORCODE_ENCAPSULATION     (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 42)
#define STUHFL_ERR_GEN2_ERRORCODE_RESPBUFOVERFLOW   (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 43)
#define STUHFL_ERR_GEN2_ERRORCODE_SECURITYTIMEOUT   (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 44)
#define STUHFL_ERR_GEN2_ERRORCODE_POWER_SHORTAGE    (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 45)
#define STUHFL_ERR_GEN2_ERRORCODE_NONSPECIFIC       (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 46)
                                                                                               
/* Upper level protocol errors for GB29768 */                                                  
#define STUHFL_ERR_GB29768_POWER_SHORTAGE           (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 48)  /*!< Not enough power from reader */
#define STUHFL_ERR_GB29768_PERMISSION_ERROR         (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 49)  /*!< Permission error */
#define STUHFL_ERR_GB29768_STORAGE_OVERFLOW         (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 50)  /*!< Tag memory too short */
#define STUHFL_ERR_GB29768_STORAGE_LOCKED           (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 51)  /*!< Tag memory locked */
#define STUHFL_ERR_GB29768_PASSWORD_ERROR           (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 52)  /*!< Password error */
#define STUHFL_ERR_GB29768_AUTH_ERROR               (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 53)  /*!< Authentication error */
#define STUHFL_ERR_GB29768_ACCESS_ERROR             (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 54)  /*!< Tag access error */
#define STUHFL_ERR_GB29768_ACCESS_TIMEOUT_ERROR     (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 55)  /*!< Tag access timeout */
#define STUHFL_ERR_GB29768_OTHER                    (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 56)  /*!< Other error occurred */

/* Upper level protocol errors for ISO6B */
#define STUHFL_ERR_ISO6B_NOTAG	                    (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 60)  /*!< No response from any tag */
#define STUHFL_ERR_ISO6B_IRQ                        (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 61)  /*!< IRQ error occured */
#define STUHFL_ERR_ISO6B_REG_FIFO                   (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 62)  /*!< FIFO error occured */
#define STUHFL_ERR_ISO6B_OTHER	                    (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 63)  /*!< Other error occurred */
#define STUHFL_ERR_ISO6B_ACCESS_TIMEOUT             (STUHFL_ERR_FIRST_ST25RU3993_ERROR - 64)  /*!< Tag access timeout */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __STUHFL_ERR_H
