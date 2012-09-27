#include "BitwiseAreaConfiguration.h"
#include "BitParameter.h"
#include "Subsystem.h"

#define base CAreaConfiguration

CBitwiseAreaConfiguration::CBitwiseAreaConfiguration(const CConfigurableElement *pConfigurableElement, const CSyncerSet *pSyncerSet)
    : base(pConfigurableElement, pSyncerSet, static_cast<const CBitParameter*>(pConfigurableElement)->getBelongingBlockSize()),
      _bBigEndian(pConfigurableElement->getBelongingSubsystem()->isBigEndian())
{
}

// Blackboard copies
void CBitwiseAreaConfiguration::copyTo(CParameterBlackboard* pToBlackboard, uint32_t uiOffset) const
{
    // Beware this code works on little endian architectures only!
    const CBitParameter* pBitParameter = static_cast<const CBitParameter*>(_pConfigurableElement);

    uint64_t uiSrcData = 0;
    uint64_t uiDstData = 0;

    /// Read/modify/write

    // Read dst blackboard
    pToBlackboard->readInteger(&uiDstData, pBitParameter->getBelongingBlockSize(), uiOffset, _bBigEndian);

    // Read src blackboard
    _blackboard.readInteger(&uiSrcData, pBitParameter->getBelongingBlockSize(), 0, _bBigEndian);

    // Convert
    uiDstData = pBitParameter->merge(uiDstData, uiSrcData);

    // Write dst blackboard
    pToBlackboard->writeInteger(&uiDstData, pBitParameter->getBelongingBlockSize(), uiOffset, _bBigEndian);
}

void CBitwiseAreaConfiguration::copyFrom(const CParameterBlackboard* pFromBlackboard, uint32_t uiOffset)
{
    // Beware this code works on little endian architectures only!
    const CBitParameter* pBitParameter = static_cast<const CBitParameter*>(_pConfigurableElement);

    uint64_t uiSrcData = 0;
    uint64_t uiDstData = 0;

    /// Read/modify/write

    // Read dst blackboard
    _blackboard.readInteger(&uiDstData, pBitParameter->getBelongingBlockSize(), 0, _bBigEndian);

    // Read src blackboard
    pFromBlackboard->readInteger(&uiSrcData, pBitParameter->getBelongingBlockSize(), uiOffset, _bBigEndian);

    // Convert
    uiDstData = pBitParameter->merge(uiDstData, uiSrcData);

    // Write dst blackboard
    _blackboard.writeInteger(&uiDstData, pBitParameter->getBelongingBlockSize(), 0, _bBigEndian);
}

