############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2024 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version.                              #
#    - the MIT license (MIT)                                               #
#    - or both in dual license, as here                                    #
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

import os
import sys
from os.path import isdir
import glob
from typing import Optional, Iterator

from .configuration import cfg


def about():
  print(f"{cfg.C_END}{cfg.C_WARNING}aGrUM{cfg.C_END} compilation tool {cfg.C_VALUE}{cfg.act_version}{cfg.C_END} for {cfg.C_WARNING}aGrUM/pyAgrum{cfg.C_VALUE}{cfg.gum_version}{cfg.C_END}")
  print(f"(c) 2010-2024 {cfg.C_MSG}aGrUM Team{cfg.C_END}")
  print("")


def setifyString(s:str)->set[str]:
  # special case for accepting agrum instead of 'aGrUM'
  return set(map(lambda x: 'aGrUM' if x.lower() == 'agrum' else 
                           'pyAgrum' if x.lower() == 'pyagrum' else x,       
                 filter(None, s.split("+"))))  # filter to setify "a++b+c" into set(['a','b','c'])


def safe_cd(current:dict[str,str], folder:str):
  trace(current, "cd " + folder)
  if not current['dry_run']:
    if folder != "..":
      if not os.path.exists(folder):
        os.mkdir(folder)
    os.chdir(folder)


def colFormat(v:str, col:str)->str:
  # s=str(v) # why should I need to stringify v ? If yes, warning with encoding : sometimes encode('utf-8') is needed
  return col + v.replace("[", cfg.C_VALUE).replace("]", col)


def trace(current:dict[str,str], cde:str):
  if current['dry_run'] or current['verbose']:
    notif(cde, cfg.prefix_trace)


def notif_oneline(s:str, pref:Optional[str]=None):
  if pref is None:
    pref = cfg.prefix_line

  print(pref + colFormat("** act Notification : " + s, cfg.C_MSG) + cfg.C_END,
        end="                                       \r")


def notif(s:str="", pref:Optional[str]=None):
  if pref is None:
    pref = cfg.prefix_line

  print(pref + colFormat("** act Notification : " + s, cfg.C_MSG) + cfg.C_END)


def warn(s:str, pref:Optional[str]=None):
  if pref is None:
    pref = cfg.prefix_line

  if cfg.verbosity:
    print(pref + colFormat("** act Warning      : " + s, cfg.C_WARNING) + cfg.C_END)


def error(s:str, pref:Optional[str]=None):
  if pref is None:
    pref = cfg.prefix_line

  print(pref + colFormat("** act Error        : " + s, cfg.C_ERROR) + cfg.C_END)


def critic(s:str, pref:Optional[str]=None, rc:int=1):
  if pref is None:
    pref = cfg.prefix_line

  error(s, pref)
  print(pref + colFormat("Stopped.", cfg.C_MSG) + cfg.C_END + "\n")

  sys.exit(rc)


def CrossPlatformRelPath(x:str, y:str)->str:
  return os.path.relpath(x, "src/testunits").replace("\\", "/")


def recglob(path:str, mask:str)->Iterator[str]:
  for item in glob.glob(path + "/*"):
    if isdir(item):
      for item in recglob(item, mask):
        yield item

  for item in glob.glob(path + "/" + mask):
    if not isdir(item):
      yield item


def srcPyAgrum()->Iterator[str]:
  for i in recglob("wrappers/pyAgrum/testunits", "*.py"):
    yield i
  for i in recglob("wrappers/pyAgrum/cmake", "*.py"):
    yield i
  for i in recglob("wrappers/pyAgrum/pyLibs", "*.py"):
    yield i
  for i in recglob("acttools", "*.py"):
    yield i

def srcAgrum()->Iterator[str]:
  for i in recglob("src/pyAgrum", "*.cpp"):
    yield i
  for i in recglob("src/pyAgrum", "*.h"):
    yield i
  for i in recglob("src/testunits", "*TestSuite.h"):
    yield i
