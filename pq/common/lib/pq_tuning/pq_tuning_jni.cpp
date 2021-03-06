/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#define LOG_TAG "MHAL_JNI"

#define MTK_LOG_ENABLE 1
#include <jni.h>

#include <utils/Log.h>
#include <utils/threads.h>
#include <cutils/log.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include "ddp_drv.h"
#ifdef PQ_VER_2_0
#include "cust_color.h"
#include "cust_tdshp.h"
#include <vendor/mediatek/hardware/pq/2.0/IPictureQuality.h>
#include <PQServiceCommon.h>

using vendor::mediatek::hardware::pq::V2_0::IPictureQuality;
using vendor::mediatek::hardware::pq::V2_0::Result;
using vendor::mediatek::hardware::pq::V2_0::dispPQIndexParams;
#endif
using namespace android;

#define UNUSED(expr) do { (void)(expr); } while (0)

typedef enum {
    PQ_PurpTone_H = 0,
    PQ_SkinTone_H,
    PQ_GrassTone_H,
    PQ_SkyTone_H,
    PQ_PurpTone_S,
    PQ_SkinTone_S,
    PQ_GrassTone_S,
    PQ_SkyTone_S,
    PQ_Sharp_Adj,
    PQ_Sat_Adj,
    PQ_Hue_Adj,
    PQ_X_Axis,
    PQ_Y_Axis,
    PQ_ADV_Select,
    PQ_Contrast_Adj,
    PQ_TuningDimension
} PQ_TuningIndex_t;

#define PQ_PURPTONE_H_NUM   (19)
#define PQ_SKINTONE_H_NUM   (19)
#define PQ_GRASSTONE_H_NUM  (19)
#define PQ_SKYTONE_H_NUM    (19)
#define PQ_PURPTONE_S_NUM   (19)
#define PQ_SKINTONE_S_NUM   (19)
#define PQ_GRASSTONE_S_NUM  (19)
#define PQ_SKYTONE_S_NUM    (19)
#define PQ_SHARP_ADJ_NUM    (10) // 0x20 as 1.0
#define PQ_SAT_ADJ_NUM      (10) // 0x80 as 1.0
#define PQ_HUE_ADJ_NUM      (255) // 0x01 as -127, 0x80 as 0, 0xFF as +127
#define PQ_X_RANGE          (2047)// screen x coordinates
#define PQ_Y_RANGE          (2047)// screen y coordinates
#define PQ_ADV_MODE         (1)// screen y coordinates
#define PQ_CONTRAST_ADJ_NUM (0)

class PqTuningJni
{
public:
    PqTuningJni();
    ~PqTuningJni();

#ifdef PQ_VER_1_0
public:
    void getPqParam(DISP_PQ_PARAM *pParam);
    void setPqParam(const DISP_PQ_PARAM *pParam);

private:
    int drvID;
#endif
};

const static jint g_u4Range[PQ_TuningDimension] =
{
    PQ_PURPTONE_H_NUM , PQ_SKINTONE_H_NUM , PQ_GRASSTONE_H_NUM , PQ_SKYTONE_H_NUM ,
    PQ_PURPTONE_S_NUM , PQ_SKINTONE_S_NUM , PQ_GRASSTONE_S_NUM , PQ_SKYTONE_S_NUM ,
    PQ_SHARP_ADJ_NUM , PQ_SAT_ADJ_NUM , PQ_HUE_ADJ_NUM , PQ_X_RANGE , PQ_Y_RANGE , PQ_ADV_MODE,
    PQ_CONTRAST_ADJ_NUM
};

unsigned int u4Indics[PQ_TuningDimension];
PqTuningJni g_hPqTuningJni;
DISP_PQ_PARAM pqparam;


PqTuningJni::PqTuningJni()
{
#ifdef PQ_VER_1_0
    drvID = open("/proc/mtk_mdp_cmdq", O_RDONLY, 0);
    if (drvID < 0)
    {
        ALOGE("[PQ] open /proc/mtk_mdp_cmdq fail...");
    }
#endif
}

PqTuningJni::~PqTuningJni()
{
#ifdef PQ_VER_1_0
    close(drvID);
#endif
}

#ifdef PQ_VER_1_0
void PqTuningJni::getPqParam(DISP_PQ_PARAM *pParam)
{
    ioctl(drvID, DISP_IOCTL_GET_PQPARAM, pParam);
}

void PqTuningJni::setPqParam(const DISP_PQ_PARAM *pParam)
{
    ioctl(drvID, DISP_IOCTL_SET_PQPARAM, pParam);
}
#endif

static jint getRange(PQ_TuningIndex_t a_eIndex)
{
    return g_u4Range[a_eIndex];
}

