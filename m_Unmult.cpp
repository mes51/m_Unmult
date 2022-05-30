#include "m_Unmult.h"

static PF_Err Unmult32(void* refcon, A_long xL, A_long yL, PF_Pixel32* inP, PF_Pixel32* outP) noexcept
{
    const PF_FpLong ca = inP->alpha;
    const PF_FpLong cr = inP->red;
    const PF_FpLong cg = inP->green;
    const PF_FpLong cb = inP->blue;

    PF_FpLong r = cr;
    PF_FpLong g = cg;
    PF_FpLong b = cb;

    if (ca < PF_MAX_CHAN16)
    {
        r = cr * ca;
        g = cg * ca;
        b = cb * ca;
    }

    PF_FpLong rate = max(max(r, g), b);
    if (rate != 0.0)
    {
        rate = 1.0 / rate;
        const PF_FpLong tr = r * rate;
        const PF_FpLong tg = g * rate;
        const PF_FpLong tb = b * rate;

        PF_FpLong a = 0;
        if (tb > 0.0)
        {
            a = b / tb;
        }
        else if (tg > 0.0)
        {
            a = g / tg;
        }
        else if (tr > 0.0)
        {
            a = r / tr;
        }

        outP->alpha = a;
        if (a > 0)
        {
            outP->red = tr;
            outP->green = tg;
            outP->blue = tb;
        }
    }
    else
    {
        outP->alpha = 0;
    }

    return PF_Err_NONE;
}

static PF_Err Unmult16(void* refcon, A_long xL, A_long yL, PF_Pixel16* inP, PF_Pixel16* outP) noexcept
{
    const A_u_short ca = inP->alpha;
    const A_u_short cr = inP->red;
    const A_u_short cg = inP->green;
    const A_u_short cb = inP->blue;

    PF_FpLong r = cr;
    PF_FpLong g = cg;
    PF_FpLong b = cb;

    if (ca < PF_MAX_CHAN16)
    {
        r = (cr * ca) / (PF_FpLong)PF_MAX_CHAN16;
        g = (cg * ca) / (PF_FpLong)PF_MAX_CHAN16;
        b = (cb * ca) / (PF_FpLong)PF_MAX_CHAN16;
    }

    PF_FpLong rate = max(max(r, g), b);
    if (rate > 0.0)
    {
        rate = PF_MAX_CHAN16 / rate;
        const PF_FpLong tr = r * rate;
        const PF_FpLong tg = g * rate;
        const PF_FpLong tb = b * rate;

        A_u_short a = 0;
        if (tb > 0.0)
        {
            a = (A_u_short)min((b * PF_MAX_CHAN16) / tb, PF_MAX_CHAN16);
        }
        else if (tg > 0.0)
        {
            a = (A_u_short)min((g * PF_MAX_CHAN16) / tg, PF_MAX_CHAN16);
        }
        else if (tr > 0.0)
        {
            a = (A_u_short)min((r * PF_MAX_CHAN16) / tr, PF_MAX_CHAN16);
        }

        if (a > 0)
        {
            outP->alpha = a;
            outP->red = (A_u_short)min(round(tr), PF_MAX_CHAN16);
            outP->green = (A_u_short)min(round(tg), PF_MAX_CHAN16);
            outP->blue = (A_u_short)min(round(tb), PF_MAX_CHAN16);
        }
        else
        {
            outP->alpha = 0;
        }
    }
    else
    {
        outP->alpha = 0;
    }

    return PF_Err_NONE;
}

