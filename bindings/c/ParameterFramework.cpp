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

#include "ParameterFramework.h"
#include <ParameterMgrPlatformConnector.h>

#include <iostream>
#include <limits>
#include <string>
#include <map>

#include <cassert>
#include <cstring>
#include <cstdlib>

using std::string;

/** Rename long pfw types to short ones in pfw namespace. */
namespace pfw
{
    typedef ISelectionCriterionInterface Criterion;
    typedef std::map<string, Criterion *> Criteria;
    typedef CParameterMgrPlatformConnector Pfw;
}

/** Class to abstract the boolean+string status api. */
class Status
{
public:
    /** Fail without an instance of status. */
    static bool failure() { return false; }
    /** Fail with the given error msg. */
    bool failure(const string &msg) { mMsg = msg; return false; }
    /** Success (no error message). */
    bool success() { mMsg.clear(); return true; }

    /** Forward a status operation.
      * @param success[in] the operaton status to forward
      *                    or forward a previous failure if omitted
      */
    bool forward(bool success = false) {
        if (success) { mMsg.clear(); }
        return success;
    }
    /** Error message accessors.
      *
      * Pfw api requires to provide a reference to a string in order
      * for it to log. This function provide a reference to a string that
      * will be added to the error message on failure.
      */
    string &msg() { return mMsg; }
private:
    string mMsg;
};

///////////////////////////////
///////////// Log /////////////
///////////////////////////////

/** Default log callback. Log to cout or cerr depending on level. */
static void defaultLogCb(void *, PfwLogLevel level, const char *logLine) {
    switch (level) {
    case pfwLogInfo:
        std::cout << logLine << std::endl;
        break;
    case pfwLogWarning:
        std::cerr << logLine << std::endl;
        break;
    };
}

static PfwLogger defaultLogger = { NULL, &defaultLogCb };

class LogWrapper : public CParameterMgrPlatformConnector::ILogger
{
public:
    LogWrapper(const PfwLogger &logger) : mLogger(logger) {}
    LogWrapper() : mLogger() {}
    virtual ~LogWrapper() {}
private:
    virtual void log(bool bIsWarning, const string &strLog)
    {
        // A LogWrapper should NOT be register to the pfw (thus log called)
        // if logCb is NULL.
        assert(mLogger.logCb != NULL);
        mLogger.logCb(mLogger.userCtx,
                      bIsWarning ? pfwLogWarning : pfwLogInfo,
                      strLog.c_str());
    }
    PfwLogger mLogger;
};

///////////////////////////////
///////////// Core ////////////
///////////////////////////////

struct PfwHandler_
{
    void setLogger(const PfwLogger *logger);
    bool createCriteria(const PfwCriterion criteria[], size_t criterionNb);

    pfw::Criteria criteria;
    pfw::Pfw *pfw;
    /** Status of the last called function.
      * Is mutable because even a const function can fail.
      */
    mutable Status lastStatus;
private:
    LogWrapper mLogger;
};


PfwHandler *pfwCreate()
{
    return new PfwHandler();
}

void pfwDestroy(PfwHandler *handle)
{
    if(handle != NULL and handle->pfw != NULL) {
        delete handle->pfw;
    }
    delete handle;
}

void PfwHandler::setLogger(const PfwLogger *logger)
{
    if (logger != NULL and logger->logCb == NULL) {
        return; // There is no callback, do not log => do not add a logger
    }
    mLogger = logger != NULL ? *logger : defaultLogger;
    pfw->setLogger(&mLogger);
}


bool PfwHandler::createCriteria(const PfwCriterion criteriaArray[], size_t criterionNb)
{
    Status &status = lastStatus;
    // Add criteria
    for (size_t criterionIndex = 0; criterionIndex < criterionNb; ++criterionIndex) {
        const PfwCriterion &criterion = criteriaArray[criterionIndex];
        if (criterion.name == NULL) {
            return status.failure("Criterion name is NULL");
        }
        if (criterion.values == NULL) {
            return status.failure("Criterion values is NULL");
        }
        // Check that the criterion does not exist
        if (criteria.find(criterion.name) != criteria.end()) {
            return status.failure("Criterion \"" + string(criterion.name) +
                                  "\" already exist");
        }

        // Create criterion type
        ISelectionCriterionTypeInterface *type =
            pfw->createSelectionCriterionType(criterion.inclusive);
        assert(type != NULL);
        // Add criterion values
        for (size_t valueIndex = 0; criterion.values[valueIndex] != NULL; ++valueIndex) {
            int value;
            if (criterion.inclusive) {
                // Check that (int)1 << valueIndex would not overflow (UB)
                if(std::numeric_limits<int>::max() >> valueIndex == 0) {
                    return status.failure("Too many values for criterion " +
                                          string(criterion.name));
                }
                value = 1 << valueIndex;
            } else {
                value = valueIndex;
            }
            const char * valueName = criterion.values[valueIndex];
            if(not type->addValuePair(value, valueName)) {
                return status.failure("Could not add value " + string(valueName) +
                                      " to criterion " + criterion.name);
            }
        }
        // Create criterion and add it to the pfw
        criteria[criterion.name] = pfw->createSelectionCriterion(criterion.name, type);
    }
    return status.success();
}


