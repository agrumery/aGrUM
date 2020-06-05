# -*- coding: utf-8 -*-
"""
`pyAgrum <http://agrum.org>`_ a scientific C++ and Python library dedicated to Bayesian Networks and other Probabilistic Graphical Models. Based on the C++ `aGrUM <https://agrum.lip6.fr>`_ library, it provides a high-level interface to the C++ part of aGrUM allowing to create, manage and perform efficient computations with Bayesian Networks.

The module is generated using the `SWIG <https://www.swig.org>`_ interface generator. Custom-written code was added to make the interface more friendly.

pyAgrum includes :
  * API documentation <https://pyagrum.readthedocs.io>,
  * examples as `notebooks <http://www-desir.lip6.fr/~phw/aGrUM/docs/last/notebooks/01-tutorial.ipynb.html>`_,
  * a `website <http://agrum.org>`_.
"""

# aGrum Licence (LGPL)
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
__project_url__ = 'http://agrum.org'

# for python2's compatibility
try:
    FileNotFoundError
except NameError:
    FileNotFoundError = IOError
## end for python2's compatibility

import numpy as np
from .deprecated import *

# selection of imports extracted from dir(.pyAgrum)
from .pyAgrum import statsObj
from .pyAgrum import Arc, Edge, DiGraph, UndiGraph, MixedGraph, DAG, CliqueGraph
from .pyAgrum import BayesNet, BayesNetFragment, EssentialGraph, MarkovBlanket
from .pyAgrum import MarkovNet, ShaferShenoyMNInference
from .pyAgrum import DiscretizedVariable, LabelizedVariable, RangeVariable, DiscreteVariable
from .pyAgrum import Potential, Instantiation, Potential
from .pyAgrum import ExactBNdistance, GibbsBNdistance, StructuralComparator
from .pyAgrum import LazyPropagation, ShaferShenoyInference, VariableElimination
from .pyAgrum import LoopyBeliefPropagation, GibbsSampling, MonteCarloSampling, ImportanceSampling, WeightedSampling
from .pyAgrum import LoopyImportanceSampling,LoopyGibbsSampling,LoopyWeightedSampling,LoopyMonteCarloSampling
from .pyAgrum import PythonApproximationListener, PythonBNListener, PythonLoadListener, PythonDatabaseGeneratorListener
from .pyAgrum import BNGenerator, IDGenerator, JunctionTreeGenerator
from .pyAgrum import BNLearner
from .pyAgrum import BNDatabaseGenerator
from .pyAgrum import InfluenceDiagram, InfluenceDiagramInference
from .pyAgrum import CredalNet, CNMonteCarloSampling, CNLoopyPropagation
from .pyAgrum import PRMexplorer

from .pyAgrum import ApproximationScheme

from .pyAgrum import initRandom, randomProba, randomDistribution, randomGeneratorSeed, getRandomGenerator, randomValue
from .pyAgrum import isOMP, setNumberOfThreads, getThreadNumber, getNumberOfRunningThreads, getNumberOfLogicalProcessors, getMaxNumberOfThreads, getDynamicThreadsNumber, setDynamicThreadsNumber

from .pyAgrum import getNestedParallelism, setNestedParallelism

from .pyAgrum import VarType_Discretized,VarType_Labelized,VarType_Range

from .pyAgrum import DefaultInLabel, DuplicateElement, DuplicateLabel, EmptyBSTree, EmptySet, GumException, FatalError, FormatNotFound, GraphError, IOError, IdError, InvalidArc, InvalidArgument, InvalidArgumentsNumber, InvalidDirectedCycle, InvalidEdge, InvalidNode, MissingValueInDatabase, MissingVariableInDatabase, NoChild, NoNeighbour, NoParent, NotFound, NullElement, OperationNotAllowed, OutOfBounds, OutOfLowerBound, OutOfUpperBound, ReferenceError, SizeError, SyntaxError, UndefinedElement, UndefinedIteratorKey, UndefinedIteratorValue, UnknownLabelInDatabase


