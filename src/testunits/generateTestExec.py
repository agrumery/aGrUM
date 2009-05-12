#!/usr/bin/python
# -*- encoding: UTF-8 -*-

################################################################################
#    Copyright (C) 2007 by Lionel Torti                                        #
#    lionel.torti@lip6.fr                                                      #
#                                                                              #
#    This program is free software; you can redistribute it and/or modify      #
#    it under the terms of the GNU General Public License as published by      #
#    the Free Software Foundation; either version 2 of the License, or         #
#    (at your option) any later version.                                       #
#                                                                              #
#    This program is distributed in the hope that it will be useful,           #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of            #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
#    GNU General Public License for more details.                              #
#                                                                              #
#    You should have received a copy of the GNU General Public License         #
#    along with this program; if not, write to the                             #
#    Free Software Foundation, Inc.,                                           #
#    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                 #
################################################################################

import os, re, sys

################################################################################
# Change the folowing globals to fit your system.                              #
# Use Absolute paths only.                                                     #
#                                                                              #
# Path to agrum                                                                #
AGRUM=".."                                                                     #
# Path to agrum libs                                                           #
AGRUM_LIB=AGRUM + "/agrum/.libs"                                               #
# Path to the .h of agrum                                                      #
AGRUM_INCLUDE="-I " + AGRUM + "/agrum -I " + AGRUM                             #
# Path to the .so of cxxtest                                                   #
CXXTEST_INCLUDE="./cxxtest"                                                    #
# Path or command to execute the cxxtest script from a Makefile                #
CXXTEST_SCRIPT="./cxxtest/cxxtestgen.py"                                       #
# The copmiler to use                                                          #
GCC="g++ -DNDEBUG"                                                       #
################################################################################

# Matcher for header files.
pattern = r"[_a-zA-Z0-9][_a-zA-Z0-9]*\.h$"
matcher = re.compile(pattern)

# Globals
_DEFAULT_HEADERS = list()           # the headers list for the default test
_HEADERS = list()                   # the headers list not in default test
_DEFAULT_TESTS   = list()           # the default test 
_TESTS = list()                     # the default test

################################################################################
#                       Type safety decorators                                 #
################################################################################
def argument_files(func):
    """Decorator for checking if first argument of func is a file."""
    def capsule(arg1):
        if not isinstance(arg1, file):
            raise TypeError("'%s' is not a file" % arg1)
        return func(arg1)
    return capsule

def argument_string(func):
    """Test if the argument of func is a string"""
    def capsule(arg):
        if not isinstance(arg, str):
            raise TypeError("'%s' is not a string" % str(arg))
        return func(arg)
    return capsule

def argument_stringlist(func):
    """Checks if the first argument of func is a string list."""
    def capsule(arg):
        if isinstance(arg, list):
            for elt in arg:
                if not isinstance(elt, str):
                    raise TypeError("'%s' is not a list of strings" % str(arg))
        return func(arg)
    return capsule

################################################################################
#                    Makefile generating functions                             #
################################################################################
def makefileHeader():
    """ Returns a string containing the header of the makefile to generate."""
    global _HEADERS, _TESTS, _DEFAULT_HEADERS, _DEFAULT_TESTS
    global CXXTEST_SCRIPT, AGRUM_LIB
    str = "AGRUM_LIB=" + AGRUM_LIB + "\n"
    str += "AGRUM_INCLUDE=" + AGRUM_INCLUDE + "\n"
    str += "CXXTEST_INCLUDE=" + CXXTEST_INCLUDE + "\n"
    str += "CXXTEST_SCRIPT=" + CXXTEST_SCRIPT + "\n"
    str += "HEADERS="
    tmp = ""
    for header in _HEADERS:
        str += header + " "
        n = len(header)
        tmp += header[:n-2] + " "
    str += "\n"
    str += "TESTS=" + tmp + "\n"
    str += "DEFAULT_HEADERS="
    for header in _DEFAULT_HEADERS:
        str += header + " "
    str += "\n"
    str += "GCC=" + GCC + "\n"
    str += "\n"
    
    str += ".PHONY = all clean runTest runAllTest $(TESTS)\n"
    str += "\n"
    str += "all: runTest\n"
    str += "\n"
    str += runAllTestRules()
    str += "\n"
    str += runTestRules()
    str += "\n"
    str += cleanRule()
    str += "\n"
    str += agrumIsOkRule()
    str += "\n"
    str += checkIncludesDir()
    str += "\n"
    return str

