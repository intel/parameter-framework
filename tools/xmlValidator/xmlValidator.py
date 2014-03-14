#! /usr/bin/python

# Copyright (c) 2014, Intel Corporation
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

from lxml import etree
from os import path
from os import walk
from sys import argv

class PrintColor():
    @staticmethod
    def success(stringToPrint):
        green=32
        PrintColor._printColor(green, stringToPrint)

    @staticmethod
    def error(stringToPrint):
        red=31
        PrintColor._printColor(red, stringToPrint)

    @staticmethod
    def _printColor(color, stringToPrint):
        """prints strings in color via ascii escape sequence"""
        print("\033[%sm%s\033[0m" % (str(color), stringToPrint))

def getSchemaFilenameFromXmlFile(xmlFilePath):
    """getSchemaFileNameFromXmlFile

    The pfw considers that the .xsd file has the same name as the
    root element name of the .xml.
    With of this knowledge, we may easily find the
    schema file we need.

    Args:
        xmlFilePath: the xml file.

    Returns:
        str: the corresponding .schema name
    """
    xmlTree = etree.parse(xmlFilePath)
    rootElement = xmlTree.getroot()
    return rootElement.tag + '.xsd'

def validateXmlWithSchema(xmlFilePath, schemaFilePath):
    """validateXmlWithSchema

    Validates an .xml file based on his corresponding schema.

    Args:
        xmlFilePath (str): the absolute path to the xml file.
        schemaFilePath (str): the absolute path to the schema.
    """
    baseXmlName = path.basename(xmlFilePath)
    baseSchemaName = path.basename(schemaFilePath)
    print 'Attempt to validate', baseXmlName, 'with', baseSchemaName

    schemaContent = etree.parse(schemaFilePath)
    schema = etree.XMLSchema(schemaContent)
    xmlContent = etree.parse(xmlFilePath)
    xmlContent.xinclude()

    if schema.validate(xmlContent):
        PrintColor.success('%s is valid' % str(baseXmlName))
    else:
        PrintColor.error('Error: %s' % str(schema.error_log))

# handle main arguments
if len(argv) != 3:
    PrintColor.error('Error: usage %s xmlDirectory schemaDirectory' % str(argv[0]))
    exit(1)

xmlDirectory = argv[1]
schemaDirectory = argv[2]

print('[*] Validate xml files in %s with %s' % (xmlDirectory, schemaDirectory))

for rootPath, _, files in walk(xmlDirectory):
    for filename in files:
        if filename.endswith('.xml'):
            xmlFilePath = path.join(rootPath, filename)
            schemaFileName = getSchemaFilenameFromXmlFile(xmlFilePath)
            schemaFilePath = path.join(schemaDirectory, schemaFileName)
            validateXmlWithSchema(xmlFilePath, schemaFilePath)