static PF_Err Unmult8(void* refcon, A_long xL, A_long yL, PF_Pixel8* inP, PF_Pixel8* outP) noexcept
{
    const A_u_char ca = inP->alpha;
    const A_u_char cr = inP->red;
    const A_u_char cg = inP->green;
    const A_u_char cb = inP->blue;

    PF_FpLong r = cr;
    PF_FpLong g = cg;
    PF_FpLong b = cb;

    if (ca < PF_MAX_CHAN8)
    {
        r = (cr * ca) / (PF_FpLong)PF_MAX_CHAN8;
        g = (cg * ca) / (PF_FpLong)PF_MAX_CHAN8;
        b = (cb * ca) / (PF_FpLong)PF_MAX_CHAN8;
    }

    PF_FpLong rate = max(max(r, g), b);
    if (rate > 0.0)
    {
        rate = PF_MAX_CHAN8 / rate;
        const PF_FpLong tr = r * rate;
        const PF_FpLong tg = g * rate;
        const PF_FpLong tb = b * rate;

        A_u_char a = 0;
        if (tb > 0.0)
        {
            a = (A_u_char)min((b * PF_MAX_CHAN8) / tb, PF_MAX_CHAN8);
        }
        else if (tg > 0.0)
        {
            a = (A_u_char)min((g * PF_MAX_CHAN8) / tg, PF_MAX_CHAN8);
        }
        else if (tr > 0.0)
        {
            a = (A_u_char)min((r * PF_MAX_CHAN8) / tr, PF_MAX_CHAN8);
        }

        if (a > 0)
        {
            outP->alpha = a;
            outP->red = (A_u_char)min(round(tr), PF_MAX_CHAN8);
            outP->green = (A_u_char)min(round(tg), PF_MAX_CHAN8);
            outP->blue = (A_u_char)min(round(tb), PF_MAX_CHAN8);
        }
        else
        {
            outP->alpha = 0;
        }
    }
    else
    {
        outP->alpha = 0;
    }

    return PF_Err_NONE;
}

static PF_Err RenderProcess(PF_InData* in_data, PF_EffectWorld* input, PF_OutData* out_data, PF_EffectWorld* output)
{
    PF_Err err = PF_Err_NONE;

    PF_Point origin;
    PF_Rect area;
    PF_PixelFormat format = PF_PixelFormat_INVALID;

    AEGP_SuiteHandler suites(in_data->pica_basicP);
    AEFX_SuiteScoper<PF_WorldSuite2> worldSuite2 = AEFX_SuiteScoper<PF_WorldSuite2>(in_data, kPFWorldSuite, kPFWorldSuiteVersion2, out_data);
    ERR(worldSuite2->PF_GetPixelFormat(input, &format));

    if (err)
    {
        return err;
    }

    switch (format)
    {
    case PF_PixelFormat_ARGB128:
        ERR(suites.IterateFloatSuite2()->iterate(in_data, 0, output->height, input, NULL, NULL, Unmult32, output));
        break;
    case PF_PixelFormat_ARGB64:
        ERR(suites.Iterate16Suite2()->iterate(in_data, 0, output->height, input, NULL, NULL, Unmult16, output));
        break;
    case PF_PixelFormat_ARGB32:
        ERR(suites.Iterate8Suite2()->iterate(in_data, 0, output->height, input, NULL, NULL, Unmult8, output));
        break;
    default:
        break;
    }

    origin.h = in_data->output_origin_x;
    origin.v = in_data->output_origin_y;

    area.left = 0;
    area.right = output->width;
    area.top = 0;
    area.bottom = output->height;

    return err;
}

static PF_Err About(const PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output)
{
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg, "%s v%d.%d\r%s", Unmult::PLUGIN_NAME.c_str(), MAJOR_VERSION, MINOR_VERSION, Unmult::PLUGIN_DESCRIPTION.c_str());

    return PF_Err_NONE;
}

static PF_Err GlobalSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) noexcept
{
    out_data->my_version = PF_VERSION(MAJOR_VERSION, MINOR_VERSION, BUG_VERSION, STAGE_VERSION, BUILD_VERSION);

    out_data->out_flags2 = PF_OutFlag2_FLOAT_COLOR_AWARE | PF_OutFlag2_SUPPORTS_SMART_RENDER | PF_OutFlag2_SUPPORTS_THREADED_RENDERING;
    out_data->out_flags = PF_OutFlag_DEEP_COLOR_AWARE | PF_OutFlag_PIX_INDEPENDENT;

    return PF_Err_NONE;
}

