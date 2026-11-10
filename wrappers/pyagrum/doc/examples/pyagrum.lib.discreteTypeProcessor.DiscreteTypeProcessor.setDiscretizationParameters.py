# requires: pandas, sklearn
import numpy as np
import pandas as pd

from pyagrum.lib.discreteTypeProcessor import DiscreteTypeProcessor

rng = np.random.default_rng(42)
X = pd.DataFrame({"x1": rng.normal(size=200), "x2": rng.integers(0, 3, size=200)})
dtp = DiscreteTypeProcessor(defaultDiscretizationMethod="uniform", defaultNumberOfBins=3)
dtp.setDiscretizationParameters("x1", "quantile", 5)
bn = dtp.discretizedTemplate(X)
bn.variable("x1").domainSize()
# 5
