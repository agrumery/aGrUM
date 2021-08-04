#!/usr/bin/python
# -*- coding: utf-8 -*-# ***************************************************************************
# *   Copyright (c) 2015 by Pierre-Henri WUILLEMIN                          *
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
from subprocess import PIPE, Popen

from .configuration import cfg
from .utils import notif, critic


def cmdline(command):
  process = Popen(
    args=command,
    stdout=PIPE,
    shell=True
  )
  return process.communicate()[0].decode()


#################################################################################################
# find make, python3
def is_tool(prog, longpath=False):
  progw = prog + ".exe"
  for dir in os.environ['PATH'].split(os.pathsep):
    if os.path.exists(os.path.join(dir, prog)):
      return prog if not longpath else '"' + os.path.join(dir, prog) + '"'
    if os.path.exists(os.path.join(dir, progw)):
      return progw if not longpath else '"' + os.path.join(dir, progw) + '"'
  return None


def check_tools(options):
  exe_py = sys.executable
  
  version = cmdline(exe_py + ' -c "from distutils import sysconfig;print((sysconfig.get_python_version())[0])"')[0]
  subversion = cmdline(exe_py + ' -c "from distutils import sysconfig;print((sysconfig.get_python_version())[2:])"')[0]

  if version == "2":
    critic('python2 is not supported anymore. Please use pyAgrum 0.21.x.')
  else:
    if int(subversion)<7:
      critic('python<3.7 is not supported anymore. Please use pyAgrum 0.21.x.')
  cfg.python_version=f"{version}.{subversion}"

  exe_cmake = is_tool("cmake")
  if exe_cmake is None:
    critic("No <cmake> utility found. Exit")

  exe_make = is_tool("mingw32-make.exe")
  if exe_make is None:
    exe_make = is_tool("make")
  if exe_make is None:
    exe_make = is_tool("msbuild")
  if exe_make is None:
    critic("No <make> utility found. Exit")

  exe_clangformat = None
  if is_tool("clang-format"):
    exe_clangformat = "clang-format"
  if exe_clangformat is None:
    for version in ['', '-13.0', '-12.0', '-11.0']:
      if is_tool(f"clang-format{version}"):
        exe_clangformat = f"clang-format{version}"
        break

  exe_msbuild = is_tool("msbuild")

  return exe_py, exe_cmake, exe_make, exe_clangformat, exe_msbuild