static int getIndex(PQ_TuningIndex_t a_eIndex)
{
#ifdef PQ_VER_1_0
    g_hPqTuningJni.getPqParam(&pqparam);
#elif defined(PQ_VER_2_0)
    sp<IPictureQuality> service = IPictureQuality::getService();
    if (service == nullptr) {
        ALOGE("failed to get HW service");
        return 0;
    }

    service->getColorIndex(SCENARIO_PICTURE, PQ_PIC_MODE_STANDARD,
        [&] (Result retval, const dispPQIndexParams &index) {
        if (retval == Result::OK) {
            memcpy(&pqparam, &index, sizeof(DISP_PQ_PARAM));
        }
    });
    service->getTDSHPIndex(SCENARIO_PICTURE, PQ_PIC_MODE_STANDARD,
        [&] (Result retval, const dispPQIndexParams &index) {
        if (retval == Result::OK) {
            pqparam.u4SHPGain = index.u4SHPGain;
        }
    });
#else
    ALOGE("getIndex() is not support");

    return 0;
#endif

    u4Indics[PQ_Sharp_Adj]    = pqparam.u4SHPGain;
    u4Indics[PQ_Sat_Adj]      = pqparam.u4SatGain;
    // LumaAdj might be added in future
    u4Indics[PQ_SkinTone_H]   = pqparam.u4HueAdj[1];
    u4Indics[PQ_GrassTone_H]  = pqparam.u4HueAdj[2];
    u4Indics[PQ_SkyTone_H]    = pqparam.u4HueAdj[3];
    u4Indics[PQ_SkinTone_S]   = pqparam.u4SatAdj[1];
    u4Indics[PQ_GrassTone_S]  = pqparam.u4SatAdj[2];
    u4Indics[PQ_SkyTone_S]    = pqparam.u4SatAdj[3];

    return (int)u4Indics[a_eIndex];
}

static jboolean setIndex(PQ_TuningIndex_t a_eIndex , unsigned int a_u4Index)
{
    int i;
    int actionID=0, RegBase = 0, RegValue = 0, err = 0;
    u4Indics[a_eIndex] = a_u4Index;
#ifdef PQ_VER_1_0
    // get default
    g_hPqTuningJni.getPqParam(&pqparam);

    pqparam.u4SHPGain = u4Indics[PQ_Sharp_Adj];
    pqparam.u4SatGain = u4Indics[PQ_Sat_Adj];
    pqparam.u4HueAdj[1] = u4Indics[PQ_SkinTone_H];
    pqparam.u4HueAdj[2] = u4Indics[PQ_GrassTone_H];
    pqparam.u4HueAdj[3] = u4Indics[PQ_SkyTone_H];
    pqparam.u4SatAdj[1] = u4Indics[PQ_SkinTone_S];
    pqparam.u4SatAdj[2] = u4Indics[PQ_GrassTone_S];
    pqparam.u4SatAdj[3] = u4Indics[PQ_SkyTone_S];

    ALOGE("Setting PQ param : [ShpG %d SatG %d SkinH %d GrassH %d SkyH %d SkinS %d GrassS %d SkyS %d] \n",
    pqparam.u4SHPGain,pqparam.u4SatGain,
    pqparam.u4HueAdj[1],pqparam.u4HueAdj[2],pqparam.u4HueAdj[3],
    pqparam.u4SatAdj[1],pqparam.u4SatAdj[2],pqparam.u4SatAdj[3]);

    g_hPqTuningJni.setPqParam(&pqparam);
#elif defined(PQ_VER_2_0)
    sp<IPictureQuality> service = IPictureQuality::getService();
    if (service == nullptr) {
        ALOGE("failed to get HW service");
        return JNI_FALSE;
    }

    switch(a_eIndex){

        case PQ_Sharp_Adj:
            {
                 service->setPQIndex(a_u4Index, SCENARIO_PICTURE, TDSHP_FLAG_TUNING, SET_PQ_SHP_GAIN, PQ_DEFAULT_TRANSITION_OFF_STEP);
            break;
            }
        case PQ_Sat_Adj:
            {
                 service->setPQIndex(a_u4Index, SCENARIO_PICTURE, TDSHP_FLAG_TUNING, SET_PQ_SAT_GAIN, PQ_DEFAULT_TRANSITION_OFF_STEP);
            break;
            }
        case PQ_SkinTone_H:
            {
                 service->setPQIndex(a_u4Index, SCENARIO_PICTURE, TDSHP_FLAG_TUNING, SET_PQ_HUE_ADJ_SKIN, PQ_DEFAULT_TRANSITION_OFF_STEP);
            break;
            }
        case PQ_GrassTone_H:
            {
                 service->setPQIndex(a_u4Index, SCENARIO_PICTURE, TDSHP_FLAG_TUNING, SET_PQ_HUE_ADJ_GRASS, PQ_DEFAULT_TRANSITION_OFF_STEP);
            break;
            }
        case PQ_SkyTone_H:
            {
                 service->setPQIndex(a_u4Index, SCENARIO_PICTURE, TDSHP_FLAG_TUNING, SET_PQ_HUE_ADJ_SKY, PQ_DEFAULT_TRANSITION_OFF_STEP);
            break;
            }
        case PQ_SkinTone_S:
            {
                 service->setPQIndex(a_u4Index, SCENARIO_PICTURE, TDSHP_FLAG_TUNING, SET_PQ_SAT_ADJ_SKIN, PQ_DEFAULT_TRANSITION_OFF_STEP);
            break;
            }
        case PQ_GrassTone_S:
            {
                 service->setPQIndex(a_u4Index, SCENARIO_PICTURE, TDSHP_FLAG_TUNING, SET_PQ_SAT_ADJ_GRASS, PQ_DEFAULT_TRANSITION_OFF_STEP);
            break;
            }
        case PQ_SkyTone_S:
            {
                 service->setPQIndex(a_u4Index, SCENARIO_PICTURE, TDSHP_FLAG_TUNING, SET_PQ_SAT_ADJ_SKY, PQ_DEFAULT_TRANSITION_OFF_STEP);
            break;
            }
        default:
            break;
    }
#else
    ALOGE("setIndex() is not support");
#endif





    return JNI_TRUE;
}

