#ifndef __AUDIO_TYPE_H__
#define __AUDIO_TYPE_H__

#include <stdint.h>
#include <sys/types.h>

#include <utils/Errors.h>
#include <utils/Vector.h>
#include <utils/String16.h>
#include <utils/String8.h>
#include <hardware_legacy/AudioSystemLegacy.h>

#include "AudioAssert.h"

#ifndef int8_t
typedef signed char         int8_t;
#endif

#ifndef uint8_t
typedef unsigned char       uint8_t;
#endif

#ifndef int16_t
typedef signed short        int16_t;
#endif

#ifndef uint16_t
typedef unsigned short      uint16_t;
#endif

#ifndef int32_t
typedef signed int          int32_t;
#endif

#ifndef uint32_t
typedef unsigned int        uint32_t;
#endif

#ifndef status_t
typedef signed int          status_t;
#endif

/*
#ifndef ssize_t
typedef signed int          ssize_t;
#endif
*/

#ifndef int32
typedef signed int          int32;
#endif

/*
#ifndef size_t
typedef long int      size_t;
#endif
*/

#ifndef uint32
typedef unsigned int        uint32;
#endif



// when call I2S start , need parameters for I2STYPE
typedef enum {
    MATV,                         //I2S Input For ATV
    FMRX,                         //I2S Input For FMRX
    FMRX_32K,                 //I2S Input For FMRX_32K
    FMRX_48K,                 //I2S Input For FMRX_48K
    I2S0OUTPUT,             //   I2S0 output
    I2S1OUTPUT,             //   I2S1 output
    HOA_SAMPLERATE,   //   use for HQA support
    NUM_OF_I2S
} I2STYPE;

typedef enum {
    MATV_DIGITAL,                         //I2S Input For MATV
    MATV_ANALOG                         //Line Input For MATV
} MATVTYPE;

#define AUDIO_LOCK_TIMEOUT_VALUE_MS (5000)  //The same with ANR

#define VM_FILE_NAME_LEN_MAX (128)

#ifdef UPLINK_LOW_LATENCY
#define AUDIO_RECORD_DROP_MS (0)    //not drop for latency
#else
#define AUDIO_RECORD_DROP_MS (120) // at max 200
#endif

#if 1 //HP switch 
//#define HIFIDAC_SWITCH
//#define SWITCH_BEFORE_HPAMP
//#define HIFI_SWITCH_BY_AUDENH

//#define EXTDAC_PMIC_MUTE
//#define RINGTONE_USE_PMIC
#endif

typedef enum {
    AUDIO_MIC_MODE_ACC = 1,
    AUDIO_MIC_MODE_DCC,
    AUDIO_MIC_MODE_DMIC,
    AUDIO_MIC_MODE_DMIC_LP,
    AUDIO_MIC_MODE_DCCECMDIFF,
    AUDIO_MIC_MODE_DCCECMSINGLE,
} AUDIO_MIC_MODE;

#endif
