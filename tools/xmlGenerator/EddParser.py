#!/usr/bin/python2
# -*-coding:utf-8 -*

# Copyright (c) 2011-2014, Intel Corporation
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



import re
import sys
import copy
from itertools import izip
from itertools import imap

# =====================================================================
""" Context classes, used during propagation and the "to PFW script" step """
# =====================================================================

class PropagationContextItem(list) :
    """Handle an item during the propagation step"""
    def __copy__(self):
        """C.__copy__() -> a shallow copy of C"""
        return self.__class__(self)

class PropagationContextElement(PropagationContextItem) :
    """Handle an Element during the propagation step"""
    def getElementsFromName(self, name):
        matchingElements = []
        for element in self :
            if element.getName() == name :
                matchingElements.append(element)
        return matchingElements


class PropagationContextOption(PropagationContextItem) :
    """Handle an Option during the propagation step"""
    def getOptionItems (self, itemName):
        items = []
        for options in self :
            items.append(options.getOption(itemName))
        return items


class PropagationContext() :
    """Handle the context during the propagation step"""
    def __init__(self, propagationContext=None) :

        if propagationContext == None :
            self._context = {
                "DomainOptions" : PropagationContextOption() ,
                "Configurations" : PropagationContextElement() ,
                "ConfigurationOptions" : PropagationContextOption() ,
                "Rules" : PropagationContextElement() ,
                "PathOptions" : PropagationContextOption() ,
        }
        else :
            self._context = propagationContext

    def copy(self):
        """return a copy of the context"""
        contextCopy = self._context.copy()

        for key in iter(self._context) :
            contextCopy[key] = contextCopy[key].__copy__()

        return self.__class__(contextCopy)

    def getDomainOptions (self):
        return self._context["DomainOptions"]

    def getConfigurations (self):
        return self._context["Configurations"]

    def getConfigurationOptions (self):
        return self._context["ConfigurationOptions"]

    def getRules (self):
        return self._context["Rules"]

    def getPathOptions (self):
        return self._context["PathOptions"]


# =====================================================
"""Element option container"""
# =====================================================

class Options () :
    """handle element options"""
    def __init__(self, options=[], optionNames=[]) :
        self.options = dict(izip(optionNames, options))
        # print(options,optionNames,self.options)


    def __str__(self) :
        ops2str = []
        for name, argument in self.options.items() :
            ops2str.append(str(name) + "=\"" + str(argument) + "\"")

        return " ".join(ops2str)

    def getOption(self, name):
        """get option by its name, if it does not exist return empty string"""
        return self.options.get(name, "")

    def setOption(self, name, newOption):
        """set option by its name"""
        self.options[name] = newOption

    def copy (self):
        """D.copy() -> a shallow copy of D"""
        copy = Options()
        copy.options = self.options.copy()
        return copy

# ====================================================
"""Definition of all element class"""
# ====================================================

