#pragma once

#include "AreaConfiguration.h"

class CBitParameter;

class CBitwiseAreaConfiguration : public CAreaConfiguration
{
public:
    CBitwiseAreaConfiguration(const CConfigurableElement* pConfigurableElement, const CSyncerSet* pSyncerSet);

private:
    // Blackboard copies
    virtual void copyTo(CParameterBlackboard* pToBlackboard, uint32_t uiOffset) const;
    virtual void copyFrom(const CParameterBlackboard* pFromBlackboard, uint32_t uiOffset);

    // Endianness
    bool _bBigEndian;
};

