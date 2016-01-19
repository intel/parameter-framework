/** @copyright
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

/** @file
 *
 * Simplified parameter framework C API.  This API does not target a perfect
 * one/one mapping with the c++ one, but rather aim ease of use and type safety
 * (as far as possible in c).  All function are reentrant and function call on
 * a pfw (PfwHandle) does not impact any other pfw.  Ie. There is no shared
 * resources between pfw instances.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "cparameter_export.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/** Lots of function in this API require non null pointer parameter.
  * Such arguments are marked NONNULL.
  */
#if defined(__clang__) || defined(__GNUC__)
#define NONNULL __attribute__((nonnull))
#define NONNULL_(...) __attribute__((nonnull(__VA_ARGS__)))
#define USERESULT __attribute__((warn_unused_result))
#elif defined(_MSC_VER)
// In visual studio's cl there is no
// equivalent of nonnull
#define NONNULL
#define NONNULL_(...)
#define USERESULT _Check_return_
#else
#error "Unknown compilator"
#endif

/** Private handle to a parameter framework.
  * A PfwHandler* is valid if:
  *  - it was created by pfwCreate
  *  - it has not been destroyed by pfwDestroyParameter
  *  - is not NULL
  * A valid handle MUST be provided to all pfw related method.
  * A valid handler MUST be destroyed with pfwDestroy before programme
  * termination.
  * @note Forward declaration to break header dependency.
 */
struct PfwHandler_;
/** Typedef for use ease. @see PfwHandler_. */
typedef struct PfwHandler_ PfwHandler;

///////////////////////////////
///////////// Log /////////////
///////////////////////////////
/** Pfw log level for the callback. */
typedef enum {
    pfwLogInfo = 55, //< Random value to avoid unfortunate mismatch.
    pfwLogWarning
} PfwLogLevel;

/** Type of the parameter framework log callback.
  * @param[in] userCtx Arbitrary context provided during callback registration.
  * @param[in] level Log level of the log line.
  * @param[in] logLine Log line (without end line control character like '\n')
  *                    to be logged. The pointer is invalidate after function
  *                    return or if any pfw function is called.
  */
typedef void PfwLogCb(void *userCtx, PfwLogLevel level, const char *logLine);

/** Logger containing a callback method and its context. */
typedef struct
{
    /** User defined arbitrary value that will be provided to all logCb call. */
    void *userCtx;
    /** Callback that will be called.
      * If NULL nothing will be logged.
      */
    PfwLogCb *logCb;
} PfwLogger;

///////////////////////////////
///////////// Core ////////////
///////////////////////////////

/** Structure of a parameter framework criterion. */
typedef struct
{
    /** Name of the criterion in the pfw configuration rules. */
    const char *name; //< Must not be null.
    bool inclusive;   //< True if the criterion is inclusive, false if exclusive.

    /** Null terminated list of criterion value names.
      *
      * Example:
      * @verbatim
      * { "Red", "Green", "Blue", NULL }
      * @endverbatim
      *
      * For an exclusive criterion, the list must not contain more elements then
      *                             INT_MAX.
      * For an inclusive criterion, the list must not contain more elements then
      *                             sizeof(int) * BIT_CHAR - 1.
      *                             Ie: (int)1 << n must *not* overflow (UB),
      *                                 were n is the number of element in the
      *                                 list. @see pfwSetCriterion
      */
    const char **values; //< Must not be null.
} PfwCriterion;

/** Create a parameter framework instance.
  * Can not fail except for memory allocation.
  */
CPARAMETER_EXPORT
PfwHandler *pfwCreate() USERESULT;

/** Destroy a parameter framework. Can not fail. */
CPARAMETER_EXPORT
void pfwDestroy(PfwHandler *handle) NONNULL;

/** Start a parameter framework.
  * @param[in] handle @see PfwHandler
  * @param[in] configPath Path to the file containing the pfw configuration.
  * @param[in] criteria An array of PfwCriterion.
  * @param[in] criterionNb The number of PfwCriterion in criteria.
  * @param[in] logger the logger to use for all operation.
  *                   If NULL, log infos to standard output and
  *                                errors to standard error.
  * @return true on success, false on failure.
  */
CPARAMETER_EXPORT
bool pfwStart(PfwHandler *handle, const char *configPath, const PfwCriterion criteria[],
              size_t criterionNb, const PfwLogger *logger) NONNULL_(1, 2, 3) USERESULT;

/** @return a string describing the last call result.
  * If the last pfw function call succeeded, return an empty string.
  * If the last pfw function call failed, return a message explaining the error cause.
  * The return pointer is invalidated if any pfw method is called on the SAME
  * PfwHandle.
  *
  * Each PfwHandle own it's last error message. It is not static nor TLS.
  * As a result, calling a pfw function with a NULL PfwHandler will result in a
  * failure WITHOUT updating the last error.
  */
CPARAMETER_EXPORT
const char *pfwGetLastError(const PfwHandler *handle) NONNULL;

