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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2025                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import os
import sys
from os.path import isdir
import glob
from typing import Optional, Iterator

try:
  from icecream import ic

  ic.configureOutput(includeContext=True, contextAbsPath=True)
  ic.configureOutput(prefix="🍋 🐞 ➤  ")
except ImportError:
  ic = lambda *a: None if not a else a[0] if len(a) == 1 else a


from .configuration import cfg


def about():
  printutf8(
    f"{cfg.C_END}{cfg.C_WARNING}aGrUM{cfg.C_END}"
    + f" compilation tool {cfg.C_VALUE}{cfg.act_version}{cfg.C_END}"
    + f" for {cfg.C_WARNING}aGrUM/pyAgrum {cfg.C_VALUE}{cfg.gum_version}{cfg.C_END}"
  )
  printutf8(f"(c) 2010-2025 {cfg.C_MSG}aGrUM Team{cfg.C_END}", end="\n\n")


def setifyString(s: str) -> set[str]:
  # special case for accepting agrum instead of 'aGrUM'
  return {
    "aGrUM" if x.lower() == "agrum" else "pyAgrum" if x.lower() == "pyagrum" else x
    for x in s.split("+")
    if x.strip() != ""
  }


def safe_cd(current: dict[str, str | bool], folder: str):
  if folder != ".." and not os.path.exists(folder):
    os.mkdir(folder)
    if current["dry_run"]:
      notif(f"  (mkdir {folder})")
  notif(f"cd {folder}")
  os.chdir(folder)


def printutf8(s: str, end="\n"):
  """
  Print a string in utf-8 encoding, handling potential encoding issues.
  """
  try:
    print(s, end=end)
  except UnicodeEncodeError:
    try:
      print(s.encode("utf-8"), end=end)
    except UnicodeEncodeError:
      try:
        print(s.encode("utf-8", "replace"), end=end)
      except UnicodeEncodeError:
        print(s.encode("utf-8", "ignore"), end=end)


def colFormat(v: str, col: str) -> str:
  return f"{col}{v.replace('[', cfg.C_VALUE).replace(']', col)}"  # .encode("utf-8")}"


def trace(current: dict[str, str | bool], cde: str):
  if current["dry_run"] or current["verbose"]:
    notif(cde, cfg.prefix_trace)


def notif_oneline(s: str, pref: Optional[str] = None):
  if pref is None:
    pref = cfg.prefix_line

  printutf8(
    f"{pref}{colFormat(s, cfg.C_MSG)}{cfg.C_END}",
    end="                                       \r",
  )


def notif(s: str = "", pref: Optional[str] = None):
  if pref is None:
    pref = cfg.prefix_line

  printutf8(f"{pref}{colFormat(s, cfg.C_MSG)}{cfg.C_END}")


def warn(s: str, pref: Optional[str] = None):
  if pref is None:
    pref = cfg.prefix_line

  printutf8(f"{pref}⚠️  {colFormat(s, cfg.C_WARNING)}{cfg.C_END}")


def error(s: str, pref: Optional[str] = None):
  if pref is None:
    pref = cfg.prefix_line

  printutf8(f"{pref}💥 {colFormat(s, cfg.C_ERROR)}{cfg.C_END}")


def critic(s: str, pref: Optional[str] = None, rc: int = 1):
  if pref is None:
    pref = cfg.prefix_line

  error(s, pref)
  printutf8(f"{pref}{colFormat('Stopped.', cfg.C_MSG)}{cfg.C_END}\n\n")

  sys.exit(rc)


def cross_platform_rel_path(x: str, y: str) -> str:
  return os.path.relpath(x, y).replace("\\", "/")


def recglob(path: str, mask: str) -> Iterator[str]:
  for item in glob.glob(path + "/*"):
    if isdir(item):
      for item in recglob(item, mask):
        yield item

  for item in glob.glob(path + "/" + mask):
    if not isdir(item):
      yield item


def srcPyNotebooks() -> Iterator[str]:
  for i in recglob("wrappers/pyagrum/doc/sphinx/notebooks/", "*.ipynb"):
    yield i


def srcPyAgrum() -> Iterator[str]:
  for i in recglob("wrappers/pyagrum/testunits", "*.py"):
    yield i
  for i in recglob("wrappers/pyagrum/cmake", "*.py"):
    yield i
  for i in recglob("wrappers/pyagrum/pyLibs", "*.py"):
    yield i
  for i in recglob("acttools", "*.py"):
    yield i
  yield "act"


def srcPyIpynbAgrum() -> Iterator[str]:
  for i in srcPyNotebooks():
    yield i
  for i in srcPyAgrum():
    yield i


def srcCmakeAgrum() -> Iterator[str]:
  for i in recglob(".", "CMakeLists.txt"):
    yield i


def srcAgrum() -> Iterator[str]:
  for i in recglob("src/", "*.h"):
    yield i
  for i in recglob("src/", "*.cpp"):
    yield i
  for i in recglob("src/docs", "*.dox"):
    yield i
  for i in recglob("src/testunits", "*TestSuite.h"):
    yield i


def srcGeneratorAgrum() -> Iterator[str]:
  for i in recglob("wrappers/", "*.i"):
    yield i
  for i in recglob("src/", "*.atg"):
    yield i