# selection of imports extracted from dir(pyAgrum)
__all__=[
  '__version__','__license__','__project_url__',        # here
  'about','availableBNExts','loadBN','saveBN','loadID','getPosterior', # in fuctions.py
  'statsObj',

  'Arc','Edge','DiGraph','UndiGraph','MixedGraph','DAG','CliqueGraph',
  'BayesNet', 'BayesNetFragment', 'EssentialGraph', 'MarkovBlanket',
  'MarkovNet','ShaferShenoyMNInference',
  'DiscretizedVariable','LabelizedVariable','RangeVariable','DiscreteVariable',
  'Potential','Instantiation','Potential',
  'ExactBNdistance','GibbsBNdistance','StructuralComparator',
  'LoopyBeliefPropagation','GibbsSampling','MonteCarloSampling', 'ImportanceSampling', 'WeightedSampling',
  'LoopyImportanceSampling','LoopyGibbsSampling','LoopyWeightedSampling','LoopyMonteCarloSampling'
  'LazyPropagation','ShaferShenoyInference','VariableElimination',
  'PythonApproximationListener','PythonBNListener','PythonLoadListener','PythonDatabaseGeneratorListener',
  'BNGenerator','IDGenerator','JunctionTreeGenerator',
  'BNLearner',
  'BNDatabaseGenerator',
  'InfluenceDiagram','InfluenceDiagramInference',
  'CredalNet','CNMonteCarloSampling','CNLoopyPropagation',
  'PRMexplorer',

  'ApproximationScheme',

  'initRandom','randomProba','randomDistribution',
  'isOMP','setNumberOfThreads','getNumberOfLogicalProcessors','getMaxNumberOfThreads',

  'initRandom','randomProba','randomDistribution','randomGeneratorSeed','getRandomGenerator','randomValue',
  'isOMP','setNumberOfThreads','getThreadNumber','getNumberOfRunningThreads','getNumberOfLogicalProcessors','getMaxNumberOfThreads','getDynamicThreadsNumber','setDynamicThreadsNumber',

  'getNestedParallelism', 'setNestedParallelism',

  'VarType_Discretized', 'VarType_Labelized', 'VarType_Range',

  'DefaultInLabel', 'DuplicateElement', 'DuplicateLabel', 'EmptyBSTree', 'EmptySet', 'GumException', 'FatalError', 'FormatNotFound', 'GraphError', 'IOError', 'IdError', 'InvalidArc', 'InvalidArgument', 'InvalidArgumentsNumber', 'InvalidDirectedCycle', 'InvalidEdge', 'InvalidNode', 'MissingValueInDatabase', 'MissingVariableInDatabase', 'NoChild', 'NoNeighbour', 'NoParent', 'NotFound', 'NullElement', 'OperationNotAllowed', 'OutOfBounds', 'OutOfLowerBound', 'OutOfUpperBound', 'ReferenceError', 'SizeError', 'SyntaxError', 'UndefinedElement', 'UndefinedIteratorKey', 'UndefinedIteratorValue', 'UnknownLabelInDatabase',

  "config"
  ]

# seed is chosen randomly :)
initRandom(0)

# configuration object
from .config import PyAgrumConfiguration
config = PyAgrumConfiguration()
try:
  # load custom configuration if any
  config.load()
except FileNotFoundError:
  pass

def about():
  """
  about() for pyAgrum

  """
  print("pyAgrum version {0}".format('@PYAGRUM_VERSION@'))
  print("(c) 2015-2020 Pierre-Henri Wuillemin, Christophe Gonzales, Lionel Torti")
  print("""
    This is free software; see the source code for copying conditions.
    There is ABSOLUTELY NO WARRANTY; not even for MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  For details, see 'pyAgrum.warranty'.
    """)


def availableBNExts():
  """ Give the list of all formats known by pyAgrum to save a Bayesian network.

  :return: a string which lists all suffixes for supported BN file formats.
  """
  return "bif|dsl|net|bifxml|o3prm|uai"


def loadBN(filename, listeners=None, verbose=False, **opts):
  """load a file with optional listeners and arguments

  :param filename: the name of the input file
  :param listeners: list of functions to execute
  :param verbose: whether to print or not warning messages
  :param system: (for O3PRM) name of the system to flatten in a BN
  :param classpath: (for O3PRM) list of folders containing classes
  :return: a BN from a file using one of the availableBNExts() suffixes.

  Listeners could be added in order to monitor its loading.

  Examples
  --------
  >>> import pyAgrum as gum
  >>>
  >>> # creating listeners
  >>> def foo_listener(progress):
  >>>    if progress==200:
  >>>        print(' BN loaded ')
  >>>        return
  >>>    elif progress==100:
  >>>        car='%'
  >>>    elif progress%10==0:
  >>>        car='#'
  >>>    else:
  >>>        car='.'
  >>>    print(car,end='',flush=True)
  >>>
  >>> def bar_listener(progress):
  >>>    if progress==50:
  >>>        print('50%')
  >>>
  >>> # loadBN with list of listeners
  >>> gum.loadBN('./bn.bif',listeners=[foo_listener,bar_listener])
  >>> # .........#.........#.........#.........#..50%
  >>> # .......#.........#.........#.........#.........#.........% | bn loaded
  """
  bn = BayesNet()

  extension = filename.split('.')[-1].upper()
  if extension == "BIF":
    warns = bn.loadBIF(filename, listeners)
  elif extension == "BIFXML":
    warns = bn.loadBIFXML(filename, listeners)
  elif extension == "DSL":
    warns = bn.loadDSL(filename, listeners)
  elif extension == "NET":
    warns = bn.loadNET(filename, listeners)
  elif extension == "O3PRM":
    warns = bn.loadO3PRM(filename, opts.get('system', ''),
                         opts.get('classpath', ''), listeners)
  elif extension == "UAI":
    warns = bn.loadUAI(filename, listeners)
  else:
    raise InvalidArgument("extension "+filename.split('.')
                          [-1]+" unknown. Please use among "+availableBNExts())

  if verbose:
    print(warns)

  bn.setProperty("name", filename)
  return bn


