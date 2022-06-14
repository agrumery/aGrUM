# - * - coding : utf - 8 - * -
"""
Deprecated for older pyAgrum
"""
import warnings

from .pyAgrum import GibbsSampling, Potential
from .pyAgrum import Arc, Edge, DiGraph, UndiGraph, MixedGraph, DAG, CliqueGraph
from .pyAgrum import BayesNet, EssentialGraph, MarkovBlanket
from .pyAgrum import InfluenceDiagram, ShaferShenoyLIMIDInference
from .pyAgrum import ExactBNdistance, GibbsBNdistance
from .pyAgrum import BNLearner, JunctionTreeGenerator


def InfluenceDiagramInference(infdiag):
  """
  Deprecated class. Use pyAgrum.ShaferShenoyIDInference instead.
  """
  warnings.warn("""
  ** pyAgrum.InfluenceDiagramInference is deprecated in pyAgrum>0.18.2.
  ** A pyAgrum.ShaferShenoyLIMIDInference has been created.
  """, DeprecationWarning, stacklevel=2)
  return ShaferShenoyLIMIDInference(infdiag)


def ShaferShenoyIDInference(infdiag):
  """
  Deprecated class. Use pyAgrum.ShaferShenoyIDInference instead.
  """
  warnings.warn(""""
  ** pyAgrum.InfluenceDiagramInference is deprecated in pyAgrum>0.18.2.
  ** A pyAgrum.ShaferShenoyLIMIDInference has been created.
  """, DeprecationWarning, stacklevel=2)
  return ShaferShenoyLIMIDInference(infdiag)


def _addDeprecatedMethods():
    def deprecated_useNoApriori(learner):
        """
    Deprecated methods in BNLearner for pyAgrum>1.1.1
    """
        warnings.warn("""
    ** pyAgrum.BNLearner.useNoAriori() is deprecated from pyAgrum>1.1.1.
    ** Please use NoPrior() instead.
    """, DeprecationWarning, stacklevel=2)
        return learner.useNoPrior()

    def deprecated_useAprioriBDeu(learner):
        """
      Deprecated methods in BNLearner for pyAgrum>1.1.1
      """
        warnings.warn("""
    ** pyAgrum.BNLearner.useAprioriBDeu() is deprecated from pyAgrum>1.1.1.
    ** Please use useBDeuPrior() instead.
    """, DeprecationWarning, stacklevel=2)
        return learner.useBDeuPrior()

    def deprecated_useAprioriSmoothing(learner):
        """
    Deprecated methods in BNLearner for pyAgrum>1.1.1
    """
        warnings.warn("""
    ** pyAgrum.BNLearner.useAprioriSmoothing() is deprecated from pyAgrum>1.1.1.
    ** Please use useSmoothingPrior() methods instead.
    """, DeprecationWarning, stacklevel=2)
        return learner.useSmoothingPrior()

    def deprecated_useAprioriDirichlet(learner):
        """
    Deprecated methods in BNLearner for pyAgrum>1.1.1
    """
        warnings.warn("""
    ** pyAgrum.BNLearner.useAprioriDirichlet() is deprecated from pyAgrum>1.1.1.
    ** Please use useDirichletPrior() methods instead.
    """, DeprecationWarning, stacklevel=2)
        return learner.useDirichletPrior()

    BNLearner.useNoApriori = deprecated_useNoApriori
    BNLearner.useAprioriBDeu = deprecated_useAprioriBDeu
    BNLearner.useAprioriSmoothing = deprecated_useAprioriSmoothing
    BNLearner.useAprioriDirichlet = deprecated_useAprioriDirichlet


def getNumberOfRunningThreads():
    warnings.warn(""""
  ** pyAgrum.getNumberOfRunningThreads is obsolete in pyAgrum>0.22.7.
  """)


def getDynamicThreadsNumber():
    warnings.warn(""""
  ** pyAgrum.getDynamicThreadsNumber is obsolete in pyAgrum>0.22.7.
  """)


def setDynamicThreadsNumber(n):
    warnings.warn(""""
  ** pyAgrum.setDynamicThreadsNumber is obsolete in pyAgrum>0.22.7.
  """)


def getNestedParallelism():
    warnings.warn(""""
  ** pyAgrum.getNestedParallelism is obsolete in pyAgrum>0.22.7.
  """)


def setNestedParallelism(n):
    warnings.warn(""""
  ** pyAgrum.setNestedParallelism is obsolete in pyAgrum>0.22.7.
  """)


def getThreadNumber():
    warnings.warn(""""
  ** pyAgrum.getThreadNumber is obsolete in pyAgrum>0.22.7.
  """)


_addDeprecatedMethods()