static PF_Err ParamsSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) noexcept
{
    out_data->num_params = UNMULT_NUM_PARAM;
    return PF_Err_NONE;
}

static PF_Err Render(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output)
{
    return RenderProcess(in_data, &params[UNMULT_INPUT]->u.ld, out_data, output);
}

static PF_Err PreRender(PF_InData* in_data, PF_OutData* out_data, PF_PreRenderExtra* extra)
{
    if (!extra)
    {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    PF_Err err = PF_Err_NONE;

    PF_RenderRequest req = extra->input->output_request;
    PF_CheckoutResult inResult;

    req.channel_mask |= PF_ChannelMask_RED | PF_ChannelMask_GREEN | PF_ChannelMask_BLUE;
    req.preserve_rgb_of_zero_alpha = FALSE;

    ERR(
        extra->cb->checkout_layer(
            in_data->effect_ref,
            UNMULT_INPUT,
            UNMULT_INPUT,
            &req,
            in_data->current_time,
            in_data->time_step,
            in_data->time_scale,
            &inResult
        )
    );

    if (!err)
    {
        UnionLRect(&inResult.result_rect, &extra->output->result_rect);
        UnionLRect(&inResult.max_result_rect, &extra->output->max_result_rect);
    }

    return err;
}

static PF_Err SmartRender(PF_InData* in_data, PF_OutData* out_data, const PF_SmartRenderExtra* extra)
{
    if (!extra)
    {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    PF_Err err = PF_Err_NONE;
    PF_Err err2 = PF_Err_NONE;

    PF_EffectWorld* inputWorld;
    PF_EffectWorld* outputWorld;
    ERR(extra->cb->checkout_layer_pixels(in_data->effect_ref, UNMULT_INPUT, &inputWorld));
    ERR(extra->cb->checkout_output(in_data->effect_ref, &outputWorld));

    ERR(RenderProcess(in_data, inputWorld, out_data, outputWorld));

    ERR(PF_ABORT(in_data));

    ERR2(extra->cb->checkin_layer_pixels(in_data->effect_ref, UNMULT_INPUT));

    return err;
}

extern "C" DllExport PF_Err PluginDataEntryFunction(PF_PluginDataPtr inPtr, PF_PluginDataCB inPluginDataCallBackPtr, SPBasicSuite* inSPBasicSuitePtr, const char* inHostName, const char* inHostVersion)
{
    PF_Err result = PF_Err_INVALID_CALLBACK;

    result = PF_REGISTER_EFFECT(
        inPtr,
        inPluginDataCallBackPtr,
        "m_Unmult",
        "mes51 m_Unmult",
        "mes51",
        AE_RESERVED_INFO
    );

    return result;
}


PF_Err EffectMain(PF_Cmd cmd, PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output, void *extra)
{
    PF_Err err = PF_Err_NONE;

    try
    {
        switch (cmd)
        {
        case PF_Cmd_ABOUT:
            err = About(in_data, out_data, params, output);
            break;
        case PF_Cmd_GLOBAL_SETUP:
            err = GlobalSetup(in_data, out_data, params, output);
            break;
        case PF_Cmd_PARAMS_SETUP:
            err = ParamsSetup(in_data, out_data, params, output);
            break;
        case PF_Cmd_RENDER:
            err = Render(in_data, out_data, params, output);
            break;
        case PF_Cmd_SMART_PRE_RENDER:
            err = PreRender(in_data, out_data, static_cast<PF_PreRenderExtra*>(extra));
            break;
        case PF_Cmd_SMART_RENDER:
            err = SmartRender(in_data, out_data, static_cast<PF_SmartRenderExtra*>(extra));
            break;
        default:
            break;
        }
    }
    catch(const PF_Err &thrown_err)
    {
        err = thrown_err;
    }

    return err;
}

