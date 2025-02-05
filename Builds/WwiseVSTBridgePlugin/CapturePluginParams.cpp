// CapturePluginParams.cpp
#include "pch.h"
#include "CapturePluginParams.h"
#include <AK/Tools/Common/AkBankReadHelpers.h>
#include <AK/SoundEngine/Common/AkSimd.h>

CapturePluginParams::CapturePluginParams()
{
    // Initialize default parameter values.
    m_params.fDummyParameter = 1.0f;
}

CapturePluginParams::~CapturePluginParams()
{
}

CapturePluginParams::CapturePluginParams(const CapturePluginParams& in_rCopy)
{
    m_params = in_rCopy.m_params;
}

AK::IAkPluginParam* CapturePluginParams::Clone(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AKASSERT(in_pAllocator != nullptr);
    return AK_PLUGIN_NEW(in_pAllocator, CapturePluginParams(*this));
}

AKRESULT CapturePluginParams::Init(AK::IAkPluginMemAlloc* in_pAllocator,
    const void* in_pParamsBlock,
    AkUInt32 in_ulBlockSize)
{
    if (in_ulBlockSize == 0)
    {
        // Use default parameters.
        m_params.fDummyParameter = 1.0f;
        return AK_Success;
    }
    return SetParamsBlock(in_pParamsBlock, in_ulBlockSize);
}

AKRESULT CapturePluginParams::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AKASSERT(in_pAllocator != nullptr);
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT CapturePluginParams::SetParamsBlock(const void* in_pParamsBlock,
    AkUInt32 in_ulBlockSize)
{
    AKRESULT eResult = AK_Success;
    AkUInt8* pParamsBlock = (AkUInt8*)in_pParamsBlock;

    // Example: read one parameter (dummy value).
    m_params.fDummyParameter = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    CHECKBANKDATASIZE(in_ulBlockSize, eResult);
    return eResult;
}

AKRESULT CapturePluginParams::SetParam(AkPluginParamID in_ParamID,
    const void* in_pValue,
    AkUInt32 in_ulParamSize)
{
    AKASSERT(in_pValue != nullptr);
    if (in_pValue == nullptr)
    {
        return AK_InvalidParameter;
    }
    AKRESULT eResult = AK_Success;
    switch (in_ParamID)
    {
    case CAPTURE_VOLUME_PROP:
        m_params.fDummyParameter = *(reinterpret_cast<const float*>(in_pValue));
        break;
    default:
        AKASSERT(!"Invalid parameter.");
        eResult = AK_InvalidParameter;
        break;
    }
    return eResult;
}
