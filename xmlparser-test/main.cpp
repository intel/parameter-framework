/* <auto_header>
 * <FILENAME>
 * 
 * INTEL CONFIDENTIAL
 * Copyright © 2011 Intel 
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
 * 
 *  AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 * 
 * 
 * </auto_header>
 */
/**
 * section: Tree
 * synopsis: Navigates a tree to print element names
 * purpose: Parse a file to a tree, use xmlDocGetRootElement() to
 *          get the root element, then walk the document and print
 *          all the element name in document order.
 * usage: tree1 filename_or_URL
 * test: tree1 test2.xml > tree1.tmp ; diff tree1.tmp tree1.res ; rm tree1.tmp
 * author: Dodji Seketeli
 * copy: see Copyright for the status of this software.
 */
#include <stdio.h>
#include <stdarg.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>

/*
 *To compile this file using gcc you can type
 *gcc `xml2-config --cflags --libs` -o xmlexample libxml2-example.c
 */
static void schemaValidityErrorFunc(void *ctx, const char *msg, ...)
{
    char acBuffer[256];
    va_list listPointer;

    (void)ctx;

    va_start( listPointer, msg );
    //vprintf(msg, listPointer);

    vsnprintf(acBuffer, sizeof(acBuffer), msg, listPointer);

    va_end( listPointer );

    puts(acBuffer);
}

static void schemaValidityWarningFunc(void *ctx, const char *msg, ...)
{
    char acBuffer[256];
    va_list listPointer;

    (void)ctx;

    va_start( listPointer, msg );

    vsnprintf(acBuffer, sizeof(acBuffer), msg, listPointer);

    va_end( listPointer );

    puts(acBuffer);
}

static int is_valid(const xmlDocPtr doc, const char *schema_filename)
{
#ifdef LIBXML_SCHEMAS_ENABLED
    xmlDocPtr schema_doc = xmlReadFile(schema_filename, NULL, XML_PARSE_NONET);
    if (schema_doc == NULL) {
        /* the schema cannot be loaded or is not well-formed */
        return -1;
    }
    xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewDocParserCtxt(schema_doc);
    if (parser_ctxt == NULL) {
        /* unable to create a parser context for the schema */
        xmlFreeDoc(schema_doc);
        return -2;
    }

    xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt);
    if (schema == NULL) {
        /* the schema itself is not valid */
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        return -3;
    }
    xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt(schema);
    if (valid_ctxt == NULL) {
        /* unable to create a validation context for the schema */
        xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        return -4;
    }
    xmlSchemaSetValidErrors(valid_ctxt, schemaValidityErrorFunc, schemaValidityWarningFunc, NULL);

    int is_valid = (xmlSchemaValidateDoc(valid_ctxt, doc) == 0);
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);

    /* force the return value to be non-negative on success */
    return is_valid ? 1 : 0;
#else
    return 1;
#endif
}

/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */
static void
print_element_names(xmlNode * a_node, int iLevel = 0)
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {

        int iIndent = iLevel;

        while (iIndent--) {
            printf("    ");
        }

        if (cur_node->type == XML_ELEMENT_NODE) {

            printf("node type: Element, name: %s\n", cur_node->name);
        }
        print_element_names(cur_node->children, iLevel + 1);
    }
}


/**
 * Simple example to parse a file called "file.xml",
 * walk down the DOM, and print the name of the
 * xml elements nodes.
 */
int
main(int argc, char **argv)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    if (argc != 3)
        return(1);

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile(argv[1], NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file %s\n", argv[1]);
    }
    // Validate
    if (!is_valid(doc, argv[2])) {
        printf("error: doc is not valid\n");

        return -1;
    }


    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);


    print_element_names(root_element);

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    return 0;
}
