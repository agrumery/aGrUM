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

#################################################################################################
# find make, python2, python3
def is_tool(prog):
    progw=prog+".exe"
    for dir in os.environ['PATH'].split(os.pathsep):
        if os.path.exists(os.path.join(dir, prog)):
            return os.path.join(dir, prog)
        if os.path.exists(os.path.join(dir, progw)):
            return os.path.join(dir, progw)
    return None


import platform
from sys import platform as os_platform

exe_python2=is_tool('python2')
exe_python3=is_tool('python3')
if exe_python2 is None and exe_python3 is None:
	exe_py=is_tool('python')
	if exe_py is not None: #would be absurd, wouldn't it ?
		if platform.python_version()[0]=="3":
			exe_python3=exe_py
		else:
			exe_python2=exe_py


MAKE=""
if is_tool("make"):
  MAKE='make '
  CMAKE='cmake '
elif is_tool("mingw32-make.exe"):
  MAKE='mingw32-make.exe '
  CMAKE='cmake -G "MinGW Makefiles" '

if MAKE=="":
  print("No <make> utility found. Exit")
  exit(1)
