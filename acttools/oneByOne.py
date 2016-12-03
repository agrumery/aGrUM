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

from .configuration import cfg
from .tests import checkTests, testNames
from .utils import notif, warn


def checkAgrumMemoryLeak(x, pourcent):
  commande = 'act test debug -t {0} -m all'.format(x)

  first = (cfg.C_VALUE + "[{:5.1f}%] ").format(pourcent)
  second = cfg.C_WARNING + x + cfg.C_END + " : "
  flag = 0

  sys.stdout.write(first + second)
  sys.stdout.flush()

  proc = Popen(commande + " --no-fun", shell=True, stdout=PIPE, stderr=STDOUT)
  out = proc.stdout.readlines()
  for line in out:
    if "NO MEMORY LEAK" in line:
      last = cfg.C_VALUE + "ok" + cfg.C_END
      flag = 1
    elif "Memory leaks found" in line:
      last = cfg.C_ERROR + line.split("|")[2].strip() + cfg.C_END
      flag = 2

  if flag == 0:
    last = cfg.C_ERROR + "?" + cfg.C_END

  print(last)
  return (second + last, flag == 1)


def checkAgrumMemoryLeaks(current):
  notif("Searching leaks test by test (may be a bit long).\n")

  res = []
  testslist = sorted(testNames(checkTests(current)))
  for i, x in enumerate(testslist):
    (msg, testOK) = checkAgrumMemoryLeak(x, (i + 1) * 100.0 / len(testslist))
    if not testOK:
      res.append(msg)

  warn("\n" + cfg.C_WARNING + "Test(s) with problem(s) :\n -{0}\n".format(
      "\n -".join(res) if len(res) > 0 else cfg.C_VALUE + "none") + cfg.C_END)
