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

# import anydbm
import shelve
from os import remove

from .configuration import cfg
from .invocation import showInvocation
from .modules import check_modules
from .tests import checkAndWriteTests
from .utils import error, notif, critic, setifyString


def parseCommandLine(current):
  return cfg.parser.parse_args()


def getCurrent():
  current = {}
  try:
    shlv = shelve.open(cfg.configFile, writeback=False)
  except:
    remove(cfg.configFile)
    shlv = shelve.open(cfg.configFile, writeback=False)

  for key in cfg.default:  # .iterkeys():
    current[key] = cfg.default[key]
    if key not in cfg.non_persistent:
      if key in shlv:
        current[key] = shlv[key]

  return current


def setCurrent(current):
  shlv = shelve.open(cfg.configFile, writeback=True)
  for key in current.keys():
    if key not in cfg.non_persistent:
      shlv[key] = current[key]


def checkCurrent(current, options, args):
  # helper
  def update(current, key, val, test):
    if test:
      if current[key] != val:
        # warn("{0} changed : {1} -> {2}".format(key,current[key],val))
        current[key] = val
    return test

  # end of helper

  # fixing options
  for opt, value in options.__dict__.items():
    if opt not in current:
      error("Options not known : {0} in {1}".format(opt, current.keys()))

    update(current, opt, value, current[opt] != value)

  bT = bA = bM = False
  # fixing args
  for ar in args:
    t = setifyString(ar)
    arg = "+".join(t)
    if update(current, 'targets', t, t.issubset(cfg.targets)):
      if bT:
        error("Targets overwritten by [{0}]".format("+".join(t)))
      bT = True
      continue
    if update(current, 'action', arg, arg in cfg.actions):
      if bA:
        error("Action overwritten by [{0}]".format(arg))
      bA = True
      continue
    if update(current, 'mode', arg, arg in cfg.modes):
      if bM:
        error("Mode overwritten by [{0}]".format(arg))
      bM = True
      continue

    critic("arg [{0}] unknown".format(arg))

  checkConsistency(current)

  if options.noSaveParams:
      pass
  else:
      setCurrent(current)
  showInvocation(current)


def checkConsistency(current):
  has_notif = False

  # helper
  def check_aGrumTest(option, current):
    if current[option]:
      prefix = "Option [{0}] acts only".format(option)
      if current['targets'] != set(['aGrUM']):
        has_notif = True
        notif(prefix + " on target [aGrUM].")
      if current['action'] != 'test':
        critic(prefix + " on action [test] (not on [{0}]).".format(current['action']))

  # end of helper

  if current['action'] == 'doc':
    if current['targets'] != set(['aGrUM']):
      notif("Action [doc] only on target [aGrUM] (for now).")
      current['targets'] = set(['aGrUM'])

  # test for only one target
  if current['action'] == 'test':
    if len(current['targets']) > 1:
      first = "aGrUM" if "aGrUM" in current['targets'] else list(current['targets'])[0]
      has_notif = True
      notif("Action [test] on only one target : selecting [" + first + "]")
      current['targets'] = [first]

  if current['stats'] and current['oneByOne']:
    has_notif = True
    notif("Options [stats] and [oneByOne] are mutually exclusive")

  # check -t and -m
  check_modules(current)
  checkAndWriteTests(current)

  check_aGrumTest('oneByOne', current)
  if current['coverage'] and current['mode'] != "debug":
    error("Option [coverage] can only be used with [debug] builds.")
    current['coverage'] = False

  if current['action'] == 'package':
    critic("Action [package] is not implemented yed")

  if has_notif:
    print("")
