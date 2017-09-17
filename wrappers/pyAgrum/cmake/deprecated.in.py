#- * - coding : utf - 8 - * -
"""
Deprecated for older pyAgrum
"""
from .pyAgrum import GibbsSampling

def GibbsInference(bn):
    print("** pyAgrum.GibbsInference is deprecated in pyAgrum>0.12.0. A pyAgrum.GibbsSampling has been created.")
    return GibbsSampling(bn)
