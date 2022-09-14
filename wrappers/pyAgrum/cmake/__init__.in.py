# -*- coding: utf-8 -*-
"""
`pyAgrum <http://agrum.org>`_ a scientific C++ and Python library dedicated to Bayesian Networks and other Probabilistic Graphical Models. Based on the C++ `aGrUM <https://agrum.lip6.fr>`_ library, it provides a high-level interface to the C++ part of aGrUM allowing to create, manage and perform efficient computations with Bayesian networks and others probabilsitic graphical models (Markov networks, influence diagrams and LIMIDs, dynamic BN, probabilistic relational models).

The module is generated using the `SWIG <https://www.swig.org>`_ interface generator. Custom-written code was added to make the interface more friendly.

pyAgrum includes :
  * API documentation <https://pyagrum.readthedocs.io>,
  * examples as `notebooks <http://www-desir.lip6.fr/~phw/aGrUM/docs/last/notebooks/Tutorial.ipynb.html>`_,
  * a `website <http://agrum.org>`_.
"""

# aGrum Licence (LGPL)
# -------------------
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU General Public License for more details.                          *
# *                                                                         *
# *   You should have received a copy of the GNU General Public License     *
# *   along with this program; if not, write to the                         *
# *   Free Software Foundation, Inc.,                                       *
# *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

__version__ = '@PYAGRUM_VERSION@'
__license__ = __doc__
__project_url__ = 'http://agrum.org'

import warnings
import numpy as np

from .deprecated import *

# selection of imports extracted from dir(.pyAgrum)
from .pyAgrum import statsObj
from .pyAgrum import Arc, Edge, DiGraph, UndiGraph, MixedGraph, DAG, CliqueGraph
from .pyAgrum import BayesNet, BayesNetFragment, EssentialGraph, MarkovBlanket
from .pyAgrum import MarkovNet, ShaferShenoyMNInference
from .pyAgrum import DiscretizedVariable, LabelizedVariable, RangeVariable, DiscreteVariable, IntegerVariable, \
    NumericalDiscreteVariable
from .pyAgrum import Potential, Instantiation, Potential
from .pyAgrum import ExactBNdistance, GibbsBNdistance, StructuralComparator
from .pyAgrum import LazyPropagation, ShaferShenoyInference, VariableElimination
from .pyAgrum import LoopyBeliefPropagation, GibbsSampling, MonteCarloSampling, ImportanceSampling, WeightedSampling
from .pyAgrum import LoopyImportanceSampling, LoopyGibbsSampling, LoopyWeightedSampling, LoopyMonteCarloSampling
from .pyAgrum import PythonApproximationListener, PythonBNListener, PythonLoadListener, PythonDatabaseGeneratorListener
from .pyAgrum import BNGenerator, IDGenerator, JunctionTreeGenerator
from .pyAgrum import BNLearner
from .pyAgrum import BNDatabaseGenerator
from .pyAgrum import InfluenceDiagram, ShaferShenoyLIMIDInference
from .pyAgrum import CredalNet, CNMonteCarloSampling, CNLoopyPropagation
from .pyAgrum import PRMexplorer

from .pyAgrum import ApproximationScheme

from .pyAgrum import initRandom, randomProba, randomDistribution, randomGeneratorSeed, getRandomGenerator, randomValue
from .pyAgrum import isOMP, getNumberOfThreads, getMaxNumberOfThreads, getNumberOfLogicalProcessors, setNumberOfThreads

from .pyAgrum import VarType_Discretized, VarType_Labelized, VarType_Range, VarType_Integer, VarType_Numerical

from .pyAgrum import DefaultInLabel, DuplicateElement, DuplicateLabel, GumException, FatalError, FormatNotFound, \
    GraphError, IOError, InvalidArc, InvalidArgument, InvalidArgumentsNumber, InvalidDirectedCycle, InvalidEdge, \
    InvalidNode, DatabaseError, MissingValueInDatabase, MissingVariableInDatabase, NoChild, NoNeighbour, NoParent, \
    NotFound, NullElement, OperationNotAllowed, OutOfBounds, ArgumentError, SizeError, SyntaxError, UndefinedElement, \
    UndefinedIteratorKey, UndefinedIteratorValue, UnknownLabelInDatabase, CPTError