class Element(object):
    """ implement a basic element

    It is the class base for all other elements as Domain, Configuration..."""
    tag = "unknown"
    optionNames = ["Name"]
    childWhiteList = []
    optionDelimiter = " "

    def __init__(self, line=None) :

        if line == None :
            self.option = Options([], self.optionNames)
        else :
            self.option = self.optionFromLine(line)

        self.children = []

    def optionFromLine(self, line) :
        # get ride of spaces
        line = line.strip()

        options = self.extractOptions(line)

        return Options(options, self.optionNames)

    def extractOptions(self, line) :
        """return the line splited by the optionDelimiter atribute

        Option list length is less or equal to the optionNames list length
        """
        options = line.split(self.optionDelimiter, len(self.optionNames) - 1)

        # get ride of leftover spaces
        optionsStrip = list(imap(str.strip, options))

        return optionsStrip

    def addChild(self, child, append=True) :
        """ A.addChid(B) -> add B to A child list if B class name is in A white List"""
        try:
            # Will raise an exception if this child is not in the white list
            self.childWhiteList.index(child.__class__.__name__)
            # If no exception was raised, add child to child list

            if append :
                self.children.append(child)
            else :
                self.children.insert(0, child)

        except ValueError:
            # the child class is not in the white list
            raise ChildNotPermitedError("", self, child)

    def addChildren(self, children, append=True) :
        """Add a list of child"""
        if append:
            # Add children at the end of the child list
            self.children.extend(children)
        else:
            # Add children at the begining of the child list
            self.children = children + self.children

    def childrenToString(self, prefix=""):
        """return raw printed children """
        body = ""
        for child in self.children :
            body = body + child.__str__(prefix)

        return body

    def __str__(self, prefix="") :
        """return raw printed element"""
        selfToString = prefix + " " + self.tag + " " + str(self.option)
        return selfToString + "\n" + self.childrenToString(prefix + "\t")

    def extractChildrenByClass(self, classTypeList) :
        """return all children whose class is in the list argument

        return a list of all children whose class in the list "classTypeList" (second arguments)"""
        selectedChildren = []

        for child in  self.children :
            for classtype in classTypeList :
                if child.__class__ == classtype :
                    selectedChildren.append(child)
                    break
        return selectedChildren

    def propagate (self, context=PropagationContext()):
        """call the propagate method of all children"""
        for child in  self.children :
            child.propagate(context)

    def getName(self):
        """return name option value. If none return "" """
        return self.option.getOption("Name")

    def setName(self, name):
        self.option.setOption("Name", name)

    def translate(self, translator):
        for child in self.children:
            child.translate(translator)

# ----------------------------------------------------------

class ElementWithTag (Element):
    """Element of this class are declared with a tag  => line == "tag: .*" """
    def extractOptions(self, line) :
        lineWithoutTag = line.split(":", 1)[-1].strip()
        options = super(ElementWithTag, self).extractOptions(lineWithoutTag)
        return options

# ----------------------------------------------------------

class ElementWithInheritance(Element):
    def propagate (self, context=PropagationContext) :
        """propagate some proprieties to children"""

        # copy the context so that everything that hapend next will only affect
        # children
        contextCopy = context.copy()

        # check for inheritance
        self.Inheritance(contextCopy)

        # call the propagate method of all children
        super(ElementWithInheritance, self).propagate(contextCopy)


class ElementWithRuleInheritance(ElementWithInheritance):
    """class that will give to its children its rules"""
    def ruleInheritance(self, context):
        """Add its rules to the context and get context rules"""

        # extract all children rule and operator
        childRules = self.extractChildrenByClass([Operator, Rule])

        # get context rules
        contextRules = context.getRules()

        # adopt rules of the beginning of the context
        self.addChildren(contextRules, append=False)

        # add previously extract rules to the context
        contextRules += childRules


# ----------------------------------------------------------

class EmptyLine (Element) :
    """This class represents an empty line.

    Will raise "EmptyLineWarning" exception at instanciation."""

    tag = "emptyLine"
    match = re.compile(r"[ \t]*\n?$").match
    def __init__ (self, line):
       raise EmptyLineWarning(line)

# ----------------------------------------------------------

class Commentary(Element):
    """This class represents a commentary.

    Will raise "CommentWarning" exception at instanciation."""

    tag = "commentary"
    optionNames = ["comment"]
    match = re.compile(r"#").match
    def __init__ (self, line):
       raise CommentWarning(line)

# ----------------------------------------------------------

class Path (ElementWithInheritance) :
    """class implementing the "path = value" concept"""
    tag = "path"
    optionNames = ["Name", "value"]
    match = re.compile(r".+=").match
    optionDelimiter = "="

    def translate(self, translator):
        translator.setParameter(self.getName(), self.option.getOption("value"))

    def Inheritance (self, context) :
        """check for path name inheritance"""
        self.OptionsInheritance(context)

    def OptionsInheritance (self, context) :
        """make configuration name inheritance """

        context.getPathOptions().append(self.option.copy())
        self.setName("/".join(context.getPathOptions().getOptionItems("Name")))


