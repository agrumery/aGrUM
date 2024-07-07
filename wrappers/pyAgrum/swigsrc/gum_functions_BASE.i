%pythoncode %{
import warnings

# seed is chosen randomly :)
initRandom(0)

def _update_config_core():
  # hook to control some parameters for core params
  setNumberOfThreads(config.asInt['core', 'default_maxNumberOfThreads'])

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


def _gum_add_properties_while_getstate_(model):
  if not hasattr(model, "setProperty"):
    return

  if config.asBool["Pickle", "add_version"]:
    model.setProperty("version", f"pyAgrum {__version__}")
  if config.asBool["Pickle", "add_date"]:
    from datetime import datetime
    model.setProperty("creation", model.propertyWithDefault("creation", datetime.now().strftime("%Y-%m-%d %H:%M:%S%z")))
    model.setProperty("lastModification", datetime.now().strftime("%Y-%m-%d %H:%M:%S%z"))


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

########################################################################################################
def deprecatedMargSumOut(self, V):
  warnings.warn("""
** pyAgrum.Potential.margSumOut() is deprecated since pyAgrum>1.14.0. Please use pyAgrum.Potential.sumOut() instead.
""", DeprecationWarning, stacklevel=2)
  return self.sumOut(V)


def deprecatedMargSumIn(self, V):
  warnings.warn("""
** pyAgrum.Potential.margSumIn() is deprecated since pyAgrum>1.14.0. Please use pyAgrum.Potential.sumIn() instead.
""", DeprecationWarning, stacklevel=2)
  return self.sumIn(V)


def deprecatedMargProdOut(self, V):
  warnings.warn("""
** pyAgrum.Potential.margProdOut() is deprecated since pyAgrum>1.14.0. Please use pyAgrum.Potential.prodOut() instead.
""", DeprecationWarning, stacklevel=2)
  return self.prodOut(V)


def deprecatedMargProdIn(self, V):
  warnings.warn("""
** pyAgrum.Potential.margProdIn() is deprecated since pyAgrum>1.14.0. Please use pyAgrum.Potential.prodIn() instead.
""", DeprecationWarning, stacklevel=2)
  return self.prodIn(V)


def deprecatedMargMinOut(self, V):
  warnings.warn("""
** pyAgrum.Potential.margMinOut() is deprecated since pyAgrum>1.14.0. Please use pyAgrum.Potential.minOut() instead.
""", DeprecationWarning, stacklevel=2)
  return self.minOut(V)


def deprecatedMargMinIn(self, V):
  warnings.warn("""
** pyAgrum.Potential.margMinIn() is deprecated since pyAgrum>1.14.0. Please use pyAgrum.Potential.minIn() instead.
""", DeprecationWarning, stacklevel=2)
  return self.minIn(V)


def deprecatedMargMaxOut(self, V):
  warnings.warn("""
** pyAgrum.Potential.margMaxOut() is deprecated since pyAgrum>1.14.0. Please use pyAgrum.Potential.maxOut() instead.
""", DeprecationWarning, stacklevel=2)
  return self.maxOut(V)


def deprecatedMargMaxIn(self, V):
  warnings.warn("""
** pyAgrum.Potential.margMaxIn() is deprecated since pyAgrum>1.14.0. Please use pyAgrum.Potential.maxIn() instead.
""", DeprecationWarning, stacklevel=2)
  return self.maxIn(V)


Potential.margSumOut = deprecatedMargSumOut
Potential.margSumIn = deprecatedMargSumIn
Potential.margProdOut = deprecatedMargProdOut
Potential.margProdIn = deprecatedMargProdIn
Potential.margMinOut = deprecatedMargMinOut
Potential.margMinIn = deprecatedMargMinIn
Potential.margMaxOut = deprecatedMargMaxOut
Potential.margMaxIn = deprecatedMargMaxIn

########################################################################################################
def deprecated_toLabelizedVar(var):
  """
  Deprecated method in gum.DiscreteVariable for pyAgrum>1.5.2
  """
  warnings.warn("""
** pyAgrum.DiscreteVariable.toLabelizedVar() is deprecated since pyAgrum>1.5.2. Please use pyAgrum.DiscreteVariable.asLabelizedVar() method instead.
""", DeprecationWarning, stacklevel=2)
  return var.asLabelizedVar()


DiscreteVariable.toLabelizedVar = deprecated_toLabelizedVar


########################################################################################################
def deprecated_toRangeVar(var):
  """
  Deprecated method in gum.DiscreteVariable for pyAgrum>1.5.2
  """
  warnings.warn("""
** pyAgrum.DiscreteVariable.toRangeVar() is deprecated since pyAgrum>1.5.2. Please use pyAgrum.DiscreteVariable.asRangeVar() method instead.
""", DeprecationWarning, stacklevel=2)
  return var.asRangeVar()


DiscreteVariable.toRangeVar = deprecated_toRangeVar


########################################################################################################
def deprecated_toIntegerVar(var):
  """
  Deprecated method in gum.DiscreteVariable for pyAgrum>1.5.2
  """
  warnings.warn("""
** pyAgrum.DiscreteVariable.toIntegerVar() is deprecated since pyAgrum>1.5.2. Please use pyAgrum.DiscreteVariable.asIntegerVar() method instead.
""", DeprecationWarning, stacklevel=2)
  return var.asIntegerVar()


DiscreteVariable.toIntegerVar = deprecated_toIntegerVar


########################################################################################################
def deprecated_toNumericalDiscreteVar(var):
  """
  Deprecated method in gum.DiscreteVariable for pyAgrum>1.5.2
  """
  warnings.warn("""
** pyAgrum.DiscreteVariable.toNumericalDiscreteVar() is deprecated since pyAgrum>1.5.2. Please use pyAgrum.DiscreteVariable.asNumericalDiscreteVar() method instead.
""", DeprecationWarning, stacklevel=2)
  return var.asNumericalDiscreteVar()


DiscreteVariable.toNumericalDiscreteVar = deprecated_toNumericalDiscreteVar


########################################################################################################
def deprecated_toDiscretizedVar(var):
  """
  Deprecated method in gum.DiscreteVariable for pyAgrum>1.5.2
  """
  warnings.warn("""
** pyAgrum.DiscreteVariable.toDiscretizedVar() is deprecated since pyAgrum>1.5.2. Please use pyAgrum.DiscreteVariable.asDiscretizedVar() methods instead.
""", DeprecationWarning, stacklevel=2)
  return var.asDiscretizedVar()


DiscreteVariable.toDiscretizedVar = deprecated_toDiscretizedVar
%}
