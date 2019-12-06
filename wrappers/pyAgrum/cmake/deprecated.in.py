# - * - coding : utf - 8 - * -
"""
Deprecated for older pyAgrum
"""
from .pyAgrum import GibbsSampling, Potential
from .pyAgrum import Arc, Edge, DiGraph, UndiGraph, MixedGraph, DAG, CliqueGraph
from .pyAgrum import BayesNet, EssentialGraph, MarkovBlanket,InfluenceDiagram
from .pyAgrum import ExactBNdistance,GibbsBNdistance
from .pyAgrum import BNLearner


def GibbsInference(bn):
  """
  Deprecated class. Use pyAgrum.GibbsSampling instead.
  """
  print("** pyAgrum.GibbsInference is deprecated in pyAgrum>0.12.0.")
  print("** A pyAgrum.GibbsSampling has been created.")
  return GibbsSampling(bn)

def JTGenerator():
  """
  Deprecated class. Use pyAgrum.JunctionTreeGenerator instead.
  """
  print("** pyAgrum.JTGenerator is deprecated in pyAgrum>0.12.6.")
  print("** A pyAgrum.JunctionTreeGenerator has been created.")
  return JunctionTreeGenerator()

def GibbsKL(p,q):
  """
  Deprecated class. Use pyAgrum.GibbsBNdistance instead.
  """
  print("** pyAgrum.GibbsKL is deprecated in pyAgrum>0.12.6.")
  print("** A pyAgrum.GibbsBNdistance has been created.")
  return GibbsBNdistance(p,q)

def BruteForceKL(p,q):
  """
  Deprecated class. Use pyAgrum.ExactBNdistance instead.
  """
  print("** pyAgrum.BruteForceKL is deprecated in pyAgrum>0.12.6.")
  print("** A pyAgrum.ExactBNdistance has been created.")
  return ExactBNdistance(p,q)

def _addDeprecatedMethods():
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
  BayesNet.ids = deprecated_ids
  InfluenceDiagram.ids = deprecated_ids

  def deprecated_setAprioriWeight(learner,weight):
    """
    Deprecated methods in BNLearner for pyAgrum>0.14.0
    """
    print("** pyAgrum.BNLearner.setAprioriWeight() is removed in pyAgrum>0.14.0.")
    print("** Please use the weight argument in `useApriori...` methods instead.")
    return
  BNLearner.setAprioriWeight=deprecated_setAprioriWeight

#from .pyAgrum import BayesNet, EssentialGraph, MarkovBlanket



_addDeprecatedMethods()
