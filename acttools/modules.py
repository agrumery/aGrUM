############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
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
#      or (at your option) any later version.                              #
#    - the MIT license (MIT)                                               #
#    - or both in dual license, as here                                    #
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

import re
import sys

from .configuration import cfg
from .utils import setifyString, notif


def parseModulesTxt() -> dict[str, str]:
  modules = {}
  module_line = re.compile(r"^\s*list\s*\(\s*APPEND\s*MODULES\s*\"(.*)\"\s*\)(\s*#\s*(.*))?")
  with open(cfg.modulesFile, "r", encoding="utf8") as f:
    for line in f:
      rep = module_line.search(line)
      if rep:
        module = rep.groups(0)[0]
        descr = rep.groups(0)[2]
        if descr == 0:
          descr = module
        modules[module] = descr
  return modules


def check_modules(current: dict[str, str]):
  setM = setifyString(current['modules'])

  if 'pyAgrum' in current['targets']:
    notif('Forcing the compilation of all modules for wrappers')
    cde = 'ALL'
  elif 'ALL' in setM or 'all' in setM:
    cde = 'ALL'
  else:
    if not setM.issubset(set(cfg.modules)):
      cde = 'LIST'
    else:
      cde = '+'.join(setM)

  if cde == 'ALL':
    current['modules'] = '+'.join(sorted(cfg.modules))
  elif cde == 'LIST':
    print("Modules must be one of the following:")
    print("    - ALL")
    for x in sorted(cfg.modules):
      print("    - " + x + " (" + cfg.moduleLabels[x] + ")")
    print("\nCurrent configuration contains the following modules which are outside of the scope:")
    for x in sorted(list(setM - set(cfg.modules))):
      print(f"    - {x} (unknown module)")
    sys.exit(1)
  else:
    current['modules'] = cde
