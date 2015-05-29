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
from optparse import OptionParser
import shelve

from configuration import cfg,warn,error,notif,critic
from invocation import showInvocation

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

def parseCommandLine(current):
    us="%prog [options] ["+"|".join(cfg.actions)+"] ["+"|".join(cfg.modes)+"] ["+"|".join(cfg.targets)+"]"
    parser=OptionParser(usage=us,description="Compilation tools for aGrUM and wrappers",
                        version="%prog v"+cfg.numversion)
    parser.add_option("", "--no-fun",
                                        help="No fancy output parser",
                                        action="store_true",
                                        dest="no_fun",
                                        default=False)
    parser.add_option("-v", "--verbose",
                                        help="more message on what is happening",
                                        action="store_true",
                                        dest="verbose",
                                        default=current['verbose'])
    parser.add_option("-q", "--quiet",
                                        help="please be quiet",
                                        action="store_false",
                                        dest="verbose",
                                        default=current['verbose'])
    parser.add_option("", "--fixed_seed",
                                        help="Random seed is fixed once for all. Hence random algorithms should be time-normalized.",
                                        action="store_true",
                                        dest="fixed_seed",
                                        default=False)
    parser.add_option("", "--stats",
                                        help="Consolidation on "+str(cfg.nbr_tests_for_stats)+" runs.",
                                        action="store_true",
                                        dest="stats",
                                        default=False)
    parser.add_option("", "--oneByOne",
                                        help="aGrUM debug tests one by one (searching leaks).",
                                        action="store_true",
                                        dest="oneByOne",
                                        default=False)
    parser.add_option("-d", "--dest",
                                        help="destination folder when installing",
                                        metavar="FOLDER",
                                        dest="destination",
                                        default=current['destination'])
    parser.add_option("-j", "--jobs",
                                        help="number of jobs",
                                        type='int',
                                        dest="jobs",
                                        default=current['jobs'])
    parser.add_option("-t","--tests",
                                        help="tests management : {show|all|test1+test2+test3}",
                                        metavar="TESTS-COMMAND",
                                        dest="tests",
                                        default=current['tests'])
    parser.add_option("-m","--module",
                                        help="module management : {show|all|module1+module2+module3}",
                                        metavar="MODULES-COMMAND",
                                        dest="modules",
                                        default=current['modules'])
    parser.add_option("", "--static_lib",
                                        help="build static library",
                                        action="store_true",
                                        dest="static_lib",
                                        default=False)
    parser.add_option("", "--python",   help="{2|3}",
                                        type="choice",
                                        choices=["2", "3"],
                                        dest="pyversion",
                                        default="3")
    return parser.parse_args()

def setifyString(s):
  return set(filter(None,s.split("+"))) # filter to setify "a++b+c" into set(['a','b','c'])

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

    error("[{0}] unknown".format(arg))

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
