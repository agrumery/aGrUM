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
import sys
from subprocess import PIPE, Popen, STDOUT

from .utils import notif, safe_cd


def callSphinx(current):
  commande = 'act lib pyAgrum release'
  notif("Compiling pyAgrum")
  if not current['dry_run']:
    proc = Popen(commande + " --no-fun", shell=True, stdout=PIPE, stderr=STDOUT)
    out = proc.stdout.readlines()
    for line in out:
      sys.stdout.write(line)
  else:
    notif('[' + commande + ']')

  notif("Sphinxing pyAgrum")
  safe_cd(current, "wrappers")
  safe_cd(current, "pyAgrum")
  safe_cd(current, "doc")
  safe_cd(current, "sphinx")
  commande = 'make html'
  if not current['dry_run']:
    proc = Popen(commande, shell=True, stdout=PIPE, stderr=STDOUT)
    out = proc.stdout.readlines()
    for line in out:
      sys.stdout.write(line)
  else:
    notif('[' + commande + ']')
  safe_cd(current, "..")
  safe_cd(current, "..")
  safe_cd(current, "..")
