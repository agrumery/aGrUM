# - * - coding : utf - 8 - * -
"""
Deprecated for older pyAgrum
"""
from .pyAgrum import GibbsSampling, Potential
from .pyAgrum import Arc, Edge, DiGraph, UndiGraph, MixedGraph, DAG, CliqueGraph
from .pyAgrum import BayesNet, EssentialGraph, MarkovBlanket


def GibbsInference(bn):
  """
  Deprecated class. Use pyAgrum.GibsSampling instead.
  """
  print("** pyAgrum.GibbsInference is deprecated in pyAgrum>0.12.0.")
  print("** A pyAgrum.GibbsSampling has been created.")
  return GibbsSampling(bn)


def addDeprecatedMethods():
  def deprecated_populate(pot, v):
    """
    Deprecated method in pyAgrum>0.12.0. See fillWith instead.
    """
    print("** pyAgrum.Potential.populate() is deprecated in pyAgrum>0.12.0.")
    print("** pyAgrum.Potential.fillWith() will be called instead.")
    return pot.fillWith(v)
  Potential.populate = deprecated_populate

  def deprecated_fill(pot, v):
    """
    Deprecated method in pyAgrum>0.12.0. See fillWith instead.
    """
    print("** pyAgrum.Potential.fill() is deprecated in pyAgrum>0.12.0.")
    print("** pyAgrum.Potential.fillWith() will be called instead.")
    return pot.fillWith(v)
  Potential.fill = deprecated_fill

  def deprecated_ids(gr):
    """
    Deprecated method in pyAgrum>0.12.0. See nodes instead.
    """
    print("** pyAgrum.*.ids() is deprecated in pyAgrum>0.12.0.")
    print("** pyAgrum.*.nodes() will be called instead.")
    return gr.nodes()
  DiGraph.ids=deprecated_ids # and subclasses
  UndiGraph.ids=deprecated_ids # and subclasses
  EssentialGraph.ids=deprecated_ids
  BayesNet.ids=deprecated_ids

#from .pyAgrum import BayesNet, EssentialGraph, MarkovBlanket



addDeprecatedMethods()
