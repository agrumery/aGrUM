# -*- coding: utf-8 -*-
"""
Helping functions and consts for pyAgrum
"""

# aGrum Licence (GPL)
# -------------------
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
#*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

from @PYAGRUM_MODULE@ import BayesNet
from @PYAGRUM_MODULE@ import InfluenceDiagram

def about():
  print("pyAgrum version {0}".format('@PYAGRUM_VERSION@'))
  print("(c) Pierre-Henri Wuillemin and others")
  print("    UPMC 2014")
  print("""
    This is free software; see the source code for copying conditions.
    There is ABSOLUTELY NO WARRANTY; not even for MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  For details, see 'pyAgrum.warranty'.
    """)

def availableBNExts():
    """
    return the list of suffix for supported BN file formats.
    """
    return "bif|dsl|net|bifxml"

def loadBN(s,listeners=None):
    """
    returns a BN from a file using one of the availableBNExts() suffixes.
    """
    bn=BayesNet()

    extension=s.split('.')[-1].upper()
    if extension=="BIF":
        res=bn.loadBIF(s,listeners)
    elif extension=="BIFXML":
        res=bn.loadBIFXML(s,listeners)
    elif extension=="DSL":
        res=bn.loadDSL(s,listeners)
    elif extension=="NET":
        res=bn.loadNET(s,listeners)
    else:
        raise Exception("extension "+s.split('.')[-1]+" unknown. Please use "+availableBNExts())

    if not res:
      raise Exception("Error(s) in "+s)

    bn.setProperty("name",s)
    return bn

def saveBN(bn,s):
    """
    save a BN into a file using the format corresponding to one of the availableBNExts() suffixes.
    """
    extension=s.split('.')[-1].upper()
    if extension=="BIF":
        bn.saveBIF(s)
    elif extension=="BIFXML":
        bn.saveBIFXML(s)
    elif extension=="DSL":
        bn.saveDSL(s)
    elif extension=="NET":
        bn.saveNET(s)
    else:
        raise Exception("extension "+s.split('.')[-1]+" unknown. Please use "+availableBNExts())



def loadID(s):
  """
  return an InfluenceDiagram from a bifxml file.
  """

  extension=s.split('.')[-1].upper()
  if extension!="BIFXML":
    raise Exception("extension "+extension+" unknown. Please use bifxml.")

  diag=InfluenceDiagram()
  res=diag.loadBIFXML(s)

  if not res:
    raise Exception("Error(s) in "+s)

  diag.setProperty("name",s)
  return diag


# OBSOLETE FUNCTIONS FOR 0.9.1
def DiscretizedVar(*args):
  from .pyAgrum import DiscretizedVariable
  print("** Notification pyAgrum : <DiscretizedVar> obsolete. Please use <DiscretizedVariable> now.")
  return DiscretizedVariable(*args)

def LabelizedVar(*args):
  from .pyAgrum import LabelizedVariable
  print("** Notification pyAgrum : <LabelizedVar> obsolete. Please use <LabelizedVariable> now.")
  return LabelizedVariable(*args)

def RangeVar(*args):
  from .pyAgrum import RangeVariable
  print("** Notification pyAgrum : <RangeVar> obsolet. Please use <RangeVariable> now.")
  return RangeVariable(*args)