class GroupPath (Path, ElementWithTag) :
    tag = "component"
    match = re.compile(tag + r" *:").match
    optionNames = ["Name"]
    childWhiteList = ["Path", "GroupPath"]

    def getPathNames (self) :
        """Return the list of all path child name"""

        pathNames = []

        paths = self.extractChildrenByClass([Path])
        for path in paths :
            pathNames.append(path.getName())

        groupPaths = self.extractChildrenByClass([GroupPath])
        for groupPath in groupPaths :
            pathNames += groupPath.getPathNames()

        return pathNames

    def translate(self, translator):
        for child in self.extractChildrenByClass([Path, GroupPath]):
            child.translate(translator)

# ----------------------------------------------------------

class Rule (Element) :
    """class implementing the rule concept

    A rule is composed of a criterion, a rule type and an criterion state.
    It should not have any child and is propagated to all configuration in parent descendants.
    """

    tag = "rule"
    optionNames = ["criterion", "type", "element"]
    match = re.compile(r"[a-zA-Z0-9_.]+ +(Is|IsNot|Includes|Excludes) +[a-zA-Z0-9_.]+").match
    childWhiteList = []

    def PFWSyntax (self, prefix=""):

        script = prefix + \
                    self.option.getOption("criterion") + " " + \
                    self.option.getOption("type") + " " + \
                    self.option.getOption("element")

        return script


class Operator (Rule) :
    """class implementing the operator concept

    An operator contains rules and other operators
    It is as rules propagated to all configuration children in parent descendants.
    It should only have the name ANY or ALL to be understood by PFW.
    """

    tag = "operator"
    optionNames = ["Name"]
    match = re.compile(r"ANY|ALL").match
    childWhiteList = ["Rule", "Operator"]

    syntax = { "ANY" : "Any" , "ALL" : "All"}

    def PFWSyntax (self, prefix=""):
        """ return a pfw rule (ex : "Any{criterion1 is state1}") generated from "self" and its children options"""
        script = ""

        script += prefix + \
                    self.syntax[self.getName()] + "{ "

        rules = self.extractChildrenByClass([Rule, Operator])

        PFWRules = []
        for rule in rules :
            PFWRules.append(rule.PFWSyntax(prefix + "    "))

        script += (" , ").join(PFWRules)

        script += prefix + " }"

        return script

# ----------------------------------------------------------

class Configuration (ElementWithRuleInheritance, ElementWithTag) :
    tag = "configuration"
    optionNames = ["Name"]
    match = re.compile(r"conf *:").match
    childWhiteList = ["Rule", "Operator", "Path", "GroupPath"]

    def composition (self, context):
        """make all needed composition

        Composition is the fact that group configuration with the same name defined
        in a parent will give their rule children to this configuration
        """

        name = self.getName()
        sameNameConf = context.getConfigurations().getElementsFromName(name)

        sameNameConf.reverse()

        for configuration in sameNameConf :
            # add same name configuration rule children to self child list
            self.addChildren(configuration.extractChildrenByClass([Operator, Rule]), append=False)


    def propagate (self, context=PropagationContext) :
        """propagate proprieties to children

        make needed compositions, join ancestor name to its name,
        and add rules previously defined rules"""

        # make all needed composition
        self.composition(context)

        super(Configuration, self).propagate(context)

    def Inheritance (self, context) :
        """make configuration name and rule inheritance"""
        # check for configuration name inheritance
        self.OptionsInheritance(context)

        # check for rule inheritance
        self.ruleInheritance(context)

    def OptionsInheritance (self, context) :
        """make configuration name inheritance """

        context.getConfigurationOptions().append(self.option.copy())
        self.setName(".".join(context.getConfigurationOptions().getOptionItems("Name")))


    def getRootPath (self) :

        paths = self.extractChildrenByClass([Path, GroupPath])

        rootPath = GroupPath()
        rootPath.addChildren(paths)

        return rootPath

    def getConfigurableElements (self) :
        """return all path name defined in this configuration"""

        return self.getRootPath().getPathNames()

    def getRuleString(self):
        """Output this configuration's rule as a string"""

        # Create a rootRule
        ruleChildren = self.extractChildrenByClass([Rule, Operator])

        # Do not create a root rule if there is only one fist level Operator rule
        if len(ruleChildren) == 1 and ruleChildren[0].__class__ == Operator :
            ruleroot = ruleChildren[0]

        else :
            ruleroot = Operator()
            ruleroot.setName("ALL")
            ruleroot.addChildren(ruleChildren)

        return ruleroot.PFWSyntax()

    def translate(self, translator):
        translator.createConfiguration(self.getName())
        translator.setRule(self.getRuleString())

        paths = self.extractChildrenByClass([Path, GroupPath])
        translator.setElementSequence(self.getConfigurableElements())
        for path in paths:
            path.translate(translator)

    def copy (self) :
        """return a shallow copy of the configuration"""

        # create configuration or subclass copy
        confCopy = self.__class__()

        # add children
        confCopy.children = list(self.children)

        # add option
        confCopy.option = self.option.copy()

        return confCopy

