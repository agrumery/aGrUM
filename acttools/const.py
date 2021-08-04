# -*- coding: utf-8 -*-
# ***************************************************************************
# *   Copyright (c) 2015 by Pierre-Henri WUILLEMIN                          *
# *   {prenom.nom}_at_lip6.fr                                               *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
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
# ***************************************************************************

# a way of creating constants in python
import sys
from typing import Dict, Any, List


class _const:
  class ConstError(TypeError):
    pass

  def __setattr__(self, name, value):
    if name in self.__dict__:
      raise self.ConstError("Can't rebind const(%s)" % name)
    self.__dict__[name] = value

  def __str__(self) -> str:
    # we do not show the colors
    d: dict[str, Any] = self.__dict__
    l: list[str] = [
      f"{self.C_MSG}{k}{self.C_END} : {self.C_VALUE}{d[k] if k[:2] != 'C_' else '(escape sequence)'}{self.C_END}" for
      k in sorted(d.keys())]
    return "\n".join(l)

sys.modules[__name__] = _const()
