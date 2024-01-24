/**
*
*  Copyright 2005-2023 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
*   {prenom.nom}_at_lip6.fr
*
*  This library is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
  *
*  You should have received a copy of the GNU Lesser General Public License
*  along with this library.  If not, see <http://www.gnu.org/licenses/>.
*
*/

// This file is part of swig code, include by pyAgrum.i and should not be used directly.
// It is not a python file but includes only python code. This is why it is a *.py file.

%pythoncode %{

import numpy
import warnings
import os.path as ospath
from numbers import Number
from typing import List

import pyAgrum

def _gum_add_properties_while_getstate_(model):
  if not hasattr(model, "setProperty"):
    return

  if config.asBool["Pickle", "add_version"]:
    model.setProperty("version", f"pyAgrum {pyAgrum.__version__}")
  if config.asBool["Pickle", "add_date"]:
    from datetime import datetime
    model.setProperty("creation", model.propertyWithDefault("creation", datetime.now().strftime("%Y-%m-%d %H:%M:%S%z")))
    model.setProperty("lastModification", datetime.now().strftime("%Y-%m-%d %H:%M:%S%z"))


def _update_config_core():
  # hook to control some parameters for core params
  setNumberOfThreads(config.asInt['core', 'default_maxNumberOfThreads'])

def about():
  """
  about() for pyAgrum

  """
  print(f"pyAgrum {pyAgrum.__version__}")
  print("(c) 2015-2023 Pierre-Henri Wuillemin, Christophe Gonzales")
  print("""
    This is free software; see the source code for copying conditions.
    There is ABSOLUTELY NO WARRANTY; not even for MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  For details, see 'pyAgrum.warranty'.
    """)


def availableBNExts():
  """ Give the list of all formats known by pyAgrum to save a Bayesian network.

  :return: a string which lists all suffixes for supported BN file formats.
  """
  return "bif|dsl|net|bifxml|o3prm|uai|xdsl|pkl"


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

      pkl suffix is used to load a pickled BN. In this case, listeners and options are ignored.

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
  elif extension == "XDSL":
    warns = bn.loadXDSL(filename, listeners)
  elif extension == "NET":
    warns = bn.loadNET(filename, listeners)
  elif extension == "O3PRM":
    warns = bn.loadO3PRM(filename, opts.get('system', ''),
                         opts.get('classpath', ''), listeners)
  elif extension == "UAI":
    warns = bn.loadUAI(filename, listeners)
  elif extension == "PKL":
    import pickle
    with open(filename, "rb") as f:
      bn = pickle.load(f)
  else:
    raise InvalidArgument("extension " + filename.split('.')
    [-1] + " unknown. Please use among " + availableBNExts())

  if verbose:
    warnings.warn(warns)

  bn.setProperty("name", bn.propertyWithDefault("name", ospath.splitext(ospath.basename(filename))[0]))
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

  Notes
  ----
      pkl suffix is used to save a BN using pickle. In this case, options are ignored.
  """
  if allowModificationWhenSaving is None:
    allowModificationWhenSaving = config.asBool["BN", "allow_modification_when_saving"]

  extension = filename.split('.')[-1].upper()

  if extension == "BIF":
    bn.saveBIF(filename, allowModificationWhenSaving)
  elif extension == "BIFXML":
    bn.saveBIFXML(filename, allowModificationWhenSaving)
  elif extension == "DSL":
    bn.saveDSL(filename, allowModificationWhenSaving)
  elif extension == "XDSL":
    bn.saveXDSL(filename, allowModificationWhenSaving)
  elif extension == "NET":
    bn.saveNET(filename, allowModificationWhenSaving)
  elif extension == "UAI":
    bn.saveUAI(filename, allowModificationWhenSaving)
  elif extension == "O3PRM":
    bn.saveO3PRM(filename, allowModificationWhenSaving)
  elif extension == "PKL":
    import pickle
    with open(filename, "wb") as f:
      pickle.dump(bn, f, pickle.HIGHEST_PROTOCOL)
  else:
    raise InvalidArgument("[pyAgrum] extension " + filename.split('.')
    [-1] + " unknown. Please use among " + availableBNExts())


def availableMNExts():
  """ Give the list of all formats known by pyAgrum to save a Markov random field.

  Returns
  ------
  str
    a string which lists all suffixes for supported MRF file formats.
  """
  return "uai|pkl"


def loadMN(filename, listeners=None, verbose=False):
  """load a MRF from a file with optional listeners and arguments

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
  pyAgrum.MarkovRandomField
    a MRF from a file using one of the availableMNExts() suffixes.

  Listeners could be added in order to monitor its loading.

  pkl suffix is used to save a BN using pickle. In this case, options are ignored.

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
  mn = MarkovRandomField()

  extension = filename.split('.')[-1].upper()
  if extension == "UAI":
    warns = mn.loadUAI(filename, listeners)
  elif extension == "PKL":
    import pickle
    with open(filename, "rb") as f:
      mn = pickle.load(f)
  else:
    raise InvalidArgument("extension " + filename.split('.')
    [-1] + " unknown. Please use among " + availableBNExts())

  if verbose:
    print(warns)

  mn.setProperty("name", mn.propertyWithDefault("name", ospath.splitext(ospath.basename(filename))[0]))
  return mn


def saveMN(mn, filename):
  """
  save a MRF into a file using the format corresponding to one of the availableWriteMNExts() suffixes.

  Parameters
  ----------
  mn : pyAgrum.MarkovRandomField)
    the MRF to save
  filename : str
    the name of the output file
  """
  extension = filename.split('.')[-1].upper()

  if extension == "UAI":
    mn.saveUAI(filename)
  elif extension == "PKL":
    import pickle
    with open(filename, "wb") as f:
      pickle.dump(mn, f, pickle.HIGHEST_PROTOCOL)
  else:
    raise InvalidArgument("extension " + filename.split('.')[-1] + " unknown. Please use among " + availableMNExts())


def availableIDExts():
  """ Give the list of all formats known by pyAgrum to save a influence diagram.

  Returns
  ------
  str
    a string which lists all suffixes for supported ID file formats.
  """
  return "xmlbif|bifxml|xml|pkl"


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

  if extension in {"BIFXML", "XMLBIF", "XML"}:
    diag = InfluenceDiagram()
    # for now, just one format
    res = diag.loadBIFXML(filename)

    if not res:
      raise IOError(f"Error(s) in {filename}")
  elif extension == "PKL":
    import pickle
    with open(filename, "rb") as f:
      diag = pickle.load(f)
  else:
    raise InvalidArgument("extension " + filename.split('.')[-1] + " unknown. Please use among " + availableIDExts())

  diag.setProperty("name", diag.propertyWithDefault("name", ospath.splitext(ospath.basename(filename))[0]))
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

  if extension in {"BIFXML", "BIFXML", "XML"}:
    infdiag.saveBIFXML(filename)
  elif extension == "PKL":
    import pickle
    with open(filename, "wb") as f:
      pickle.dump(infdiag, f, pickle.HIGHEST_PROTOCOL)
  else:
    raise InvalidArgument("extension " + filename.split('.')[-1] + " unknown. Please use among " + availableBNExts())


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
  warnings.warn("""
** pyAgrum.fastMN is deprecated in pyAgrum>1.5.2.
** pyAgrum.fastMRF is called instead.
""", DeprecationWarning, stacklevel=2)
  return fastMRF(structure, domain_size)


def fastMRF(structure, domain_size=2):
  """
  Create a Markov random field with a modified dot-like syntax which specifies:
      - the structure 'a-b-c;b-d;c-e;' where each chain 'a-b-c' specifies a factor,
      - the type of the variables with different syntax (cf documentation).

  Examples
  --------
  >>> import pyAgrum as gum
  >>> bn=gum.fastMRF('A--B[1,3]--C{yes|No};C--D[2,4]--E[1,2.5,3.9]',6)

  Parameters
  ----------
  structure : str
          the string containing the specification
  domain_size : int
          the default domain size for variables

  Returns
  -------
  pyAgrum.MarkovRandomField
          the resulting Markov random field
  """
  return MarkovRandomField.fastPrototype(structure, domain_size)


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


def getPosterior(model, *, target, evs=None):
  """
  Compute the posterior of a single target (variable) in a BN given evidence


  getPosterior uses a VariableElimination inference.
  If more than one target is needed with the same set of evidence or if the same
  target is needed with more than one set of evidence, this function is not
  relevant since it creates a new inference engine every time it is called.

  Parameters
  ----------
  bn : pyAgrum.BayesNet or pyAgrum.MarkovRandomField
    The probabilistic Graphical Model
  target: string or int
    variable name or id (forced keyword argument)
  evs:  Dict[name|id:val, name|id : List[ val1, val2 ], ...]. (optional forced keyword argument)
    the (hard and soft) evidence

  Returns
  -------
    posterior (pyAgrum.Potential or other)
  """
  if isinstance(model, BayesNet):
    inf = VariableElimination(model)
  elif isinstance(model, MarkovRandomField):
    inf = ShaferShenoyMRFInference(model)
  else:
    raise InvalidArgument("Argument model should be a PGM (BayesNet or MarkovRandomField")

  if evs is not None:
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


def randomBN(*, n: int = 5, names: List[str] = None, ratio_arc: float = 1.2, domain_size: int = 2) -> BayesNet:
  """
  Creates a random BN using the (forced) keyword parameters. This function use :class:`pyAgrum.BNGenerator` but the random
  variables will be named w.r.t. a topological order.

  Warning
  -------
  Number of nodes given with arg `n`or `names` must be bigger than 4, in order to be consistant

  Examples
  --------
  >>> bn=gum.randomBN()
  >>> bn=gum.randomBN(n=10)
  >>> bn=gum.randomBN(names="ABCDEF")
  >>> bn=gum.randomBN(names=["Asia","Tuberculosis","Smoking"],ratio_arc=1.5,domain_size=3)

  Warnings
  --------
  This function has only keyword parameters (no positional).

  Parameters
  ----------
  n : int
      number of nodes
  names: List[str]
      list of names
  ratio_arc: float
      number of arcs = n * ratio_arc
  domain_size: int
      the domain size for the variables.

  Returns
  -------
    pyAgrum.BayesNet
  """
  nbr = n if names is None else len(names)
  if nbr <= 3:
    raise ArgumentError("A BN can not be randomly generated from less than 4 nodes.")

  gen = BNGenerator()
  bn = gen.generate(nbr, int(nbr * ratio_arc), domain_size)

  if names is not None:
    # try to find very rare name
    for i in bn.nodes():
      bn.changeVariableName(i, f"__{i}{i}__{i}{i}__")
    for i, nod in enumerate(bn.topologicalOrder()):
      bn.changeVariableName(nod, names[i])

  return bn


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
        new_dis = [1 / n for _ in range(n)]

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


def fastGraph(msg:str):
  """
  Create a graph with a dot-like syntax which specifies the structure 'a->b->c;b-d<-e;' where a,b,c,d,e,.. are unsigned int

  Warnings
  --------
    The choice of "-" for edges is unambiguous because "-" is not a valid character for a node id (unsigned int).
    Moreover, "--" is already used by `pyAgrum.fastMRF` to specify factors.

  Parameters
  ----------
    msg : str
      the string containing the specification

  Returns
  -------
    pyAgrum.DiGraph ou pyAgrum.UndiGraph ou pyAgrum.MixedGraph
  """
  # regexp to recognize strings with only unsigned int, ";", "-" followed by ">" or ">"


  import re
  if not re.match(r"^[\d;\-><]+$", msg):
    raise InvalidArgument("fastGraph only accepts strings with only unsigned int, ';', '->' and '<-'")

  is_arc="->" in msg or "<-" in msg
  is_edge="-" in msg
  if is_arc:
    if is_edge:
      m=pyAgrum.MixedGraph()
    else:
      m=pyAgrum.DiGraph()
  else:
    m=pyAgrum.UndiGraph()

  def addEdgeIn(m,msg):
    t=msg.split("-")
    if len(t)==1:
      n1=int(msg)
      deb=n1
      if not m.existsNode(n1):
        m.addNodeWithId(n1)
    else:
      deb,n1=addEdgeIn(m,t[0])
      for i in range(1,len(t)):
        d,f=addEdgeIn(m,t[i])
        m.addEdge(n1,d)
        n1=f
    return deb,n1
  def addRevArcsIn(m,msg):
    t=msg.split("<-")
    if len(t)==1:
      deb,n1=addEdgeIn(m,msg)
    else:
      deb,fin=addEdgeIn(m,t[0])
      n1=fin
      for i in range(1,len(t)):
        d,f=addEdgeIn(m,t[i])
        m.addArc(d,n1)
        n1=f
    return deb,n1
  def addArcsIn(m,msg):
    t=msg.split("->")
    if len(t)==1:
      deb,n1=addRevArcsIn(m,msg)
    else:
      deb,fin=addRevArcsIn(m,t[0])
      n1=fin
      for i in range(1,len(t)):
        d,f=addRevArcsIn(m,t[i])
        m.addArc(n1,d)
        n1=f
    return deb,n1

  for l in msg.split(";"):
    if len(l)>0:
      t=addArcsIn(m,l)

  return m



# seed is chosen randomly :)
initRandom(0)

# configuration object
from .config import PyAgrumConfiguration

config = PyAgrumConfiguration()
config.add_hook(_update_config_core)
config.run_hooks()

try:
  # load custom configuration if any
  config.load()
except FileNotFoundError:
  pass


%}
