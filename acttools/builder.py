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
from utils import trace,setifyString,safe_cd
from multijobs import execCde

def buildCmake(current):
    line="cmake .."

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

    line+=" -DBUILD_ALL=OFF"
    for module in setifyString(current["modules"]):
        line+=" -DBUILD_"+module+"=ON"

    if current["fixed_seed"]:
        line+=" -DGUM_RANDOMSEED="+cfg.fixedSeedValue
    else:
        line+=" -DGUM_RANDOMSEED=0"

    execFromLine(current,line)


def buildMake(current):
    line="make"

    if current["action"]=="test":
      if current["targets"]==set(["aGrUM"]):
        line+=" gumTest"

    if current["action"]=="install":
      line+=" install"
      if current["targets"]==set(["pyAgrum"]):
        line+=" -C wrappers/pyAgrum"

    if current["action"]=="uninstall":
      line+=" uninstall"
      if current["targets"]==set(["pyAgrum"]):
        line+=" -C wrappers/pyAgrum"

    line+=" -j "+str(current["jobs"])

    execFromLine(current,line)



def buildPost(current):
    if current["action"]=="test":
      if current["targets"]==set(["aGrUM"]):
        safe_cd(current,"src")
        execFromLine(current,"gumTest")
        safe_cd(current,"..")


def execFromLine(current,line):
    trace(current,line)
    if not current['dry_run']:
        return execCde(line,current)


