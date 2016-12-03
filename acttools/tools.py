#!/usr/bin/python
# -*- coding: utf-8 -*-

# ***************************************************************************
# *   Copyright (C) 2015 by Pierre-Henri WUILLEMIN                          *
# *   {prenom.nom}_at_lip6.fr                                               *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
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
# ***************************************************************************
import os
from subprocess import PIPE, Popen

from .utils import notif, critic


def cmdline(command):
  process = Popen(
      args=command,
      stdout=PIPE,
      shell=True
  )
  return process.communicate()[0].decode()


#################################################################################################
# find make, python2, python3
def is_tool(prog, longpath=False):
  progw = prog + ".exe"
  for dir in os.environ['PATH'].split(os.pathsep):
    if os.path.exists(os.path.join(dir, prog)):
      return prog if not longpath else os.path.join(dir, prog)
    if os.path.exists(os.path.join(dir, progw)):
      return progw if not longpath else os.path.join(dir, progw)
  return None


def check_tools(options):
  if options.python == "3":
    exe_py = is_tool('python3', True)
    if exe_py is None:
      exe_py = is_tool('python', True)
  else:
    exe_py = is_tool('python2', True)
    if exe_py is None:
      exe_py = is_tool('python', True)
  print(exe_py)
  version = cmdline(exe_py + ' -c "from distutils import sysconfig;print((sysconfig.get_python_version())[0])"')[0]

  if version == "2":
    if options.python == "3":
      notif('python3 not found. Using python2 instead')
      options.python = "2"
  elif version == "3":
    if options.python == "2":
      notif('python2 not found. Using python3 instead')
      options.python = "3"
  else:
    critic("No version found for python. Found version : <{}>".format(version))

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
  for version in range(9, 5, -1):
    if is_tool("clang-format-3.{}".format(version)):
      exe_clangformat = "clang-format-3.{}".format(version)
      break

  exe_msbuild = is_tool("msbuild")

  return exe_py, exe_cmake, exe_make, exe_clangformat, exe_msbuild
