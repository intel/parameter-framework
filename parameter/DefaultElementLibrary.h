/*
 * INTEL CONFIDENTIAL
 * Copyright 2013 Intel
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
 */
#pragma once

#include "ElementLibrary.h"
#include "ElementBuilder.h"

#include <map>
#include <string>

/** Factory that creates an element given an xml element. If no matching builder is found, it uses
  * the default builder.
  *
  * @tparam CDefaultElementBuilder is the class of the element builder to use if no corresponding
  *                                builder is found for a given xml element.
  */
template<class CDefaultElementBuilder>
class CDefaultElementLibrary: public CElementLibrary
{
public:

    explicit CDefaultElementLibrary(bool bEnableDefaultMechanism = true);
    virtual ~CDefaultElementLibrary() {}

    /** Enable the default builder fallback mechanism.
      * @see createElement() for more detail on this mechanism.
      *
      * @param[in] bEnable if true/false, activate/deactivate the default builder mechanism.
      */
    void enableDefaultMechanism(bool bEnable) {
        _bEnableDefaultMechanism = bEnable;
    }


    /** Create and return an element instanciated depending on an xmlElement.
      *
      * @param[in] xmlElement: The xml element used to find a matching builder
      *
      * @return If a matching builder is found, return an element created from the builder,
      *         otherwise:
      *             If the default mechanism is enable (@see enableDefaultMechanism),
      *                 create the elemen with the default element builder.
      *             otherwise, return NULL.
      */
    CElement* createElement(const CXmlElement& xmlElement) const;

private:
    bool _bEnableDefaultMechanism;
    CDefaultElementBuilder _DefaultElementBuilder;
};

template<class CDefaultElementBuilder>
CDefaultElementLibrary<CDefaultElementBuilder>::CDefaultElementLibrary(bool bEnableDefaultMechanism) :
        _bEnableDefaultMechanism(bEnableDefaultMechanism),
        _DefaultElementBuilder() {}

template<class CDefaultElementBuilder>
CElement* CDefaultElementLibrary<CDefaultElementBuilder>::createElement(const CXmlElement& xmlElement) const
{
    CElement* builtElement = CElementLibrary::createElement(xmlElement);

    if (builtElement != NULL) {
        // The element was created, return it
        return builtElement;
    }

    if (!_bEnableDefaultMechanism) {
        // The default builder mechanism is not enabled
        return NULL;
    }

    // Use the default builder
    return _DefaultElementBuilder.createElement(xmlElement);
}

