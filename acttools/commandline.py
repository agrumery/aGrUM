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
import shelve

from configuration import cfg
from utils import warn,error,notif,critic

import re
from optparse import OptionParser

from configuration import cfg

from invocation import showInvocation

def initParams():
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
    cfg.default['dry_run']=False

    cfg.actions=set("lib test install doc clean show uninstall package".split())
    cfg.modes=set("debug release".split())
    cfg.targets=set("aGrUM pyAgrum jAgrum".split())
    cfg.modules=parseModulesTxt()

    cfg.non_persistent=["fixed_seed","stats","no_fun","static_lib","oneByOne","dry_run"]

def parseCommandLine(current):
    us="%prog [options] ["+"|".join(cfg.actions)+"] ["+"|".join(cfg.modes)+"] ["+"|".join(cfg.targets)+"]"
    cfg.parser=OptionParser(usage=us,description="Compilation tools for aGrUM and wrappers",
                        version="%prog v"+cfg.numversion)
    cfg.parser.add_option("", "--no-fun",
                                        help="No fancy output parser",
                                        action="store_true",
                                        dest="no_fun",
                                        default=False)
    cfg.parser.add_option("-v", "--verbose",
                                        help="more message on what is happening",
                                        action="store_true",
                                        dest="verbose",
                                        default=current['verbose'])
    cfg.parser.add_option("-q", "--quiet",
                                        help="please be quiet",
                                        action="store_false",
                                        dest="verbose",
                                        default=current['verbose'])
    cfg.parser.add_option("", "--fixed_seed",
                                        help="Random seed is fixed once for all. Hence random algorithms should be time-normalized.",
                                        action="store_true",
                                        dest="fixed_seed",
                                        default=False)
    cfg.parser.add_option("", "--stats",
                                        help="Consolidation on "+str(cfg.nbr_tests_for_stats)+" runs.",
                                        action="store_true",
                                        dest="stats",
                                        default=False)
    cfg.parser.add_option("", "--oneByOne",
                                        help="aGrUM debug tests one by one (searching leaks).",
                                        action="store_true",
                                        dest="oneByOne",
                                        default=False)
    cfg.parser.add_option("-d", "--dest",
                                        help="destination folder when installing",
                                        metavar="FOLDER",
                                        dest="destination",
                                        default=current['destination'])
    cfg.parser.add_option("-j", "--jobs",
                                        help="number of jobs",
                                        type='int',
                                        dest="jobs",
                                        default=current['jobs'])
    cfg.parser.add_option("-t","--tests",
                                        help="tests management : {show|all|test1+test2+test3}",
                                        metavar="TESTS-COMMAND",
                                        dest="tests",
                                        default=current['tests'])
    cfg.parser.add_option("-m","--module",
                                        help="module management : {show|all|module1+module2+module3}",
                                        metavar="MODULES-COMMAND",
                                        dest="modules",
                                        default=current['modules'])
    cfg.parser.add_option("", "--static_lib",
                                        help="build static library",
                                        action="store_true",
                                        dest="static_lib",
                                        default=False)
    cfg.parser.add_option("", "--python",   help="{2|3}",
                                        type="choice",
                                        choices=["2", "3"],
                                        dest="pyversion",
                                        default="3")
    cfg.parser.add_option("", "--dry-run",  help="dry run",
                                        action="store_true",
                                        dest="dry_run",
                                        default=False)
    return cfg.parser.parse_args()

def setifyString(s):
  return set(filter(None,s.split("+"))) # filter to setify "a++b+c" into set(['a','b','c'])

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

def getCurrent():
  current={}
  shlv=shelve.open(cfg.configFile,writeback=True)
  for key in cfg.default.iterkeys():
    current[key]=cfg.default[key]
    if not key in cfg.non_persistent:
      if shlv.has_key(key):
        current[key]=shlv[key]

  return current

def setCurrent(current):
  shlv=shelve.open(cfg.configFile,writeback=True)
  for key in current.keys():
    if not key in cfg.non_persistent:
      shlv[key]=current[key]

def updateCurrent(current,options,args):
  #helper
  def update(current,key,val,test):
    if test:
      if current[key]!=val:
        warn("{0} changed : {1} -> {2}".format(key,current[key],val))
        current[key]=val
    return test
  #end of helper

  # fixing options
  for opt, value in options.__dict__.items():
    if opt not in current:
      error("Options not known : {0} in {1}".format(opt,current.keys()))

    update(current,opt,value,current[opt]!=value)

  bT=bA=bM=False
  # fixing args
  for arg in args:
    t=setifyString(arg)
    if update(current,'targets',t,t.issubset(cfg.targets)):
      if bT:
        error("Targets overwritten by [{0}]".format("+".join(t)))
      bT=True
      continue
    if update(current,'action',arg,arg in cfg.actions):
      if bA:
        error("Action overwritten by [{0}]".format(arg))
      bA=True
      continue
    if update(current,'mode',arg,arg in cfg.modes):
      if bM:
        error("Mode overwritten by [{0}]".format(arg))
      bM=True
      continue

    error("arg [{0}] unknown".format(arg))

  setCurrent(current)
  showInvocation(current)
  checkConsistency(current)


def checkConsistency(current):
  # helper
  def check_aGrumTest(option,current):
    if current[option] :
      prefix="Option [{0}] acts only".format(option)
      if current['targets']!=['aGrUM']:
        notif(prefix+" on target [aGrUM].")
      if current['action']!='test':
        critic(prefix+" on action [test] (not on [{0}]).".format(current['action']))
  #end of helper

  if current['stats'] and current['oneByOne']:
    notif("Options [stats] and [oneByOne] are mutually exclusive")

  check_aGrumTest('stats',current)
  check_aGrumTest('oneByOne',current)
