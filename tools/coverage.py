#!/usr/bin/env python3

# INTEL CONFIDENTIAL
# Copyright  2013 Intel
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
# disclosed in any way without Intels prior express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or delivery
# of the Materials, either expressly, by implication, inducement, estoppel or
# otherwise. Any license under such intellectual property rights must be
# express and approved by Intel in writing.

"""
Generate a coverage report by parsing parameter framework log.

The coverage report contains the:
 - domain
 - configuration
 - rule
 - criterion
basic coverage statistics.
"""

import xml.dom.minidom
import sys
import re
import logging

FORMAT = '%(levelname)s: %(message)s'
logging.basicConfig(stream=sys.stderr, level=logging.WARNING, format=FORMAT)
logger = logging.getLogger("Coverage")

class CustomError(Exception):
	pass

class ChildError(CustomError):
	def __init__(self, parent, child):
		self.parent = parent
		self.child = child

class ChildNotFoundError(ChildError):
	def __str__(self):
		return "Unable to find the child %s in %s" % (self.child, self.parent)

class DuplicatedChildError(ChildError):
	def __str__(self):
		return "Add existing child %s in %s." % (self.child, self.parent)

class Element():
	"""Root class for all coverage elements"""
	tag = "element"

	def __init__(self, name):

		self.parent = None
		self.children = []

		self.nbUse = 0

		self.name = name

		self.debug("New element")


	def __str__(self):
		return  "%s (%s)" % (self.name, self.tag)

	def __eq__(self, compared):
		self.debug(lambda : "Comparing:\n%s" % self.dump())
		self.debug(lambda : "With:\n%s" % compared.dump())
		result = self.name == compared.name and self.children == compared.children
		self.debug("Result is %s" % result)
		return result


	def getName(self, default=""):
		return self.name or default

	def hasChildren(self):
		return bool(self.children)

	def getChildren(self):
		return self.children

	def _getDescendants(self):
		for child in self.children:
			yield child
			for descendant in child._getDescendants() :
				yield descendant

	def getChildFromName(self, childName):

		for child in self.children :

			if child.getName() == childName :
				return child

		self.debug("Child %s not found" % childName, logging.ERROR)

		self.debug("Child list :")

		for child in self.children :
			self.debug("  - %s" % child)

		raise ChildNotFoundError(self, childName)


	def addChild(self, child):
		self.debug("new child: " + child.name)
		self.children.append(child)
		child._adoptedBy(self)

	def _adoptedBy(self, parent):
		assert(not self.parent)
		self.parent = parent

	def _getElementNames(self, elementList):
		return (substate.name for substate in elementList)

	def _description(self, withCoverage, withNbUse):
		description = self.name

		if withNbUse or withCoverage :
			description += " has been used " + str(self.nbUse) + " time"

		if withCoverage :
			description += self._coverageFormating(self._getCoverage())

		return description


	def _getCoverage(self):

		coverageDependance = list(self._getCoverageDependance())

		nbcoverageDependence = len(coverageDependance)

		if nbcoverageDependence == 0:
			# Coverage makes no sense without any dependence
			return None

		nbcoverageDependanceUsed = len([element
				for element in coverageDependance
				if element.hasBeenUsed()])

		return nbcoverageDependanceUsed / nbcoverageDependence

	def _getCoverageDependance(self):
		return self.children

	def _coverageFormating(self, coverage):
		# If no coverage provided
		if not coverage :
			return ""

		# Calculate coverage
		return " (%s coverage)" % self._number2percent(coverage)

	@staticmethod
	def _number2percent(number):
		"""Format a number to a integer % string

		example: _number2percent(0.6666) -> "67%"
		"""
		return "{0:.0f}%".format(100 * number)


	def _dumpDescription(self, withCoverage, withNbUse):

		self.debug("yelding description")
		yield RankedLine(self._description(withCoverage, withNbUse), lineSuffix="")

		for dumped in self._dumpPropagate(withCoverage, withNbUse) :
			yield dumped

	def _dumpPropagate(self, withCoverage, withNbUse):

		for child in self.children :
			for dumpedDescription in child._dumpDescription(withCoverage, withNbUse) :
				yield dumpedDescription.increasedRank()


	def dump(self, withCoverage=False, withNbUse=True):

		return "\n".join(
				str(dumpedDescription) for dumpedDescription in
						self._dumpDescription(withCoverage, withNbUse))

	def exportToXML(self):
		domElement = xml.dom.minidom.Element(self.tag)
		self._XMLaddAttributes(domElement)

		for child in self.children :
			domElement.appendChild(child.exportToXML())

		return domElement

	def _XMLaddAttributes(self, domElement):
		attributes = self._getXMLAttributes()

		coverage = self._getCoverage()
		if coverage != None :
			attributes["Coverage"] = self._number2percent(coverage)

		for key, value in attributes.items():
			domElement.setAttribute(key, value)

	def _getXMLAttributes(self):
		return {
				"Name": self.name,
				"NbUse": str(self.nbUse)
				}

	def _incNbUse(self):
		self.nbUse += 1

	def childUsed(self, child):
		self._incNbUse()
		# Propagate to parent
		self._tellParentThatChildUsed()

	def _tellParentThatChildUsed(self):
		if self.parent :
			self.parent.childUsed(self)


	def parentUsed(self):
		self._incNbUse()
		# Propagate to children
		for child in self.children :
			child.parentUsed()

	def hasBeenUsed(self):
		return self.nbUse > 0

	def operationOnChild(self, path, operation):

		if path:
			return self._operationPropagate(path, operation)
		else :
			self.debug("operating on self")
			return operation(self)

	def _operationPropagate(self, path, operation):

		childName = path.pop(0)
		child = self.getChildFromName(childName)

		return child.operationOnChild(path, operation)



	def debug(self, stringOrFunction, level=logging.DEBUG):
		"""Print a debug line on stderr in tree form

		If the debug line is expensive to generate, provide callable
		object, it will be called if log is enable for this level.
		This callable object should return the logline string.
		"""
		if logger.isEnabledFor(level):

			# TODO: use buildin callable if python >= 3.2
			if hasattr(stringOrFunction, "__call__"):
				string = stringOrFunction()
			else:
				string = stringOrFunction

			rankedLine = DebugRankedLine("%s: %s" % (self, string))
			self._logDebug(rankedLine, level)

	def _logDebug(self, rankedLine, level):

		if self.parent:
			self.parent._logDebug(rankedLine.increasedRank(), level)
		else :
			logger.log(level, str(rankedLine))




