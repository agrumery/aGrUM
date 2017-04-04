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
from __future__ import print_function

import glob
import os
import shutil
import sys
from subprocess import call

from .configuration import cfg
from .oneByOne import checkAgrumMemoryLeaks
from .stats import profileAgrum
from .utils import trace, notif, critic
from .callSphinx import callSphinx
from .wheel_builder import wheel


def isSpecialAction(current):
  if current["oneByOne"] == True:
    return True
  if current["stats"] == True:
    return True
  if current["action"] == 'doc' and current["target"] == 'pyAgrum':
    return True

  return current["action"] in set(["clean", "show", "autoindent"])


def specialActions(current):
  if current["action"] == "clean":
    # trace(current,"Special action [clean]")
    if not current["dry_run"]:
      cleanAll()
    print("")
    return True

  if current["action"] == "show":
    # trace(current,"Special action [show]")
    # action=show is the only action still performed even if dry_run=True
    showAct2Config(current)
    print("")
    return True

  if current["action"] == "autoindent":
    # trace(current,"Special action [autoindent]")
    autoindent(current)
    print("")
    return True

  if current["action"] == "wheel":
    wheel(current)
    return True

  if current["oneByOne"] == True:
    trace(current, "Special action [oneByOne]")
    checkAgrumMemoryLeaks(current)
    return True

  if current["stats"] == True:
    profileAgrum(current)
    return True

  if current["action"] == 'doc':
    if 'pyAgrum' in current["targets"]:
      callSphinx(current)
      return True

  return False


def cleanAll():
  print(cfg.C_WARNING + "cleaning" + cfg.C_END + " ... ", end="")
  sys.stdout.flush()
  if os.path.isdir("build"):
    shutil.rmtree("build")
    print(cfg.C_VALUE + "done" + cfg.C_END)
  else:
    print(cfg.C_VALUE + "nothing to do" + cfg.C_END)


def showAct2Config(current):
  def aff(k):
    notif("[{0}] => {1}".format(k, current[k]))

  for k in cfg.mains:
    aff(k)
  print("")

  for k in current.keys():
    if not k in cfg.mains and not k in cfg.non_persistent:
      aff(k)
  print("")

  for k in cfg.non_persistent:
    aff(k)


from os.path import isdir


def recglob(path, mask):
  for item in glob.glob(path + "/*"):
    if isdir(item):
      for item in recglob(item, mask):
        yield item

  for item in glob.glob(path + "/" + mask):
    if not isdir(item):
      yield item


def srcAgrum():
  for i in recglob("src/agrum", "*.cpp"):
    yield i
  for i in recglob("src/agrum", "*.h"):
    yield i
  for i in recglob("src/testunits", "*TestSuite.h"):
    yield i


def autoindent(current):
  if cfg.clangformat == None:
    critic("No clang-format tool has been found.")

  for src in srcAgrum():
    line = cfg.clangformat + " -i " + src
    notif("autoindent [{0}]".format(src))
    trace(current, line)
    if not current["dry_run"]:
      call(line, shell=True)
