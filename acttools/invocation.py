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
def _getParam(name):
  return C_END+'--'+C_ERROR+name

def getParam(name):
  return _getParam(name)+" "

def getValParam(name,val):
  return _getParam(name)+C_END+'='+C_VALUE+val+" "

def getCommand(name):
  return OKGREEN+name+" "

def showInvocation(current):
    invocation=C_WARNING+"invocation"+C_END+" : "+"act "

    invocation+=getCommand(current['rule'])

    if not current['rule'] in "show clean uninstall default".split():
      if current['rule']=='wrapper':
        invocation+=getCommand(current['wrapper'])
      else:
        invocation+=getCommand(current['option'])

      invocation+=getValParam('module',current['module'])

      if (current['rule']=='test'):
        invocation+=getValParam('testlist',current['testlist'])
        if current['test_base']:
          invocation+=getParam("with-test-base")
        else:
          invocation+=getParam("without-test-base")

      if (current['rule']=='install'):
        invocation+=getValParam('dest',current['destination'])

      invocation+=getValParam('platform',current['platform'])

      if (current['verbose']):
        invocation+=getParam('verbose')
      else:
        invocation+=getParam('quiet')

      if (current['static_lib']):
        invocation+=getParam('static_lib')

      if (current['stats']):
        invocation+=getParam('stats')

      if (current['1by1']):
        invocation+=getParam('1by1')

      if (current['fixed_seed']):
        invocation+=getParam('fixed_seed')

      if (current['no_fun']):
        invocation+=getParam('no_fun')

      invocation+=getValParam('jobs',str(current['jobs']))

      invocation+=getValParam('python',current['pyversion'])
    else:
      if current['rule']=="uninstall":
        invocation+=getCommand(current['option'])

    invocation+=C_END
    return invocation