class FromDomElement(Element):
	def __init__(self, DomElement):
		self._initFromDom(DomElement)
		super().__init__(self.name)


	def _initFromDom(self, DomElement):
		self.name = DomElement.getAttribute("Name")



class DomElementLocation():
	def __init__(self, classConstructor, path=None):
		self.classConstructor = classConstructor
		if path :
			self.path = path
		else :
			self.path = []

		self.path.append(classConstructor.tag)


class DomPopulatedElement(Element):
	"""Default child populate

	Look for each dom element with tag specified in self.tag
	and instantiate it with the dom element
	"""
	childClasses = []

	def populate(self, dom):

		for childDomElementLocation in self.childClasses :

			self.debug("Looking for child %s in path %s" % (
				childDomElementLocation.path[-1], childDomElementLocation.path))

			for childDomElement in self._findChildFromTagPath(dom, childDomElementLocation.path) :

				childElement = childDomElementLocation.classConstructor(childDomElement)
				self.addChild(childElement)

				childElement.populate(childDomElement)

	def _findChildFromTagPath(self, dom, path):
		if not path :
			yield dom
		else :
			# Copy list
			path = list(path)

			tag = path.pop(0)

			# Find element with tag
			self.debug("Going to find elements with tag %s in %s" % (tag, dom))
			self.debug(lambda: "Nb of solutions: %s" % len(dom.getElementsByTagName(tag)))

			for elementByTag in dom.getElementsByTagName(tag) :

				self.debug("Found element: %s" % elementByTag)

				# If the same tag is found
				if elementByTag in dom.childNodes :

					# Yield next level
					for element in self._findChildFromTagPath(elementByTag, path) :
						yield element


class Rule(Element):

	def usedIfApplicable(self, criteria):
		childApplicability = (child.usedIfApplicable(criteria)
				for child in self.children)

		isApplicable = self._isApplicable(criteria, childApplicability)

		if isApplicable :
			self._incNbUse()

		self.debug("Rule applicability: %s" % isApplicable)
		assert(isApplicable == True or isApplicable == False)

		return isApplicable


	def _isApplicable(self, criteria, childApplicability):
		# Forcing evaluation of all child by the list creation
		return all(list(childApplicability))


