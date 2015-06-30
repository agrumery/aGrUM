#!/usr/bin/python
# -*- coding: utf-8 -*-

#***************************************************************************
#*   Copyright (C) 2015 by Pierre-Henri WUILLEMIN                          *
#*   {prenom.nom}_at_lip6.fr                                               *
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License as published by  *
#*   the Free Software Foundation; either version 2 of the License, or     *
#*   (at your option) any later version.                                   *
#*                                                                         *
#*   This program is distributed in the hope that it will be useful,       *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU General Public License for more details.                          *
#*                                                                         *
#*   You should have received a copy of the GNU General Public License     *
#*   along with this program; if not, write to the                         *
#*   Free Software Foundation, Inc.,                                       *
#*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
#***************************************************************************
import sys
import os

from configuration import cfg

def about():
  print(cfg.C_END+cfg.C_WARNING+"aGrUM"+cfg.C_END+" compilation tool "+cfg.C_VALUE+cfg.numversion+cfg.C_END)
  print("(c) 2010-15 "+cfg.C_MSG+"aGrUM Team"+cfg.C_END)
  print("")

def setifyString(s):
  return set(filter(None,s.split("+"))) # filter to setify "a++b+c" into set(['a','b','c'])

def safe_cd(current,folder):
  trace(current,"cd "+folder)
  if not current['dry_run']:
    if folder!="..":
      if not os.path.exists(folder):
        os.mkdir(folder)
    os.chdir(folder)

def colFormat(v,col):
  s=str(v)
  return col+s.replace("[",cfg.C_VALUE).replace("]",col)

def trace(current,cde):
  if current['dry_run'] or current['verbose']:
    notif(cde,cfg.prefixe_trace)

def notif(s,pref=None):
  if pref is None:
    pref=cfg.prefixe_line

  print(pref+colFormat(s,cfg.C_MSG)+cfg.C_END)

def warn(s,pref=None):
  if pref is None:
    pref=cfg.prefixe_line

  if cfg.verbosity:
    print(pref+colFormat(s,cfg.C_WARNING)+cfg.C_END)

def error(s,pref=None):
  if pref is None:
    pref=cfg.prefixe_line

  print(pref+colFormat(s,cfg.C_ERROR)+cfg.C_END)

def critic(s,pref=None):
  if pref is None:
    pref=cfg.prefixe_line

  error(s,pref)
  print(pref+colFormat("Stopped.",cfg.C_MSG)+cfg.C_END+"\n")

  sys.exit(1)
