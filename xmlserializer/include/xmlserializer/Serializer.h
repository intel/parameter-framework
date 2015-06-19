/*
 * Copyright (c) 2015, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include "xmlserializer/Node.h"

#include "XmlStreamDocSink.h"
#include "XmlMemoryDocSource.h"
#include "XmlSource.h"

#include <string>
#include <ostream>
#include <stdexcept>

namespace core
{
namespace xml
{
namespace serialization
{

/** Allows to serialize XML data */
class Serializer : private IXmlSource
{
public:

    /** Serialize XML data using a dedicated binding
     *
     * @param[out] output destination stream of the generated serialized data
     * @param[in] rootNode the structure which is binding XML data to the structure in memory
     * @param[in] schema XML schema file corresponding to generated XML
     *            if empty, XML validity is not checked
     */
    Serializer(std::ostream &output,
               const binding::Node &rootNode,
               const std::string &schemaFile = "")
        : mCurrentNode(rootNode)
    {
        if (!output.good()) {
            throw std::runtime_error("When serializing" + rootNode.first +
                                     ": Provided output stream is not valid.");
        }

        std::string error;
        CXmlSerializingContext context(error);
        CXmlMemoryDocSource source(this, !schemaFile.empty(), rootNode.first,
                                   schemaFile, "parameter-framework", "");

        CXmlStreamDocSink sink(output);
        if (!sink.process(source, context)) {
            throw std::runtime_error(error);
        }
    }

private:

    /** Private constructor used to create serializer for subnodes
     *
     * @param[in] rootNode the structure which is binding xml data to the structure in memory
     */
    Serializer(const binding::Node &node)
        : mCurrentNode(node)
    {
    }

    /** Parse the Node member and modify the current XML node in consequence
     *
     * @param[in] xmlElement current xml node
     * @param[out] context error context used to communicate errors to upper layers
     */
    virtual void toXml(CXmlElement& xmlElement,
                       CXmlSerializingContext& context) const override
    {
        for(auto &attribute : mCurrentNode.second.attributes) {
            xmlElement.setAttribute(attribute.getName(), attribute.get());
        }

        for(auto &child : mCurrentNode.second.childs) {
            // Create corresponding child element
            CXmlElement xmlChildElement;
            xmlElement.createChild(xmlChildElement, child.first);

            // Continue serialization
            Serializer childSerializer(child);
            childSerializer.toXml(xmlChildElement, context);
        }
    }

    /** Node parsed by this serializer */
    binding::Node mCurrentNode;
};

} /** serialization namespace */
} /** xml namespace */
} /** core namespace */
