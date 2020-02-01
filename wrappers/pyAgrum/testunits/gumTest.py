from __future__ import print_function

import os
import platform
import sys
from sys import platform as os_platform


import logging
if __name__ == "__main__":
  print("ON gumTest")
  FORMAT = '%(asctime)s | %(levelname)s | %(filename)s:%(lineno)d | %(funcName)s | %(message)s'
  logging.basicConfig(filename='pyAgrumTests.log',
                      filemode='w', format=FORMAT, level=logging.DEBUG)
  logging.info("[pyAgrum]                       log messages")
  logging.info("[pyAgrum]")
    
os.chdir(os.path.dirname(__file__ if __file__[0] == '/' else "./" + __file__))

mod = "standalone"
testNotebooks = False

for cde in sys.argv:
  if cde in ["debug", "release"]:
    mod = cde
  elif cde in ["all"]:
    testNotebooks = (cde == "all")

logging.info("+ Mode detected : " + mod)
logging.info("+ Testing notebooks : " + str(testNotebooks))

if sys.version_info < (3, 0):
  if testNotebooks:
    logging.warning("No notebook tests in python2")
    testNotebooks = False

if mod != "standalone":
  if mod == "debug":
    libagrum = os.path.abspath("../../../build/debug/wrappers")
  else:
    libagrum = os.path.abspath("../../../build/release/wrappers")
  sys.path.insert(0, libagrum)  # to force to use local pyAgrum for the tests (and not installed one)

import pyAgrum as gum

total_errs = 0

logging.info("pyAgrum on Python {0} - {1}".format(platform.python_version(), os_platform))
logging.info("pyAgrum path : {}".format(gum.__file__))

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
