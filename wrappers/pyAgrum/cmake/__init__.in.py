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

from .functions import *

# selection of imports extracted from dir(pyAgrum)
from @PYAGRUM_MODULE@ import Arc,Edge,DiGraph,UndiGraph,MixedGraph,DAG,CliqueGraph

from @PYAGRUM_MODULE@ import BayesNet
from @PYAGRUM_MODULE@ import DiscretizedVariable,LabelizedVariable,RangeVariable,DiscreteVariable
from @PYAGRUM_MODULE@ import Potential,Instantiation,UtilityTable
from @PYAGRUM_MODULE@ import BruteForceKL,GibbsKL
from @PYAGRUM_MODULE@ import GibbsInference,LazyPropagation
from @PYAGRUM_MODULE@ import PythonApproximationListener,PythonBNListener,PythonLoadListener

from @PYAGRUM_MODULE@ import BNGenerator,IDGenerator
from @PYAGRUM_MODULE@ import BNLearner

from @PYAGRUM_MODULE@ import InfluenceDiagram,InfluenceDiagramInference

from @PYAGRUM_MODULE@ import initRandom,randomProba,randomDistribution

from @PYAGRUM_MODULE@ import isOMP,setNumberOfThreads,getNumberOfLogicalProcessors,getMaxNumberOfThreads

from @PYAGRUM_MODULE@ import CredalNet,CNMonteCarloSampling,CNLoopyPropagation

from @PYAGRUM_MODULE@ import DefaultInLabel,DuplicateElement,DuplicateLabel,EmptyBSTree,EmptySet,Exception,FatalError,FormatNotFound,GraphError,IOError,IdError,InvalidArc,InvalidArgument,InvalidArgumentsNumber,InvalidDirectedCycle,InvalidEdge,InvalidNode,NoChild,NoNeighbour,NoParent,NotFound,NullElement,OperationNotAllowed,OutOfBounds,OutOfLowerBound,OutOfUpperBound,ReferenceError,SizeError,SyntaxError,UndefinedElement,UndefinedIteratorKey,UndefinedIteratorValue,UnknownLabelInDatabase

# seed is chosen randomly :)
initRandom(0)
