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

from typing import Any

from .configuration import cfg


def _getParam(name: str, c_error: str, c_end: str) -> str:
  return f"{c_end}--{c_error}{name}"


def getParam(name: str, c_error: str, c_end: str) -> str:
  return f"{_getParam(name, c_error, c_end)} "


def getValParam(name: str, val: Any, c_value: str, c_error: str, c_end: str):
  return f"{_getParam(name, c_error, c_end)} {c_end}={c_value}{val} "


def getCommand(name: str, c_warning: str) -> str:
  return f"{c_warning}{name} "


def showInvocation(current: dict[str, str], forced: bool = False):
  if forced or not current["no_fun"]:
    invocation = getInvocation(current, True)
    print(f"{cfg.C_WARNING}invocation{cfg.C_END} : {invocation}")
    print("")


def getInvocation(current: dict[str, str], colored: bool = False) -> str:
  if colored:
    c_warning, c_error, c_value, c_end = cfg.C_WARNING, cfg.C_ERROR, cfg.C_VALUE, cfg.C_END
  else:
    c_warning = c_error = c_value = c_end = ""

  invocation = "act "

  invocation += getCommand(current["action"], c_warning)

  if current["action"] not in cfg.specialActions:
    invocation += getCommand("+".join(current["targets"]), c_warning)
    invocation += getCommand(current["mode"], c_warning)

  for opt in current.keys():
    if opt not in ["action", "mode", "targets"]:
      if opt not in cfg.non_persistent:
        if opt not in cfg.swapOptions.keys():
          if opt in current.keys():
            invocation += getValParam(opt, current[opt], c_value, c_error, c_end)

  for opt in cfg.swapOptions.keys():
    invocation += getParam(cfg.swapOptions[opt][current[opt]], c_error, c_end)

  for opt in cfg.non_persistent:
    if current[opt]:
      invocation += getParam(opt, c_error, c_end)

  invocation += c_end
  return invocation
