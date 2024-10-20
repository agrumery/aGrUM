#!/usr/bin/python
# -*- coding: utf-8 -*-# ***************************************************************************
# *   Copyright (c) 2015-2024 by Pierre-Henri WUILLEMIN                          *
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


def srcAgrum()->Iterator[str]:
  for i in recglob("src/agrum", "*.cpp"):
    yield i
  for i in recglob("src/agrum", "*.h"):
    yield i
  for i in recglob("src/testunits", "*TestSuite.h"):
    yield i
