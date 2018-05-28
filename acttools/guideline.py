#!/usr/bin/python
# -*- coding : utf-8 -*-

# ***************************************************************************
# * Copyright(C) 2015 by Pierre - Henri WUILLEMIN *
# * {prenom.nom } _at_lip6.fr *
# * *
# * This program is free software; you can redistribute it and / or modify *
# * it under the terms of the GNU General Public License as published by *
# * the Free Software Foundation; either version 2 of the License, or *
# *(at your option) any later version.*
# * *
# * This program is distributed in the hope that it will be useful, *
# * but WITHOUT ANY WARRANTY; without even the implied warranty of *
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the *
# * GNU General Public License for more details.*
# * *
# * You should have received a copy of the GNU General Public License *
# * along with this program; if not, write to the *
# * Free Software Foundation, Inc., *
# * 59 Temple Place - Suite 330, Boston, MA 02111 - 1307, USA.*
# ***************************************************************************

from __future__ import print_function
import os.path
from subprocess import call


from .utils import trace, notif, critic, warn, error, recglob, srcAgrum
from .configuration import cfg

from .missingDocs import computeNbrError

def guideline(current, modif=False):
  if modif:
    notif("[aGrUM guideline (with correction)]")
  else:
    notif("[aGrUM guideline]")

  nbrError = 0

  notif("  (1) [*.cpp] file for every [*.h] file ")
  nbrError += _checkCppFileExists(current, modif)
  notif("  (2) check for GPL license")
  nbrError += _checkForGPLlicense(current, modif)
  notif("  (3) check for format")
  nbrError += _checkForFormat(current, modif)
  notif("  (4) check for missing documentation in pyAgrum")
  nbrError += _checkForMissingDocs(modif)

  return nbrError


def _checkForFormat(current, modif):
  nbrError = 0
  if cfg.clangformat is None:
    error("No correct [clang-format] tool has been found.")
  else:
    with open(os.devnull, "w") as blackhole:
      for src in srcAgrum():
        exceptions = ['/external/', 'Parser', 'Scanner']
        if any(subs in src for subs in exceptions):
          continue

        line = cfg.clangformat + " " + src + " | cmp " + src + " -"
        if call(line, shell=True, stderr=blackhole, stdout=blackhole)==1:
          nbrError += 1
          if modif:
            line = cfg.clangformat + " -i " + src
            call(line, shell=True)
            notif("    [" + src + "] not correctly formatted : [changed]")
          else:
            notif("    [" + src + "] not correctly formatted")
  return nbrError

def __addGPLatTop(filename):
  with open(filename, "r") as origine:
    code = origine.read()
  with open(filename, "w") as dest:
    dest.write(_template_license)
    dest.write(code)


def _checkForGPLlicense(current, modif):
  nbrError = 0

  exceptions = ['/mvsc/', '/external/', '/cxxtest/', 'Parser', 'Scanner']
  for agrumfile in srcAgrum():
    if any(subs in agrumfile for subs in exceptions):
      continue

    fragment = ""
    nbr = 0
    with open(agrumfile, "r") as f:
      for line in f:
        if nbr == 4:
          continue
        fragment += line
        nbr += 1

    if "Copyright (C) 20" not in fragment:
      nbrError += 1
      if modif:
        __addGPLatTop(agrumfile)
        notif("    [" + agrumfile + "] has no copyright in its first lines : [changed]")
      else:
        notif("    [" + agrumfile + "] has no copyright in its first lines")

  return nbrError


def __addCppFileForHeader(header, cppfile):
  subinclude = header[4:]  # remove the /src
  cppfile = header[:-1] + "cpp"  # name

  with open(cppfile, 'w') as out:
    out.write(_template_cpp.replace("{include_file}", subinclude))


def _checkCppFileExists(current, modif):
  nbrError = 0

  exceptions = ['/mvsc/', '/signal/', '/external/', 'multidim/patterns/', 'agrum.h', 'inline.h']
  for header in recglob("src/agrum", "*.h"):
    if any(subs in header for subs in exceptions):
      continue

    subs = header[:-1]
    if subs.endswith("_tpl."):
      continue
    if subs.endswith("_inl."):
      continue
    cppfile = subs + "cpp"
    if not os.path.isfile(cppfile):
      nbrError += 1
      if modif:
        __addCppFileForHeader(header, cppfile)
        error("No cpp file for [" + header + "h] : [added]")
      else:
        error("No cpp file for [" + header + "h]")

  return nbrError

def _checkForMissingDocs(modif):
  nbrError = computeNbrError(True)
  if(nbrError>0):
    if(nbrError==1):
      error(str(nbrError)+" undocumented method")
    else:
      error(str(nbrError)+" undocumented methods")
  return nbrError

_template_license = """
/**************************************************************************
*   Copyright (C) 2017 by Pierre-Henri WUILLEMIN  and Christophe GONZALES *
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
_template_cpp = _template_license + """

/**
 * @file
 * @brief Class to include at least once this header
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */

#include <{include_file}>

"""