def runAllTestRules():
    """ Returns the Makefile rules to generate and run all test."""
    str = "runAllTest: agrumIsOK .runAllTest.cxx\n"
    str += generateLdLibraryPath("./.runAllTest.cxx")
    str += "\n"
    str += ".runAllTest.cxx: .runAllTest.cpp " + retrieveAllDependence() + "\n"
    str += "\t$(GCC) $(AGRUM_INCLUDE) -I $(CXXTEST_INCLUDE) -L$(AGRUM_LIB) -lagrum -o $@ $^\n"
    str += "\n"
    str += ".runAllTest.cpp: $(DEFAULT_HEADERS) $(HEADERS) $(AGRUM_LIB)/libagrum.so \n"
    str += "\t$(CXXTEST_SCRIPT) -o $@ --runner=AgrumErrorPrinter $^\n"
    str += "\n"
    return str

def runTestRules():
    """ Returns the Makefile rules to generate and run default test."""
    str = "runTest: agrumIsOK .runTest.cxx\n"
    str += generateLdLibraryPath("./.runTest.cxx")
    str += "\n"
    str += ".runTest.cxx: .runTest.cpp " + retrieveDefaultDependence() + "\n"
    str += "\t$(GCC) $(AGRUM_INCLUDE) -I $(CXXTEST_INCLUDE) -L$(AGRUM_LIB) -lagrum -o $@ $^\n"
    str += "\n"
    str += ".runTest.cpp: $(DEFAULT_HEADERS) $(AGRUM_LIB)/libagrum.so \n"
    str += "\t$(CXXTEST_SCRIPT) -o $@ --runner=AgrumErrorPrinter $^\n"
    str += "\n"
    return str

def cleanRule():
    """ Returns the Makefil rule to clean the test workspace."""
    str = "clean:\n"
    str += "\trm -f " + r"`ls -a | egrep '^\..*\.cxx'` "
    str += r"`ls -a | egrep '^\..*\.cpp'` .runAllTest.* .runTest.*" + "\n"
    str += "\t(cd classes; make clean)\n"
    str += "\n"
    return str

def agrumIsOkRule():
    """force agrum to be compiled before testing"""
    str ="agrumIsOK:\n"
    str +="\t@echo -n \"Checking agrum : \"\n"
    str +="\t@(cd " + AGRUM + ";make > /dev/null)\n"
    str +="\t@echo \"OK\"\n"
    return str

@argument_string
def generateLdLibraryPath(exeFile):
    """modify the LD_LIBRARY_PATH env. var"""
    return "\t@( export LD_LIBRARY_PATH=$(AGRUM_LIB):$(LD_LIBRARY_PATH) ; "+exeFile+" ; export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH))\n"

def checkIncludesDir():
    """Checks if the include dir exists, and create a rule to recusively call
    classes/make."""
    if (os.access("classes", os.F_OK)):
        str = "checkIncludesDir:\n"
        str = "\t(cd classes; make > /dev/null)\n"
        return str
    else:
        return ""

@argument_string
def makefileOutput(header):
    """ Returns a string containing the necessary Makefile rules to generate a 
    test with cxxtest.
    """
    dependence = retrieveDependence(header)

    n = len(header)
    basename = header[:n-2]
    
    str = basename + ":" + "agrumIsOK ." + basename + ".cxx\n"
    str += generateLdLibraryPath("./." + basename + ".cxx")
    str += "\n"
    str += "." + basename + ".cxx: " + "." + basename + ".cpp $(AGRUM_LIB)/libagrum.so " + dependence + "\n"
    str += "\t(cd include;make " + dependence + ")\n"
    str += "\t" + "$(GCC) $(AGRUM_INCLUDE) -I $(CXXTEST_INCLUDE) -L$(AGRUM_LIB) -lagrum -o $@ $^ " + dependence + "\n"
    str += "\n"
    str += "." + basename + ".cpp" + ": " + header + " \n"
    str += "\t" + "$(CXXTEST_SCRIPT) -o $@ --error-printer $^\n"
    str += "\n"
    return str

