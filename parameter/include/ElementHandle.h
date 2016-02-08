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

#include "parameter_export.h"

#include <stdint.h>
#include <string>
#include <vector>

/** Forward declaration of private classes.
 * Client should not use those class.
 * They are not part of the public api and may be remove/renamed in any release.
 * @{
 */
class CParameterMgr;
class CConfigurableElement;
class CBaseParameter;
/** @} */

/** TODO */
class PARAMETER_EXPORT ElementHandle
{
public:
    /** @return element's name. */
    std::string getName() const;

    /** @return element's size in bytes.
     *
     * If the element size in bit is not a multiple of CHAR_BIT (8)
     * it is rounded to the upper multiple.
     * Effectively returning the element memory footprint.
     */
    size_t getSize() const;

    /** @return true if the element is a parameter, false otherwise. */
    bool isParameter() const;

    /** @return a human readable description of the element. */
    std::string getDescription() const;

    /** @return is the element and all its descendant not in a domain.
     *
     * Only rogue elements are allowed to be set.
     * */
    bool isRogue() const;

    /** @return true if the element is an array, false otherwise.*/
    bool isArray() const;

    /** @return the parameter array length.
     *          0 if the element is not an array (scalar).
     */
    size_t getArrayLength() const;

    /** @return element's path in the parameter hierarchy tree. */
    std::string getPath() const;

    /** @return element's kind.
     *
     * Ie: a string identifying the type of Element.
     */
    std::string getKind() const;

    std::vector<ElementHandle> getChildren();

    /** Get mapping data of the element context
     *
     * Retrieve mapping data associated to a given key if any.
     * If the key is not present in this element, query ancestors.
     *
     * @param[in] strKey the input mapping key
     * @param[out] strValue the resulting mapping value in case of success
     * @return true for if mapping key exists, false otherwise
     */
    bool getMappingData(const std::string &strKey, std::string &strValue) const;

    /** Gets element structure description as XML string
     *
     * @return the output XML string
     */
    bool getStructureAsXML(std::string &xmlStructure, std::string &error) const;

    /** Gets element settings as XML string
     *
     * @param[out] xmlValue the values to get
     * @param[out] error On failure (false returned) will contain a human
     *                   readable description of the error.
     *                   On success (true returned) the content is not
     *                   specified.
     *
     * @note returned value format depends on the current ParameterMgr format
     *       control properties, including value space and output raw format.
     *       @see ParameterMgrPlatformConnector::setOutputRawFormat
     *       @see ParameterMgrPlatformConnector::setValueSpace
     *
     * @return true on success, false on failure
     */
    bool getAsXML(std::string &xmlValue, std::string &error) const;

    /** Sets element settings as XML string
     *
     * @param[in] xmlValue the values to set
     * @param[out] error On failure (false returned) will contain a human
     *                   readable description of the error.
     *                   On success (true returned) the content is not
     *                   specified.
     *
     * @note
     *    - targeted element needs to be rogue for this operation to be allowed
     *    - structure of the passed XML element must match the targeted
     *      configurable element's one otherwise this operation will fail
     *    - expected value format depends on current value space.
     *      @see ParameterMgrPlatformConnector::valueSpaceIsRaw
     *
     * @return true on success, false otherwise
     */
    bool setAsXML(const std::string &xmlValue, std::string &error);

    /** Gets element settings in binary format
     *
     * @param[out] bytesValue the output vector
     * @param[out] error unused
     *
     * @returns true
     */
    bool getAsBytes(std::vector<uint8_t> &bytesValue, std::string &error) const;

    /** Sets element settings in binary format
     *
     * @param[out] bytesValue the output vector
     * @param[out] error On failure (false returned) will contain a human
     *                   readable description of the error.
     *                   On success (true returned) the content is not
     *                   specified.
     *
     * @note
     *    - targeted element needs to be rogue for this operation to be allowed
     *    - size of the passed array must match that of the element
     */
    bool setAsBytes(const std::vector<uint8_t> &bytesValue, std::string &error);

    /** Access (get or set) parameters as different types.
     *
     * Will fail if the element is not a paramete.
     * Array access will fail if the parameter is not an array.
     *
     * @param value if get, the value to get (in parameter)
     *              if set, the value to set (out parameter)
     *
     * Setting an array requires the std::vector size to match the arrayLength.
     * Ie: value.size() == arrayLength()
     *
     * @param[out] error On failure (false returned) will contain a human
     *                   readable description of the error.
     *                   On success (true returned) the content is not
     *                   specified.
     * @return true if the access was successful,
     *         false otherwise (see error for the detail)
     * @{
     */

    /** Boolean access @{ */
    bool getAsBoolean(bool &value, std::string &error) const;
    bool setAsBoolean(bool value, std::string &error);
    bool setAsBooleanArray(const std::vector<bool> &value, std::string &error);
    bool getAsBooleanArray(std::vector<bool> &value, std::string &error) const;
    /** @} */

    /** Integer Access @{ */
    bool setAsInteger(uint32_t value, std::string &error);
    bool getAsInteger(uint32_t &value, std::string &error) const;
    bool setAsIntegerArray(const std::vector<uint32_t> &value, std::string &error);
    bool getAsIntegerArray(std::vector<uint32_t> &value, std::string &error) const;
    /** @} */

    /** Signed Integer Access @{ */
    bool setAsSignedInteger(int32_t value, std::string &error);
    bool getAsSignedInteger(int32_t &value, std::string &error) const;
    bool setAsSignedIntegerArray(const std::vector<int32_t> &value, std::string &error);
    bool getAsSignedIntegerArray(std::vector<int32_t> &value, std::string &error) const;
    /** @} */

    /** Double Access @{ */
    bool setAsDouble(double value, std::string &error);
    bool getAsDouble(double &value, std::string &error) const;
    bool setAsDoubleArray(const std::vector<double> &value, std::string &error);
    bool getAsDoubleArray(std::vector<double> &value, std::string &error) const;
    /** @} */

    /** String Access @{ */
    bool setAsString(const std::string &value, std::string &error);
    bool getAsString(std::string &value, std::string &error) const;
    bool setAsStringArray(const std::vector<std::string> &value, std::string &error);
    bool getAsStringArray(std::vector<std::string> &value, std::string &error) const;
    /** @} */

    /** @} */

protected:
    ElementHandle(CConfigurableElement &element, CParameterMgr &parameterMgr);
    friend CParameterMgr; // So that it can build the handler

private:
    template <class T>
    bool setAs(const T value, std::string &error) const;
    template <class T>
    bool getAs(T &value, std::string &error) const;

    CBaseParameter &getParameter();
    const CBaseParameter &getParameter() const;

    /** Check that the parameter value can be modify.
     *
     * @param arrayLength[in] If accessing as an array: the new value array length
     *                        Otherwise: 0
     * @param error[out] If access is forbidden: a human readable message explaining why
     *                   Otherwise: not modified.
     *
     * @return true if the parameter value can be retrieved, false otherwise.
     */
    bool checkSetValidity(size_t arrayLength, std::string &error) const;

    /** Check that the parameter value can be retrieved.
     *
     * @param asArray[in] true if accessing as an array, false otherwise.
     * @param error[out] If access is forbidden, a human readable message explaining why
     *                   Otherwise, not modified.
     *
     * @return true if the parameter value can be retrieved, false otherwise.
     */
    bool checkGetValidity(bool asArray, std::string &error) const;

    /** Reference to the handled Configurable element. */
    CConfigurableElement &mElement;

    CParameterMgr &mParameterMgr;
};
