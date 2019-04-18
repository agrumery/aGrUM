#!/usr/bin/python
# -*- coding: utf-8 -*-# ***************************************************************************
# *   Copyright (C) 2015 by Pierre-Henri WUILLEMIN                          *
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

from __future__ import print_function

import sys
from math import sqrt
from subprocess import PIPE, Popen, STDOUT
from time import localtime

from .builder import getCmake, getMake, getPost
from .configuration import cfg
from .invocation import getInvocation
from .utils import notif, safe_cd


def simple_stats(n, s, s2):
  return (s / n), sqrt(s2 / n - pow(s / n, 2))


def profileAgrum(current):
  dic = {k: current[k] for k in current.keys()}
  dic['stats'] = False
  dic['fixed_seed'] = True
  dic['no-fun'] = True

  command = getInvocation(dic)

  target = "+".join(current['targets'])
  safe_cd(current, "build")
  safe_cd(current, current["mode"])

  notif("[cmake]")
  cm = getCmake(dic, target)
  proc = Popen(cm, shell=True, stdout=PIPE, stderr=STDOUT)
  out = proc.stdout.readlines()
  if current['verbose']:
    for line in out:
      print(line, end="")

  notif("[make]")
  ma = getMake(dic, target)
  proc = Popen(ma, shell=True, stdout=PIPE, stderr=STDOUT)
  out = proc.stdout.readlines()
  if current['verbose']:
    for line in out:
      print(line, end="")

  po, rc = getPost(dic, target)

  notif("[{0} runs] (please be patient) ...".format(cfg.nbr_tests_for_stats))
  sdt = 0
  sdt2 = 0
  print("--   n :     duration   mean      stdev   final time")
  for i in range(cfg.nbr_tests_for_stats):
    proc = Popen(po, shell=True, stdout=PIPE, stderr=STDOUT)

    out = proc.stdout.readlines()
    dt = 0
    for line in out:
      if "Profiling" in line:
        dt = float(line.split(" ")[3]) / 1000.0

    sdt += dt
    sdt2 += dt * dt
    mean, stdev = simple_stats(i + 1, sdt, sdt2)
    notif(" {0:2} : [{1:10.3f}] [{2:10.3f}] [{3:10.3f}]   {4}:{5}".format(i, dt, mean, stdev, localtime().tm_hour,
                                                                          localtime().tm_min))  # sdt/(i+1),sdt2/(i+1)))

  safe_cd(current, "..")
  safe_cd(current, "..")

  sys.exit(0)
