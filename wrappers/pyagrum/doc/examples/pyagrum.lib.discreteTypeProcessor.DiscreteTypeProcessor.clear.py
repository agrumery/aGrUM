# requires: pandas, sklearn
import numpy as np
import pandas as pd

from pyagrum.lib.discreteTypeProcessor import DiscreteTypeProcessor

rng = np.random.default_rng(42)
X = pd.DataFrame({"x1": rng.normal(size=200), "x2": rng.integers(0, 3, size=200)})
dtp = DiscreteTypeProcessor(defaultDiscretizationMethod="uniform", defaultNumberOfBins=3)
bn = dtp.discretizedTemplate(X)
dtp.numberOfContinuous, dtp.totalNumberOfBins
# (1, 3)
dtp.clear()
dtp.numberOfContinuous, dtp.totalNumberOfBins
# (0, 0)