class CriterionRule(FromDomElement, DomPopulatedElement, Rule):
	tag = "SelectionCriterionRule"
	childClasses = []
	isApplicableOperations = {
				"Includes" : lambda criterion, value:     criterion.stateIncludes(value),
				"Excludes" : lambda criterion, value: not criterion.stateIncludes(value),
				"Is"       : lambda criterion, value:     criterion.stateIs(value),
				"IsNot"    : lambda criterion, value: not criterion.stateIs(value)
			}

	def _initFromDom(self, DomElement):
		self.selectionCriterion = DomElement.getAttribute("SelectionCriterion")
		self.matchesWhen = DomElement.getAttribute("MatchesWhen")
		self.value = DomElement.getAttribute("Value")
		self.name = "%s %s %s" % (self.selectionCriterion, self.matchesWhen, self.value)

		applicableOperationWithoutValue = self.isApplicableOperations[self.matchesWhen]
		self.isApplicableOperation = lambda criterion: applicableOperationWithoutValue(criterion, self.value)

	def _isApplicable(self, criteria, childApplicability):

		return criteria.operationOnChild([self.selectionCriterion],
				self.isApplicableOperation)


class CompoundRule(FromDomElement, DomPopulatedElement, Rule):
	"""CompoundRule can be of type ALL or ANY"""
	tag = "CompoundRule"
	# Declare childClasses but define it at first class instantiation
	childClasses = None

	def __init__(self, dom):
		# Define childClasses at first class instantiation
		if self.childClasses == None :
			self.childClasses = [DomElementLocation(CriterionRule),
					DomElementLocation(CompoundRule)]
		super().__init__(dom)

	def _initFromDom(self, DomElement):

		type = DomElement.getAttribute("Type")
		self.ofTypeAll = {"All" : True, "Any" : False}[type]
		self.name = type

	def _isApplicable(self, criteria, childApplicability):
		if self.ofTypeAll :
			applicability = super()._isApplicable(criteria, childApplicability)
		else:
			# Forcing evaluation of all child by the list creation
			applicability = any(list(childApplicability))

		return applicability

class RootRule(DomPopulatedElement, Rule):
	tag = "RootRule"
	childClasses = [DomElementLocation(CompoundRule)]

	def populate(self, dom):
		super().populate(dom)
		self.debug("Children: %s" % self.children)
		# A configuration can only have one or no rule
		assert(len(self.children) <= 1)

	def _getCoverageDependance(self):
		return self._getDescendants()


class CriteronStates(Element):
	"""Root of configuration application criterion state"""
	tag = "CriterionStates"

	def parentUsed(self, criteria):
		"""Add criteria to child if not exist, if exist increase it's nbUse"""
		self._incNbUse()

		matches = [child for child in self.children if child == criteria]

		assert(len(matches) <= 1)

		if matches :
			self.debug("Criteria state has already been encounter")
			currentcriteria = matches[0]
		else :
			self.debug("Criteria state has never been encounter, saving it")
			currentcriteria = criteria
			self.addChild(criteria)

		currentcriteria.parentUsed()



class Configuration(FromDomElement, DomPopulatedElement):
	tag = "Configuration"
	childClasses = []

	class IneligibleConfigurationAppliedError(CustomError):

		def __init__(self, configuration, criteria):
			self.configuration = configuration
			self.criteria = criteria

		def __str__(self):

			return ("Applying ineligible %s, "
				"rule:\n%s\n"
				"Criteria current state:\n%s" % (
					self.configuration,
					self.configuration.rootRule.dump(withCoverage=False, withNbUse=False),
					self.criteria.dump(withCoverage=False, withNbUse=False)
					))

	def __init__(self, DomElement):
		super().__init__(DomElement)

		self.rootRule = RootRule("RootRule")
		self.addChild(self.rootRule)

		self.criteronStates = CriteronStates("CriterionStates")
		self.addChild(self.criteronStates)

	def populate(self, dom):
		# Delegate to rootRule
		self.rootRule.populate(dom)

	def _getCoverage(self):
		# Delegate to rootRule
		return self.rootRule._getCoverage()

	def used(self, criteria):

		self._incNbUse()

		# Propagate use to parents
		self._tellParentThatChildUsed()

		# Propagate to criterion coverage
		self.criteronStates.parentUsed(criteria.export())

		# Propagate to rules
		if not self.rootRule.usedIfApplicable(criteria) :

			self.debug("Applied but rule does not match current "
                       "criteria (parent: %s) " % self.parent.name,
					logging.FATAL)

			raise self.IneligibleConfigurationAppliedError(self, criteria.export())

	def _dumpPropagate(self, withCoverage, withNbUse):
		self.debug("Going to ask %s for description" % self.rootRule)
		for dumpedDescription in self.rootRule._dumpDescription(
				withCoverage=withCoverage,
				withNbUse=withNbUse) :
			yield dumpedDescription.increasedRank()

		self.debug("Going to ask %s for description" % self.criteronStates)
		for dumpedDescription in self.criteronStates._dumpDescription(
				withCoverage=False,
				withNbUse=withNbUse) :
			yield dumpedDescription.increasedRank()


