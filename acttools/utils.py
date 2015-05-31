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

import os
from configuration import cfg

def safe_cd(current,folder):
  trace(current,"cd "+folder)
  if not current['dry_run']:
    if folder!="..":
      if not os.path.exists(folder):
        os.mkdir(folder)
    os.chdir(folder)

def trace(current,cde):
  if current['dry_run'] or current['verbose']:
    notif(cde)

def notif(s):
  print("-- "+cfg.C_VALUE+s+cfg.C_END)

def warn(s):
  if cfg.verbosity:
    print("-- "+cfg.C_WARNING+s+cfg.C_END)

def error(s):
  print("-- "+cfg.C_ERROR+s+cfg.C_END)

def critic(s):
  error(s)
  print("\n-- "+cfg.C_ERROR+"stopped."+cfg.C_END+"\n")
  sys.exit(1)
