#!/usr/bin/python
# -*- coding: utf-8 -*-# ***************************************************************************
# *   Copyright (c) 2015-2024 by Pierre-Henri WUILLEMIN                     *
# *   {prenom.nom}_at_lip6.fr                                               *
# *                                                                         *
# *   "act" is free software; you can redistribute it and/or modify         *
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
# **************************************************************************
import os
import shutil
import sys

from .configuration import cfg
from .oneByOne import checkAgrumMemoryLeaks
from .stats import profileAgrum
from .utils import trace, notif, critic
from .callSphinx import callSphinx
from .wheel_builder import wheel
from .wheel_builder import nightly_wheel
from .guideline import guideline


def isSpecialAction(current: dict[str, str]) -> bool:
  if current["oneByOne"]:
    return True
  if current["stats"]:
    return True
  if current["action"] == 'doc' and current["target"] == 'pyAgrum':
    return True

  return current["action"] in set(cfg.specialActions)


def specialActions(current: dict[str, str]) -> bool:
  if current["action"] == "clean":
    if not current["dry_run"]:
      cleanAll()
    print("")
    return True

  if current["action"] == "show":
    # action=show is the only action still performed even if dry_run=True
    showAct2Config(current)
    print("")
    return True

  if current["action"] == "guideline":
    # trace(current,"Special action [guideline]")
    nbrError = guideline(current, current['correction'])
    notif("-----------------------------")
    if nbrError > 0:
      if not current['correction']:
        critic(f"{nbrError} Guideline error(s) found.\n\n" +
               "Please consider using [act guideline --correction] in order to correct some of these errors.",
               None, nbrError)
      else:
        critic(f"{nbrError} Guideline error(s) found.", None, nbrError)
    else:
      notif("No guideline error found.")

    print("")
    return True

  if current["action"] == "wheel":
    wheel(current)
    return True

  if current["action"] == "nightly_wheel":
    nightly_wheel(current)
    return True

  if current["oneByOne"]:
    trace(current, "Special action [oneByOne]")
    checkAgrumMemoryLeaks(current)
    return True

  if current["stats"]:
    profileAgrum(current)
    return True

  if current["action"] == 'doc' and 'pyAgrum' in current["targets"]:
    callSphinx(current)
    return True

  return False


def cleanAll():
  print(f"{cfg.C_WARNING}cleaning all{cfg.C_END} ...", end="")
  sys.stdout.flush()
  if os.path.isdir("build"):
    shutil.rmtree("build")
    print(f"{cfg.C_VALUE}done{cfg.C_END}")
  else:
    print(f"{cfg.C_VALUE}nothing to do{cfg.C_END}")


def showAct2Config(current: dict[str, str]):
  def aff_key(key: str):
    notif(f"[{key}] => {current[key]}")

  for k in cfg.mains:
    aff_key(k)
  print("")

  for k in current.keys():
    if k not in cfg.mains and k not in cfg.non_persistent:
      aff_key(k)
  print("")

  for k in cfg.non_persistent:
    aff_key(k)