class Domain(FromDomElement, DomPopulatedElement):
	tag = "ConfigurableDomain"
	childClasses = [DomElementLocation(Configuration, ["Configurations"])]


class Domains(DomPopulatedElement):
	tag = "Domains"
	childClasses = [DomElementLocation(Domain, ["ConfigurableDomains"])]


class RankedLine():
	def __init__(self, string,
				stringPrefix="|-- ",
				rankString="|   ",
				linePrefix="",
				lineSuffix="\n"):
		self.string = string
		self.rank = 0
		self.stringPrefix = stringPrefix
		self.rankString = rankString
		self.linePrefix = linePrefix
		self.lineSuffix = lineSuffix

	def increasedRank(self):
		self.rank += 1
		return self

	def __str__(self):
		return self.linePrefix + \
			self.rank * self.rankString + \
			self.stringPrefix + \
			self.string + \
			self.lineSuffix

class DebugRankedLine(RankedLine):

	def __init__(self, string, lineSuffix=""):
		super().__init__(string,
				stringPrefix="",
				rankString="   ",
				linePrefix="",
				lineSuffix=lineSuffix)


class CriterionState(Element):
	tag = "CriterionState"
	def used(self):
		self._incNbUse()


class Criterion(Element):
	tag = "Criterion"
	inclusivenessTranslate = {True: "Inclusive", False: "Exclusive"}

	def __init__(self, name, isInclusif, stateNamesList, currentStateNamesList):
		super().__init__(name)
		self.isInclusif = isInclusif

		for state in stateNamesList :
			self.addChild(CriterionState(state))

		self.currentState = []

		# Set current state as provided
		self.currentState = [self.getChildFromName(childName)
							for childName in currentStateNamesList]

	def childUsed(self, child):
		self.currentState = child
		super().childUsed(child)

	def changeState(self, subStateNames):
		self.debug("Changing state from: %s to: %s" % (
					list(self._getElementNames(self.currentState)),
					subStateNames))

		assert(len(subStateNames) > 0 or self.isInclusif)

		newCurrentState = []
		for subStateName in subStateNames :
			subState = self.getChildFromName(subStateName)
			subState.used()
			newCurrentState.append(subState)

		self.currentState = newCurrentState

		self._incNbUse()
		self._tellParentThatChildUsed()

	def export(self):
		subStateNames = self._getElementNames(self.currentState)
		return Criterion(self.name, self.isInclusif, subStateNames, subStateNames)

	def stateIncludes(self, subStateName):
		subStateCurrentNames = list(self._getElementNames(self.currentState))

		self.debug("Testing if %s is included in %s" % (subStateName, subStateCurrentNames))

		isIncluded = subStateName in subStateCurrentNames
		self.debug("IsIncluded: %s" % isIncluded)

		return isIncluded


	def stateIs(self, subStateNames):
		if len(self.currentState) != 1 :
			return False
		else :
			return self.stateIncludes(subStateNames)

	def _getXMLAttributes(self):
		attributes = super()._getXMLAttributes()
		attributes["Type"] = self.inclusivenessTranslate[self.isInclusif]
		return attributes


class Criteria(Element):
	tag = "Criteria"

	def export(self):
		self.debug("Exporting criteria")
		assert(self.children)

		exported = Criteria(self.name)
		for child in self.children :
			exported.addChild(child.export())
		return exported

	def addChild(self, child):
		if child in self.children:
			raise DuplicatedChildError(self, child)
		super().addChild(child)