class GroupConfiguration (Configuration) :
    tag = "GroupConfiguration"
    optionNames = ["Name"]
    match = re.compile(r"(supConf|confGroup|confType) *:").match
    childWhiteList = ["Rule", "Operator", "GroupConfiguration", "Configuration", "GroupPath"]

    def composition (self, context) :
        """add itself in context for configuration composition

        Composition is the fact that group configuration with the same name defined
        in a parent will give their rule children to this configuration
        """

        # copyItself
        selfCopy = self.copy()

        # make all needed composition
        super(GroupConfiguration, self).composition(context)

        # add the copy in context for futur configuration composition
        context.getConfigurations().append(selfCopy)


    def getConfigurableElements (self) :
        """return a list. Each elements consist of a list of configurable element of a configuration

        return a list consisting of all configurable elements for each configuration.
        These configurable elements are organized in a list"""
        configurableElements = []

        configurations = self.extractChildrenByClass([Configuration])
        for configuration in configurations :
            configurableElements.append(configuration.getConfigurableElements())

        groudeConfigurations = self.extractChildrenByClass([GroupConfiguration])
        for groudeConfiguration in groudeConfigurations :
            configurableElements += groudeConfiguration.getConfigurableElements()

        return configurableElements

    def translate(self, translator):
        for child in self.extractChildrenByClass([Configuration, GroupConfiguration]):
            child.translate(translator)

# ----------------------------------------------------------