@argument_files
def dumpMakefile(makefile):
    """Dumps the Makefile in makefile with the necessary rules to generate and
    do test with cxxtest for each C++ header in headers.

    It is supposed that the read / write access have been checked.
    We suppose that the headers have been checked previously.
    """
    global _HEADERS, _TESTS, _DEFAULT_HEADERS, _DEFAULT_TESTS
    try:
        makefile.write(makefileHeader())
        makefile.write("\n")
    except Exception, e:
        print "Read / write error occured: %s" % e
        
    headers = list()
    if _HEADERS == _DEFAULT_HEADERS:
        headers.extend(_HEADERS)
    else:
        headers.extend(_HEADERS)
        headers.extend(_DEFAULT_HEADERS)

    for header in headers:
        try:
            makefile.write(makefileOutput(header))
        except Exception, e:
            print "Read / write error occured: %s" % e

    makefile.write("\n")
    makefile.flush()

################################################################################
#                    Configuration checking functions                          #
################################################################################
def checkHeaders():
    """ Check for the headers in the current directory, and initialize global
    variables correctly.
    
    @param headers: the headers used for the default test.
    """
    global _HEADERS, _TESTS, _DEFAULT_HEADERS, _DEFAULT_TESTS
    filesList = os.listdir(".")
    for file in filesList:
        if matcher.match(file):
            _HEADERS.append(file)
            n = len(file) - 2
            _TESTS.append(file[:n])

    # To check if the arguments are real header filenames.
    headersCount = len(_HEADERS)
    
    if len(_DEFAULT_HEADERS) == 0:
    # Then the default test test all headers
        _DEFAULT_HEADERS = _HEADERS
        _DEFAULT_TESTS = _TESTS
    else:
    # Remove default headers from _HEADERS and _TESTS and add them to
    # _DEFAULT_HEADERS and _DEFAULT_TESTS
        for header in _DEFAULT_HEADERS:
            _HEADERS.remove(header)
            n = len(header) - 2
            _TESTS.remove(header[:n])

    currentHeadersCount = (len(_HEADERS) + len(_DEFAULT_HEADERS))

    # Checking for wrong arguments
    if (headersCount != currentHeadersCount and 
       headersCount != (currentHeadersCount / 2)):
       raise ValueError("No corresponding file for default headers: %s" % _HEADERS)

def printHelp():
    """Prints the usage of this script."""
    scriptName = sys.argv[0]
    if scriptName.startswith("./"):
        scriptName = scriptName[2:]

    str = "Usage: " + scriptName + " [OPTION]... [HEADERS]...\n"
    str += "Generates a Makefile which helps building and running tests with\n"
    str += "CxxTest.\n"
    str += "\n"
    str += "The options are:\n"
    str += "--cxx=[PATH]\t\tPath to the CxxTest script to use\n"
    str += "--agrum=[PATH]\t\tPath to the agrum library\n"
    str += "-h, --help\t\tDisplay this information\n"
    str += "\n"
    str += "The HEADERS option is used for the default test, wich can be build and run\n"
    str += "by running 'make' or 'make runTest'.\n"
    str += "'make runAllTest' builds and run all tests found in the current directory.\n"
    str += "Each test can be run separetly calling 'make BASENAME', where BASENAME stands \n"
    str += "for the basename of the correponding test header.\n"
    str += "If no HEADERS is given, then all headers in the current directory are use for\n"
    str += "the default test.\n"
    str += "\n"
    str += "Temporary files are hidden, to clean your workspace you can call 'make clean'.\n"
    str += "\n"
    str += "Report bugs to lionel.torti@lip6.fr.\n"
    print str

def checkArgs():
    """Checks the arguments and do corresponding configuration."""
    global _HEADERS, _TESTS, _DEFAULT_HEADERS, _DEFAULT_TESTS
    global CXXTEST_SCRIPT, AGRUM_LIB

    for arg in sys.argv[1:]:
        if matcher.match(arg):
            _DEFAULT_HEADERS.append(arg)
        elif arg == "--help" or arg == "-h":
            printHelp()
            exit(0)
        elif arg.startswith("--cxx="):
            CXXTEST_SCRIPT=arg[6:]
        elif arg.startswith("--agrum="):
            AGRUM_LIB=arg[8:]
        else:
            raise ValueError("%s is not a valid header filename" % arg)

