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
// The generated python module will be named "PyPfw"
// the "directors" feature is used to derive Python classes and make them look
// like derived C++ classes (calls to virtual methods will be properly
// forwarded to Python) - only on classes for which is it specified, see
// ILogger below..
%module(directors="1", threads="1") PyPfw

%feature("director:except") {
    if ($error != NULL) {
        throw Swig::DirectorMethodException();
    }
}
%exception {
    try { $action }
    catch (Swig::DirectorException &e) { SWIG_fail; }
}

%include "std_string.i"
%include "std_vector.i"
%include "typemaps.i"

// We need to tell SWIG that std::vector<std::string> is a vector of strings
namespace std {
    %template(StringVector) vector<string>;
}

// Tells swig that 'std::string& strError' must be treated as output parameters
// TODO: make it return a tuple instead of a list
%apply std::string &OUTPUT { std::string& strError };

// Automatic python docstring generation
// FIXME: because of the typemap above, the output type is wrong for methods
// that can return an error string.
// TODO: document each function manually ?
%feature("autodoc", "1");


// rename "CParameterMgrFullConnector" into the nicer "ParameterFramework" name
%rename(ParameterFramework) CParameterMgrFullConnector;
class CParameterMgrFullConnector
{

%{
#include "ParameterMgrFullConnector.h"
%}

public:
    CParameterMgrFullConnector(const std::string& strConfigurationFilePath);

    bool start(std::string& strError);

    void setLogger(ILogger* pLogger);

    ISelectionCriterionTypeInterface* createSelectionCriterionType(bool bIsInclusive);
    ISelectionCriterionInterface* createSelectionCriterion(const std::string& strName,
            const ISelectionCriterionTypeInterface* pSelectionCriterionType);
    ISelectionCriterionInterface* getSelectionCriterion(const std::string& strName);

    // Configuration application
    void applyConfigurations();

    bool getForceNoRemoteInterface() const;
    void setForceNoRemoteInterface(bool bForceNoRemoteInterface);

    void setFailureOnMissingSubsystem(bool bFail);
    bool getFailureOnMissingSubsystem() const;

    void setFailureOnFailedSettingsLoad(bool bFail);
    bool getFailureOnFailedSettingsLoad();

    void setSchemaFolderLocation(const std::string& strSchemaFolderLocation);
    void setValidateSchemasOnStart(bool bValidate);
    bool getValidateSchemasOnStart() const;

    // Tuning mode
    bool setTuningMode(bool bOn, std::string& strError);
    bool isTuningModeOn() const;

    // Current value space for user set/get value interpretation
    void setValueSpace(bool bIsRaw);
    bool isValueSpaceRaw() const;

    // Current Output Raw Format for user get value interpretation
    void setOutputRawFormat(bool bIsHex);
    bool isOutputRawFormatHex() const;

    // Automatic hardware synchronization control (during tuning session)
    bool setAutoSync(bool bAutoSyncOn, std::string& strError);
    bool isAutoSyncOn() const;
    bool sync(std::string& strError);

    // User set/get parameters
%apply std::string &INOUT { std::string& strValue };
    bool accessParameterValue(const std::string& strPath, std::string& strValue, bool bSet, std::string& strError);
    bool accessConfigurationValue(const std::string &strDomain, const std::string &strConfiguration, const std::string& strPath, std::string& strValue, bool bSet, std::string& strError);
%clear std::string& strValue;

    bool getParameterMapping(const std::string& strPath, std::string& strValue) const;

    // Creation/Deletion
    bool createDomain(const std::string& strName, std::string& strError);
    bool deleteDomain(const std::string& strName, std::string& strError);
    bool renameDomain(const std::string& strName, const std::string& strNewName, std::string& strError);
    bool deleteAllDomains(std::string& strError);
%apply std::string &OUTPUT { std::string& strResult }
    bool setSequenceAwareness(const std::string& strName, bool bSequenceAware, std::string& strResult);
    bool getSequenceAwareness(const std::string& strName, bool& bSequenceAware, std::string& strResult);
%clear std::string& strResult;
    bool createConfiguration(const std::string& strDomain, const std::string& strConfiguration, std::string& strError);
    bool deleteConfiguration(const std::string& strDomain, const std::string& strConfiguration, std::string& strError);
    bool renameConfiguration(const std::string& strDomain, const std::string& strConfiguration, const std::string& strNewConfiguration, std::string& strError);

