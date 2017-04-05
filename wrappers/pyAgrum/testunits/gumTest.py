import os
import platform
import sys
from sys import platform as os_platform


os.chdir(os.path.dirname('./'+__file__))
libagrum = os.path.abspath("../../../build/release/wrappers")
sys.path.insert(0, libagrum)  # to force to use local pyAgrum for the tests (and not installed one)

import pyAgrum as gum


total_errs = 0

print("pyAgrum on Python {0} - {1}".format(platform.python_version(), os_platform), end='\n', file=sys.stdout)
print("pyAgrum path : {}".format(gum.__file__), end='\n', file=sys.stdout)

print("*****************")
print("Python Test Suite")
print("*****************")
import TestSuite

try:
  total_errs += TestSuite.errs
except NameError:
  pass
except:
  total_errs += 1
  print("=> Error in TestSuite")

if len(sys.argv)==2 and sys.argv[1]=='all':
  print("\n")
  print("*******************")
  print("Notebook Test Suite")
  print("*******************")
  import NotebookTestSuite

  try:
    total_errs += NotebookTestSuite.errs
  except NameError:
    pass
  except:
    total_errs += 1
    print("=> Error in NotebookTestSuite")

  print("\n\nErrors : " + str(total_errs))

sys.exit(total_errs)
