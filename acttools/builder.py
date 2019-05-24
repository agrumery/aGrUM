#!/usr/bin/python
# -*- coding: utf-8 -*-
# ***************************************************************************
# *   Copyright (C) 2015 by Pierre-Henri WUILLEMIN                          *
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
import platform
import multiprocessing
import os

from .configuration import cfg
from .multijobs import execCde
from .utils import trace, setifyString, critic,notif


def getCmake(current, target):
  line = cfg.cmake + " ../.."  # we are in build/[release|target]

  line += " -DCMAKE_EXPORT_COMPILE_COMMANDS=ON "  # for clang-tidy

  if current["mode"] == "release":
    line += " -DCMAKE_BUILD_TYPE=RELEASE"
  else:
    line += " -DCMAKE_BUILD_TYPE=DEBUG"

  if current["withSQL"]:
    line += " -DUSE_NANODBC=ON"
  else:
    line += " -DUSE_NANODBC=OFF"

  line += " -DCMAKE_INSTALL_PREFIX=" + '"' + current["destination"] + '"'

  if current["verbose"]:
    line += " -DCMAKE_VERBOSE_MAKEFILE=ON"
  else:
    line += " -DCMAKE_VERBOSE_MAKEFILE=OFF"

  if current["static_lib"]:
    line += " -DBUILD_SHARED_LIBS=OFF"
  else:
    line += " -DBUILD_SHARED_LIBS=ON"

  if current["coverage"]:
    line += " -DGUM_COVERAGE=ON"
  else:
    line += " -DGUM_COVERAGE=OFF"

  line += " -DBUILD_ALL=OFF"
  for module in setifyString(current["modules"]):
    line += " -DBUILD_" + module + "=ON"

  if current["fixed_seed"]:
    line += " -DGUM_RANDOMSEED=" + cfg.fixedSeedValue
  else:
    line += " -DGUM_RANDOMSEED=0"

  if target != "pyAgrum":
    line += " -DBUILD_PYTHON=OFF"
  else:
    line += " -DBUILD_PYTHON=ON"

  line += " -DPYAGRUM_REQUIRED_PYTHON_VERSION=" + current["python"]

  line += " -DPYTHON_TARGET=" + cfg.python

  if platform.system() == "Windows":
    if current["mvsc"]:
      line += ' -G "Visual Studio 14 2015 Win64"'
    elif current["mvsc32"]:
      line += ' -G "Visual Studio 14 2015"'
    elif current["mvsc17"]:
      line += ' -G "Visual Studio 15 2017 Win64"'
    elif current["mvsc17_32"]:
      line += ' -G "Visual Studio 15 2017"'
    elif current["mingw64"]:
      line += ' -G "MinGW Makefiles"'

  return line


def buildCmake(current, target):
  line = getCmake(current, target)
  execFromLine(current, line)


def getMake(current, target):
  if current["mvsc"] or current["mvsc32"] or current["mvsc17"] or current["mvsc17_32"]:
    return getForMsBuildSystem(current, target)
  else:
    return getForMakeSystem(current, target)


def getNbrOfJobs(jobrequest):
  # number of jobs
  nbrProc = multiprocessing.cpu_count()
  if nbrProc == 1:
    return "1"
  else:
    if jobrequest == "except1":
      return str(nbrProc - 1)
    elif jobrequest == "half":
      return str(int(nbrProc / 2))  # >=1
    elif jobrequest == "all":
      return str(nbrProc)
    else:
      try:
        nbrJob = int(jobrequest)
        if nbrJob<1:
          nbrJob=1
        if nbrJob > nbrProc:
          return str(nbrProc)
        else:
          return str(nbrJob)
      except ValueError:
        return 1


def getForMsBuildSystem(current, target):
  if cfg.msbuild is None:
    critic("MsBuild not found")
  else:
    nbrJobs=getNbrOfJobs(current['jobs'])
    notif("Compilation using ["+nbrJobs+"] jobs.")
    if current["action"] == "test":
      if target == "aGrUM":
        line = cfg.msbuild + ' agrum.sln /t:gumTest /p:Configuration="Release"'
      elif target == "pyAgrum":
        line = cfg.msbuild + ' agrum.sln /t:_pyAgrum /p:Configuration="Release"'
      else:  # if target!= "pyAgrum":
        critic(
            "Action '" + current[
              "action"] + "' not treated for target '" + target + "' for now in windows weird world.")
    elif current["action"] == "install":
      line = cfg.msbuild + ' INSTALL.vcxproj /p:Configuration="Release"'
    else:
      critic("Action '" + current["action"] + "' not treated for now in windows weird world.")
    line += ' /p:BuildInParallel=true /maxcpucount:' + nbrJobs
  return line


def getForMakeSystem(current, target):
  line = cfg.make

  nbrJobs=getNbrOfJobs(current['jobs'])
  notif("Compilation using  ["+nbrJobs+"] jobs.")

  if current["action"] == "test":
    if target == "aGrUM":
      line += " gumTest"
    elif target != "pyAgrum":
      critic("Action '" + current["action"] + "' not treated for target '" + target + "'.")
  elif current["action"] == "install":
    line += " install"
  elif current["action"] == "uninstall":
    line += " uninstall"
  elif current["action"] == "lib":
    pass
  elif current["action"] == "doc":
    line += " doc"
  else:
    critic("Action '" + current["action"] + "' not treated for now")

  line += " -j " + nbrJobs

  if target == "pyAgrum":
    line += " -C wrappers/pyAgrum"

  return line


def buildMake(current, target):
  line = getMake(current, target)
  execFromLine(current, line)


def getPost(current, target):
  if current["action"] == "test":
    if target == "aGrUM":
      if cfg.os_platform == "win32":
        if current["mingw64"]:
          line = "src\\gumTest.exe"
        else:
          line = "src\\Release\\gumTest.exe"  # debug or release create Release folder
      else:
        line = "src/gumTest"
      return line, True
    elif target == "pyAgrum":
      if current['tests'] == 'quick':
        gumTest = "gumTest.py"
      elif current['tests'] == 'all':  # all is with NOTEBOOKStest
        gumTest = "gumTest.py all"
      else:
        critic("Only [-t all] or [-t quick] for testing pyAgrum.")

      if cfg.os_platform == "win32":
        line = 'copy /Y "wrappers\pyAgrum\Release\_pyAgrum.pyd" "wrappers\pyAgrum\." & ' + cfg.python + " ..\\..\\wrappers\\pyAgrum\\testunits\\" + gumTest
      else:
        line = cfg.python + " ../../wrappers/pyAgrum/testunits/" + gumTest
      line += " " + current['mode']
      return line, True
  return "", False


def buildPost(current, target):
  line, checkRC = getPost(current, target)
  if line != "":
    execFromLine(current, line, checkRC)


def execFromLine(current, line, checkRC=True):
  trace(current, line)
  if not current['dry_run']:
    rc = execCde(line, current)
    if checkRC:
      if rc > 0:
        critic("Received error {0}".format(rc), rc=rc)