    // Save/Restore
    bool restoreConfiguration(const std::string& strDomain, const std::string& strConfiguration, std::list<std::string>& strError);
    bool saveConfiguration(const std::string& strDomain, const std::string& strConfiguration, std::string& strError);

    // Configurable element - domain association
    bool addConfigurableElementToDomain(const std::string& strDomain, const std::string& strConfigurableElementPath, std::string& strError);
    bool removeConfigurableElementFromDomain(const std::string& strDomain, const std::string& strConfigurableElementPath, std::string& strError);
    bool split(const std::string& strDomain, const std::string& strConfigurableElementPath, std::string& strError);
    bool setElementSequence(const std::string& strDomain, const std::string& strConfiguration, const std::vector<std::string>& astrNewElementSequence, std::string& strError);

    bool setApplicationRule(const std::string& strDomain, const std::string& strConfiguration, const std::string& strApplicationRule, std::string& strError);
%apply std::string &OUTPUT { std::string& strResult }
    bool getApplicationRule(const std::string& strDomain, const std::string& strConfiguration, std::string& strResult);
%clear std::string& strResult;
    bool clearApplicationRule(const std::string& strDomain, const std::string& strConfiguration, std::string& strError);

    bool importDomainsXml(const std::string& strXmlSource, bool bWithSettings, bool bFromFile,
                          std::string& strError);
    bool importSingleDomainXml(const std::string& strXmlSource, bool bOverwrite,
                               std::string& strError);
    bool importSingleDomainXml(const std::string& xmlSource, bool overwrite, bool withSettings,
                               bool fromFile, std::string& strError);

// Tells swig that "strXmlDest" in the two following methods are "inout"
// parameters
%apply std::string &INOUT { std::string& strXmlDest };
    bool exportDomainsXml(std::string& strXmlDest, bool bWithSettings, bool bToFile,
                          std::string& strError) const;

    bool exportSingleDomainXml(std::string& strXmlDest, const std::string& strDomainName, bool bWithSettings,
                               bool bToFile, std::string& strError) const;
%clear std::string& strXmlDest;
};

// SWIG nested class support is not complete - cf.
// http://swig.org/Doc2.0/SWIGPlus.html#SWIGPlus_nested_classes
// This link also explains how to trick SWIG and pretend that
// ILogger is a toplevel class (whereas it actually is an inner class of
// CParameterMgrFullConnector
// Logger interface
%feature("director") ILogger;
%nestedworkaround CParameterMgrFullConnector::ILogger;
class ILogger
{
    public:
        virtual void log(bool bIsWarning, const std::string& strLog) = 0;
    protected:
        virtual ~ILogger() {}
};
%{
typedef CParameterMgrFullConnector::ILogger ILogger;
%}

class ISelectionCriterionTypeInterface
{
%{
#include "SelectionCriterionTypeInterface.h"
%}

public:
    virtual bool addValuePair(int iValue, const std::string& strValue) = 0;
    virtual bool getNumericalValue(const std::string& strValue, int& iValue) const = 0;
    virtual bool getLiteralValue(int iValue, std::string& strValue) const = 0;
    virtual bool isTypeInclusive() const = 0;
    virtual std::string getFormattedState(int iValue) const = 0;

protected:
    virtual ~ISelectionCriterionTypeInterface() {}
};

class ISelectionCriterionInterface
{
%{
#include "SelectionCriterionInterface.h"
%}

public:
    virtual void setCriterionState(int iState) = 0;
    virtual int getCriterionState() const = 0;
    virtual std::string getCriterionName() const = 0;
    virtual const ISelectionCriterionTypeInterface* getCriterionType() const = 0;

protected:
    virtual ~ISelectionCriterionInterface() {}
};