class Domain (ElementWithRuleInheritance, ElementWithTag) :
    tag = "domain"
    sequenceAwareKeyword = "sequenceAware"

    match = re.compile(r"domain *:").match
    optionNames = ["Name", sequenceAwareKeyword]
    childWhiteList = ["Configuration", "GroupConfiguration", "Rule", "Operator"]

    def propagate (self, context=PropagationContext) :
        """ propagate name, sequenceAwareness and rule to children"""

        # call the propagate method of all children
        super(Domain, self).propagate(context)

        self.checkConfigurableElementUnicity()

    def Inheritance (self, context) :
        """check for domain name, sequence awarness and rules inheritance"""
        # check for domain name and sequence awarness inheritance
        self.OptionsInheritance(context)

        # check for rule inheritance
        self.ruleInheritance(context)

    def OptionsInheritance(self, context) :
        """ make domain name and sequence awareness inheritance

        join to the domain name all domain names defined in context and
        if any domain in context is sequence aware, set sequenceAwareness to True"""

        # add domain options to context
        context.getDomainOptions().append(self.option.copy())

        # set name to the junction of all domain name in context
        self.setName(".".join(context.getDomainOptions().getOptionItems("Name")))

        # get sequenceAwareness of all domains in context
        sequenceAwareList = context.getDomainOptions().getOptionItems(self.sequenceAwareKeyword)
        # or operation on all booleans in sequenceAwareList
        sequenceAwareness = False
        for sequenceAware in sequenceAwareList :
            sequenceAwareness = sequenceAwareness or sequenceAware
        # current domain sequenceAwareness = sequenceAwareness
        self.option.setOption(self.sequenceAwareKeyword, sequenceAwareness)


    def extractOptions(self, line) :
        """Extract options from the definition line"""
        options = super(Domain, self).extractOptions(line)

        sequenceAwareIndex = self.optionNames.index(self.sequenceAwareKeyword)

        # translate the keyword self.sequenceAwareKeyword if specified to boolean True,
        # to False otherwise
        try :
            if options[sequenceAwareIndex] == self.sequenceAwareKeyword :
               options[sequenceAwareIndex] = True
            else:
               options[sequenceAwareIndex] = False
        except IndexError :
            options = options + [None] * (sequenceAwareIndex - len(options)) + [False]
        return options

    def getRootConfiguration (self) :
        """return the root configuration group"""
        configurations = self.extractChildrenByClass([Configuration, GroupConfiguration])

        configurationRoot = GroupConfiguration()

        configurationRoot.addChildren(configurations)

        return configurationRoot

    # TODO: don't do that in the parser, let the PFW tell you that
    def checkConfigurableElementUnicity (self):
        """ check that all configurable elements defined in child configuration are the sames"""

        # get a list. Each elements of is the configurable element list of a configuration
        configurableElementsList = self.getRootConfiguration().getConfigurableElements()

        # if at least two configurations in the domain
        if len(configurableElementsList) > 1 :

            # get first configuration configurable element list sort
            configurableElementsList0 = list(configurableElementsList[0])
            configurableElementsList0.sort()

            for configurableElements in configurableElementsList :
                # sort current configurable element list
                auxConfigurableElements = list(configurableElements)
                auxConfigurableElements.sort()

                if auxConfigurableElements != configurableElementsList0 :
                    # if different, 2 configurations those not have the same configurable element list
                    # => one or more configurable element is missing in one of the 2 configuration
                    raise UndefinedParameter(self.getName())


    def translate(self, translator):
        sequence_aware = self.option.getOption(self.sequenceAwareKeyword)
        translator.createDomain(self.getName(), sequence_aware)

        configurations = self.getRootConfiguration()
        configurableElementsList = configurations.getConfigurableElements()

        # add configurable elements
        if len(configurableElementsList) != 0 :
            for configurableElement in configurableElementsList[0] :
                translator.addElement(configurableElement)

        configurations.translate(translator)

class GroupDomain (Domain) :
    tag = "groupDomain"
    match = re.compile(r"(supDomain|domainGroup) *:").match
    childWhiteList = ["GroupDomain", "Domain", "GroupConfiguration", "Rule", "Operator"]

    def translate(self, translator):
        for child in self.extractChildrenByClass([Domain, GroupDomain]):
            child.translate(translator)

# ----------------------------------------------------------

class Root(Element):
    tag = "root"
    childWhiteList = ["Domain", "GroupDomain"]


# ===========================================
""" Syntax error Exceptions"""
# ===========================================

class MySyntaxProblems(SyntaxError) :
    comment = "syntax error in %(line)s "

    def __init__(self, line=None, num=None):
        self.setLine(line, num)

    def __str__(self):

        if self.line :
            self.comment = self.comment % {"line" : repr(self.line)}
        if self.num :
            self.comment = "Line " + str(self.num) + ", " + self.comment
        return self.comment

    def setLine (self, line, num):
        self.line = str(line)
        self.num = num


# ---------------------------------------------------------

class MyPropagationError(MySyntaxProblems) :
    """ Syntax error Exceptions used in the propagation step"""
    pass

class UndefinedParameter(MyPropagationError) :
    comment = "Configurations in domain '%(domainName)s' do not all set the same parameters "
    def __init__ (self, domainName):
        self.domainName = domainName
    def __str__ (self):
        return self.comment % { "domainName" : self.domainName }


# -----------------------------------------------------
""" Syntax error Exceptions used by parser"""

