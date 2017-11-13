#!/ usr / bin / python
#- * - coding : utf - 8 - * -

#***************************************************************************
#* Copyright(C) 2015 by Pierre - Henri WUILLEMIN *
#* {prenom.nom } _at_lip6.fr *
#* *
#* This program is free software; you can redistribute it and / or modify *
#* it under the terms of the GNU General Public License as published by *
#* the Free Software Foundation; either version 2 of the License, or *
#*(at your option) any later version.*
#* *
#* This program is distributed in the hope that it will be useful, *
#* but WITHOUT ANY WARRANTY; without even the implied warranty of *
#* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the *
#* GNU General Public License for more details.*
#* *
#* You should have received a copy of the GNU General Public License *
#* along with this program; if not, write to the *
#* Free Software Foundation, Inc., *
#* 59 Temple Place - Suite 330, Boston, MA 02111 - 1307, USA.*
#***************************************************************************

from __future__ import print_function
import os.path

from .utils import trace, notif, critic, warn, error, recglob, srcAgrum
from .configuration import cfg


def guideline(current,modif=False):
  notif("[aGrUM guideline]")
  notif("  # [*.cpp] file for every [*.h] file ")
  _checkCppFileExists(current,modif)
  notif("  # check for GPL license")
  _checkForGPLlicense(current,modif)

def __addGPLatTop(filename):
  with open(filename,"r") as origine:
    code=origine.read()
  with open(filename,"w") as dest:
    dest.write(_template_license)
    dest.write(code)

def _checkForGPLlicense(current,modif):
  exceptions=['/mvsc/','/external/','/cxxtest/','Parser','Scanner']
  for agrumfile in srcAgrum():
    if any(subs in agrumfile for subs in exceptions):
      continue

    fragment=""
    nbr=0
    with open(agrumfile,"r") as f:
      for line in f:
         if nbr==4:
           continue
         fragment+=line
         nbr+=1
        
    if "Copyright (C) 20" not in fragment:
      if modif:
        __addGPLatTop()
        notif("    ["+agrumfile+"] has no copyright in its first lines : [changed]")
      else:
        notif("    ["+agrumfile+"] has no copyright in its first lines")

def __addCppFileForHeader(header, cppfile):
  subinclude=header[4:] # remove the /src
  cppfile=header[:-1]+"cpp" # name

  with open(cppfile, 'w') as out:
    out.write(_template_cpp.replace("{include_file}", subinclude))

def _checkCppFileExists(current,modif):
  exceptions=['/mvsc/','/signal/','/external/','agrum.h','inline.h']

  for header in recglob("src/agrum", "*.h"):
    if any(subs in header for subs in exceptions):
      continue

    subs = header[:-1]
    if subs.endswith("_tpl."):
      continue
    if subs.endswith("_inl."):
      continue
    cppfile=subs+"cpp"
    if not os.path.isfile(cppfile):
      if modif:
        __addCppFileForHeader(header, cppfile)
        error("No cpp file for ["+header+"h] : [added]")
      else:
        error("No cpp file for ["+header+"h]")

_template_license="""
/***************************************************************************
*   Copyright (C) 2017 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
*   {prenom.nom}_at_lip6.fr                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/


"""
_template_cpp= _template_license+"""

/**
 * @file
 * @brief Class to include at least once this header
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */

#include <{include_file}>

"""