# selection of imports extracted from dir(pyAgrum)
__all__ = [
    '__version__', '__license__', '__project_url__',
    'about',
    'availableBNExts', 'loadBN', 'saveBN',
    'availableMNExts', 'loadMN', 'saveMN',
    'availableIDExts', 'loadID', 'saveID',
    'getPosterior',
    'statsObj',

    'Arc', 'Edge', 'DiGraph', 'UndiGraph', 'MixedGraph', 'DAG', 'CliqueGraph',
    'BayesNet', 'BayesNetFragment', 'EssentialGraph', 'MarkovBlanket',
    'MarkovNet', 'ShaferShenoyMNInference',
    'DiscretizedVariable', 'LabelizedVariable', 'RangeVariable', 'DiscreteVariable', 'IntegerVariable',
    'NumericalDiscreteVariable',
    'Potential', 'Instantiation', 'Potential',
    'ExactBNdistance', 'GibbsBNdistance', 'StructuralComparator',
    'LoopyBeliefPropagation', 'GibbsSampling', 'MonteCarloSampling', 'ImportanceSampling', 'WeightedSampling',
    'LoopyImportanceSampling', 'LoopyGibbsSampling', 'LoopyWeightedSampling', 'LoopyMonteCarloSampling'
                                                                              'LazyPropagation',
    'ShaferShenoyInference', 'VariableElimination',
    'PythonApproximationListener', 'PythonBNListener', 'PythonLoadListener', 'PythonDatabaseGeneratorListener',
    'BNGenerator', 'IDGenerator', 'JunctionTreeGenerator',
    'BNLearner',
    'BNDatabaseGenerator',
    'InfluenceDiagram', 'ShaferShenoyLIMIDInference',
    'CredalNet', 'CNMonteCarloSampling', 'CNLoopyPropagation',
    'PRMexplorer',

    'ApproximationScheme',
    'initRandom', 'randomProba', 'randomDistribution', 'randomGeneratorSeed', 'getRandomGenerator', 'randomValue',

    'isOMP', 'setNumberOfThreads', 'getNumberOfThreads', 'getMaxNumberOfThreads', 'getNumberOfLogicalProcessors',
    # 'getThreadNumber','getNumberOfRunningThreads','getDynamicThreadsNumber','setDynamicThreadsNumber','getNestedParallelism', 'setNestedParallelism',

    'VarType_Discretized', 'VarType_Labelized', 'VarType_Range', 'VarType_Integer', 'VarType_Numerical',

    'DefaultInLabel', 'DuplicateElement', 'DuplicateLabel', 'GumException', 'FatalError', 'FormatNotFound',
    'GraphError', 'IOError', 'InvalidArc', 'InvalidArgument', 'InvalidArgumentsNumber', 'InvalidDirectedCycle',
    'InvalidEdge', 'InvalidNode', 'DatabaseError', 'MissingValueInDatabase', 'MissingVariableInDatabase', 'NoChild',
    'NoNeighbour', 'NoParent', 'NotFound', 'NullElement', 'OperationNotAllowed', 'OutOfBounds', 'ArgumentError',
    'SizeError', 'SyntaxError', 'UndefinedElement', 'UndefinedIteratorKey', 'UndefinedIteratorValue',
    'UnknownLabelInDatabase', "CPTError",

    "config"
]

# seed is chosen randomly :)
initRandom(0)

# configuration object
from .config import PyAgrumConfiguration

config = PyAgrumConfiguration()


def _update_config_core():
    # hook to control some parameters for core params
    setNumberOfThreads(int(config['core', 'default_maxNumberOfThreads']))


config.add_hook(_update_config_core)
config.run_hooks()

try:
    # load custom configuration if any
    config.load()
except FileNotFoundError:
    pass


