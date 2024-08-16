# ***************************************************************************
# *   Copyright (c) 2015-2024 by Pierre-Henri WUILLEMIN                     *
# *   {prenom.nom}_at_lip6.fr                                               *
# *                                                                         *
# *   "act" is free software; you can redistribute it and/or modify         *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU General Public License for more details.                          *
# *                                                                         *
# *   You should have received a copy of the GNU General Public License     *
# *   along with this program; if not, write to the                         *
# *   Free Software Foundation, Inc.,                                       *
# *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
# **************************************************************************
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
  if forced or not current['no_fun']:
    invocation = getInvocation(current, True)
    print(f"{cfg.C_WARNING}invocation{cfg.C_END} : {invocation}")
    print("")


def getInvocation(current: dict[str, str], colored: bool = False) -> str:
  if colored:
    c_warning, c_error, c_value, c_end = cfg.C_WARNING, cfg.C_ERROR, cfg.C_VALUE, cfg.C_END
  else:
    c_warning = c_error = c_value = c_end = ''

  invocation = "act "

  invocation += getCommand(current['action'], c_warning)

  if current['action'] not in cfg.specialActions:
    invocation += getCommand("+".join(current['targets']), c_warning)
    invocation += getCommand(current['mode'], c_warning)

  for opt in current.keys():
    if opt not in ['action', 'mode', 'targets']:
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
