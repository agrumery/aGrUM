############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
Specification of options and default values for the configuration of act
"""

from argparse import ArgumentParser
import sys

from . import const as cfg
from .modules import parseModulesTxt
from .tools import check_tools

try:
  import colorama

  colorama.init()
  cfg.withColour = True
except ModuleNotFoundError:
  try:
    import tendo.ansiterm

    cfg.withColour = True
  except ModuleNotFoundError:
    cfg.withColour = True

cfg.act_version_major = "2"
cfg.act_version_minor = "4"
cfg.act_version = cfg.act_version_major + "." + cfg.act_version_minor

res = dict()
with open("VERSION.txt", "r") as version_file:
  for line in version_file:
    if line.startswith("#"):
      continue
    t = line.split('"')
    if len(t) == 3:
      res[t[0][4:].strip()] = t[1].strip()

cfg.gum_version_major = res["AGRUM_VERSION_MAJOR"]
cfg.gum_version_minor = res["AGRUM_VERSION_MINOR"]
cfg.gum_version_patch = res["AGRUM_VERSION_PATCH"]

cfg.gum_version = f"{cfg.gum_version_major}.{cfg.gum_version_minor}.{cfg.gum_version_patch}"

cfg.modulesFile = "src/modules.txt"  # the file to parse to find the modules
cfg.configFile = ".options.act2.pickle"  #
cfg.fixedSeedValue = "10"

cfg.nbr_tests_for_stats = 100

cfg.minimal_python_api = "cp310"

# for message
cfg.prefix_line = "-- "
cfg.prefix_trace = "==> "

cfg.os_platform = sys.platform


def initParams():
  cfg.default = {
    "action": "lib",
    "targets": {"aGrUM"},
    "modules": "ALL",
    "mode": "release",
    "verbose": False,
    "destination": "/usr/local",
    "jobs": "except1",
    "static_lib": False,
    "fixed_seed": False,
    "no_fun": False,
    "stats": False,
    "oneByOne": False,
    "tests": "all",
    "python3target": sys.executable,
    "clangpath": "",
    "gccpath": "",
    "threads": "omp",
    "dry_run": False,
    "coverage": False,
    "withSQL": True,
    "build": "all",
    "noSaveParams": False,
    "correction": False,
    "build_graph": False,
    "profiling": False,
    "stable_abi_off": False,
    "no_gil": False,  # used to disable the GIL in the python wrapper
  }

  if cfg.os_platform == "Windows":
    cfg.default["compiler"] = "mvsc22"
  else:
    cfg.default["compiler"] = "gcc"

  cfg.actions = set("lib test install doc clean show uninstall package guideline wheel nightly_wheel".split())
  cfg.modes = set("debug release".split())
  cfg.targets = set("aGrUM pyAgrum jAgrum".split())
  cfg.moduleLabels = parseModulesTxt()
  cfg.modules = set(cfg.moduleLabels)

  cfg.non_persistent = [
    "fixed_seed",
    "stats",
    "no_fun",
    "static_lib",
    "oneByOne",
    "dry_run",
    "coverage",
    "noSaveParams",
    "correction",
    "build_graph",
  ]
  cfg.mains = ["action", "targets", "mode"]
  cfg.specialActions = ["show", "clean", "guideline"]
  cfg.swapOptions = {
    "verbose": {True: "verbose", False: "quiet"},
    "withSQL": {True: "withSQL", False: "withoutSQL"},
  }


def configureOptions(current: dict[str, str]):
  cfg.parser = ArgumentParser(
    prog="act",
    description="Compilation tools for aGrUM and wrappers",
    epilog="[%(prog)s v" + cfg.act_version + "]",
  )
  cfg.parser.add_argument(
    "cmds",
    nargs="*",
    default="",
    help=f"Specify among the action ({'|'.join(sorted(cfg.actions))}), the targets (in [{','.join(cfg.targets)}]) and the mode ({'|'.join(sorted(cfg.modes))}).",
  )
  cfg.parser.add_argument(
    "--no-fun",
    help="no fancy output parser.",
    action="store_true",
    dest="no_fun",
    default=False,
  )
  cfg.parser.add_argument(
    "-v",
    "--verbose",
    help="more message on what is happening.",
    action="store_true",
    dest="verbose",
    default=current["verbose"],
  )
  cfg.parser.add_argument(
    "-q",
    "--quiet",
    help="please be quiet.",
    action="store_false",
    dest="verbose",
    default=current["verbose"],
  )
  cfg.parser.add_argument(
    "--withSQL",
    help="connection to SQL datasource via ODBC.",
    action="store_true",
    dest="withSQL",
    default=current["withSQL"],
  )
  cfg.parser.add_argument(
    "--withoutSQL",
    help="no connection to SQL datasource via ODBC.",
    action="store_false",
    dest="withSQL",
    default=current["withSQL"],
  )
  cfg.parser.add_argument(
    "--fixed_seed",
    help="random seed is fixed once for all. Hence random algorithms should be time-normalized.",
    action="store_true",
    dest="fixed_seed",
    default=False,
  )
  cfg.parser.add_argument(
    "--stats",
    help="consolidation on " + str(cfg.nbr_tests_for_stats) + " runs.",
    action="store_true",
    dest="stats",
    default=False,
  )
  cfg.parser.add_argument(
    "--oneByOne",
    help="aGrUM debug tests one by one (searching leaks).",
    action="store_true",
    dest="oneByOne",
    default=False,
  )
  cfg.parser.add_argument(
    "-d",
    "--destination",
    help="destination folder when installing.",
    metavar="FOLDER",
    dest="destination",
    default=current["destination"],
  )
  cfg.parser.add_argument(
    "-j",
    "--jobs",
    help="number of jobs : {half|halfexcept1|all|except1|1|2|...}.",
    dest="jobs",
    default=current["jobs"],
  )
  cfg.parser.add_argument(
    "-t",
    "--tests",
    help="tests management : {show|all|test1+test2+test3}.",
    metavar="TESTS-COMMAND",
    dest="tests",
    default=current["tests"],
  )
  cfg.parser.add_argument(
    "-m",
    "--modules",
    help="module management : {show|all|module1+module2+module3}.",
    metavar="MODULES-COMMAND",
    dest="modules",
    default=current["modules"],
  )
  cfg.parser.add_argument(
    "--static_lib",
    help="build static library.",
    action="store_true",
    dest="static_lib",
    default=False,
  )
  cfg.parser.add_argument(
    "--python3target",
    help="python3 target.",
    metavar="PATH",
    dest="python3target",
    default=current["python3target"],
  )
  cfg.parser.add_argument(
    "--clangpath",
    help="path for clang.",
    metavar="FOLDER",
    dest="clangpath",
    default=current["clangpath"],
  )
  cfg.parser.add_argument(
    "--gccpath",
    help="path for gcc.",
    metavar="FOLDER",
    dest="gccpath",
    default=current["gccpath"],
  )
  cfg.parser.add_argument(
    "-tt",
    "--threads",
    help="defines the preferred kind of threads used by aGrUM: {omp|stl}.",
    choices=["omp", "stl"],
    dest="threads",
    default="omp",
  )
  cfg.parser.add_argument(
    "--dry-run",
    help="dry run and prints cmake invocation with the current options.",
    action="store_true",
    dest="dry_run",
    default=False,
  )
  cfg.parser.add_argument(
    "--coverage",
    help="build with code coverage options enable (debug only).",
    action="store_true",
    dest="coverage",
    default=False,
  )
  cfg.parser.add_argument(
    "--compiler",
    help="compilers : {gcc|clang|mvsc22|mvsc22_32|mvsc19|mvsc19_32|mvsc17|mvsc17_32|mvsc15|mvsc15_32|mingw64}.",
    choices=[
      "gcc",
      "clang",
      "mvsc22",
      "mvsc22_32",
      "mvsc19",
      "mvsc19_32",
      "mvsc17",
      "mvsc17_32",
      "mvsc15",
      "mvsc15_32",
      "mingw64",
    ],
    dest="compiler",
    default=current["compiler"],
  )
  cfg.parser.add_argument(
    "--build",
    help="build options : {all|no-cmake|no-make|doc-only}.",
    choices=["all", "no-cmake", "no-make", "doc-only"],
    dest="build",
    default="all",
  )
  cfg.parser.add_argument(
    "--no-saveParams",
    help="act will not save as default the parameters of this invocation.",
    action="store_true",
    dest="noSaveParams",
    default=False,
  )
  cfg.parser.add_argument(
    "--correction",
    help="act guideline will change the files instead of only show them",
    action="store_true",
    dest="correction",
    default=False,
  )
  cfg.parser.add_argument(
    "--build_graph",
    help="act guideline will build the dependency graph",
    action="store_true",
    dest="build_graph",
    default=False,
  )
  cfg.parser.add_argument(
    "--profiling",
    help="act will add options for profiling",
    action="store_true",
    dest="profiling",
    default=False,
  )
  cfg.parser.add_argument(
    "--stable_abi_off",
    help="build wheel without stable abi",
    action="store_true",
    dest="stable_abi_off",
    default=False,
  )
  cfg.parser.add_argument(
    "--no_gil",
    help="build with no gil python (experimental)",
    action="store_true",
    dest="no_gil",
    default=False,
  )


def configureColors(no_fun: bool = False):
  if no_fun or not cfg.withColour:
    cfg.C_VALUE = ""
    cfg.C_WARNING = ""
    cfg.C_ERROR = ""
    cfg.C_END = ""
    cfg.C_MSG = ""
  else:
    cfg.C_VALUE = "\033[1m\033[32m"
    cfg.C_WARNING = "\033[1m\033[33m"
    cfg.C_ERROR = "\033[1m\033[31m"
    cfg.C_END = "\033[0m"
    cfg.C_MSG = "\033[1m\033[34m"


def configureOutputs(options):
  cfg.verbosity = options.verbose
  configureColors(options.no_fun)


def configureTools():
  (cfg.python, cfg.cmake, cfg.make, cfg.clangformat, cfg.msbuild, cfg.ruff) = check_tools()
