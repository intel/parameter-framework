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

#include "XmlSink.h"
#include "XmlDocSource.h"
#include "XmlMemoryDocSink.h"

namespace core
{
namespace xml
{
namespace serialization
{

/** Indicates the source of the data
 *
 * File: the provided source will point to a file
 * String: the provided source will point contains the data directly
 */
enum class ImportSource { File, String };

/** Allows to deserialize XML data
 *
 * @tparam sourceType the format used for the source of data
 */
template<ImportSource sourceType>
class Deserializer : private IXmlSink
{
public:

    /** Deserialize xml data using a dedicated binding
     *
     * @param[in] xmlSource holder of the data to deserialize
     *              if ImportSource is String,
     *                  xmlSource should be a string containing the xml to deserialize
     *                  XIncludes tags are not available, the file must contain all needed data
     *              if ImportSource is File,
     *                  xmlSource should be a string containing the path to the xml file
     *                  XIncludes tags are processed
     * @param[in] rootNode the structure which is binding xml data to the structure in memory
     * @param[in] schema XML schema file corresponding to xmlSource
     *            if empty, XML validity is not checked
     */
    Deserializer(const std::string &xmlSource,
                 binding::Node rootNode,
                 const std::string &schema = "")
        : mCurrentNode(rootNode)
    {
        std::string error;
        CXmlSerializingContext context(error);

        // If a file is used, xincludes can be processed
        auto doc = CXmlDocSource::mkXmlDoc(xmlSource,
                                           sourceType == ImportSource::File,
                                           sourceType == ImportSource::File,
                                           error);

        // Validate through schema only if schema path is not empty
        CXmlDocSource source(doc, !schema.empty(), schema, rootNode.first);

        CXmlMemoryDocSink sink(this);
        if (!sink.process(source, context)) {
            throw std::runtime_error(error);
        }
    }

private:

    /** Private constructor used to create deserializer for subnodes
     *
     * @param[in] rootNode the structure which is binding xml data to the structure in memory
     */
    Deserializer(binding::Node node)
        : mCurrentNode(node)
    {
    }

    /** Parse an XML element to deserialize it into the desired structure
     *
     * @param[in] xmlElement current xml node
     * @param[out] context error context used to communicate errors to upper layers
     * @return true in case of success, false otherwiser
     */
    virtual bool fromXml(const CXmlElement& xmlElement,
                         CXmlSerializingContext& context) override
    {
        for(auto &attribute : mCurrentNode.second.attributes) {
            std::string rawAttribute;
            if (xmlElement.hasAttribute(attribute.getName())) {
                xmlElement.getAttribute(attribute.getName(), rawAttribute);
                try {
                    attribute.set(rawAttribute);
                } catch (std::invalid_argument &e) {
                    std::string error = "When parsing node '" + mCurrentNode.first  +
                                        "': Attribute error '" + attribute.getName() +
                                        "': " + e.what();
                    context.setError(error);
                    return false;
                }
            }
        }

        if (xmlElement.getNbChildElements() != 0) {
            CXmlElement::CChildIterator childIterator(xmlElement);
            CXmlElement childNode;
            while ( childIterator.next(childNode)) {
                try
                {
                    // retrieve node and propagate to dedicated deserializer
                    binding::Node childNodeStruct =
                        {childNode.getType(), mCurrentNode.second.childs.at(childNode.getType())};

                    Deserializer childSerializer(childNodeStruct);
                    if (!childSerializer.fromXml(childNode, context)) {
                        return false;
                    }
                } catch (std::out_of_range &e) {
                    std::string error = "When parsing node '" + mCurrentNode.first  +
                                        "': child node '" + childNode.getType() + "' not found";
                    context.setError(error);
                    return false;
                }
            }
        }
        return true;
    }

    /** Node parsed by this deserializer */
    binding::Node mCurrentNode;
};

} /** serialization namespace */
} /** xml namespace */
} /** core namespace */
