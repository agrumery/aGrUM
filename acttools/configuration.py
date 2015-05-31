#!/usr/bin/python
# -*- coding: utf-8 -*-

#***************************************************************************
#*   Copyright (C) 2015 by Pierre-Henri WUILLEMIN                          *
#*   {prenom.nom}_at_lip6.fr                                               *
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License as published by  *
#*   the Free Software Foundation; either version 2 of the License, or     *
#*   (at your option) any later version.                                   *
#*                                                                         *
#*   This program is distributed in the hope that it will be useful,       *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU General Public License for more details.                          *
#*                                                                         *
#*   You should have received a copy of the GNU General Public License     *
#*   along with this program; if not, write to the                         *
#*   Free Software Foundation, Inc.,                                       *
#*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
#***************************************************************************
import const as cfg
from modules import parseModulesTxt

try:
  import colorama
  colorama.init()
  cfg.withColour=True
except:
  try:
    import tendo.ansiterm
    cfg.withColour=True
  except:
    cfg.withColour=False
    pass

cfg.numversion="2.0"
cfg.modulesFile="src/modules.txt" # the file to parse to find the modules
cfg.configFile=".options.act2.py" #

cfg.nbr_tests_for_stats=40

def configureColors(no_fun=False):
  if no_fun or not cfg.withColour:
    cfg.C_VALUE = ''
    cfg.C_WARNING = ''
    cfg.C_ERROR = ''
    cfg.C_END = ''
  else:
    cfg.C_VALUE = '\033[1m\033[32m'
    cfg.C_WARNING = '\033[1m\033[33m'
    cfg.C_ERROR = '\033[1m\033[31m'
    cfg.C_END = '\033[0m'

def configureOutputs(options):
  cfg.verbosity=options.verbose
  configureColors(options.no_fun)

def about():
  print(cfg.C_END+cfg.C_WARNING+"aGrUM"+cfg.C_END+" compilation tool "+cfg.C_VALUE+cfg.numversion+cfg.C_END)
  print("(c) 2010-15 "+cfg.C_ERROR+"aGrUM Team"+cfg.C_END)
  print("")
