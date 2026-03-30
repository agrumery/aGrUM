############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
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
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import argparse
import os
import platform
import sys
from pathlib import Path
from sys import platform as os_platform

import logging

_thisdir = os.path.dirname(os.path.abspath(__file__))
if _thisdir not in sys.path:
  sys.path.insert(0, _thisdir)
from utils.gumTestOutput import notif, warn, error

# List of modules to test (empty string means all modules) - Keep this list up to date with the modules.
# This list is used to check the validity of the '-m <module>' argument and to display the list of available modules with '-m list' or '-m show'
# Note : the tool 'act' uses this variable to know which modules to test when the user asks for 'act -m <module>'.
PYAGRUM_TEST_MODULES = {"", "main", "skbn", "causal", "causaleffect", "clg", "ctbn", "bnmixture", "explain"}


def go():
  cwd = os.getcwd()
  FORMAT = "[pyAgrum] %(asctime)s | %(levelname)s | %(filename)s:%(lineno)d | %(funcName)s | %(message)s"

  os.chdir(Path(__file__).resolve().parent)

  test_modules = PYAGRUM_TEST_MODULES

  parser = argparse.ArgumentParser(prog="gumTest.py", add_help=False)
  parser.add_argument("mode", nargs="?", default="release", choices=["debug", "release"], type=str.lower)
  parser.add_argument("loc",  nargs="?", default="local",   choices=["installed", "local"], type=str.lower)
  parser.add_argument("-m", dest="module", default="")
  parser.add_argument("-t", dest="suite",  default="all")
  args, _ = parser.parse_known_args(sys.argv[1:])

  mod      = args.mode
  islocal  = args.loc != "installed"
  testNotebooks = False
  notebooksOnly = False
  test_suite = "" if args.suite == "all" else args.suite

  m = args.module
  if m in {"list", "show"}:
    notif(f"Available modules: {sorted(test_modules - {'', 'main'})}")
    notif("(use '-m <module>' to run only that module without notebooks)")
    sys.exit(0)
  elif m == "all+nb":
    testNotebooks = True
    test_module = ""
  elif m == "nb":
    testNotebooks = True
    notebooksOnly = True
    test_module = ""
  elif m in {"", "all"} or m.startswith("all-") or m in test_modules:
    test_module = m
  else:
    error(
      f"[-m] unknown value '{m}'. Expected: all[+nb][-mod1[-mod2…]], nb, list, show, or <module> with module in {sorted(test_modules - {'', 'main'})}"
    )
    sys.exit(1)

  logfilename = "/pyAgrumTests.log"
  if mod != "standAlone":
    logfilename = "/../../.." + logfilename
  logfilename = cwd + logfilename
  notif(f"log : {logfilename}")

  log = logging.getLogger("gumTestLog")
  log.setLevel(logging.DEBUG)  # better to have too much log than not enough
  fh = logging.FileHandler(logfilename, mode="w", encoding=None, delay=False)
  fh.setFormatter(logging.Formatter(FORMAT))
  log.addHandler(fh)
  log.propagate = False

  log.info("Mode detected : " + mod)
  log.info("Testing notebooks : " + str(testNotebooks))

  if mod != "standAlone":
    if mod == "debug":
      libagrum = os.path.abspath("../../../build/pyAgrum/debug/wrappers")
    else:
      libagrum = os.path.abspath("../../../build/pyAgrum/release/wrappers")
    sys.path.insert(0, libagrum)  # to force using local pyAgrum for the tests (and not installed one)

  import pyagrum as gum

  total_errs = 0

  log.info("on Python {0} - {1}".format(platform.python_version(), os_platform))
  log.info("path : {}".format(gum.__file__))

  notif("*****************")
  notif("Python Test Suite")
  notif("*****************")

  import utils.testsOnPython as testsOnPython

  if not notebooksOnly:
    total_errs += testsOnPython.runTests(local=islocal, test_module=test_module, test_suite=test_suite, log=log)

  if testNotebooks:
    log.info("Tests on notebooks")
    notif()
    notif("*******************")
    notif("Notebook Test Suite")
    notif("*******************")
    from utils.testsOnNotebooks import runNotebooks

    try:
      total_errs += runNotebooks()
    except NameError:
      pass
    except Exception as e:
      log.warning(f"Error in NotebookTestSuite : {e}")
      total_errs += 1
      error("Error in NotebookTestSuite")

  os.chdir(cwd)
  notif("-" * 70)
  notif(" log file ")
  notif("-" * 70)
  with open(logfilename, "r") as logfile:
    for f in logfile:
      if "[pyAgrum]" in f:
        notif(f.rstrip())
  notif("-" * 70)

  notif()
  (error if total_errs > 0 else notif)(f"Errors : {total_errs}")

  sys.exit(total_errs)


if __name__ == "__main__":
  go()
  sys.exit(0)
