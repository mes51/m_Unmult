#pragma once

#ifndef UNMULT_H
#define UNMULT_H

typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned short		u_int16;
typedef unsigned long		u_long;
typedef short int			int16;
#define PF_TABLE_BITS	12
#define PF_TABLE_SZ_16	4096

#define PF_DEEP_COLOR_AWARE 1

#include "AEConfig.h"

#ifdef AE_OS_WIN
    typedef unsigned short PixelType;
    #include <Windows.h>
#endif

#include "entry.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "AE_EffectCBSuites.h"
#include "String_Utils.h"
#include "AE_GeneralPlug.h"
#include "AEFX_ChannelDepthTpl.h"
#include "AEGP_SuiteHandler.h"
#include "AEFX_SuiteHelper.h"
#include "Smart_Utils.h"

#include "m_Unmult_Strings.h"

#define	MAJOR_VERSION   1
#define	MINOR_VERSION   0
#define	BUG_VERSION     0
#define	STAGE_VERSION   PF_Stage_DEVELOP
#define	BUILD_VERSION   1

#define	SKELETON_GAIN_MIN   0
#define	SKELETON_GAIN_MAX   100
#define	SKELETON_GAIN_DFLT  10

enum
{
    UNMULT_INPUT = 0,
    UNMULT_NUM_PARAM
};

extern "C"
{
    DllExport PF_Err EffectMain(PF_Cmd cmd, PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output, void *extra);
}

#endif // UNMULT_H