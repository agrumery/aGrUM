#!/usr/bin/python
# -*- coding: utf-8 -*-
# ***************************************************************************
# *   Copyright (C) 2015 by Pierre-Henri WUILLEMIN                          *
# *   {prenom.nom}_at_lip6.fr                                               *
# *                                                                         *
# *   "act" is free software; you can redistribute it and/or modify         *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU General Public License for more details.                          *
# *                                                                         *
# *   You should have received a copy of the GNU General Public License     *
# *   along with this program; if not, write to the                         *
# *   Free Software Foundation, Inc.,                                       *
# *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
# **************************************************************************
from optparse import OptionParser
from sys import platform

from . import const as cfg
from .modules import parseModulesTxt
from .tools import check_tools

try:
  import colorama

  colorama.init()
  cfg.withColour = True
except:
  try:
    import tendo.ansiterm

    cfg.withColour = True
  except:
    cfg.withColour = True
    pass

cfg.__version_major="2"
cfg.__version_minor="2"
cfg.__version = cfg.__version_major+"."+cfg.__version_minor

cfg.modulesFile = "src/modules.txt"  # the file to parse to find the modules
cfg.configFile = ".options.act2.pickle"  #
cfg.fixedSeedValue = "10"

cfg.nbr_tests_for_stats = 20

# for message
cfg.prefixe_line = "-- "
cfg.prefixe_trace = "==> "

cfg.os_platform = platform


def initParams():
  cfg.default = {}
  cfg.default['action'] = "lib"
  cfg.default['targets'] = set(["aGrUM"])
  cfg.default['modules'] = 'ALL'
  cfg.default['mode'] = "release"
  cfg.default['verbose'] = False
  cfg.default['destination'] = "/usr/local"
  cfg.default['jobs'] = "except1"
  cfg.default['static_lib'] = False
  cfg.default['fixed_seed'] = False
  cfg.default['no_fun'] = False
  cfg.default['stats'] = False
  cfg.default['oneByOne'] = False
  cfg.default['tests'] = 'all'
  cfg.default['python'] = "3"
  cfg.default['python3lib'] = ""
  cfg.default['python2lib'] = ""
  cfg.default['python3include'] = ""
  cfg.default['python2include'] = ""
  cfg.default['dry_run'] = False
  cfg.default['coverage'] = False
  cfg.default['withSQL'] = True
  cfg.default['mvsc'] = False
  cfg.default['mvsc32'] = False
  cfg.default['mvsc17'] = False
  cfg.default['mvsc17_32'] = False
  cfg.default['mvsc19'] = False
  cfg.default['mvsc19_32'] = False
  cfg.default['mingw64'] = False
  cfg.default['build'] = "all"
  cfg.default['noSaveParams'] = False
  cfg.default['correction'] = False

  cfg.actions = set("lib test install doc clean show uninstall package guideline wheel nightly_wheel".split())
  cfg.modes = set("debug release".split())
  cfg.targets = set("aGrUM pyAgrum jAgrum".split())
  cfg.moduleLabels = parseModulesTxt()
  cfg.modules = set(cfg.moduleLabels)

  cfg.non_persistent = ["fixed_seed", "stats", "no_fun", "static_lib", "oneByOne", "dry_run", "coverage","noSaveParams","correction"]
  cfg.mains = ["action", "targets", "mode"]
  cfg.specialActions = ["show", "clean", "guideline"]
  cfg.swapOptions = {
    "verbose": {
      True : "verbose",
      False: "quiet"
    },
    "withSQL": {
      True : "withSQL",
      False: "withoutSQL"
    }
  }


