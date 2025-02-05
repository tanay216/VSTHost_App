#include "pch.h"
// CapturePluginParams.h
#ifndef CAPTUREPLUGINPARAMS_H
#define CAPTUREPLUGINPARAMS_H

#include <AK/SoundEngine/Common/IAkPlugin.h>

// Parameter IDs for the CapturePlugin (add more as needed).
enum CapturePluginParamIDs
{
    CAPTURE_VOLUME_PROP = 0,
    CAPTURE_SAMPLE_MAX_PARAMS  // Keep last
};

// Structure for CapturePlugin parameters.
struct CapturePluginParamStruct
{
    float fDummyParameter;  // Replace with your actual parameters.
};

//-----------------------------------------------------------------------------
// Name: class CapturePluginParams
// Desc: Implementation of the parameter node for the CapturePlugin.
//-----------------------------------------------------------------------------
class CapturePluginParams : public AK::IAkPluginParam
{
public:
    CapturePluginParams();
    virtual ~CapturePluginParams();
    CapturePluginParams(const CapturePluginParams& in_rCopy);

    // Create a duplicate instance.
    virtual AK::IAkPluginParam* Clone(AK::IAkPluginMemAlloc* in_pAllocator) override;

    // Init/Term.
    virtual AKRESULT Init(AK::IAkPluginMemAlloc* in_pAllocator,
        const void* in_pParamsBlock,
        AkUInt32 in_ulBlockSize) override;
    virtual AKRESULT Term(AK::IAkPluginMemAlloc* in_pAllocator) override;

    // Set parameter block.
    virtual AKRESULT SetParamsBlock(const void* in_pParamsBlock,
        AkUInt32 in_ulBlockSize) override;

    // Optionally, update one parameter.
    virtual AKRESULT SetParam(AkPluginParamID in_ParamID,
        const void* in_pValue,
        AkUInt32 in_ulParamSize) override;

    const CapturePluginParamStruct& GetCurrentParams() const { return m_params; }

private:
    CapturePluginParamStruct m_params;
};

#endif // CAPTUREPLUGINPARAMS_H
