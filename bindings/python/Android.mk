# Copyright (c) 2015, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

LOCAL_PATH := $(call my-dir)

# Low-level python wrapper
include $(CLEAR_VARS)
# When importing a python module from a shared library, the name of the lib has
# to be exactly the same as the module name it contains. SWIG generates a
# python module called "PyPfw" that relies on the one that we are building as a
# shared library here, and that is named "_PyPfw"
# See https://docs.python.org/2/extending/extending.html#the-module-s-method-table-and-initialization-function
LOCAL_MODULE := _PyPfw

LOCAL_CPP_EXTENSION := .cxx
# As long as the parameter-framework is compiled with gcc, we must avoid
# compiling the bindings with clang and compile with gcc instead.
LOCAL_CLANG := false
# Android only provides a 32bit version of python.
LOCAL_32_BIT_ONLY := true

LOCAL_SHARED_LIBRARIES := libparameter_host
LOCAL_STATIC_LIBRARIES := libxmlserializer_host

# python is only available in 32bits for now, thus arch is forced to 32bits
PYTHON_INSTALL_PATH := prebuilts/python/$(HOST_OS)-x86/2.7.5/
PYTHON_INCLUDES_PATH := $(PYTHON_INSTALL_PATH)/include/python2.7
PYTHON_BIN_PATH := $(PYTHON_INSTALL_PATH)/bin

LOCAL_C_INCLUDES := \
    $(PYTHON_INCLUDES_PATH) \
    $(HOST_OUT_HEADERS)/parameter

# The 'unused-but-set-variable' warning must be disabled because SWIG generates
# files that do not respect that constraint.
# '-DSWIG_PYTHON_SILENT_MEMLEAK' is needed because the "memleak" warning
# pollutes the standard output. At the time of writing, the only warning is
# spurious anyway, as it relates to "ILogger *" which is an abstract
# class/interface class and as such cannot be destroyed.
LOCAL_CFLAGS := -Wno-unused-but-set-variable -fexceptions -DSWIG_PYTHON_SILENT_MEMLEAK

# Undefined symbols will be resolved at runtime
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := intel

LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_IS_HOST_MODULE := true

generated-sources-dir := $(call local-generated-sources-dir)

LOCAL_GENERATED_SOURCES := $(generated-sources-dir)/pfw_wrap.cxx $(generated-sources-dir)/pfw_wrap.h

LOCAL_EXPORT_C_INCLUDE_DIRS := $(generated-sources-dir)

# Careful, we need to invoque the android python config not the host's one.
# BTW, the intenal install directory of python is hardcoded to a dummy value,
# thus we need to manually add the correct path to libs to the library list.
ifeq ($(HOST_OS), linux)
LOCAL_LDLIBS += $(shell $(PYTHON_BIN_PATH)/python $(PYTHON_BIN_PATH)/python-config --ldflags) \
                -L $(PYTHON_INSTALL_PATH)/lib/
else
LOCAL_LDLIBS += $(shell python-config --ldflags)
endif

$(generated-sources-dir)/pfw_wrap.h: $(generated-sources-dir)/pfw_wrap.cxx

# The PyPfw.py file is generated in the directory given by -outdir switch, thus
# this directory must be put in the python path to be reachable
$(generated-sources-dir)/pfw_wrap.cxx: HOST_ARCH := $(HOST_ARCH)
$(generated-sources-dir)/pfw_wrap.cxx: HOST_OS := $(HOST_OS)
$(generated-sources-dir)/pfw_wrap.cxx: HOST_LIBRARY_PATH := $(HOST_LIBRARY_PATH)

$(generated-sources-dir)/pfw_wrap.cxx: $(LOCAL_PATH)/pfw.i
	@echo "Generating Python binding files"
	mkdir -p $(dir $@) # surprisingly, path is not generated by build system
	mkdir -p $(HOST_LIBRARY_PATH)
	prebuilts/misc/$(HOST_OS)-$(HOST_ARCH)/swig/swig \
		-Iprebuilts/misc/common/swig/include/2.0.11/python/ \
		-Iprebuilts/misc/common/swig/include/2.0.11/ \
		-Wall -Werror -v -python -c++ -outdir $(HOST_LIBRARY_PATH)/ -o $@ $^

include $(BUILD_HOST_SHARED_LIBRARY)

