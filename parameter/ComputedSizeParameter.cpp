/* <auto_header>
 * <FILENAME>
 * 
 * INTEL CONFIDENTIAL
 * Copyright © 2011 Intel 
 * Corporation All Rights Reserved.
 * 
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel’s prior express written permission.
 * 
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 * 
 *  AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 * 
 * 
 * </auto_header>
 */
#include "ComputedSizeParameter.h"
#include "ConfigurableElement.h"
#include "ParameterType.h"
#include "InstanceConfigurableElement.h"
#include "ComputedSizeParameterType.h"
#include <assert.h>
#include <sstream>
#include "ParameterAccessContext.h"

#define base CParameter

CComputedSizeParameter::CComputedSizeParameter(const string& strName, const CTypeElement* pTypeElement) : base(strName, pTypeElement), _pReferredElement(NULL)
{
}

uint32_t CComputedSizeParameter::getFootPrint() const
{
    // Virtual parameter
    return 0;
}

bool CComputedSizeParameter::init(string& strError)
{
    // Seek referred parameter in parent hierarchy
    const CElement* pParent = getParent();

    assert(pParent);

    _pReferredElement = static_cast<const CConfigurableElement*>(pParent->findChild(static_cast<const CComputedSizeParameterType*>(getTypeElement())->getReferredElementName()));

    if (!_pReferredElement) {

        strError = "Could not find referred Parameter " + getTypeElement()->getName() + " from " + getKind() + " " + getPath();

        return false;
    }

    return true;
}

bool CComputedSizeParameter::doSetValue(const string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext)
{
    // Read only kind of parameter, can't be set!
    (void)strValue;
    (void)uiOffset;

    parameterAccessContext.setError("Read only parameter");

    return false;
}

void CComputedSizeParameter::doGetValue(string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    assert(_pReferredElement);
    // Parameter can't be an array
    assert(uiOffset == getOffset());

    static_cast<const CParameterType*>(getTypeElement())->asString(_pReferredElement->getFootPrint(), strValue, parameterAccessContext);
}

