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
from utils import trace
from multijobs import execCde

def buildCmake(current):
    line="cmake .."

    if current["mode"]=="release":
        line+=" -DCMAKE_BUILD_TYPE=RELEASE"
    else:
        line+=" -DCMAKE_BUILD_TYPE=DEBUG"

    if current["pyversion"]=="3":
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

    trace(current,line)
    if not current['dry_run']:
        return execCde(line,options)


def buildMake(current):
    return "undone"

def buildPost(current):
    return "undone"
