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
from .configuration import cfg
from .utils import trace,setifyString,safe_cd,critic
from .multijobs import execCde

def buildCmake(current,target):
    line=cfg.cmake+" ../.." # we are in build/[release|target]

    if current["mode"]=="release":
        line+=" -DCMAKE_BUILD_TYPE=RELEASE"
    else:
        line+=" -DCMAKE_BUILD_TYPE=DEBUG"

    if current["python"]=="3":
        line+=" -DFOR_PYTHON2=OFF"
    else:
        line+=" -DFOR_PYTHON2=ON"

    line+=" -DCMAKE_INSTALL_PREFIX="+current["destination"]

    if current["verbose"]:
        line+=" -DCMAKE_VERBOSE_MAKEFILE=ON"
    else:
        line+=" -DCMAKE_VERBOSE_MAKEFILE=OFF"

    if current["static_lib"]:
        line+=" -DBUILD_SHARED_LIBS=OFF"
    else:
        line+=" -DBUILD_SHARED_LIBS=ON"

    if current["coverage"]:
      line+=" -DGUM_COVERAGE=ON"
    else:
      line+=" -DGUM_COVERAGE=OFF"

    line+=" -DBUILD_ALL=OFF"
    for module in setifyString(current["modules"]):
        line+=" -DBUILD_"+module+"=ON"

    if current["fixed_seed"]:
        line+=" -DGUM_RANDOMSEED="+cfg.fixedSeedValue
    else:
        line+=" -DGUM_RANDOMSEED=0"

    if target != "pyAgrum":
      line+=" -DBUILD_PYTHON=OFF"
    else:
      line+=" -DBUILD_PYTHON=ON"

    execFromLine(current,line)

def buildMake(current,target):
    line=cfg.make

    if current["action"]=="test":
      if target =="aGrUM":
        line+=" gumTest"
      elif target!= "pyAgrum":
        critic("Action '"+current["action"]+"' not treated for target '"+target+"'.")
    elif current["action"]=="install":
      line+=" install"
    elif current["action"]=="uninstall":
      line+=" uninstall"
    elif current["action"]=="lib":
      pass
    elif current["action"]=="doc":
      line+=" doc"
    else:
      critic("Action '"+current["action"]+"' not treated for now")

    line+=" -j "+str(current["jobs"])

    if target=="pyAgrum":
      line+=" -C wrappers/pyAgrum"

    execFromLine(current,line)



def buildPost(current,target):
    if current["action"]=="test":
        if target=="aGrUM":
            safe_cd(current,"src")
            if cfg.os_platform=="win32":
              rc=execFromLine(current,"gumTest.exe")
            else:
              rc=execFromLine(current,"./gumTest")
            safe_cd(current,"..")
        elif target=="pyAgrum":
            run_cde="PYTHONPATH=wrappers "
            if cfg.os_platform=="win32":
                run_cde+=cfg.python+" ..\\..\\wrappers\\pyAgrum\\testunits\\TestSuite.py"
            else:
                run_cde+=cfg.python+" ../../wrappers/pyAgrum/testunits/TestSuite.py"
            execFromLine(current,run_cde,checkRC=False)


def execFromLine(current,line,checkRC=True):
    trace(current,line)
    if not current['dry_run']:
        rc=execCde(line,current)
        if checkRC:
          if rc>0:
            critic("Received error {0}".format(rc),rc=rc)