/** Set a criterion value given its name and value.
  * @param[in] handle @see PfwHandler
  * @param[in] name The name of the criterion that need to be changed.
  * @param[in] value If the criterion is exclusive, the index of the new value.
  *              If the criterion is inclusive, a bit field where each bit
  *              correspond to the value index.
  * For an inclusive criterion defined as such: { "Red", "Green", "Blue", NULL }
  * to set the value Green and Blue, value has to be 1<<1 | 1<<2 = 0b110 = 6.
  * For an exclusive criterion defined as such: { "Car", "Boat", "Plane", NULL }
  * to set the value Plane, value has to be 2.
  *
  * Criterion change do not have impact on the parameters value
  * (no configuration applied) until the changes are committed using pfwApplyConfigurations.
  *
  * @return true on success and false on failure.
  */
CPARAMETER_EXPORT
bool pfwSetCriterion(PfwHandler *handle, const char name[], int value) NONNULL USERESULT;
/** Get a criterion value given its name.
  * Same usage as pfwSetCriterion except that value is an out param.
  * Get criterion will return the last value setted with pfwSetCriterion independantly of
  * pfwCommitCritenio.
  */
CPARAMETER_EXPORT
bool pfwGetCriterion(const PfwHandler *handle, const char name[], int *value) NONNULL USERESULT;

/** Commit criteria change and change parameters according to the configurations.
  * Criterion do not have impact on the parameters value when changed,
  * instead they are staged and only feed to the rule engine
  * (who then impact parameter values according to the configuration) when
  * committed with this function.
  *
  * @param[in] handle @see PfwHandler
  * @return true on success and false on failure.
  */
CPARAMETER_EXPORT
bool pfwApplyConfigurations(const PfwHandler *handle) NONNULL USERESULT;

///////////////////////////////
/////// Parameter access //////
///////////////////////////////

/** Handler to a pfw parameter.
  * A PfwParameterHandler* is valid if:
  *  - it was created by pfwBindParameter
  *  - it has not been destroyed by pfwDestroyParameter
  *  - is not NULL
  *  - the pfwHandle used to created is still valid (ie. it must not outlive
  *    its parent pfw)
  * A valid handle MUST be provided to all pfw parameter related method.
  * Any created handle MUST be destroyed (with pfwDestroyParameter) before
  * the PfwHandler that was used for its creation.
  * @note Forward declaration to break header dependency.
  */
struct PfwParameterHandler_;
typedef struct PfwParameterHandler_ PfwParameterHandler;

/** Construct the handle to a parameter given its path.
  * The PfwHandle MUST stay valid until PfwParameterHandler destruction.
  * @return a PfwParameterHandler on success, NULL on error.
  *         @see pfwGetLastError for error detail.
  */
CPARAMETER_EXPORT
PfwParameterHandler *pfwBindParameter(PfwHandler *handle, const char path[]) NONNULL;
/** Destroy a parameter handle. Can not fail. */
CPARAMETER_EXPORT
void pfwUnbindParameter(PfwParameterHandler *handle) NONNULL;

/** Access the value of a previously bind int parameter.
  * @param[in] handle Handler to a valid parameter.
  * @param[in] value Non null pointer to an integer that will
  *        hold the parameter value on success, undefined otherwise.
  * return true of success, false on failure.
  */
CPARAMETER_EXPORT
bool pfwGetIntParameter(const PfwParameterHandler *handle, int32_t *value) NONNULL USERESULT;

/** Set the value of a previously bind int parameter.
  * @param[in] handle Handler to a valid parameter.
  * @param[in] value The parameter value to set.
  * return true of success, false on failure.
  */
CPARAMETER_EXPORT
bool pfwSetIntParameter(PfwParameterHandler *handle, int32_t value) NONNULL USERESULT;

/** Access the value of a previously bind string parameter.
  * @param[in] handle Handler to a valid parameter.
  * @param[out] value Non null pointer on a string.
  *                   Will point on the parameter value string on success,
  *                   NULL on failure.
  *                   The callee MUST free the returned string using pfwFree after use.
  * @return true on success, false on failure.
  */
CPARAMETER_EXPORT
bool pfwGetStringParameter(const PfwParameterHandler *handle, char *value[]) NONNULL;

/** Set the value of a previously bind string parameter.
  * @param[in] handle Handler to a valid parameter
  * @param[in] value Non null pointer to a null terminated string to set.
  */
CPARAMETER_EXPORT
bool pfwSetStringParameter(PfwParameterHandler *handle, const char value[]) NONNULL USERESULT;

/** Frees the memory space pointed to by ptr,
  *  which must have been returned by a previous call to the pfw.
  *
  * @param[in] ptr pointer to the memory to free.
  * @see man 3 free for usage.
  * @note Wrapper around the standard free to avoid problems
  *       in case of a different pfw and client allocator.
  */
CPARAMETER_EXPORT
void pfwFree(void *ptr);

#undef NONNULL
#undef NONNULL_
#undef USERESULT

#ifdef __cplusplus
}
#endif
