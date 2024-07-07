%pythoncode %{
import warnings

def availableMNExts():
  """ Give the list of all formats known by pyAgrum to save a Markov random field.

  Returns
  ------
  str
    a string which lists all suffixes for supported MRF file formats.
  """
  return "uai|pkl"


def loadMN(filename, listeners=None, verbose=False):
  warnings.warn("""
** loadMN() is deprecated since pyAgrum>1.7.2. Please use loadMRF() methods instead.
""", DeprecationWarning, stacklevel=2)
  return loadMRF(filename,listeners,verbose)

def loadMRF(filename, listeners=None, verbose=False):
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
  >>> gum.loadMRF('./bn.uai',listeners=[foo_listener,bar_listener])
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
  warnings.warn("""
** saveMN() is deprecated since pyAgrum>1.7.2. Please use saveMRF() methods instead.
""", DeprecationWarning, stacklevel=2)
  return saveMRF(mn,filename)

def saveMRF(mn, filename):
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


def fastMN(structure, domain="[2]"):
  warnings.warn("""
** fastMN() is deprecated since pyAgrum>1.7.2. Please use fastMRF() methods instead.
""", DeprecationWarning, stacklevel=2)
  return fastMRF(structure, domain)

def fastMRF(structure, domain="[2]"):
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
  domain: int or str
          the default domain size (int) or domain specification (str) for variables (default is "[2]"

  Returns
  -------
  pyAgrum.MarkovRandomField
          the resulting Markov random field
  """
  return MarkovRandomField.fastPrototype(structure, domain)


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
  if isinstance(model, pyAgrum.bn.BayesNet):
    inf = pyAgrum.bn.VariableElimination(model)
  elif isinstance(model, MarkovRandomField):
    inf = ShaferShenoyMRFInference(model)
  else:
    raise InvalidArgument("Argument model should be a PGM (BayesNet or MarkovRandomField")

  if evs is not None:
    inf.setEvidence(evs)
  inf.addTarget(target)
  inf.makeInference()
  # creating a new Potential from posterior(will disappear with ie)
  return pyAgrum.base.Potential(inf.posterior(target))

########################################################################################################
def MarkovNet(*args, **kwargs):
  """
  Deprecated class. Use pyAgrum.MarkovRandomField instead.
  """
  warnings.warn("""
** pyAgrum.MarkovNet is deprecated in pyAgrum>1.5.2.
** A pyAgrum.MarkovRandomField has been created.
""", DeprecationWarning, stacklevel=2)
  return MarkovRandomField(*args, **kwargs)

########################################################################################################
def ShaferShenoyMNInference(mrf):
  """
  Deprecated class. Use pyAgrum.ShaferShenoyMRFInference instead.
  """
  warnings.warn("""
** pyAgrum.ShaferShenoyMNInference is deprecated in pyAgrum>1.5.2.
** A pyAgrum.ShaferShenoyMRFInference has been created.
""", DeprecationWarning, stacklevel=2)
  return ShaferShenoyMRFInference(mrf)

########################################################################################################
def deprecated_MN(mrfie):
  """
  Deprecated method in gum.ShaferShenoyMRFInference for pyAgrum>1.5.2
  """
  warnings.warn("""
** pyAgrum.ShaferShenoyMRFInference.MN() is deprecated since pyAgrum>1.5.2. Please use pyAgrum.ShaferShenoyMRFInference.MRF() methods instead.
""", DeprecationWarning, stacklevel=2)
  return mrfie.MRF()

ShaferShenoyMRFInference.MN = deprecated_MN

########################################################################################################
ShaferShenoyMRFInference.H = pyAgrum.bn.deprecatedH
ShaferShenoyMRFInference.VI = pyAgrum.bn.deprecatedVI
ShaferShenoyMRFInference.I = pyAgrum.bn.deprecatedI
%}