def about():
    """
    about() for pyAgrum

    """
    print("pyAgrum @PYAGRUM_VERSION@")
    print("(c) 2015-2022 Pierre-Henri Wuillemin, Christophe Gonzales")
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
    """load a BN from a file with optional listeners and arguments

    Parameters
    ----------
    filename: str
        the name of the input file
    listeners: List[object]
        list of functions to execute when listening
    verbose: bool
        whether to print or not warning messages
    system: str
        (for O3PRM) name of the system to flatten in a BN
    classpath: List[str]
        (for O3PRM) list of folders containing classes

    Returns
    -------
    pyAgrum.BayesNet
        a BN from a file using one of the availableBNExts() suffixes.

    Notes
    ----
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
        raise InvalidArgument("extension " + filename.split('.')
        [-1] + " unknown. Please use among " + availableBNExts())

    if verbose:
        print(warns)

    bn.setProperty("name", filename)
    return bn


def saveBN(bn, filename, allowModificationWhenSaving=None):
    """
    save a BN into a file using the format corresponding to one of the availableWriteBNExts() suffixes.

    Parameters
    ----------
    bn : pyAgrum.BayesNet
      the BN to save
    filename : str
      the name of the output file
    allowModificationWhenSaving: bool
        whether syntax errors in the BN should throw a FatalError or can be corrected. Also controlled by `pyAgrum.config["BN","allow_modification_when_saving"]`.
    """
    if allowModificationWhenSaving is None:
        allowModificationWhenSaving = config["BN", "allow_modification_when_saving"] == "True"

    extension = filename.split('.')[-1].upper()
    if extension == "BIF":
        bn.saveBIF(filename, allowModificationWhenSaving)
    elif extension == "BIFXML":
        bn.saveBIFXML(filename, allowModificationWhenSaving)
    elif extension == "DSL":
        bn.saveDSL(filename, allowModificationWhenSaving)
    elif extension == "NET":
        bn.saveNET(filename, allowModificationWhenSaving)
    elif extension == "UAI":
        bn.saveUAI(filename, allowModificationWhenSaving)
    elif extension == "O3PRM":
        bn.saveO3PRM(filename, allowModificationWhenSaving)
    else:
        raise InvalidArgument("[pyAgrum] extension " + filename.split('.')
        [-1] + " unknown. Please use among " + availableBNExts())


def availableMNExts():
    """ Give the list of all formats known by pyAgrum to save a Markov network.

    Returns
    ------
    str
      a string which lists all suffixes for supported MN file formats.
    """
    return "uai"


def availableIDExts():
    """ Give the list of all formats known by pyAgrum to save a influence diagram.

    Returns
    ------
    str
      a string which lists all suffixes for supported ID file formats.
    """
    return "bifxml"


def loadMN(filename, listeners=None, verbose=False):
    """load a MN from a file with optional listeners and arguments

    Parameters
    ----------
    filename: str
      the name of the input file
    listeners: List[Object]
      list of functions to execute
    verbose: bool
      whether to print or not warning messages

    Returns
    -------
    pyAgrum.MarkovNet
      a MN from a file using one of the availableMNExts() suffixes.

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
    >>> gum.loadMN('./bn.uai',listeners=[foo_listener,bar_listener])
    >>> # .........#.........#.........#.........#..50%
    >>> # .......#.........#.........#.........#.........#.........% | bn loaded
    """
    mn = MarkovNet()

    extension = filename.split('.')[-1].upper()
    if extension == "UAI":
        warns = mn.loadUAI(filename, listeners)
    else:
        raise InvalidArgument("extension " + filename.split('.')
        [-1] + " unknown. Please use among " + availableBNExts())

    if verbose:
        print(warns)

    mn.setProperty("name", filename)
    return mn


def saveMN(mn, filename):
    """
    save a MN into a file using the format corresponding to one of the availableWriteMNExts() suffixes.

    Parameters
    ----------
    mn : pyAgrum.MarkovNet)
      the MN to save
    filename : str
      the name of the output file
    """
    extension = filename.split('.')[-1].upper()
    if extension not in availableMNExts():
        raise InvalidArgument("[pyAgrum] extension " + filename.split('.')
        [-1] + " unknown. Please use among " + availableMNExts())

    # for now, just one format
    mn.saveUAI(filename)


def loadID(filename):
    """
    read a gum.InfluenceDiagram from a ID file

    Parameters
    ----------
    filename: str
      the name of the input file

    Returns
    -------
    pyAgrum.InfluenceDiagram
      the InfluenceDiagram
    """
    extension = filename.split('.')[-1].upper()
    if extension != "BIFXML":
        raise InvalidArgument("extension " + extension +
                              " unknown. Please use bifxml.")

    diag = InfluenceDiagram()
    # for now, just one format
    res = diag.loadBIFXML(filename)

    if not res:
        raise Exception("Error(s) in " + filename)

    diag.setProperty("name", filename)
    return diag


def saveID(infdiag, filename):
    """
    save an ID into a file using the format corresponding to one of the availableWriteIDExts() suffixes.

    Parameters
    ----------
    infdiag : pyAgrum.InfluenceDiagram
      the Influence Diagram to save
    filename : str
      the name of the output file
    """
    extension = filename.split('.')[-1].upper()
    if extension not in availableIDExts():
        raise InvalidArgument("[pyAgrum] extension " + filename.split('.')
        [-1] + " unknown. Please use among " + availableIDExts())

    # for now, just one format
    infdiag.saveBIFXML(filename)


def fastBN(structure, domain_size=2):
    """
    Create a Bayesian network with a dot-like syntax which specifies:
        - the structure 'a->b->c;b->d<-e;',
        - the type of the variables with different syntax (cf documentation).

    Examples
    --------
    >>> import pyAgrum as gum
    >>> bn=gum.fastBN('A->B[1,3]<-C{yes|No}->D[2,4]<-E[1,2.5,3.9]',6)

    Parameters
    ----------
    structure : str
            the string containing the specification
    domain_size : int
            the default domain size for variables

    Returns
    -------
    pyAgrum.BayesNet
            the resulting bayesian network
    """
    return BayesNet.fastPrototype(structure, domain_size)


def fastMN(structure, domain_size=2):
    """
    Create a Markov network with a modified dot-like syntax which specifies:
        - the structure 'a-b-c;b-d;c-e;' where each chain 'a-b-c' specifies a factor,
        - the type of the variables with different syntax (cf documentation).

    Examples
    --------
    >>> import pyAgrum as gum
    >>> bn=gum.fastMN('A--B[1,3]--C{yes|No};C--D[2,4]--E[1,2.5,3.9]',6)

    Parameters
    ----------
    structure : str
            the string containing the specification
    domain_size : int
            the default domain size for variables

    Returns
    -------
    pyAgrum.MarkovNet
            the resulting Markov network
    """
    return MarkovNet.fastPrototype(structure, domain_size)


def fastID(structure, domain_size=2):
    """
    Create an Influence Diagram with a modified dot-like syntax which specifies:
        - the structure and the type of the variables following :ref:`fast syntax<Quick specification of (randomly parameterized) graphical models>`,
        - a prefix for the type of node (chance/decision/utiliy nodes):

          - ``a`` : a chance node named 'a' (by default)
          - ``$a`` : a utility node named 'a'
          - ``*a`` : a decision node named 'a'

    Examples
    --------
    >>> import pyAgrum as gum
    >>> bn=gum.fastID('A->B[1,3]<-*C{yes|No}->$D<-E[1,2.5,3.9]',6)

    Parameters
    ----------
    structure : str
            the string containing the specification
    domain_size : int
            the default domain size for variables

    Returns
    -------
    pyAgrum.InfluenceDiagram
            the resulting Influence Diagram
    """
    return InfluenceDiagram.fastPrototype(structure, domain_size)


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
      {name|id:val, name|id : [ val1, val2 ], ...}
    target: string or int
      variable name or id

    Returns
    -------
      posterior (pyAgrum.Potential or other)
    """
    if isinstance(model, BayesNet):
        inf = VariableElimination(model)
    elif isinstance(model, MarkovNet):
        inf = ShaferShenoyMNInference(model)
    else:
        raise InvalidArgument("Argument model should be a PGM (BayesNet or MarkovNet")

    inf.setEvidence(evs)
    inf.addTarget(target)
    inf.makeInference()
    # creating a new Potential from posterior(will disappear with ie)
    return Potential(inf.posterior(target))


