# -*- coding: utf-8 -*-
"""
[pyAgrum](https://forge.lip6.fr/projects/pyagrum/wiki)  is a Python wrapper for the C++ [aGrUM](https://agrum.lip6.fr) library.
It provides a high-level interface to the part of aGrUM allowing to create, handle and make computations into Bayesian Networks.

The module is generated using the [SWIG](https://www.swig.org) interface generator. Custom-written code was added to make the interface more friendly.

pyAgrum has [comprehensive API documentation](http://www-desir.lip6.fr/~phw/aGrUM/dev/pdoc/),
[examples as notebooks](http://www-desir.lip6.fr/~phw/aGrUM/officiel/notebooks/) and a [website](https://forge.lip6.fr/projects/pyagrum/wiki).
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

from @PYAGRUM_MODULE@ import *

# seed is chosen randomly :)
initRandom(0)

# selection of imports extracted from dir(pyAgrum)
__all__=[
  '__version__','__license__','__project_url__',
  'availableBNExts','availableBNExts','loadBN','saveBN','loadID',
  
  'Arc','Edge','DiGraph','UndiGraph','MixedGraph','DAG','CliqueGraph',
  'BayesNet',
  'DiscretizedVariable','LabelizedVariable','RangeVariable','DiscreteVariable',
  'Potential','Instantiation','UtilityTable',
  'BruteForceKL','GibbsKL',
  'GibbsInference','LazyPropagation',
  'PythonApproximationListener','PythonBNListener','PythonLoadListener',
  'BNGenerator','IDGenerator','JTGenerator',
  'BNLearner',
  'InfluenceDiagram','InfluenceDiagramInference',
  
  'initRandom','randomProba','randomDistribution',
  'isOMP','setNumberOfThreads','getNumberOfLogicalProcessors','getMaxNumberOfThreads',
  
  'CredalNet','CNMonteCarloSampling','CNLoopyPropagation',
  
  'PRMexplorer',
  
  'DefaultInLabel','DuplicateElement','DuplicateLabel','EmptyBSTree','EmptySet','GumException','FatalError','FormatNotFound','GraphError','IOError','IdError','InvalidArc','InvalidArgument','InvalidArgumentsNumber','InvalidDirectedCycle','InvalidEdge','InvalidNode','NoChild','NoNeighbour','NoParent','NotFound','NullElement','OperationNotAllowed','OutOfBounds','OutOfLowerBound','OutOfUpperBound','ReferenceError','SizeError','SyntaxError','UndefinedElement','UndefinedIteratorKey','UndefinedIteratorValue','UnknownLabelInDatabase'
  ]
