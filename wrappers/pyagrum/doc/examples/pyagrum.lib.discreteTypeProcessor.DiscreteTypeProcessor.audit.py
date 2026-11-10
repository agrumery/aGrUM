# requires: pandas, sklearn
import numpy as np
import pandas as pd

from pyagrum.lib.discreteTypeProcessor import DiscreteTypeProcessor

rng = np.random.default_rng(42)
X = pd.DataFrame({"x1": rng.normal(size=200), "x2": rng.integers(0, 3, size=200)})
dtp = DiscreteTypeProcessor(defaultDiscretizationMethod="uniform", defaultNumberOfBins=3)
audit = dtp.audit(X)
audit["x1"]["type"], audit["x1"]["method"], audit["x1"]["nbBins"]
# ('Continuous', 'uniform', 3)
audit["x2"]["type"], audit["x2"]["method"]  # few values: not discretized
# ('Discrete', 'NoDiscretization')