def checkExistence(path, mode):
    """Returns True if path is a existing file or directory, return False
    otherwise."""
    if path.startswith("~"):
        msg = "No support for paths starting with ~, please use"
        msg += " absolute or relative paths.\n"
        raise ValueError(msg)

    if os.access(path, mode):
        return True
    else:
        return False

def checkConf():
    """Checks if the global configuration variables are correct."""
    if not checkExistence(CXXTEST_SCRIPT, os.X_OK):
        raise ValueError("Invalid path to CxxTest script: %s" % CXXTEST_SCRIPT)
    if not checkExistence(AGRUM + "/agrum", os.R_OK):
        raise ValueError("Invalid path to agrum's library: %s" % AGRUM_LIB)

################################################################################
#                               Includes handling                              #
################################################################################

@argument_string
def getObjectFileName(include):
    str     = include.strip()
    start   = len('#include "')
    end     = len(str) - len('.h"')

    return str[start:end] + ".o"

@argument_string
def checkBeginClassDef(line):
    """Checks if the line is the begining of the definition of the class test
    suite.

    A class test suite is defined by a line with the contents 
    "class .*: public CxxTest::Testsuite"."""
    pattern = '^\s*class\s*\w*\s*:\s*public\s*CxxTest::TestSuite\s*$'

    if (re.match(pattern, line) == None):
        return False
    else:
        return True

@argument_string
def retrieveDependence(headerName):
    """Retrieves the files used in the classes directory by the test suite
    pointed by headerName.

    Returns a string containing a list of the .o files used by the test
    suite pointed by headerName."""

    fileList = ""
    if (checkExistence(headerName, os.R_OK)):
        headerFile = file(headerName)
        for line in headerFile:
            if (line.startswith('#include "classes/')):
                fileList += getObjectFileName(line) + " "
            elif (checkBeginClassDef(line)):
                break

    return fileList.strip()

def retrieveAllDependence():
    """Retrieves all the files used by all the suite tests."""
    global _DEFAULT_HEADERS, _HEADERS

    fileList = []
    for filename in _DEFAULT_HEADERS:
        for object in retrieveDependence(filename).split(' '):
            if (fileList.count(object) == 0):
                fileList.append(object)

    for filename in _HEADERS:
        for object in retrieveDependence(filename).split(' '):
            if (fileList.count(object) == 0):
                fileList.append(object)

    finalList = ""
    for filename in fileList:
        finalList += filename + " "
    
    return finalList.strip()

def retrieveDefaultDependence():
    """Retrieves the files used by the default suite test. _DEFAULT_HEADERS is
    empty, then return the result of retrieveAllDependence."""
    global _DEFAULT_HEADERS

    if len(_DEFAULT_HEADERS) == 0:
        return retrieveAllDependence()

    fileList = []
    for filename in _DEFAULT_HEADERS:
        for object in retrieveDependence(filename).split(' '):
            if (fileList.count(object) == 0):
                fileList.append(object)

    finalList = ""
    for filename in fileList:
        finalList += filename + " "
    
    return finalList.strip()
        
################################################################################
#                               Script execution                               #
################################################################################

checkArgs()
checkConf()
checkHeaders()
makefile = file(os.getcwd() + "/Makefile", 'w')
dumpMakefile(makefile)
print "Makefile generated."

################################################################################
#                           Tests functions                                    #
################################################################################

def testMakefilOutput():
    s = ""
    for i in range(80):
        s += "#"
    print s
    print "Testing makefileOutput"
    try:
        print makefileOutput("MyHeader.h")
        print "Test 1: succes"
    except Exception, e:
        print "Test 1: failure  - %s" % e

    try:
        print makefileOutput(123456)
        print "Test 2: failure"
    except:
        print "Test 2: succes"
    s = ""
    for i in range(80):
        s += "#"
    print s

def testGetObjectFile():
    print getObjectFileName('#include "plop.h"')
    print checkBeginClassDef('#incklude aplzpeale')
    print checkBeginClassDef(" class Plop : public CxxTest::TestSuite  ")
    print retrieveDependence("MixedGraphTestSuite.h")

def testDependeceretrieval():
    print retrieveAllDependence()
    print retrieveDefaultDependence()


################################################################################
#                        Script execution in test mode                         #
################################################################################

#testDependeceretrieval()
