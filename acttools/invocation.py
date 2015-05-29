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
from configuration import cfg

def _getParam(name):
  return cfg.C_END+'--'+cfg.C_ERROR+name

def getParam(name):
  return _getParam(name)+" "

def getValParam(name,val):
  return _getParam(name)+cfg.C_END+'='+cfg.C_VALUE+str(val)+" "

def getCommand(name):
  return cfg.C_WARNING+name+" "

def showInvocation(current,forced=False):
  if forced or not current['no_fun']:
    invocation=cfg.C_WARNING+"invocation"+cfg.C_END+" : "+"act "

    invocation+=getCommand(current['action'])
    invocation+=getCommand(current['mode'])
    invocation+=getCommand("+".join(current['targets']))

    for opt in current.keys():
      if not opt in ['action','mode','targets']:
        if not opt in cfg.non_persistent:
          invocation+=getValParam(opt,current[opt])

    for opt in cfg.non_persistent:
      if current[opt]:
        invocation+=getParam(opt)

    invocation+=cfg.C_END
    print(invocation)
    print("")
