/*
 * INTEL CONFIDENTIAL
 * Copyright © 2013 Intel
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
#include "XmlElement.h"
#include "XmlSerializingContext.h"
#include <string>

struct _xmlDoc;
struct _xmlNode;
struct _xmlError;

/**
  * The CXmlDocSource is used by CXmlDocSink.
  * The interaction between the xml source and xml sink is defined
  * in the process method of CXmlDocSink. One can subclass CXmlDocSource
  * for different purposes by implementing the populate method and then
  * use it with any existing implementation of CXmlDocSink.
  */
class CXmlDocSource
{
public:
    /**
      * Constructor
      *
      * @param[out] pDoc a pointer to the xml document that will be filled by the class
      * @param[in] pRootNode a pointer to the root element of the document.
      */
    CXmlDocSource(_xmlDoc* pDoc, _xmlNode* pRootNode = NULL);

    /**
      * Constructor
      *
      * @param[out] pDoc a pointer to the xml document that will be filled by the class
      * @param[in] strXmlSchemaFile a string containing the path to the schema file
      * @param[in] strRootElementType a string containing the root element type
      * @param[in] strRootElementName a string containing the root element name
      * @param[in] strNameAttributeName a string containing the name of the root name attribute
      */
    CXmlDocSource(_xmlDoc* pDoc,
                           const string& strXmlSchemaFile,
                           const string& strRootElementType,
                           const string& strRootElementName,
                           const string& strNameAttrituteName);

    /**
      * Constructor
      *
      * @param[out] pDoc a pointer to the xml document that will be filled by the class
      * @param[in] strXmlSchemaFile a string containing the path to the schema file
      * @param[in] strRootElementType a string containing the root element type
      */
    CXmlDocSource(_xmlDoc* pDoc, const string& strXmlSchemaFile, const string& strRootElementType);

    /**
      * Destructor
      */
    virtual ~CXmlDocSource();

    /**
      * Method called by the CXmlDocSink::process method.
      *
      * @param[out] serializingContext is used as error output
      *
      * @return false if there are any error
      */
    virtual bool populate(CXmlSerializingContext& serializingContext) = 0;

    /**
      * Method that returns the root element of the Xml tree.
      *
      * @param[out] xmlRootElement a reference to the CXmleElement destination
      */
    void getRootElement(CXmlElement& xmlRootElement) const;

    /**
      * Getter method.
      *
      * @return the root element's name
      */
    string getRootElementName() const;

    /**
      * Getter method.
      * Method that returns the root element's attribute with name matching strAttributeName.
      *
      * @param[in] strAttributeName is a string used to find the corresponding attribute
      *
      * @return the value of the root's attribute named as strAttributeName
      */
    string getRootElementAttributeString(const string& strAttributeName) const;

    /**
      * Getter method.
      * Method that returns the xmlDoc contained in the Source.
      * (Can be used in a Doc Sink)
      *
      * @return the document _pDoc
      */
    _xmlDoc* getDoc() const;

    /**
      * Method that validates the Xml doc contained in pDoc
      *
      * @param[out] serializingContext is used as error output
      *
      * @return false if any error occurs
      */
    virtual bool validate(CXmlSerializingContext& serializingContext);


protected:

    /**
      * Doc
      */
    _xmlDoc* _pDoc;

    /**
      * Root node
      */
    _xmlNode* _pRootNode;

    /**
      * libxml2 library cleanup
      */
    static bool _bLibXml2CleanupScheduled;

private:

    /**
      * Method that initializes class internal attributes in constructor
      */
    void init();

    /** Method that check the validity of the document with the xsd file.
      *
      * @return true if document is valid, false if any error occures
      */
    bool isInstanceDocumentValid();

    /** Validity error display method
      *
      * @param[in] pUserData pointer to the data to validate
      * @param[out] pError is the xml error output
      */
    static void schemaValidityStructuredErrorFunc(void* pUserData, _xmlError* pError);

    /**
      * Schema file
      */
    string _strXmlSchemaFile;

    /**
      * Element type info
      */
    string _strRootElementType;

    /**
      * Element name info
      */
    string _strRootElementName;

    /**
      * Element name attribute info
      */
    string _strNameAttrituteName;

    /**
      * Boolean that enables the root element name attribute check
      */
    bool _bNameCheck;
};
