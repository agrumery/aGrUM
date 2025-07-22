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
from .utils import *


def get_invocation(current: dict[str, str | bool], colored: bool = False) -> str:
  """
  Generate the command line invocation string based on the current configuration.

  Parameters
  ----------
  current: dict[str, str | bool]
      The current configuration dictionary containing action, target, mode, and other options.
  colored: bool, optional
      If True, the output will include color codes for better visibility in terminal.
      Defaults to False.

  Returns
  -------
    str
      A formatted string representing the command line invocation.
  """

  def _get_param_name(name: str, c_error: str, c_end: str) -> str:
    return f"{c_end}--{c_error}{name}"

  def _get_param(name: str, c_error: str, c_end: str) -> str:
    return f"{_get_param_name(name, c_error, c_end)} "

  def _get_val_param(name: str, val: Any, c_value: str, c_error: str, c_end: str):
    return f"{_get_param_name(name, c_error, c_end)}{c_end}={c_value}{val} "

  def _(name: str, c_warning: str) -> str:
    return f"{c_warning}{name} "

  if colored:
    c_warning, c_error, c_value, c_end = cfg.C_WARNING, cfg.C_ERROR, cfg.C_VALUE, cfg.C_END
  else:
    c_warning = c_error = c_value = c_end = ""

  invocation = "act "

  invocation += _(current["action"], c_warning)

  if current["action"] not in cfg.specialActions:
    invocation += _(current["target"], c_warning)
    invocation += _(current["mode"], c_warning)

  for opt in current.keys():
    if opt not in ["action", "mode", "target"]:
      if opt not in cfg.non_persistent and opt not in cfg.swapOptions.keys() and opt in current.keys():
        invocation += _get_val_param(opt, current[opt], c_value, c_error, c_end)

  for opt in cfg.swapOptions.keys():
    invocation += _get_param(cfg.swapOptions[opt][current[opt]], c_error, c_end)

  for opt in cfg.non_persistent:
    if current[opt]:
      invocation += _get_param(opt, c_error, c_end)

  invocation += c_end
  return invocation


def show_invocation(current: dict[str, str | bool]):
  printutf8(f"\n{cfg.C_WARNING}{get_invocation(current, True)}{cfg.C_END}\n\n")
