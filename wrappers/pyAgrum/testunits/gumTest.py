# (c) Copyright 2015-2024 by Pierre-Henri Wuillemin(@LIP6)
# (pierre-henri.wuillemin@lip6.fr)
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.
# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!

import os
import platform
import sys
from sys import platform as os_platform

import logging


def go():
  cwd = os.getcwd()
  FORMAT = '[pyAgrum] %(asctime)s | %(levelname)s | %(filename)s:%(lineno)d | %(funcName)s | %(message)s'

  if os.path.isabs(__file__):
    os.chdir(os.path.dirname(__file__))
  else:
    os.chdir(os.path.dirname("./" + __file__))

  test_modules = {'', 'main', 'skbn', 'causal', 'clg', 'ctbn'}

  mod = "release"  # release|debug|standAlone (test the installed version)
  testNotebooks = False
  test_module = ""  # all modules
  for cde in sys.argv:
    if cde in ["debug", "release"]:
      mod = cde
    elif cde == "all":
      testNotebooks = (cde == "all")
    elif cde[:5] == 'quick':
      test_module = cde[6:]
      if test_module not in test_modules:
        print(f"[-t quick_module] but module '{test_module}' not in {test_modules}")
        sys.exit(1)

  logfilename = "/pyAgrumTests.log"
  if mod != "standAlone":
    logfilename = "/../../.." + logfilename
  logfilename = cwd + logfilename
  print(f"log : {logfilename}")

  log = logging.getLogger("gumTestLog")
  log.setLevel(logging.DEBUG)  # better to have too much log than not enough
  fh = logging.FileHandler(logfilename, mode='w', encoding=None, delay=False)
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

  import pyAgrum as gum

  total_errs = 0

  log.info("on Python {0} - {1}".format(platform.python_version(), os_platform))
  log.info("path : {}".format(gum.__file__))

  print("*****************")
  print("Python Test Suite")
  print("*****************")

  import testsOnPython

  total_errs += testsOnPython.runTests(local=len(sys.argv) > 1, test_module=test_module, log=log)

  if testNotebooks:
    log.info("Tests on notebooks")
    print("\n")
    print("*******************")
    print("Notebook Test Suite")
    print("*******************")
    from testsOnNotebooks import runNotebooks

    try:
      total_errs += runNotebooks()
    except NameError:
      pass
    except Exception as e:
      log.warning(f"Error in NotebookTestSuite : {e}")
      total_errs += 1
      print("=> Error in NotebookTestSuite")

  os.chdir(cwd)
  print("-" * 70)
  print(" log file ")
  print("-" * 70)
  with open(logfilename, "r") as logfile:
    for f in logfile.readlines():
      if "[pyAgrum]" in f:
        print(f, end='')
  print("-" * 70)

  print("\n\n\nErrors : " + str(total_errs))

  sys.exit(total_errs)


if __name__ == "__main__":
  go()
  sys.exit(0)
