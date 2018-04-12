from __future__ import print_function

import os
import platform
import sys
from sys import platform as os_platform

os.chdir(os.path.dirname(__file__ if __file__[0] == '/' else "./" + __file__))

mod = "standalone"
testNotebooks = False

for cde in sys.argv:
  if cde in ["debug", "release"]:
    mod = cde
  elif cde in ["all"]:
    testNotebooks = (cde == "all")

print("+ Mode detected : " + mod)
print("+ Testing notebooks : " + str(testNotebooks))

if mod != "standalone":
  if mod == "debug":
    libagrum = os.path.abspath("../../../build/debug/wrappers")
  else:
    libagrum = os.path.abspath("../../../build/release/wrappers")
  sys.path.insert(0, libagrum)  # to force to use local pyAgrum for the tests (and not installed one)

import pyAgrum as gum

total_errs = 0

print("pyAgrum on Python {0} - {1}".format(platform.python_version(), os_platform), end='\n', file=sys.stdout)
print("pyAgrum path : {}".format(gum.__file__), end='\n', file=sys.stdout)

print("*****************")
print("Python Test Suite")
print("*****************")
import testsOnPython

try:
  total_errs += testsOnPython.errs
except NameError:
  pass
except:
  total_errs += 1
  print("=> Error in TestSuite")

if testNotebooks:
  print("\n")
  print("*******************")
  print("Notebook Test Suite")
  print("*******************")
  from testsOnNotebooks import runNotebooks

  try:
    total_errs += runNotebooks()
  except NameError:
    pass
  except:
    total_errs += 1
    print("=> Error in NotebookTestSuite")

print("\n\nErrors : " + str(total_errs))

sys.exit(total_errs)
