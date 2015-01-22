# INTEL CONFIDENTIAL
# Copyright (c) 2014 Intel
# Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation or its suppliers
# or licensors. Title to the Material remains with Intel Corporation or its
# suppliers and licensors. The Material contains trade secrets and proprietary
# and confidential information of Intel or its suppliers and licensors. The
# Material is protected by worldwide copyright and trade secret laws and
# treaty provisions. No part of the Material may be used, copied, reproduced,
# modified, published, uploaded, posted, transmitted, distributed, or
# disclosed in any way without Intel's prior express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or delivery
# of the Materials, either expressly, by implication, inducement, estoppel or
# otherwise. Any license under such intellectual property rights must be
# express and approved by Intel in writing.

LOCAL_PATH := $(call my-dir)

# Resources are not compiled so the prebuild mechanism is used to export them.
# Schemas are only used by host, in order to validate xml files
##################################################

include $(CLEAR_VARS)
LOCAL_MODULE := ParameterFrameworkConfiguration.xsd
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(HOST_OUT)/etc/parameter-framework/Schemas
LOCAL_IS_HOST_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ConfigurableDomain.xsd
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(HOST_OUT)/etc/parameter-framework/Schemas
LOCAL_IS_HOST_MODULE := true
LOCAL_REQUIRED_MODULES := \
    ParameterSettings.xsd
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ConfigurableDomains.xsd
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(HOST_OUT)/etc/parameter-framework/Schemas
LOCAL_IS_HOST_MODULE := true
LOCAL_REQUIRED_MODULES := \
    ConfigurableDomain.xsd
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := SystemClass.xsd
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(HOST_OUT)/etc/parameter-framework/Schemas
LOCAL_IS_HOST_MODULE := true
LOCAL_REQUIRED_MODULES := \
    FileIncluder.xsd \
    Subsystem.xsd
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ParameterSettings.xsd
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(HOST_OUT)/etc/parameter-framework/Schemas
LOCAL_IS_HOST_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := FileIncluder.xsd
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(HOST_OUT)/etc/parameter-framework/Schemas
LOCAL_IS_HOST_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := Subsystem.xsd
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(HOST_OUT)/etc/parameter-framework/Schemas
LOCAL_IS_HOST_MODULE := true
LOCAL_REQUIRED_MODULES := \
    ComponentLibrary.xsd
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ComponentLibrary.xsd
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(HOST_OUT)/etc/parameter-framework/Schemas
LOCAL_IS_HOST_MODULE := true
LOCAL_REQUIRED_MODULES := \
    ComponentTypeSet.xsd \
    W3cXmlAttributes.xsd
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ComponentTypeSet.xsd
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(HOST_OUT)/etc/parameter-framework/Schemas
LOCAL_IS_HOST_MODULE := true
LOCAL_REQUIRED_MODULES := \
    Parameter.xsd \
    W3cXmlAttributes.xsd
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := W3cXmlAttributes.xsd
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(HOST_OUT)/etc/parameter-framework/Schemas
LOCAL_IS_HOST_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := Parameter.xsd
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(HOST_OUT)/etc/parameter-framework/Schemas
LOCAL_IS_HOST_MODULE := true
include $(BUILD_PREBUILT)
##################################################
