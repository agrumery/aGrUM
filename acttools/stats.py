#!/usr/bin/python
# -*- coding: utf-8 -*-# ***************************************************************************
# *   Copyright (c) 2015 by Pierre-Henri WUILLEMIN                          *
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
import sys
from math import sqrt
from subprocess import PIPE, Popen, STDOUT
from time import localtime

from .builder import getCmake, getMake, getPost
from .configuration import cfg
from .invocation import getInvocation
from .utils import notif, safe_cd


def simple_stats(n, s, s2):
  v = (s2/n - pow(s/n,2))
  if n==1:
    sig=sqrt(v)
    halfA=0
  else:
    sig=sqrt(v/(n-1))
    halfA=1.96*sig
  return (s / n), sig, halfA


def profileAgrum(current):
  dic = {k: current[k] for k in current.keys()}
  dic['stats'] = False
  dic['fixed_seed'] = True
  dic['no-fun'] = True

  command = getInvocation(dic)

  target = "+".join(current['targets'])
  safe_cd(current, "build")
  safe_cd(current, "agrum")
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

  notif(f"[{cfg.nbr_tests_for_stats} runs] (please be patient) ...")
  sdt = 0
  sdt2 = 0
  
  dmin=float('inf')
  dmax=float('-inf')

  notif("")
  notif("-------------|------------------|-------------------|")
  notif("  time   #it |    XP (stdev)    | confid. interval  |")
  notif("-------------|------------------|-------------------|")
  for i in range(cfg.nbr_tests_for_stats):
    dt=0
    while dt==0:
      proc = Popen(po, shell=True, stdout=PIPE, stderr=STDOUT)

      out = proc.stdout.readlines()
      for line in out:
        if b"Profiling" in line:
          t=line.split(b" ")
          v=t[4] if len(t[3])==0 else t[3]
          dt = float(v) / 1000.0
      if dt==0:
        notif(f"[error : duration=0]")
      if dt>dmax:
        dmax=dt
      if dt<dmin:
        dmin=dt

    sdt += dt
    sdt2 += dt * dt
    mean, stdev, halfA = simple_stats(i + 1, sdt, sdt2)
    t=localtime()
    notif(f"{t.tm_hour:02d}:{t.tm_min:02d}:{t.tm_sec:02d} #[{i+1:02d}] | {dt:7.3f}s ({stdev:3.3f}) | [{mean:8.3f} ± {halfA:3.3f}]s |")

  notif("-------------|------------------|-------------------|")
  notif("  time   #it |    XP (stdev)    | confid. interval  |")
  notif("-------------|------------------|-------------------|")
  notif(f"=> Conclusion on {cfg.nbr_tests_for_stats} iteration(s) : {dmin:8.3f} <-- [{mean:8.3f} ± {halfA:3.3f}] --> {dmax:8.3f}")

  safe_cd(current, "..")
  safe_cd(current, "..")

  sys.exit(0)
