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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2025                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
Specification of options and default values for the configuration of act
"""

import sys
import os
import re
from argparse import ArgumentParser
from subprocess import PIPE, Popen
from typing import Any, Optional

# import colorama
# colorama.init()


class _const:
  class ConstError(TypeError):
    pass

  def __setattr__(self, name, value):
    if name in self.__dict__:
      raise self.ConstError(f"Can't rebind const({name})={self.__dict__[name]} to {value}")
    self.__dict__[name] = value

  def __str__(self) -> str:
    # we do not show the colors
    d: dict[str, Any] = self.__dict__
    l: list[str] = [
      f"{self.C_MSG}{k}{self.C_END} : {self.C_VALUE}{d[k] if not k.startswith('C_') else '(escape sequence)'}{self.C_END}"
      for k in sorted(d.keys())
    ]
    return "\n".join(l)

  def __repr__(self):
    d: dict[str, Any] = self.__dict__
    l: list[str] = [f"{k} : {d[k] if not k.startswith('C_') else '(escape sequence)'}" for k in sorted(d.keys())]
    return "\n".join(l)


cfg = _const()


def config_critic(message: str) -> None:
  """
  Print a critical message and exit the program.

  Parameters
  ----------
  message: str
      The message to print.
  """
  print(f"Critical error during configuration : {message}")
  sys.exit(1)


def cmdline(command: str) -> str:
  process = Popen(args=command, stdout=PIPE, shell=True)
  return process.communicate()[0].decode()


def is_tool(prog: str, longpath=False) -> Optional[str]:
  progw = prog + ".exe"
  for dirname in os.environ["PATH"].split(os.pathsep):
    if os.path.exists(os.path.join(dirname, prog)):
      return prog if not longpath else '"' + os.path.join(dirname, prog) + '"'
    if os.path.exists(os.path.join(dirname, progw)):
      return progw if not longpath else '"' + os.path.join(dirname, progw) + '"'
  return None


def check_tools() -> tuple[str, str, str, str, str, str, str]:
  exe_py = f'"{sys.executable}"'
  res = cmdline(exe_py + ' -c "import platform;print(platform.python_version())"')
  version, subversion, patch = res.strip().split(".")

  if int(version) < 3 or int(subversion) < 10:
    config_critic("python<3.10 is not supported anymore.")
  version_py = f"{version}.{subversion}"

  exe_cmake = is_tool("cmake")
  if exe_cmake is None:
    config_critic("No <cmake> utility found. Exit")

  exe_make = is_tool("mingw32-make.exe")
  if exe_make is None:
    exe_make = is_tool("make")
  if exe_make is None:
    exe_make = is_tool("msbuild")
  if exe_make is None:
    config_critic("No <make> utility found. Exit")

  exe_clangformat = None
  if is_tool("clang-format"):
    exe_clangformat = "clang-format"
  if exe_clangformat is None:
    for version in ["", "-13.0", "-12.0", "-11.0"]:
      if is_tool(f"clang-format{version}"):
        exe_clangformat = f"clang-format{version}"
        break

  exe_msbuild = is_tool("msbuild")
  exe_ruff = is_tool("ruff")

  return exe_py, version_py, exe_cmake, exe_make, exe_clangformat, exe_msbuild, exe_ruff


def parse_modules_txt() -> dict[str, str]:
  modules = {}
  module_line = re.compile(r"^\s*list\s*\(\s*APPEND\s*MODULES\s*\"(.*)\"\s*\)(\s*#\s*(.*))?")
  with open(cfg.modulesFile, "r", encoding="utf8") as f:
    for ll in f:
      rep = module_line.search(ll)
      if rep:
        module = rep.groups(0)[0]
        descr = rep.groups(0)[2]
        if descr == 0:
          descr = module
        modules[module] = descr
  return modules


def parse_py_modules() -> set[str]:
  """
  Parse the pyagrum modules from the pymodulesPath directory.
  Returns a set of module names.
  """
  import os

  for root, dirs, files in os.walk(cfg.pymodulesPath):
    return {d for d in dirs if d != "lib"}
  return {}  # Return an empty set if no modules found


cfg.withColour = True

cfg.act_version_major = "3"
cfg.act_version_minor = "0"
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
cfg.pymodulesPath = "wrappers/pyagrum/pyLibs"  # the path to the python modules
cfg.configFile = ".options.act3.pickle"  #
cfg.fixedSeedValue = "10"

cfg.nbr_tests_for_stats = 100

cfg.minimal_python_api = "cp310"

# for message
cfg.prefix_line = "🍋  "
cfg.prefix_trace = "🍋 💥"

cfg.os_platform = sys.platform


def init_params() -> None:
  """
  Initialize the default parameters for act.
  This function sets up the default configuration values, actions, modes, targets, modules,
  and other options used by act and stores it in the kind of singleton `cfg` (configuration) module.
  """
  cfg.default = {
    "action": "lib",
    "target": "aGrUM",
    "modules": "all",
    "mode": "Release",
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
    "withSQL": False,
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

  cfg.actions = set("lib test install doc clean purge show uninstall package guideline wheel nightly_wheel".split())
  cfg.modes = set("debug release".split())
  cfg.targets = {"agrum", "pyagrum"}  # lowercase for relaxing constraints
  cfg.modules = parse_modules_txt()
  cfg.pymodules = parse_py_modules()

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
  cfg.mains = ["action", "target", "mode"]
  cfg.specialActions = ["show", "clean", "purge", "guideline"]
  cfg.swapOptions = {
    "verbose": {True: "verbose", False: "quiet"},
    "withSQL": {True: "withSQL", False: "withoutSQL"},
  }
  cfg.buildPath = {"Release": "release", "Debug": "debug"}

  (cfg.python, cfg.python_version, cfg.cmake, cfg.make, cfg.clangformat, cfg.msbuild, cfg.ruff) = check_tools()


def configure_cli_options(current: dict[str, str | bool]) -> None:
  """
  Configure the command line options for act.

  Parameters
  ----------
  current: dict[str, str | bool]
    The current configuration dictionary that will be updated with the parsed options (or used by default).
  """
  cfg.parser = ArgumentParser(
    prog="act",
    description="Compilation tools for aGrUM and wrappers",
    epilog="[%(prog)s v" + cfg.act_version + "]",
  )
  cfg.parser.add_argument(
    "cmds",
    nargs="*",
    default="",
    help=f"Specify among the action ({'|'.join(sorted(cfg.actions))}), the target (in [{','.join(cfg.targets)}]) and the mode ({'|'.join(sorted(cfg.modes))}).",
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


def configure_colors(no_fun: bool = False):
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
