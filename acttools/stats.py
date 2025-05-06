############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import sys
from math import sqrt
from subprocess import PIPE, Popen, STDOUT
from time import localtime
from typing import Any

from .builder import getCmake, getMake, getPost
from .configuration import cfg
from .utils import notif, safe_cd


def simple_stats(n, s, s2) -> tuple[float, float, float]:
  v = s2 / n - pow(s / n, 2)
  if n == 1:
    sig = sqrt(v)
    halfA = 0
  else:
    sig = sqrt(v / (n - 1))
    halfA = 1.96 * sig
  return (s / n), sig, halfA


def profileAgrum(current: dict[str, Any]):
  dic = {k: current[k] for k in current.keys()}
  dic["stats"] = False
  dic["fixed_seed"] = True
  dic["no-fun"] = True

  target = "+".join(current["targets"])
  safe_cd(current, "build")
  safe_cd(current, "agrum")
  safe_cd(current, current["mode"])

  notif("[cmake]")
  cm = getCmake(dic, target)
  proc = Popen(cm, shell=True, stdout=PIPE, stderr=STDOUT)
  out = proc.stdout.readlines()
  if current["verbose"]:
    for line in out:
      print(line, end="")

  notif("[make]")
  ma = getMake(dic, target)
  proc = Popen(ma, shell=True, stdout=PIPE, stderr=STDOUT)
  out = proc.stdout.readlines()
  if current["verbose"]:
    for line in out:
      print(line, end="")

  po, _ = getPost(dic, target)

  notif(f"[{cfg.nbr_tests_for_stats} runs] (please be patient) ...")
  sdt = 0
  sdt2 = 0

  dmin = float("inf")
  dmax = float("-inf")

  mean = halfA = 0

  notif("")
  barre = "-------------|------------------|-------------------|"
  notif(barre)
  notif("  time   #it |    XP (stdev)    | confid. interval  |")
  notif(barre)
  for i in range(cfg.nbr_tests_for_stats):
    dt = 0
    while dt == 0:
      proc = Popen(po, shell=True, stdout=PIPE, stderr=STDOUT)

      out = proc.stdout.readlines()
      for line in out:
        if b"Profiling" in line:
          t = line.split(b" ")
          v = t[4] if len(t[3]) == 0 else t[3]
          dt = float(v) / 1000.0
      if dt == 0:
        notif("[error : duration=0]")
      if dt > dmax:
        dmax = dt
      if dt < dmin:
        dmin = dt

    sdt += dt
    sdt2 += dt * dt
    mean, stdev, halfA = simple_stats(i + 1, sdt, sdt2)
    t = localtime()
    notif(
      f"{t.tm_hour:02d}:{t.tm_min:02d}:{t.tm_sec:02d} #[{i + 1:02d}] | {dt:7.3f}s ({stdev:3.3f}) | [{mean:8.3f} ± {halfA:3.3f}]s |"
    )

  notif(barre)
  notif("  time   #it |    XP (stdev)    | confid. interval  |")
  notif(barre)
  notif(
    f"=> Conclusion on {cfg.nbr_tests_for_stats} iteration(s) : {dmin:8.3f} <-- [{mean:8.3f} ± {halfA:3.3f}] --> {dmax:8.3f}"
  )

  safe_cd(current, "..")
  safe_cd(current, "..")

  sys.exit(0)
