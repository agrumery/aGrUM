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
from __future__ import print_function
import os
import shutil

from configuration import cfg
from utils import trace,notif

def specialActions(current):
  if current["action"]=="clean":
    trace(current,"Special action [clean]")
    if not current["dry_run"]:
      cleanAll()
    print("")
    return True

  if current["action"]=="show":

    trace(current,"Special action [show]")

    # action=show is the only action still performed even if dry_run=True
    showAct2Config(current)
    print("")
    return True

  return False


def cleanAll():
    print(cfg.C_WARNING+" cleaning"+cfg.C_ENDC+" ... ",end="")
    sys.stdout.flush()
    if os.path.isdir("build"):
      shutil.rmtree("build")
      print(cfg.C_OKGREEN+"done"+cfg.C_ENDC)
    else:
      print(cfg.C_OKGREEN+"nothing to do"+cfg.C_ENDC)


def showAct2Config(current):
  def aff(k):
    notif("[{0}] => {1}".format(k,current[k]))

  for k in cfg.mains:
    aff(k)
  print("")

  for k in current.keys():
    if not k in cfg.mains and not k in cfg.non_persistent:
      aff(k)
  print("")

  for k in cfg.non_persistent:
    aff(k)
