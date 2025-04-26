/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/




%pythoncode %{
from typing import List
import os.path as ospath
import warnings

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
  pyagrum.BayesNet
      a BN from a file using one of the availableBNExts() suffixes.

  Notes
  ----
      Listeners could be added in order to monitor its loading.

      pkl suffix is used to load a pickled BN. In this case, listeners and options are ignored.

  Examples
  --------
  >>> import pyagrum as gum
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
  bn : pyagrum.BayesNet
    the BN to save
  filename : str
    the name of the output file
  allowModificationWhenSaving: bool
      whether syntax errors in the BN should throw a FatalError or can be corrected. Also controlled by `pyagrum.config["BN","allow_modification_when_saving"]`.

  Notes
  ----
      pkl suffix is used to save a BN using pickle. In this case, options are ignored.
  """
  if allowModificationWhenSaving is None:
    allowModificationWhenSaving = pyagrum.config.asBool["BN", "allow_modification_when_saving"]

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

def fastBN(structure:str, domain="[2]"):
  """
  Create a Bayesian network with a dot-like syntax which specifies:
      - the structure 'a->b->c;b->d<-e;',
      - the type of the variables with different syntax (cf documentation).

  Examples
  --------
  >>> import pyagrum as gum
  >>> bn=gum.fastBN('A->B[1,3]<-C{yes|No}->D[2,4]<-E[1,2.5,3.9]',6)

  Parameters
  ----------
  structure : str
          the string containing the specification
  domain : int or str
          the default domain size (int) or domain specification (str) for variables (default is "[2]"

  Returns
  -------
  pyagrum.BayesNet
          the resulting bayesian network
  """
  return BayesNet.fastPrototype(structure, domain)


def generateSample(bn, n=1, name_out=None, show_progress=False, with_labels=True, random_order=True):
  """
  generate a CSV file of samples from a bn.

  Parameters
  ----------
  bn: pyagrum.BayesNet
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
  Creates a random BN using the (forced) keyword parameters. This function use :class:`pyagrum.BNGenerator` but the random
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
    pyagrum.BayesNet
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
  bn : pyagrum.pyagrum.BayesNet
    A bayesian network
  intervention : Dict[str,List[str|float|int]]
    set of variables on which we intervene to force the value
  observation : Dict[str,List[str|float|int]]
    set of variables whose value is observed

  Returns
  -------
  inter_bn : new bayesian network reflecting the interventions and observations (pyagrum.pyagrum.BayesNet)
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


%}