def saveBN(bn, filename):
  """
  save a BN into a file using the format corresponding to one of the availableWriteBNExts() suffixes.

  :parma bn(gum.BayesNet): the BN to save
  :param filename(str): the name of the output file
  """
  extension = filename.split('.')[-1].upper()
  if extension == "BIF":
    bn.saveBIF(filename)
  elif extension == "BIFXML":
    bn.saveBIFXML(filename)
  elif extension == "DSL":
    bn.saveDSL(filename)
  elif extension == "NET":
    bn.saveNET(filename)
  elif extension == "UAI":
    bn.saveUAI(filename)
  elif extension == "O3PRM":
    bn.saveO3PRM(filename)
  else:
    raise InvalidArgument("[pyAgrum] extension "+filename.split('.')
                          [-1]+" unknown. Please use among "+availableBNExts())


def loadID(filename):
  """
  read a gum.InfluenceDiagram from a bifxml file

  :param filename: the name of the input file
  :return: an InfluenceDiagram
  """

  extension = filename.split('.')[-1].upper()
  if extension != "BIFXML":
    raise InvalidArgument("extension "+extension +
                          " unknown. Please use bifxml.")

  diag = InfluenceDiagram()
  res = diag.loadBIFXML(filename)

  if not res:
    raise Exception("Error(s) in "+filename)

  diag.setProperty("name", filename)
  return diag

def fastBN(structure, domain_size=2):
  """
Create a Bayesian network with a dot-like syntax which specifies:
    - the structure 'a->b->c;b->d<-e;'.
    - the type of the variables with different syntax:

      - by default, a variable is a gum.RangeVariable using the default domain size (second argument)
      - with `'a[10]'`, the variable is a gum.RangeVariable using 10 as domain size (from 0 to 9)
      - with 'a[3,7]', the variable is a gum.RangeVariable using a domainSize from 3 to 7
      - with 'a[1,3.14,5,6.2]', the variable is a gum.DiscretizedVariable using the given ticks (at least 3 values)
      - with 'a{top|middle|bottom}', the variable is a gum.LabelizedVariable using the given labels.

Note 
----
  - If the dot-like string contains such a specification more than once for a variable, the first specification will be used.
  - the CPTs are randomly generated.
    
Examples
--------
>>> import pyAgrum as gum
>>> bn=gum.fastBN('A->B[1,3]<-C{yes|No}->D[2,4]<-E[1,2.5,3.9]',6)

Parameters
----------
structure : str
        the string containing the specification
domainSize : int
        the default domain size for variables

Returns
-------
pyAgrum.BayesNet
        the resulting bayesian network 
  """
  return BayesNet.fastPrototype(structure, domain_size)

def fastMN(structure, domain_size=2):
  """
Create a Markov network with a dot-like syntax which specifies:
    - the structure 'a-b-c;b-d;c-e;'.
    - the type of the variables with different syntax:

      - by default, a variable is a gum.RangeVariable using the default domain size (second argument)
      - with `'a[10]'`, the variable is a gum.RangeVariable using 10 as domain size (from 0 to 9)
      - with 'a[3,7]', the variable is a gum.RangeVariable using a domainSize from 3 to 7
      - with 'a[1,3.14,5,6.2]', the variable is a gum.DiscretizedVariable using the given ticks (at least 3 values)
      - with 'a{top|middle|bottom}', the variable is a gum.LabelizedVariable using the given labels.

Note 
----
  - If the dot-like string contains such a specification more than once for a variable, the first specification will be used.
  - the potentials are randomly generated.
    
Examples
--------
>>> import pyAgrum as gum
>>> bn=gum.fastMN('A-B[1,3]-C{yes|No};C-D[2,4]-E[1,2.5,3.9]',6)

Parameters
----------
dotlike : str
        the string containing the specification
domainSize : int
        the default domain size for variables

Returns
-------
pyAgrum.MARKOVNet
        the resulting Markov network 
  """
  return MarkovNet.fastPrototype(structure, domain_size)