def generateSample(bn, n=1, name_out=None, show_progress=False, with_labels=True, random_order=True):
    """
    generate a CSV file of samples from a bn.

    Parameters
    ----------
    bn: pyAgrum.BayesNet
      the Bayes Net from which the sample is generated
    n: int
      the number of samples
    name_out: str
      the name for the output csv filename. If name_out is None, a pandas.DataFrame is generated
    show_progress: bool
      if True, show a progress bar. Default is False
    with_labels: bool
      if True, use the labels of the modalities of variables in the csv. If False, use their ids. Default is True
    random_order: bool
      if True, the columns in the csv are randomized sorted. Default is True

    Returns
    -------
    float|Tuple[pandas.DataFrame,float]
      the log2-likelihood of the generated base or if name_out is None, the couple (generated pandas.DataFrame,log2-likelihood)
    """
    genere = BNDatabaseGenerator(bn)
    if show_progress:
        from tqdm import tqdm
        pbar = tqdm(total=100, desc=name_out, bar_format='{desc}: {percentage:3.0f}%|{bar}|', ncols=60)
        listen = PythonDatabaseGeneratorListener(genere)

        def whenStep(x, y):
            pbar.update(1)

        def whenStop(msg):
            pbar.close()

        listen.setWhenProgress(whenStep)
        listen.setWhenStop(whenStop)

    if random_order:
        genere.setRandomVarOrder()
    ll = genere.drawSamples(n)

    if name_out is not None:
        genere.toCSV(name_out, with_labels)

    if show_progress:
        print(f"Log2-Likelihood : {ll}")

    if name_out is not None:
        return ll
    else:
        return genere.to_pandas(with_labels), ll