/////////////////////////////////////////////////////////////////////////////////

static jint getPurpToneHRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_PurpTone_H);
}

static jint getPurpToneHIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_PurpTone_H);
}

static jboolean setPurpToneHIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_PurpTone_H , index);
}

/////////////////////////////////////////////////////////////////////////////////

static jint getSkinToneHRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_SkinTone_H);
}

static jint getSkinToneHIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_SkinTone_H);
}

static jboolean setSkinToneHIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_SkinTone_H , index);
}

/////////////////////////////////////////////////////////////////////////////////


static jint getGrassToneHRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_GrassTone_H);
}

static jint getGrassToneHIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_GrassTone_H);
}

static jboolean setGrassToneHIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_GrassTone_H , index);
}

/////////////////////////////////////////////////////////////////////////////////

static jint getSkyToneHRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_SkyTone_H);
}

static jint getSkyToneHIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_SkyTone_H);
}

static jboolean setSkyToneHIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_SkyTone_H , index);
}

/////////////////////////////////////////////////////////////////////////////////

static jint getPurpToneSRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_PurpTone_S);
}

static jint getPurpToneSIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_PurpTone_S);
}

static jboolean setPurpToneSIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_PurpTone_S , index);
}

/////////////////////////////////////////////////////////////////////////////////

static jint getSkinToneSRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_SkinTone_S);
}

static jint getSkinToneSIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_SkinTone_S);
}

static jboolean setSkinToneSIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_SkinTone_S , index);
}

/////////////////////////////////////////////////////////////////////////////////


static jint getGrassToneSRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_GrassTone_S);
}

static jint getGrassToneSIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_GrassTone_S);
}

static jboolean setGrassToneSIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_GrassTone_S , index);
}

/////////////////////////////////////////////////////////////////////////////////

static jint getSkyToneSRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_SkyTone_S);
}

static jint getSkyToneSIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_SkyTone_S);
}

static jboolean setSkyToneSIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_SkyTone_S , index);
}

/////////////////////////////////////////////////////////////////////////////////


static jint getSharpAdjRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_Sharp_Adj);
}

static jint getSharpAdjIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_Sharp_Adj);
}

static jboolean setSharpAdjIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_Sharp_Adj , index);
}

/////////////////////////////////////////////////////////////////////////////////


static jint getSatAdjRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_Sat_Adj);
}

static jint getSatAdjIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_Sat_Adj);
}

static jboolean setSatAdjIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_Sat_Adj , index);
}

/////////////////////////////////////////////////////////////////////////////////


static jint getHueAdjRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_Hue_Adj);
}

static jint getHueAdjIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_Hue_Adj);
}

static jboolean setHueAdjIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_Hue_Adj , index);
}

/////////////////////////////////////////////////////////////////////////////////


static jint getXAxisRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_X_Axis);
}

static jint getXAxisIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_X_Axis);
}

static jboolean setXAxisIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_X_Axis , index);
}

/////////////////////////////////////////////////////////////////////////////////

static jint getYAxisRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_Y_Axis);
}

static jint getYAxisIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_Y_Axis);
}

static jboolean setYAxisIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_Y_Axis , index);
}

/////////////////////////////////////////////////////////////////////////////////

static jint getPQ_ADV_SelectRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_ADV_Select);
}

static jint getPQ_ADV_SelectIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_ADV_Select);
}

static jboolean setPQ_ADV_SelectIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_ADV_Select , index);
}

/////////////////////////////////////////////////////////////////////////////////

