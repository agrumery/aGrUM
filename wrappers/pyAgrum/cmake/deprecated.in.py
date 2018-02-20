# - * - coding : utf - 8 - * -
"""
Deprecated for older pyAgrum
"""
from .pyAgrum import GibbsSampling, Potential


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
    print("** pyAgrum.Potential.populate is deprecated in pyAgrum>0.12.0.")
    print("** pyAgrum.Potential.fillWith will be called instead.")
    return pot.fillWith(v)
  Potential.populate = deprecated_populate

  def deprecated_fill(pot, v):
    """
    Deprecated method in pyAgrum>0.12.0. See fillWith instead.
    """
    print("** pyAgrum.Potential.fill is deprecated in pyAgrum>0.12.0.")
    print("** pyAgrum.Potential.fillWith will be called instead.")
    return pot.fillWith(v)
  Potential.fill = deprecated_fill



addDeprecatedMethods()
