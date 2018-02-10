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
    print("** pyAgrum.Potential.populate is deprecated in pyAgrum>0.12.0.")
    print("** pyAgrum.Potential.fillWith will be called instead.")
    return pot.fillWith(v)
  Potential.populate = deprecated_populate
  Potential.populate.__doc__ = "Deprecated method. See fillWith instead."

  def deprecated_fill(pot, v):
    print("** pyAgrum.Potential.fill is deprecated in pyAgrum>0.12.0.")
    print("** pyAgrum.Potential.fillWith will be called instead.")
    return pot.fillWith(v)
  Potential.fill = deprecated_fill
  Potential.fill.__doc__ = "Deprecated method. See fillWith instead."


addDeprecatedMethods()