bool pfwStart(PfwHandler *handle, const char *configPath,
              const PfwCriterion criteria[], size_t criterionNb,
              const PfwLogger *logger)
{
    // Check that the api is correctly used
    if (handle == NULL) { return Status::failure(); }
    Status &status = handle->lastStatus;

    if (handle->pfw != NULL) {
        return status.failure("Can not start an already started parameter framework");
    }
    if (configPath == NULL) {
        return status.failure("char *configPath is NULL, "
                              "while starting the parameter framework");
    }
    if (criteria == NULL) {
        return status.failure("char *criteria is NULL, "
                              "while starting the parameter framework "
                              "(config path is " + string(configPath) + ")");
    }
    // Create a pfw
    handle->pfw = new CParameterMgrPlatformConnector(configPath);

    handle->setLogger(logger);

    if (not handle->createCriteria(criteria, criterionNb)) {
        return status.failure();
    }

    return status.forward(handle->pfw->start(status.msg()));
}

const char *pfwGetLastError(const PfwHandler *handle)
{
    return handle == NULL ? NULL : handle->lastStatus.msg().c_str();
}

static pfw::Criterion *getCriterion(const pfw::Criteria &criteria,
                                    const string &name)
{
    pfw::Criteria::const_iterator it = criteria.find(name);
    return it == criteria.end() ? NULL : it->second;
}

bool pfwSetCriterion(PfwHandler *handle, const char name[], int value)
{
    if (handle == NULL) { return Status::failure(); }
    Status &status = handle->lastStatus;
    if (name == NULL) {
        return status.failure("char *name of the criterion is NULL, "
                              "while setting a criterion.");
    }
    if (handle->pfw == NULL) {
        return status.failure("Can not set criterion \"" + string(name) +
                              "\" as the parameter framework is not started.");
    }
    pfw::Criterion *criterion = getCriterion(handle->criteria, name);
    if (criterion == NULL) {
        return status.failure("Can not set criterion " + string(name) + " as does not exist");
    }
    criterion->setCriterionState(value);
    return status.success();
}
bool pfwGetCriterion(const PfwHandler *handle, const char name[], int *value)
{
    if (handle == NULL) { return Status::failure(); }
    Status &status = handle->lastStatus;
    if (name == NULL) {
        return status.failure("char *name of the criterion is NULL, "
                              "while getting a criterion.");
    }
    if (handle->pfw == NULL) {
        return status.failure("Can not get criterion \"" + string(name) +
                              "\" as the parameter framework is not started.");
    }
    if (value == NULL) {
        return status.failure("Can not get criterion \"" + string(name) +
                              "\" as the out value is NULL.");
    }
    pfw::Criterion *criterion = getCriterion(handle->criteria, name);
    if (criterion == NULL) {
        return status.failure("Can not get criterion " + string(name) + " as it does not exist");
    }
    *value = criterion->getCriterionState();
    return status.success();
}

bool pfwApplyConfigurations(const PfwHandler *handle)
{
    if (handle == NULL) { return Status::failure(); }
    Status &status = handle->lastStatus;
    if (handle->pfw == NULL) {
        return status.failure("Can not commit criteria "
                              "as the parameter framework is not started.");
    }
    handle->pfw->applyConfigurations();
    return status.success();
}

///////////////////////////////
/////// Parameter access //////
///////////////////////////////

struct PfwParameterHandler_
{
    PfwHandler &pfw;
    CParameterHandle &parameter;
};

PfwParameterHandler *pfwBindParameter(PfwHandler *handle, const char path[])
{
    if (handle == NULL) { return NULL; }
    Status &status = handle->lastStatus;
    if (path == NULL) {
        status.failure("Can not bind a parameter without its path");
        return NULL;
    }
    if (handle->pfw == NULL) {
        status.failure("The parameter framework is not started, "
                       "while trying to bind parameter \"" + string(path) + "\")");
        return NULL;
    }

    CParameterHandle *paramHandle;
    paramHandle = handle->pfw->createParameterHandle(path, status.msg());
    if (paramHandle == NULL) {
        return NULL;
    }

    status.success();
    PfwParameterHandler publicHandle = {*handle, *paramHandle};
    return new PfwParameterHandler(publicHandle);
}

void pfwUnbindParameter(PfwParameterHandler *handle)
{
    if (handle == NULL) { return; }
    delete &handle->parameter;
    delete handle;
}


bool pfwGetIntParameter(const PfwParameterHandler *handle, int32_t *value)
{
    if (handle == NULL) { return Status::failure(); }
    Status &status = handle->pfw.lastStatus;
    if (value == NULL) {
        return status.failure("int32_t *value is NULL, "
                    "while trying to get parameter \"" +
                    handle->parameter.getPath() + "\" value as int)");
    }
    return status.forward(handle->parameter.getAsSignedInteger(*value, status.msg()));
}
bool pfwSetIntParameter(PfwParameterHandler *handle, int32_t value)
{
    if (handle == NULL) { return Status::failure(); }
    Status &status = handle->pfw.lastStatus;
    return status.forward(handle->parameter.setAsSignedInteger(value, status.msg()));
}

bool pfwGetStringParameter(const PfwParameterHandler *handle, const char *value[])
{
    if (handle == NULL) { return Status::failure(); }
    Status &status = handle->pfw.lastStatus;
    if (value == NULL) {
        return status.failure("char **value is NULL, "
                    "while trying to get parameter \"" +
                    handle->parameter.getPath() + "\" value as string)");
    }
    *value = NULL;
    string retValue;
    bool success = handle->parameter.getAsString(retValue, status.msg());
    if (not success) { return status.forward(); }

    *value = strdup(retValue.c_str());
    return status.success();
}

bool pfwSetStringParameter(PfwParameterHandler *handle, const char value[])
{
    if (handle == NULL) { return Status::failure(); }
    Status &status = handle->pfw.lastStatus;
    return status.forward(handle->parameter.setAsString(value, status.msg()));
}

void pfwFree(void *ptr) { std::free(ptr); }

