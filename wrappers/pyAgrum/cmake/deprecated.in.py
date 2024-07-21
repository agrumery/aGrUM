"""
Deprecated for older pyAgrum
"""
import warnings
import functools

from .pyAgrum import MarkovRandomField, ShaferShenoyMRFInference
from .pyAgrum import InformationTheory, LazyPropagation
from .pyAgrum import BNLearner, JunctionTreeGenerator
from .pyAgrum import DiscreteVariable
from .pyAgrum import Potential



def deprecated_arg(newA: str, oldA: str, version: str):
  """
  Annotation of a function when changing the name of an argument of the function

  Example
  ------
  @gum.deprecated_arg("x","old_x","1.8")
  def f(x:int):
    return 2*X

  Parameters
  ----------
  newA:str
    the new name of the argument
  oldA:str
    the old name of the argument
  version:str
    the version of pyAgrum
  """

  def deco(f):
    @functools.wraps(f)
    def wrapper(*args, **kwargs):
      if oldA in kwargs:
        if newA in kwargs:
          warnings.warn(
            f"""
** pyAgrum : argument '{oldA}' is deprecated since '{version}', '{newA}' is used instead.
""", DeprecationWarning, stacklevel=2)
          kwargs.pop(oldA)
        else:
          warnings.warn(
            f"""
** pyAgrum : argument '{oldA}' is deprecated since '{version}', please use '{newA}' is instead.
""", DeprecationWarning, stacklevel=2)
          kwargs[newA] = kwargs.pop(oldA)
      return f(*args, **kwargs)

    return wrapper

  return deco


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
def deprecated_learnMixedGraph(learner):
  """
  Deprecated methods in BNLearner for pyAgrum>1.5.2
  """
  warnings.warn("""
** pyAgrum.BNLearner.learnMixedGraph() is deprecated since pyAgrum>1.5.2. Please use learnPDAG() methods instead.
""", DeprecationWarning, stacklevel=2)
  return learner.learnPDAG()


BNLearner.learnMixedGraph = deprecated_learnMixedGraph


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
def deprecatedVI(self, X, Y):
  """
  Deprecated VI in LazyPropagation/ShaferShenoyMRFInference
  """
  warnings.warn("""
** pyAgrum.{inference}.VI() is deprecated since pyAgrum>1.7.1. Please use class pyAgrum.InformationTheory instead.
""", DeprecationWarning, stacklevel=2)
  it = InformationTheory(self, X, Y)
  return it.variationOfInformationXY()


ShaferShenoyMRFInference.VI = deprecatedVI
LazyPropagation.VI = deprecatedVI


########################################################################################################
def deprecatedI(self, X, Y):
  """
  Deprecated I in LazyPropagation/ShaferShenoyMRFInference
  """
  warnings.warn("""
** pyAgrum.{inference}.I() is deprecated since pyAgrum>1.7.1. Please use class pyAgrum.InformationTheory instead.
""", DeprecationWarning, stacklevel=2)
  it = InformationTheory(self, X, Y)
  return it.mutualInformationXY()


ShaferShenoyMRFInference.I = deprecatedI
LazyPropagation.I = deprecatedI


########################################################################################################
def deprecatedH(self, X):
  """
  Deprecated I in LazyPropagation/ShaferShenoyMRFInference
  """
  warnings.warn("""
** pyAgrum.{inference}.H() is deprecated since pyAgrum>1.7.1. Please use class pyAgrum.InformationTheory instead.
""", DeprecationWarning, stacklevel=2)
  it = InformationTheory(self, X, [])
  return it.entropyX()


ShaferShenoyMRFInference.H = deprecatedH
LazyPropagation.H = deprecatedH


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