class ConfigAppliedWithoutCriteriaError(CustomError):
	def __init__(self, configurationName, domainName):
		self.configurationName = configurationName
		self.domainName = domainName
	def __str__(self):
		return ('Applying configuration "%s" from domain "%s" before declaring criteria' %
				(self.configurationName, self.domainName))

class ParsePFWlog():
	MATCH = "match"
	ACTION = "action"

	def __init__(self, domains, criteria):

		self.domains = domains;
		self.criteria = criteria;

		configApplicationRegext = r""".*Applying configuration "(.*)" from domain "([^"]*)"""
		matchConfigApplicationLine = re.compile(configApplicationRegext).match

		criterionCreationRegext = ", ".join([
					r""".*Criterion name: (.*)""",
					r"""type kind: (.*)""",
					r"""current state: (.*)""",
					r"""states: {(.*)}"""
				])
		matchCriterionCreationLine = re.compile(criterionCreationRegext).match

		changingCriterionRegext = r""".*Selection criterion changed event: Criterion name: (.*), current state: ([^\n\r]*)"""
		matchChangingCriterionLine = re.compile(changingCriterionRegext).match

		self.lineLogTypes = [
					{
						self.MATCH: matchConfigApplicationLine,
						self.ACTION: self._configApplication
					}, {
						self.MATCH: matchCriterionCreationLine,
						self.ACTION: self._criterionCreation
					}, {
						self.MATCH: matchChangingCriterionLine,
						self.ACTION: self._changingCriterion
					}
				]

	@staticmethod
	def _formatCriterionList(liststring, separator):
		list = liststring.split(separator)
		if len(list) == 1 and list[0] == "<none>":
			list = []
		return list

	def _criterionCreation(self, matchCriterionCreation):
		# Unpack
		criterionName, criterionType, currentCriterionStates, criterionStates = matchCriterionCreation.group(1, 2, 3, 4)

		criterionStateList = self._formatCriterionList(criterionStates, ", ")

		criterionIsInclusif = {"exclusive" : False, "inclusive" : True}[criterionType]

		currentcriterionStateList = self._formatCriterionList(currentCriterionStates, "|")

		logger.info("Creating criterion: " + criterionName +
					" (" + criterionType + ") " +
					" with current state: " + str(currentcriterionStateList) +
					", possible states:" + str(criterionStateList))

		self.criteria.addChild(Criterion(
					criterionName,
					criterionIsInclusif,
					criterionStateList,
					currentcriterionStateList
				))

	def _changingCriterion(self, matchChangingCriterion):
		# Unpack
		criterionName, newCriterionSubStateNames = matchChangingCriterion.group(1, 2)

		newCriterionState = self._formatCriterionList(newCriterionSubStateNames, "|")

		logger.info("Changing criterion %s to %s" % (criterionName , newCriterionState))

		path = [criterionName]
		changeCriterionOperation = lambda criterion : criterion.changeState(newCriterionState)
		self.criteria.operationOnChild(path, changeCriterionOperation)

	def _configApplication(self, matchConfig):
		# Unpack
		configurationName, domainName = matchConfig.group(1, 2)

		# Check that at least one criterion exist
		if not self.criteria.hasChildren() :
			logger.error("Applying configuration before declaring criteria")
			logger.info("Is the log starting at PFW boot ?")
			raise ConfigAppliedWithoutCriteriaError(configurationName, domainName)

		# Change criterion state
		path = [domainName, configurationName]
		usedOperation = lambda element : element.used(self.criteria)

		logger.info("Applying configuration %s from domain %s" % (
				configurationName, domainName))

		self.domains.operationOnChild(path, usedOperation)


	def _digest(self, lineLogType, lineLog):
		match = lineLogType[self.MATCH](lineLog)
		if match :
			lineLogType[self.ACTION](match)
			return True
		return False

	def parsePFWlog(self, lines):
		for lineLog in lines:

			logger.debug("Parsing line :%s" % lineLog.rstrip())

			digested = (self._digest(lineLogType, lineLog)
					for lineLogType in self.lineLogTypes)

			if not any(digested):
				logger.debug("Line does not match, dropped")


class Root(Element):
	tag = "Root"
	def __init__(self, name, dom):
		super().__init__(name)
		# Create domain tree
		self.domains = Domains("Domains")
		self.domains.populate(dom)
		self.addChild(self.domains)
		# Create criterion list
		self.criteria = Criteria("CriterionRoot")
		self.addChild(self.criteria)

	def exportToXML(self):
		"""Export tree to an xml document"""
		impl = xml.dom.minidom.getDOMImplementation()
		newdoc = impl.createDocument(None, self.name, None)
		XMLDocElement = newdoc.documentElement

		for child in self.children:
			XMLDocElement.appendChild(child.exportToXML())

		return newdoc