class MySyntaxError(MySyntaxProblems) :
    """ Syntax error Exceptions used by parser"""
    pass

class MySyntaxWarning(MySyntaxProblems) :
    """ Syntax warning Exceptions used by parser"""
    pass

class IndentationSyntaxError(MySyntaxError) :
    comment = """syntax error in %(line)s has no father element.
    You can only increment indentation by one tabutation per line")"""

class EmptyLineWarning(MySyntaxWarning):
    comment = "warning : %(line)s is an empty line and has been ommited"

class CommentWarning(MySyntaxWarning):
    comment = "warning : %(line)s is a commentary and has been ommited"

class ChildNotPermitedError(MySyntaxError):
    def __init__(self, line, fatherElement, childElement):
        self.comment = "syntax error in %(line)s, " + fatherElement.tag + " should not have a " + childElement.tag + " child."
        super(ChildNotPermitedError, self).__init__(line)


class UnknownElementTypeError(MySyntaxError):
    comment = " error in line %(line)s , not known element type were matched "

class SpaceInIndentationError(MySyntaxError):
    comment = " error in ,%(line)s space is not permited in indentation"


# ============================================
"""Class creating the DOM elements from a stream"""
# ============================================

class ElementsFactory(object)  :
    """Element factory, return an instance of the first matching element

    Test each element list in elementClass and instanciate it if it's methode match returns True
    The method match is called with input line as argument
    """
    def __init__ (self):
        self.elementClass = [
        EmptyLine ,
        Commentary,
        GroupDomain,
        Domain,
        Path,
        GroupConfiguration,
        Configuration,
        Operator,
        Rule,
        GroupPath
        ]

    def createElementFromLine (self, line) :
        """return an instance of the first matching element

        Test each element list in elementClass and instanciate it if it's methode match returns True
        The method match is called with the argument line.
        Raise UnknownElementTypeError if no element matched.
        """
        for element in self.elementClass :
            if element.match(line) :
                # print (line + element.__class__.__name__)
                return element(line)
        # if we have not find any
        raise UnknownElementTypeError(line)

#------------------------------------------------------

class Parser(object) :
    """Class implementing the parser"""
    def __init__(self):
        self.rankPattern = re.compile(r"^([\t ]*)(.*)")
        self.elementFactory = ElementsFactory()
        self.previousRank = 0

    def __parseLine__(self, line):

        rank, rest = self.__getRank__(line)

        # instanciate the coresponding element
        element = self.elementFactory.createElementFromLine(rest)

        self.__checkIndentation__(rank)

        return rank, element

    def __getRank__(self, line):
        """return the rank, the name and the option of the input line

the rank is the number of tabulation (\t) at the line beginning.
the rest is the rest of the line."""
        # split line in rank and rest
        rank = self.rankPattern.match(line)
        if rank :
            rank, rest = rank.group(1, 2)
        else :
            raise MySyntaxError(line)

        # check for empty line
        if rest == "" :
            raise EmptyLineWarning(line)

        # check for space in indentation
        if rank.find(" ") > -1 :
            raise SpaceInIndentationError(line)

        rank = len (rank) + 1  # rank starts at 1


        return rank, rest


    def __checkIndentation__(self, rank):
        """check if indentation > previous indentation + 1. If so, raise IndentationSyntaxError"""
        if (rank > self.previousRank + 1) :
            raise IndentationSyntaxError()
        self.previousRank = rank

    def parse(self, stream, verbose=False):
        """parse a stream, usually a opened file"""
        myroot = Root("root")
        context = [myroot]  # root is element of rank 0
        warnings = ""

        for num, line in enumerate(stream):
            try:
                rank, myelement = self.__parseLine__(line)

                while len(context) > rank :
                    context.pop()
                context.append(myelement)
                context[-2].addChild(myelement)

            except MySyntaxWarning, ex:
                ex.setLine(line, num + 1)
                if verbose :
                    print >>sys.stderr, ex

            except MySyntaxError, ex :
                ex.setLine(line, num + 1)
                raise

        return myroot

