/*
 * Copyright (c) 2011-2014, Intel Corporation
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

#include <limits>
#include <sstream>
#include <string>
#include <stdint.h>
#include <cmath>

/* details namespace is here to hide implementation details to header end user. It
 * is NOT intended to be used outside. */
namespace details
{

/** Helper class to limit instantiation of templates */
template<typename T>
struct ConvertionAllowed;

/* List of allowed types for conversion */
template<> struct ConvertionAllowed<bool> {};
template<> struct ConvertionAllowed<uint64_t> {};
template<> struct ConvertionAllowed<int64_t> {};
template<> struct ConvertionAllowed<uint32_t> {};
template<> struct ConvertionAllowed<int32_t> {};
template<> struct ConvertionAllowed<uint16_t> {};
template<> struct ConvertionAllowed<int16_t> {};
template<> struct ConvertionAllowed<float> {};
template<> struct ConvertionAllowed<double> {};

template<typename T>
static inline bool convertTo(const std::string &str, T &result)
{
    /* Check that conversion to that type is allowed.
     * If this fails, this means that this template was not intended to be used
     * with this type, thus that the result is undefined. */
    ConvertionAllowed<T>();

    if (str.find_first_of(std::string("\r\n\t\v ")) != std::string::npos) {
        return false;
    }

    /* Check for a '-' in string. If type is unsigned and a - is found, the
     * parsing fails. This is made necessary because "-1" is read as 65535 for
     * uint16_t, for example */
    if (str.find("-") != std::string::npos
        && !std::numeric_limits<T>::is_signed) {
        return false;
    }

    std::stringstream ss(str);

    /* Sadly, the stream conversion does not handle hexadecimal format, thus
     * check is done manually */
    if (str.substr(0, 2) == "0x") {
        if (std::numeric_limits<T>::is_integer) {
            ss >> std::hex >> result;
        }
        else {
            /* Conversion undefined for non integers */
            return false;
        }
    } else {
        ss >> result;
    }

    return ss.eof() && !ss.fail() && !ss.bad();
}
} // namespace details

/**
 * Convert a string to a given type.
 *
 * This template function read the value of the type T in the given string.
 * The function does not allow to have white spaces around the value to parse
 * and tries to parse the whole string, which means that if some bytes were not
 * read in the string, the function fails.
 * Hexadecimal representation (ie numbers starting with 0x) is supported only
 * for integral types conversions.
 * Result may be modified, even in case of failure.
 *
 * @param[in]  str    the string to parse.
 * @param[out] result reference to object where to store the result.
 *
 * @return true if conversion was successful, false otherwise.
 */
template<typename T>
static inline bool convertTo(const std::string &str, T &result)
{
    return details::convertTo<T>(str, result);
}

/**
 * Specialization for int16_t of convertTo template function.
 *
 * This function follows the same paradigm than it's generic version.
 *
 * The specific implementation is made necessary because the stlport version of
 * string streams is bugged and does not fail when giving overflowed values.
 * This specialisation can be safely removed when stlport behaviour is fixed.
 *
 * @param[in]  str    the string to parse.
 * @param[out] result reference to object where to store the result.
 *
 * @return true if conversion was successful, false otherwise.
 */
template<>
inline bool convertTo<int16_t>(const std::string &str, int16_t &result)
{
    int64_t res;

    if (!convertTo<int64_t>(str, res)) {
        return false;
    }

    if (res > std::numeric_limits<int16_t>::max() || res < std::numeric_limits<int16_t>::min()) {
        return false;
    }

    result = static_cast<int16_t>(res);
    return true;
}

/**
 * Specialization for float of convertTo template function.
 *
 * This function follows the same paradigm than it's generic version and is
 * based on it but makes furthers checks on the returned value.
 *
 * The specific implementation is made necessary because the stlport conversion
 * from string to float behaves differently than GNU STL: overflow produce
 * +/-Infinity rather than an error.
 *
 * @param[in]  str    the string to parse.
 * @param[out] result reference to object where to store the result.
 *
 * @return true if conversion was successful, false otherwise.
 */
template<>
inline bool convertTo<float>(const std::string &str, float &result)
{
    if (!details::convertTo(str, result)) {
        return false;
    }

    if (std::abs(result) == std::numeric_limits<float>::infinity() ||
        result == std::numeric_limits<float>::quiet_NaN()) {
        return false;
    }

    return true;
}

/**
 * Specialization for double of convertTo template function.
 *
 * This function follows the same paradigm than it's generic version and is
 * based on it but makes furthers checks on the returned value.
 *
 * The specific implementation is made necessary because the stlport conversion
 * from string to double behaves differently than GNU STL: overflow produce
 * +/-Infinity rather than an error.
 *
 * @param[in]  str    the string to parse.
 * @param[out] result reference to object where to store the result.
 *
 * @return true if conversion was successful, false otherwise.
 */
template<>
inline bool convertTo<double>(const std::string &str, double &result)
{
    if (!details::convertTo(str, result)) {
        return false;
    }

    if (std::abs(result) == std::numeric_limits<double>::infinity() ||
        result == std::numeric_limits<double>::quiet_NaN()) {
        return false;
    }

    return true;
}

/**
 * Specialization for boolean of convertTo template function.
 *
 * This function follows the same paradigm than it's generic version.
 * This function accepts to parse boolean as "0/1" or "false/true" or
 * "FALSE/TRUE".
 * The specific implementation is made necessary because the behaviour of
 * string streams when parsing boolean values is not sufficient to fit our
 * requirements. Indeed, parsing "true" will correctly parse the value, but the
 * end of stream is not reached which makes the ss.eof() fails in the generic
 * implementation.
 *
 * @param[in]  str    the string to parse.
 * @param[out] result reference to object where to store the result.
 *
 * @return true if conversion was successful, false otherwise.
 */
template<>
inline bool convertTo<bool>(const std::string &str, bool &result)
{
    if (str == "0" || str == "FALSE" || str == "false") {
        result = false;
        return true;
    }

    if (str == "1" || str == "TRUE" || str == "true") {
        result = true;
        return true;
    }

    return false;
}