# ============================
# Command line argument parser
# ============================


class ArgumentParser:
	"""class that parse command line arguments with argparse library

	Result of parsing are the class attributes.
	"""
	levelTranslate = [logging.WARNING, logging.INFO, logging.DEBUG]

	def __init__(self):

		try:
			# As argparse is only in the stdlib since python 3.2,
			# testing its availability
			import argparse

		except ImportError:
			logger.warning("Unable to import argparse "
						   "(parser for command-line options and arguments), "
						   "using default argument values:")

			logger.warning(" - InputFile: stdin")
			self.inputFile = sys.stdin

			logger.warning(" - OutputFile: stdout")
			self.outputFile = sys.stdout

			try:
				self.domainsFile = sys.argv[1]
			except IndexError as ex:
				logger.fatal("No domain file provided (first argument)")
				raise ex
			else:
				logger.warning(" - Domain file: " + self.domainsFile)

			logger.warning(" - Output format: xml")
			self.XMLreport = True

			logger.warning(" - Debug level: error")
			self.debugLevel = logging.INFO
		else :

			myArgParser = argparse.ArgumentParser(description='Generate PFW report')

			myArgParser.add_argument(
						'domainsFile',
						type=argparse.FileType('r'),
						help="the PFW domain XML file"
					)
			myArgParser.add_argument(
						'pfwlog', nargs='?',
						type=argparse.FileType('r'), default=sys.stdin,
						help="the PFW log file, default stdin"
					)
			myArgParser.add_argument(
						'-o', '--output',
						dest="outputFile",
						type=argparse.FileType('w'), default=sys.stdout,
						help="the coverage report output file, default stdout"
					)
			myArgParser.add_argument(
						'-v', '--verbose',
						dest="debugLevel", default=0,
						action='count',
						help="print debug warnings from warning (default) to debug (-vv)"
					)

			outputFormatGroupe = myArgParser.add_mutually_exclusive_group(required=False)

			outputFormatGroupe.add_argument(
						'--xml',
						dest="xmlFlag",
						action='store_true',
						help=" XML coverage output report"
					)
			outputFormatGroupe.add_argument(
						'--raw',
						dest="rawFlag",
						action='store_true',
						help="raw coverage output report"
					)

			# Process command line arguments
			options = myArgParser.parse_args()

			# Mapping to attributes
			self.inputFile = options.pfwlog
			self.outputFile = options.outputFile
			self.domainsFile = options.domainsFile

			# Output report in xml if flag not set
			self.XMLreport = not options.rawFlag

			# Setting logger level
			levelCapped = min(options.debugLevel, len(self.levelTranslate) - 1)
			self.debugLevel = self.levelTranslate[levelCapped]



def main():

	errorDuringLogParsing = -1
	errorDuringArgumentParsing = 1

	try:
		commandLineArguments = ArgumentParser()
	except LookupError as ex:
		logger.error("Error during argument parsing")
		logger.debug(str(ex))
		sys.exit(errorDuringArgumentParsing)

	# Setting logger level
	logger.setLevel(commandLineArguments.debugLevel)
	logger.info("Log level set to: %s" %
			logging.getLevelName(commandLineArguments.debugLevel))

	# Create tree from XML
	dom = xml.dom.minidom.parse(commandLineArguments.domainsFile)

	# Create element tree
	root = Root("Coverage", dom)

	# Parse PFW events
	parser = ParsePFWlog(root.domains, root.criteria)

	try:
		parser.parsePFWlog(commandLineArguments.inputFile.readlines())
	except CustomError as ex:
		logger.fatal("Error during parsing log file %s: %s" %
			(commandLineArguments.inputFile, ex))
		sys.exit(errorDuringLogParsing)

	# Output report
	outputFile = commandLineArguments.outputFile

	if not commandLineArguments.XMLreport :
		outputFile.write("%s\n" % root.dump(withCoverage=True, withNbUse=True))
	else :
		outputFile.write(root.exportToXML().toprettyxml())


# Execute main function if the python interpreter is running this module as the main program
if __name__ == "__main__" :
	main()

