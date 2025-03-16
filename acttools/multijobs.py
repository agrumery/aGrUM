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

import io
import os
import sys
from subprocess import PIPE, Popen, call
from threading import Thread

from .configuration import cfg
from .utils import error

try:
  from Queue import Queue, Empty
except ImportError:
  from queue import Queue, Empty  # python 3.x

ON_POSIX = 'posix' in sys.builtin_module_names


def prettifying_errors(line: str) -> str:
  res = ""

  if line == "":
    return ""

  # swig prettyfying
  s = line.split("aGrUM-dev/wrappers/")
  if len(s) == 2:
    ss = s[1].split("src/agrum")
    if len(ss) >= 2:
      s[1] = "agrum" + ss[1]
    else:
      ss = s[1].split("../swig")
      if len(ss) >= 2:
        s[1] = "swig" + ss[1]

    t = s[1].split(": ")
    if len(t) == 3:
      return f"{res} {cfg.C_MSG}{t[0]}{cfg.C_END}: {cfg.C_ERROR}{t[1]}{cfg.C_END}: {cfg.C_VALUE}{t[2]}{cfg.C_END}"
    else:
      return f"{res}{s[1]}"
  return line


def prettifying(line: str) -> str:
  # prettifying the line
  res = ""

  if line == "":
    return ""

  for eop in ['done', 'works', 'Success', 'found']:
    leop = len(eop)
    try:
      if line[-leop:] == eop:
        line = f"{line[:-leop]}{cfg.C_VALUE}{eop}{cfg.C_END}"
        break
    except TypeError:
      return f"((({line})))"

  # prettifying compilation
  s = line.split("%]")
  if len(s) >= 2:
    res = cfg.C_WARNING + "%]".join(s[:-1]) + "%]" + cfg.C_END + " "
    line = s[-1].strip()

  if line == "":
    return res.rstrip()

  def remove_dirs(path: str) -> str:
    return path[:path.rfind(" ")]

  # prettifying (compacting) path
  if line[0] == "/":  # we keep message beginning with full path
    return res + line

  s = line.split("agrum.dir/")
  if len(s) == 2:
    return f"{res}{remove_dirs(s[0])} {cfg.C_MSG}{s[1].rstrip()}{cfg.C_END}"

  s = line.split("_pyAgrum.dir/__/__/")  # for agrum in pyAgrum
  if len(s) == 2:
    return f"{res}{remove_dirs(s[0])} {cfg.C_MSG}{s[1].rstrip()}{cfg.C_END}"

  s = line.split("_pyAgrum.dir/")  # for specific pyAgrum files
  if len(s) == 2:
    return f"{res}{remove_dirs(s[0])} {cfg.C_MSG}{s[1].rstrip()}{cfg.C_END}"

  s = line.split("/generated-files/")
  if len(s) == 2:
    return f"{res}{remove_dirs(s[0])} {cfg.C_MSG}generated-files/{s[1].rstrip()}{cfg.C_END}"

  s = line.split(" /")
  if len(s) == 2:
    return f"{res}{remove_dirs(s[0])} {cfg.C_MSG}/{s[1].rstrip()}{cfg.C_END}"

  # prettifying test execution
  s = line.split(". [")
  if len(s) == 2:
    return f"{res}{s[0]}. {cfg.C_VALUE}[{s[1].rstrip()}{cfg.C_END}"

  s = line.split("# [")
  if len(s) == 2:
    return f"{res}{s[0]}# {cfg.C_VALUE}[{s[1].rstrip()}{cfg.C_END}"

  s = line.split(" ... ")
  if len(s) == 2:
    ss = s[0].split('(')
    if len(ss) == 2:
      return f"{res}{cfg.C_WARNING}{ss[0]}{cfg.C_VALUE}({ss[1]}{cfg.C_END} ... {s[1]}"

    ss = s[0].split("-")
    if len(ss) == 2:
      sss = ss[1].split(".")
      return f"{res}{cfg.C_WARNING}{ss[0]}{cfg.C_VALUE} {sss[0]}{cfg.C_END} ... {s[1]}"

  # end of test execution
  s = line.split("##")
  if len(s) == 3:
    return f"{res}{cfg.C_WARNING}##{cfg.C_END}{s[1]}{cfg.C_MSG}##{cfg.C_END}{s[2]}"

  if line[0:6] == "Failed":
    return f"Failed {cfg.C_MSG}{line[7:]}{cfg.C_END}"
  if line[0:6] == "Succes":
    return f"Success {cfg.C_MSG}{line[14:]}{cfg.C_END}"

  if line.endswith("<--- failed"):
    return f"{res}{line[0:-11]}{cfg.C_ERROR}<--- failed{cfg.C_END}"

  s = line.split("Memory leaks found")
  if len(s) == 2:
    return f"{res}{s[0]}{cfg.C_ERROR}Memory leaks found{cfg.C_END}{s[1]}"

  return line


def threaded_execution(cde):
  os.environ["PYTHONUNBUFFERED"] = "1"
  p = Popen(cde, shell=True, bufsize=1, stdout=PIPE, stderr=PIPE)

  inp = Queue()
  sout = io.open(p.stdout.fileno(), 'rb', closefd=False)
  serr = io.open(p.stderr.fileno(), 'rb', closefd=False)

  def Pump(stream, category):
    queue = Queue()

    def rdr():
      while True:
        buf = stream.read1(8192)
        if len(buf) > 0:
          queue.put(buf)
        else:
          queue.put(None)
          return

    def clct():
      active = True
      while active:
        r = queue.get()
        try:
          while True:
            r1 = queue.get(timeout=0.005)
            if r1 is None:
              active = False
              break
            else:
              r += r1
        except Empty:
          pass
        inp.put((category, r))

    for tgt in [rdr, clct]:
      th = Thread(target=tgt)
      th.daemon = True
      th.start()

  Pump(sout, 'stdout')
  Pump(serr, 'stderr')

  waiter = "|/-\\"
  w_pos = 0

  def readerLoop(lastline: str) -> str:
    chan, lines = inp.get(True, timeout=0.1)
    if chan == 'stdout' and lines is not None:
      try:
        lines = (lines.decode('utf-8')).split("\n")
      except UnicodeDecodeError:
        try:
          lines = lines.split("\n")
        except:
          print('ERRROR')
          print(lines)

      for i in range(len(lines) - 1):
        if lines[i] != "":
          print(prettifying(lines[i]).rstrip())
      lastline = lines[-1]
      print(prettifying(lastline).rstrip(), end="")
      sys.stdout.flush()
    elif chan == 'stderr' and lines is not None:
      lastline = ""
      lines = lines.decode('utf-8').split("\n")
      for line in lines:
        if line != "":
          if line[0] == "/":
            print(prettifying_errors(line))
          else:
            if "IPKernelApp" in line:
              pass  # do nothing for Kernel App warnings
            else:
              error(line)
    return lastline

  last = ""
  while p.poll() is None:
    # App still working
    try:
      last = readerLoop(last)
    except Empty:
      print(waiter[w_pos] + "\b", end="")
      sys.stdout.flush()
      w_pos = (w_pos + 1) % len(waiter)

  # flushing the buffers
  while True:
    try:
      last = readerLoop(last)
    except Empty:
      break

  return p.returncode


def execCde(commande, current):
  if current["no_fun"]:
    rc = call(commande, shell=True)
  else:
    rc = threaded_execution(commande)

  return rc
