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

import os
import shutil
import sys

from .configuration import cfg
from .oneByOne import checkAgrumMemoryLeaks
from .stats import profileAgrum
from .utils import trace, notif, critic
from .callSphinx import callSphinx
from .wheel_builder import wheel
from .wheel_builder import nightly_wheel
from .guideline import guideline


def isSpecialAction(current: dict[str, str]) -> bool:
  if current["oneByOne"]:
    return True
  if current["stats"]:
    return True
  if current["action"] == 'doc' and current["target"] == 'pyAgrum':
    return True

  return current["action"] in set(cfg.specialActions)


def specialActions(current: dict[str, str]) -> bool:
  if current["action"] == "clean":
    if not current["dry_run"]:
      cleanAll()
    print("")
    return True

  if current["action"] == "show":
    # action=show is the only action still performed even if dry_run=True
    showAct2Config(current)
    print("")
    return True

  if current["action"] == "guideline":
    # trace(current,"Special action [guideline]")
    nbrError = guideline(current, current['correction'])
    notif("-----------------------------")
    if nbrError > 0:
      if not current['correction']:
        critic(f"{nbrError} Guideline error(s) found.\n\n" +
               "Please consider using [act guideline --correction] in order to correct some of these errors.",
               None, nbrError)
      else:
        critic(f"{nbrError} Guideline error(s) found.", None, nbrError)
    else:
      notif("No guideline error found.")

    print("")
    return True

  if current["action"] == "wheel":
    wheel(current)
    return True

  if current["action"] == "nightly_wheel":
    nightly_wheel(current)
    return True

  if current["oneByOne"]:
    trace(current, "Special action [oneByOne]")
    checkAgrumMemoryLeaks(current)
    return True

  if current["stats"]:
    profileAgrum(current)
    return True

  if current["action"] == 'doc' and 'pyAgrum' in current["targets"]:
    callSphinx(current)
    return True

  return False


def cleanAll():
  print(f"{cfg.C_WARNING}cleaning all{cfg.C_END} ...", end="")
  sys.stdout.flush()
  if os.path.isdir("build"):
    shutil.rmtree("build")
    print(f"{cfg.C_VALUE}done{cfg.C_END}")
  else:
    print(f"{cfg.C_VALUE}nothing to do{cfg.C_END}")


def showAct2Config(current: dict[str, str]):
  def aff_key(key: str):
    notif(f"[{key}] => {current[key]}")

  for k in cfg.mains:
    aff_key(k)
  print("")

  for k in current.keys():
    if k not in cfg.mains and k not in cfg.non_persistent:
      aff_key(k)
  print("")

  for k in cfg.non_persistent:
    aff_key(k)