static jint getContrastAdjRange(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getRange(PQ_Contrast_Adj);
}

static jint getContrastAdjIndex(JNIEnv *env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);

    return getIndex(PQ_Contrast_Adj);
}

static jboolean setContrastAdjIndex(JNIEnv *env, jobject thiz , int index)
{
    UNUSED(env);
    UNUSED(thiz);

    return setIndex(PQ_Contrast_Adj , index);
}


/////////////////////////////////////////////////////////////////////////////////

//JNI register
////////////////////////////////////////////////////////////////
static const char *classPathName = "com/mediatek/galleryfeature/pq/filter/Filter";

static JNINativeMethod g_methods[] = {

  {"nativeGetSkinToneHRange",  "()I", (void*)getSkinToneHRange },
  {"nativeGetSkinToneHIndex",  "()I", (void*)getSkinToneHIndex },
  {"nativeSetSkinToneHIndex",  "(I)Z", (void*)setSkinToneHIndex },
  {"nativeGetGrassToneHRange",  "()I", (void*)getGrassToneHRange },
  {"nativeGetGrassToneHIndex",  "()I", (void*)getGrassToneHIndex },
  {"nativeSetGrassToneHIndex",  "(I)Z", (void*)setGrassToneHIndex },
  {"nativeGetSkyToneHRange",  "()I", (void*)getSkyToneHRange },
  {"nativeGetSkyToneHIndex",  "()I", (void*)getSkyToneHIndex },
  {"nativeSetSkyToneHIndex",  "(I)Z", (void*)setSkyToneHIndex },
  {"nativeGetSkinToneSRange",  "()I", (void*)getSkinToneSRange },
  {"nativeGetSkinToneSIndex",  "()I", (void*)getSkinToneSIndex },
  {"nativeSetSkinToneSIndex",  "(I)Z", (void*)setSkinToneSIndex },
  {"nativeGetGrassToneSRange",  "()I", (void*)getGrassToneSRange },
  {"nativeGetGrassToneSIndex",  "()I", (void*)getGrassToneSIndex },
  {"nativeSetGrassToneSIndex",  "(I)Z", (void*)setGrassToneSIndex },
  {"nativeGetSkyToneSRange",  "()I", (void*)getSkyToneSRange },
  {"nativeGetSkyToneSIndex",  "()I", (void*)getSkyToneSIndex },
  {"nativeSetSkyToneSIndex",  "(I)Z", (void*)setSkyToneSIndex },
  {"nativeGetSharpAdjRange",  "()I", (void*)getSharpAdjRange},
  {"nativeGetSharpAdjIndex",  "()I", (void*)getSharpAdjIndex},
  {"nativeSetSharpAdjIndex",  "(I)Z", (void*)setSharpAdjIndex},
  {"nativeGetSatAdjRange",  "()I", (void*)getSatAdjRange },
  {"nativeGetSatAdjIndex",  "()I", (void*)getSatAdjIndex },
  {"nativeSetSatAdjIndex",  "(I)Z", (void*)setSatAdjIndex},
  {"nativeGetHueAdjRange",  "()I", (void*)getHueAdjRange },
  {"nativeGetHueAdjIndex",  "()I", (void*)getHueAdjIndex },
  {"nativeSetHueAdjIndex",  "(I)Z", (void*)setHueAdjIndex},
  {"nativeGetXAxisRange",  "()I", (void*)getXAxisRange },
  {"nativeGetXAxisIndex",  "()I", (void*)getXAxisIndex },
  {"nativeSetXAxisIndex",  "(I)Z", (void*)setXAxisIndex},
  {"nativeGetYAxisRange",  "()I", (void*)getYAxisRange },
  {"nativeGetYAxisIndex",  "()I", (void*)getYAxisIndex },
  {"nativeSetYAxisIndex",  "(I)Z", (void*)setYAxisIndex},
  {"nativeGetContrastAdjRange",  "()I", (void*)getContrastAdjRange },
  {"nativeGetContrastAdjIndex",  "()I", (void*)getContrastAdjIndex },
  {"nativeSetContrastAdjIndex",  "(I)Z", (void*)setContrastAdjIndex}

};

/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        ALOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        ALOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

// ----------------------------------------------------------------------------

/*
 * This is called by the VM when the shared library is first loaded.
 */

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    ALOGI("JNI_OnLoad");

    if (JNI_OK != vm->GetEnv((void **)&env, JNI_VERSION_1_4)) {
        ALOGE("ERROR: GetEnv failed");
        goto bail;
    }

    if (!registerNativeMethods(env, classPathName, g_methods, sizeof(g_methods) / sizeof(g_methods[0]))) {
        ALOGE("ERROR: registerNatives failed");
        goto bail;
    }

    result = JNI_VERSION_1_4;

    UNUSED(reserved);

bail:
    return result;
}

