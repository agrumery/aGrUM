#!/usr/bin/python
# -*- coding: utf-8 -*-

# ***************************************************************************
# *   Copyright (C) 2015 by Pierre-Henri WUILLEMIN                          *
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
import re
import sys

from .configuration import cfg
from .utils import setifyString, notif


def parseModulesTxt():
  modules = {}
  module_line = re.compile(r"^\s*list\s*\(\s*APPEND\s*MODULES\s*\"(.*)\"\s*\)(\s*#\s*(.*))?")
  with open(cfg.modulesFile, "r") as f:
    for line in f:
      rep = module_line.search(line)
      if rep:
        module = rep.groups(0)[0]
        descr = rep.groups(0)[2]
        if descr == 0:
          descr = module
        modules[module] = descr
  return modules


def check_modules(current):
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
    print("Modules is in")
    print("    - ALL")
    for x in sorted(cfg.modules):
      print("    - " + x + " (" + cfg.moduleLabels[x] + ")")
    sys.exit(1)
  else:
    current['modules'] = cde
