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

from configuration import cfg

def getCurrent():
  current={}
  shlv=shelve.open(cfg.configFile,writeback=True)
  for key in cfg.default.iterkeys():
    current[key]=cfg.default[key]
    if not key in cfg.non_persistent:
      if shlv.has_key(key):
        current[key]=shlv[key]

  return current

def initParser(current):
    us="%prog [options] ["+"|".join(cfg.LIST_ACTIONS)+"] ["+"|".join(cfg.LIST_MODES)+"] ["+"|".join(cfg.LIST_TARGETS)+"]"
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
    parser.add_option("", "--1by1",
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
    parser.add_option("-t","--testlist",
                                        help="testlist management : {show|all|test1+test2+test3}",
                                        metavar="TESTS-COMMAND",
                                        dest="testlist",
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
    return parser
