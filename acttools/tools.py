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
import platform
from sys import platform as os_platform

from .utils import critic

#################################################################################################
# find make, python2, python3
def is_tool(prog):
    progw=prog+".exe"
    for dir in os.environ['PATH'].split(os.pathsep):
        if os.path.exists(os.path.join(dir, prog)):
            return prog #os.path.join(dir, prog)
        if os.path.exists(os.path.join(dir, progw)):
            return progw #os.path.join(dir, progw)
    return None


def check_tools():
    exe_python2=is_tool('python2')
    exe_python3=is_tool('python3')
    if exe_python2 is None and exe_python3 is None:
    	exe_py=is_tool('python')
    	if exe_py is not None: #would be absurd, wouldn't it ?
    		if platform.python_version()[0]=="3":
    			exe_python3=exe_py
    		else:
    			exe_python2=exe_py


    exe_cmake=is_tool("cmake")
    if exe_cmake is None:
      critic("No <cmake> utility found. Exit")

    exe_make=is_tool("mingw32-make.exe")
    if exe_make is None:
      exe_make=is_tool("make")
    if exe_make is None:
      critic("No <make> utility found. Exit")

    exe_clangformat=None
    if is_tool("clang-format"):
        exe_clangformat="clang-format"
    for version in range(9,5,-1):
        if is_tool("clang-format-3.{}".format(version)):
            exe_clangformat="clang-format-3.{}".format(version)
            break

    exe_msbuild=is_tool("msbuild")

    return (exe_python2,exe_python3,exe_cmake,exe_make,exe_clangformat,exe_msbuild)