def generateCSV(bn, name_out, n=1, show_progress=False, with_labels=False, random_order=True):
    """
    Deprecated. Please use `pyAgrum.generateSample` instead.
    """
    warnings.warn(f"""
  ** pyAgrum.generatedCSV is deprecated since pyAgrum>0.22.7.
  ** A call to 
      'generateSample(bn, {n=}, {name_out=}, {show_progress=}, {with_labels=}, {random_order=})' 
  ** is executed.
    
  """, DeprecationWarning, stacklevel=2)

    generateSample(bn, n, name_out, show_progress, with_labels, random_order)


def log2(p):
    """Compute p.log2() in a new Potential without modifying p

    Parameters
    ----------
    p : pyAgrum.Potential
      The potential on which to apply log2 function

    Returns
    -------
      a pyAgrum.Potential
    """
    return Potential(p).log2()


def mutilateBN(bn, intervention=None, observation=None):
    """
    Modify the bayesian network bn to reflect the effect of interventions and/or observations on a set of variables.
    Due to the causal nature of interventions, we suppose the given bn to have a causal interpretation.
    Warning: experimental use of evidence definition

    Interventions or observations can be HARD or SOFT.

      Hard interventions or observations:
          1) [0,... 1, 0] -> sum(x) = 1
          3) X : [n] -> with n a value

      Soft interventions or observations:
          1) X : [empty list] -> equiprobability is assumed
          2) X : [x1, ... xn] -> sum(x) = 1
          3) X : [1, ... 1, 0] -> sum(x) >= 1
          4) X : [n1, n2, n3] -> with n_i values that could happen

      X is the name of a variable

    Parameters
    ----------
    bn : pyAgrum.pyAgrum.BayesNet
      A bayesian network
    intervention : Dict[str,List[str|float|int]]
      set of variables on which we intervene to force the value
    observation : Dict[str,List[str|float|int]]
      set of variables whose value is observed

    Returns
    -------
    inter_bn : new bayesian network reflecting the interventions and observations (pyAgrum.pyAgrum.BayesNet)
    evidence : dictionary of all evidences for future inferences (dict)
    """
    if intervention is None:
        intervention = dict()

    if observation is None:
        observation = dict()

    inter_bn = BayesNet(bn)

    # Check that a variable is not an intervention and an observation
    if len(set(intervention).intersection(set(observation))) > 0:
        raise ValueError('A variable can\'t be an intervention and an observation')

    evidence = dict()  # Track the new distribution to update
    list_hard = dict()  # Track the hard values
    toModify = {"intervention": intervention, "observation": observation}

    ## Delete relations
    for typeSet in toModify:

        # For each variable we wish to modify
        for var in toModify[typeSet]:

            # Get the ID and the name
            if var in bn.names():
                var_id = bn.idFromName(var)

            else:
                var_id = var
                var = bn.variable(var_id).name()

            # Delete relations from parents for interventions
            if typeSet == "intervention":
                for par in bn.parents(var):
                    inter_bn.eraseArc(par, var_id)

            # Determine the new distributions
            n = bn.variable(var).domainSize()
            new_dis = toModify[typeSet][var]
            hard = False

            if len(new_dis) == 0:  # soft 1)
                new_dis = [1 / n for k in range(n)]

            elif str in [type(i) for i in new_dis]:  # hard - soft 3) 4)
                new_dis = [1 if bn.variable(var).labels()[i] == new_dis[0] else 0 for i in range(n)]

                if len(toModify[typeSet][var]) == 1:
                    new_val = toModify[typeSet][var][0]
                    hard = True

            elif sum(new_dis) == 1 and 1 in new_dis:  # hard 1)
                new_val = bn.variable(var).labels()[new_dis.index(1)]
                hard = True

            evidence[var] = new_dis

            # If hard values
            if hard:
                # Track the new values
                list_hard[var] = new_val

                # Delete relation toward children
                for chi in bn.children(var):
                    inter_bn.eraseArc(var_id, chi)

    ## Update the distributions
    for var in list(evidence):

        # Update variable if intervention
        if var in intervention:
            inter_bn.cpt(var).fillWith(evidence[var])

        # Update children if hard evidence
        if var in list_hard:
            for chi in bn.children(var):
                new_cpt = bn.cpt(chi)[list_hard]

                inter_bn.cpt(chi)[:] = new_cpt

            # If intervention, remove var
            if var in intervention:
                inter_bn.erase(var)
                del evidence[var]

    return (inter_bn, evidence)