def getPosterior(model, evs, target):
  """
  Compute the posterior of a single target (variable) in a BN given evidence


  getPosterior uses a VariableElimination inference.
  If more than one target is needed with the same set of evidence or if the same
  target is needed with more than one set of evidence, this function is not
  relevant since it creates a new inference engine every time it is called.

  Parameters
  ----------
  bn : pyAgrum.BayesNet or pyAgrum.MarkovNet
    The probabilistic Graphical Model   
  evs:  dictionaryDict
    {name/id:val, name/id : [ val1, val2 ], ...}
  target: string or int
    variable name or id

  Returns
  -------
    posterior (gum::Potential)
  """
  if isinstance(model,BayesNet):
    inf = VariableElimination(model)
  elif isinstance(model,MarkovNet):
    inf = ShaferShenoyMNInference(model)
  else:
    raise InvalidArgument("Argument model should be a PGM (BayesNet or MarkovNet")

  inf.setEvidence(evs)
  inf.addTarget(target)
  inf.makeInference()
# creating a new Potential from posterior(will disappear with ie)
  return Potential(inf.posterior(target))


def generateCSV(bn, name_out, n, visible=False, with_labels=False, random_order=True):
  """
  generate a CSV file of samples from a bn.

  :param bn: the BN from which the sample is generated
  :type bn: pyAgrum.BayesNet
  :param name_out: the name for the output filename
  :type name_out: string
  :param n: the number of samples
  :type n: int
  :param visible: if True, show a progress bar
  :type visible: boolean
  :param with_labels: if True, use the labels of the modalities of variables in the csv. If False, use their ids.
  :type with_labels: boolean
  :param random_order: if True, the columns in the csv are randomized sorted
  :type random_order: boolean  
  :return: the log2-likelihood of the generated base
  """

  genere = BNDatabaseGenerator(bn)
  if visible:
    from pyAgrum.lib._utils.progress_bar import ProgressBar
    prog = ProgressBar(name_out + ' : ', 0, 100, 77, mode='static', char='#')
    prog.display()
    listen = PythonDatabaseGeneratorListener(genere)

    def whenStep(x, y):
      prog.increment_amount()
      prog.display()

    def whenStop(msg):
      prog.update_amount(100)
      prog.display()
      print()

    listen.setWhenProgress(whenStep)
    listen.setWhenStop(whenStop)

  if random_order:
    genere.setRandomVarOrder()
  ll = genere.drawSamples(n)

  genere.toCSV(name_out, with_labels)

  if visible:
    print("Log2-Likelihood : {}".format(ll))

  return ll

def log2(p):
  """Compute p.log2() in a new Potential without modifying p
  
  Parameters
  ----------
  p : pyAgrum.Potential 
    The potential on which to apply log2 function

  Returns
  -------
    a gpyAgrumum.Potential
  """
  return Potential(p).log2()


def forDarkTheme():
  """ change the color for arcs and text in graphs to be more visible in dark theme
  """
  config["notebook", "default_arc_color"] = "#AAAAAA"


def forLightTheme():
  """ change the color for arcs and text in graphs to be more visible in light theme
  """
  config["notebook", "default_arc_color"] = "#202020"


def getBlackInTheme():
  """ return the color used for arc and text in graphs
  """
  return config["notebook", "default_arc_color"]


####################
# Private functions
####################
def _proba2rgb(p, cmap, withSpecialColor):
  (r, g, b, _) = cmap(p)
  r = "%02x" % int(r * 256)
  g = "%02x" % int(g * 256)
  b = "%02x" % int(b * 256)

  if withSpecialColor:  # add special color for p=0 or p=1
    if p == 0.0:
      r, g, b = "FF", "33", "33"
    elif p == 1.0:
      r, g, b = "AA", "FF", "FF"

  return r, g, b


def _proba2color(p, cmap):
  r, g, b = _proba2rgb(p, cmap, withSpecialColor=False)
  return "#" + r + g + b


def _proba2bgcolor(p, cmap):
  r, g, b = _proba2rgb(p, cmap, withSpecialColor=True)
  return "#" + r + g + b


def _proba2fgcolor(p, cmap):
  if max([eval("0x" + s[0]) for s in _proba2rgb(p, cmap, withSpecialColor=True)]) <= 12:
    return "#FFFFFF"
  else:
    return "#000000"