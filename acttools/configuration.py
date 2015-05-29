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
import sys
import re

try:
   import colorama
   colorama.init()
except:
   try:
       import tendo.ansiterm
   except:
       pass
import const as cfg

cfg.numversion="2.0"
cfg.modulesFile="src/modules.txt" # the file to parse to find the modules
cfg.configFile=".options.act2.py" #

cfg.nbr_tests_for_stats=40

def parseModulesTxt(filename=cfg.modulesFile):
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
  return set(modules)

cfg.actions=set("lib test install doc clean show uninstall package".split())
cfg.modes=set("debug release".split())
cfg.targets=set("aGrUM pyAgrum jAgrum".split())
cfg.modules=parseModulesTxt()

cfg.default={}
cfg.default['action']="lib"
cfg.default['targets']=set(["aGrUM"])
cfg.default['modules']='ALL'
cfg.default['mode']="release"
cfg.default['verbose']=False
cfg.default['destination']="/usr"
cfg.default['jobs']=5
cfg.default['static_lib']=False
cfg.default['fixed_seed']=False
cfg.default['no_fun']=False
cfg.default['stats']=False
cfg.default['oneByOne']=False
cfg.default['tests']='all'
cfg.default['pyversion']="3"

cfg.non_persistent=["fixed_seed","stats","no_fun","static_lib","oneByOne"]


def configureColors(no_fun=False):
  if no_fun:
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

def notif(s):
  print("-- "+cfg.C_VALUE+s+cfg.C_END)

def warn(s):
  if cfg.verbosity:
    print("-- "+cfg.C_WARNING+s+cfg.C_END)

def error(s):
  print("-- "+cfg.C_ERROR+s+cfg.C_END)

def critic(s):
  error(s)
  print("\n-- "+cfg.C_ERROR+"stopped."+cfg.C_END+"\n")
  sys.exit(1)