def configureOptions(current):
  us = "%prog [options] [" + "|".join(sorted(cfg.actions)) + "] [" + "|".join(cfg.modes) + "] [" + "|".join(
      cfg.targets) + "]"
  cfg.parser = OptionParser(usage=us, description="Compilation tools for aGrUM and wrappers",
                            version="%prog v" + cfg.__version)
  cfg.parser.add_option("", "--no-fun",
                        help="no fancy output parser.",
                        action="store_true",
                        dest="no_fun",
                        default=False)
  cfg.parser.add_option("-v", "--verbose",
                        help="more message on what is happening.",
                        action="store_true",
                        dest="verbose",
                        default=current['verbose'])
  cfg.parser.add_option("-q", "--quiet",
                        help="please be quiet.",
                        action="store_false",
                        dest="verbose",
                        default=current['verbose'])
  cfg.parser.add_option("", "--withSQL",
                        help="connection to SQL datasource via ODBC.",
                        action="store_true",
                        dest="withSQL",
                        default=current['withSQL'])
  cfg.parser.add_option("", "--withoutSQL",
                        help="no connection to SQL datasource via ODBC.",
                        action="store_false",
                        dest="withSQL",
                        default=current['withSQL'])
  cfg.parser.add_option("", "--fixed_seed",
                        help="random seed is fixed once for all. Hence random algorithms should be time-normalized.",
                        action="store_true",
                        dest="fixed_seed",
                        default=False)
  cfg.parser.add_option("", "--stats",
                        help="consolidation on " + str(cfg.nbr_tests_for_stats) + " runs.",
                        action="store_true",
                        dest="stats",
                        default=False)
  cfg.parser.add_option("", "--oneByOne",
                        help="aGrUM debug tests one by one (searching leaks).",
                        action="store_true",
                        dest="oneByOne",
                        default=False)
  cfg.parser.add_option("-d", "--destination",
                        help="destination folder when installing.",
                        metavar="FOLDER",
                        dest="destination",
                        default=current['destination'])
  cfg.parser.add_option("-j", "--jobs",
                        help="number of jobs : {half|halfexcept1|all|except1|1|2|...}.",
                        type='string',
                        dest="jobs",
                        default=current['jobs'])
  cfg.parser.add_option("-t", "--tests",
                        help="tests management : {show|all|test1+test2+test3}.",
                        metavar="TESTS-COMMAND",
                        dest="tests",
                        default=current['tests'])
  cfg.parser.add_option("-m", "--modules",
                        help="module management : {show|all|module1+module2+module3}.",
                        metavar="MODULES-COMMAND",
                        dest="modules",
                        default=current['modules'])
  cfg.parser.add_option("", "--static_lib",
                        help="build static library.",
                        action="store_true",
                        dest="static_lib",
                        default=False)
  cfg.parser.add_option("", "--python",
                        help="defines against wich version of python to build: {2|3}.",
                        type="choice",
                        choices=["2", "3"],
                        dest="python",
                        default="3")
  cfg.parser.add_option("", "--python2lib",
                        help="root folder for lib python2.",
                        metavar="FOLDER",
                        dest="python2lib",
                        default=current['python2lib'])
  cfg.parser.add_option("", "--python3lib",
                        help="root folder for lib python3.",
                        metavar="FOLDER",
                        dest="python3lib",
                        default=current['python3lib'])
  cfg.parser.add_option("", "--python2include",
                        help="root folder for include python2.",
                        metavar="FOLDER",
                        dest="python2include",
                        default=current['python2include'])
  cfg.parser.add_option("", "--python3include",
                        help="root folder for include python3.",
                        metavar="FOLDER",
                        dest="python3include",
                        default=current['python3include'])
  cfg.parser.add_option("", "--dry-run",
                        help="dry run and prints cmake invocation with the current options.",
                        action="store_true",
                        dest="dry_run",
                        default=False)
  cfg.parser.add_option("", "--coverage",
                        help="build with code coverage options enable (debug only).",
                        action="store_true",
                        dest="coverage",
                        default=False)
  cfg.parser.add_option("", "--mvsc",
                        help="use Microsoft Visual Studio15 C++ compiler 64bits (Windows only).",
                        action="store_true",
                        dest="mvsc",
                        default=current['mvsc'])
  cfg.parser.add_option("", "--mvsc32",
                        help="use Microsoft Visual Studio15 C++ compiler 32bits (Windows only).",
                        action="store_true",
                        dest="mvsc32",
                        default=current['mvsc32'])
  cfg.parser.add_option("", "--mvsc17",
                        help="use Microsoft Visual Studio17 C++ compiler 64bits (Windows only).",
                        action="store_true",
                        dest="mvsc17",
                        default=current['mvsc17'])
  cfg.parser.add_option("", "--mvsc17_32",
                        help="use Microsoft Visual Studio17 C++ compiler 32bits (Windows only).",
                        action="store_true",
                        dest="mvsc17_32",
                        default=current['mvsc17_32'])
  cfg.parser.add_option("", "--mvsc19",
                        help="use Microsoft Visual Studio19 C++ compiler 64bits (Windows only).",
                        action="store_true",
                        dest="mvsc19",
                        default=current['mvsc19'])
  cfg.parser.add_option("", "--mvsc19_32",
                        help="use Microsoft Visual Studio19 C++ compiler 32bits (Windows only).",
                        action="store_true",
                        dest="mvsc19_32",
                        default=current['mvsc19_32'])
  cfg.parser.add_option("", "--mingw64",
                        help="use minGW64 C++ Compiler (Windows only).",
                        action="store_true",
                        dest="mingw64",
                        default=current['mingw64'])
  cfg.parser.add_option("", "--build",
                        help="build options : {all|no-cmake|no-make|doc-only}.",
                        type="choice",
                        choices=["all", "no-cmake", "no-make","doc-only"],
                        dest="build",
                        default="all")
  cfg.parser.add_option("", "--no-saveParams",
                        help="act will not save as default the parameters of this invocation.",
                        action="store_true",
                        dest="noSaveParams",
                        default=False)
  cfg.parser.add_option("", "--correction",
                        help="act guideline will change the files instead of only show them",
                        action="store_true",
                        dest="correction",
                        default=False)


def configureColors(no_fun=False):
  if no_fun or not cfg.withColour:
    cfg.C_VALUE = ''
    cfg.C_WARNING = ''
    cfg.C_ERROR = ''
    cfg.C_END = ''
    cfg.C_MSG = ''
  else:
    cfg.C_VALUE = '\033[1m\033[32m'
    cfg.C_WARNING = '\033[1m\033[33m'
    cfg.C_ERROR = '\033[1m\033[31m'
    cfg.C_END = '\033[0m'
    cfg.C_MSG = '\033[1m\033[34m'


def configureOutputs(options):
  cfg.verbosity = options.verbose
  configureColors(options.no_fun)


def configureTools(options):
  (cfg.python, cfg.cmake, cfg.make, cfg.clangformat, cfg.msbuild) = check_tools(options)
