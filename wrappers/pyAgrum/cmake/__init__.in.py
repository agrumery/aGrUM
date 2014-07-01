# -*- coding: utf-8 -*-
"""
pyAgrum is a Python wrapper for the C++ aGrUM library.
It provides a high-level interface to the part of aGrUM allowing to create, handle and make computations into Bayesian Networks.

The module is a straightforward application of the SWIG interface generator. Custom-written code was added to make the interface more similar to OpenBayes, a free Bayesian Network library for Python.

pyAgrum is GPL.

site : https://forge.lip6.fr/projects/pyagrum/wiki
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

__version__ = '@PYAGRUM_VERSION@'
__license__ = __doc__
__project_url__ = 'http://forge.lip6.fr/projects/pyagrum'

# selection of imports extracted from dir(pyAgrum)
from pyAgrum import GUM_MAJOR_VERSION,GUM_MINOR_VERSION,GUM_PATCH_VERSION,GUM_VERSION

from pyAgrum import Arc,Edge,BayesNet,DiGraph,Vector_double
from pyAgrum import DiscreteVar,DiscretizedVar,Instantiation,LabelizedVar,RangeVar,Variable
from pyAgrum import Potential
from pyAgrum import BruteForceKL,GibbsInference,GibbsKL,LazyPropagation
from pyAgrum import PythonApproximationListener,PythonBNListener,PythonLoadListener
from pyAgrum import BNGenerator
from pyAgrum import BNLearnerFromCSV
from pyAgrum import initRandom,randomProba,randomDistribution

from pyAgrum import isOMP,setNumberOfThreads,getNumberOfLogicalProcessors,getMaxNumberOfThreads
from pyAgrum import CredalNet,CNMonteCarloSampling,CNLoopyPropagation

from pyAgrum import DefaultInLabel,DuplicateElement,DuplicateLabel,EmptyBSTree,EmptySet,Exception,FatalError,FormatNotFound,GraphError,IOError,IdError,InvalidArc,InvalidArgument,InvalidArgumentsNumber,InvalidDirectedCycle,InvalidEdge,InvalidNode,NoChild,NoNeighbour,NoParent,NotFound,NullElement,OperationNotAllowed,OutOfBounds,OutOfLowerBound,OutOfUpperBound,ReferenceError,SizeError,SyntaxError,UndefinedElement,UndefinedIteratorKey,UndefinedIteratorValue


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

def about():
  print("pyAgrum version {0}.{1}.{2}".format(GUM_MAJOR_VERSION,GUM_MINOR_VERSION,GUM_PATCH_VERSION))
  print("(c) Pierre-Henri Wuillemin and others")
  print("    UPMC 2014")
  print("""
	This is free software; see the source code for copying conditions.
	There is ABSOLUTELY NO WARRANTY; not even for MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE.  For details, see 'pyAgrum.warranty'.
	""")