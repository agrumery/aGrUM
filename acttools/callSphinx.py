#!/usr/bin/python
# -*- coding: utf-8 -*-
# ***************************************************************************
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
from subprocess import PIPE, Popen, STDOUT

from .configuration import cfg
from .utils import notif, safe_cd


def _callSphinx(current: dict[str, str], cmd: str):
  if not current['dry_run']:
    proc = Popen(cmd, shell=True, stdout=PIPE, stderr=STDOUT)
    out = proc.stdout.readlines()
    for line in out:
      try:
        print(line.decode('utf-8'), end="")
      except ValueError:
        print(str(line), end="")
  else:
    notif('[' + cmd + ']')


def callSphinx(current: dict[str, str]):
  if current['build'] != 'doc-only':
    notif("Compiling pyAgrum")
    _callSphinx(current, f'{cfg.python} act lib pyAgrum release --no-fun')

  notif("Sphinxing pyAgrum")
  safe_cd(current, "wrappers")
  safe_cd(current, "pyAgrum")
  safe_cd(current, "doc")
  _callSphinx(current, 'make')

  safe_cd(current, "..")
  safe_cd(current, "..")
  safe_cd(current, "..")
