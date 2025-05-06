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

import sys
from subprocess import PIPE, Popen, STDOUT

from .configuration import cfg
from .tests import checkTests, testNames
from .utils import notif


def checkAgrumMemoryLeak(x: str, percent: float):
  cmd = f"{sys.executable} act test debug -t {x} -m all"

  first = f"{cfg.C_VALAUE} [{percent:5.1f}%] "
  second = f"{cfg.C_WARNING}{x}{cfg.C_END} : "
  last = ""
  flag = 0

  sys.stdout.write(first + second)
  sys.stdout.flush()

  proc = Popen(cmd + " --no-fun", shell=True, stdout=PIPE, stderr=STDOUT)
  out = proc.stdout.readlines()
  for line in out:
    if b"NO MEMORY LEAK" in line:
      last = f"{cfg.C_VALUE}ok{cfg.C_END}"
      flag = 1
    elif b"Memory leaks found" in line:
      line = str(line)
      last = f"{cfg.C_ERROR}{line.split('|')[2].strip()}{cfg.C_END}"
      flag = 2

  if flag == 0:
    last = f"{cfg.C_ERROR}?{cfg.C_END}"

  print(last)
  return second + last, flag == 1


def checkAgrumMemoryLeaks(current: dict[str, str]):
  notif("Searching leaks test by test (may be a bit long).\n")

  res = []
  tests_list = sorted(testNames(checkTests(current)))
  for i, x in enumerate(tests_list):
    (msg, testOK) = checkAgrumMemoryLeak(x, (i + 1) * 100.0 / len(tests_list))
    if not testOK:
      res.append(msg)

  sres = "\n -".join(res) if len(res) > 0 else cfg.C_VALUE + "none"
  print(f"\n{cfg.C_WARNING}Test(s) with problem(s) :\n -{sres}\n{cfg.C_END}")
