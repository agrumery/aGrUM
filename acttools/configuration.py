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


##########################################################################################################################
# ANSI colored string
try:
   import colorama
   colorama.init()
except:
   try:
       import tendo.ansiterm
   except:
       pass
import re
import const

const.numversion="2.0"
const.modulesFile="src/modules.txt" # the file to parse to find the modules
const.configFile=".options.act2.py" #


const.C_VALUE = '\033[1m\033[32m'
const.C_WARNING = '\033[1m\033[33m'
const.C_ERROR = '\033[1m\033[31m'
const.C_END = '\033[0m'

const.nbr_tests_for_stats=40

def parseModulesTxt(filename=const.modulesFile):
  modules={}
  module_line=re.compile(r"^\s*list\s*\(\s*APPEND\s*MODULES\s*\"(.*)\"\s*\)(\s*#\s*(.*))?")
  with open(filename,"r") as f:
    for line in f:
      rep=module_line.search(line)
      if rep:
        module=rep.groups(0)[0]
        descr=rep.groups(0)[2]
        if descr==0:
          descr=module
        modules[module]=descr
  return modules

const.LIST_ACTIONS="lib test install doc clean show uninstall package".split()
const.LIST_MODE="debug release".split()
const.LIST_TARGETS="aGrUM pyAgrum jAgrum".split()
const.LIST_MODULES=parseModulesTxt(modulesFile)

const.default['actions']="lib"
const.default['targets']="aGrUM"
const.default['modules']='ALL'
const.default['mode']="release"
const.default['verbose']=False
const.default['destination']="/usr"
const.default['jobs']="5"
const.default['static_lib']=False
const.default['fixed_seed']=False
const.default['no_fun']=False
const.default['stats']=False
const.default['onebyone']=False
const.default['tests']='all'
const.default['pyversion']="3"

const.non_persistent=["fixed_seed","stats","no_fun","static_lib","onebyone","stats"]


def about():
  print(const.C_END+const.C_WARNING+"aGrUM"+const.C_END+" compilation tool "+const.C_VALUE+numversion+const.C_END)
  print("(c) 2010-15 "+const.C_ERROR+"aGrUM Team"+const.C_END)
  print("\n")
