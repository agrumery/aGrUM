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
from .utils import trace, notif, critic, srcAgrum
from .callSphinx import callSphinx
from .wheel_builder import wheel
from .guideline import guideline


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

  if current["action"] == "guideline":
    # trace(current,"Special action [guideline]")
    nbrError=guideline(current,current['correction'])
    if nbrError>0:
      critic("Guideline error(s) found.",None,nbrError)
    else:
      notif("No guideline error found.")

    print("")
    return True

  if current["action"] == "wheel":
    wheel(current)
    return True

  if current["oneByOne"]:
    trace(current, "Special action [oneByOne]")
    checkAgrumMemoryLeaks(current)
    return True

  if current["stats"]:
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

def autoindent(current):
  if cfg.clangformat == None:
    critic("No clang-format tool has been found.")

  for src in srcAgrum():
    line = cfg.clangformat + " -i " + src
    notif("autoindent [{0}]".format(src))
    trace(current, line)
    if not current["dry_run"]:
      call(line, shell=True)
