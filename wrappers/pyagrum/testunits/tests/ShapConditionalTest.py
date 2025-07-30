# Les imports
import unittest
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum as gum
from pyagrum.explain import ConditionalShapValues
import pandas as pd
import numpy as np
import itertools

# Load the data
data = pd.read_csv('tests/resources/iris.csv')
data['PetalLengthCm'] = pd.cut(data['PetalLengthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['PetalWidthCm'] = pd.cut(data['PetalWidthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['SepalLengthCm'] = pd.cut(data['SepalLengthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['SepalWidthCm'] = pd.cut(data['SepalWidthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)

# Create the Bayesian Network and the ConditionalShapValues instance
learner = gum.BNLearner(data)
bn = learner.learnBN()
explainer = ConditionalShapValues(bn, 5)

class ShapConditionalTest(pyAgrumTestCase) :
    
    def test__shap_1dim(self) :
        instance_0 = {'SepalLengthCm': 1, 'SepalWidthCm': 3, 'PetalLengthCm': 0, 'PetalWidthCm': 0}
        instance_1 = {'SepalLengthCm': 0, 'SepalWidthCm': 2, 'PetalLengthCm': 0, 'PetalWidthCm': 0}
        instance_2 = {'SepalLengthCm': 0, 'SepalWidthCm': 3, 'PetalLengthCm': 0, 'PetalWidthCm': 0}
        instance_3 = {'SepalLengthCm': 0, 'SepalWidthCm': 3, 'PetalLengthCm': 0}
        instance_4 = {'SepalLengthCm': 0, 'SepalWidthCm': 3}


        posterior0 = sum(list(explainer.compute((instance_0, False))._values[1].values())) + explainer.baseline[1]
        posterior1 = sum(list(explainer.compute((instance_1, False))._values[1].values())) + explainer.baseline[1]
        posterior2 = sum(list(explainer.compute((instance_2, False))._values[1].values())) + explainer.baseline[1]
        posterior3 = sum(list(explainer.compute((instance_3, False))._values[1].values())) + explainer.baseline[1]
        posterior4 = sum(list(explainer.compute((instance_4, False))._values[1].values())) + explainer.baseline[1]

        explainer.ie.eraseAllEvidence()

        explainer.ie.updateEvidence(instance_0)
        x = explainer.func( explainer.ie.posterior(explainer.target)[1] ).round(5)
        assert x == round(posterior0, 5), f'{x} ?= {round(posterior0, 5)}'

        explainer.ie.updateEvidence(instance_1)
        x = explainer.func( explainer.ie.posterior(explainer.target)[1] ).round(5)
        assert x == round(posterior1, 5), f'{x} ?= {round(posterior1, 5)}'

        explainer.ie.updateEvidence(instance_2)
        x = explainer.func( explainer.ie.posterior(explainer.target)[1] ).round(5)
        assert x == round(posterior2, 5), f'{x} ?= {round(posterior2, 5)}'

        explainer.ie.eraseAllEvidence()
        explainer.ie.updateEvidence(instance_3)
        x = explainer.func( explainer.ie.posterior(explainer.target)[1] ).round(5)
        assert x == round(posterior3, 5), f'{x} ?= {round(posterior3, 5)}'

        explainer.ie.eraseAllEvidence()
        explainer.ie.updateEvidence(instance_4)
        x = explainer.func( explainer.ie.posterior(explainer.target)[1] ).round(5)
        assert x == round(posterior4, 5), f'{x} ?= {round(posterior4, 5)}'

    def test_globalShap(self):
        # Test the global_shap function
        combinations = np.array(list(itertools.product([1], range(5), range(5), range(5), range(5), range(1)))).astype(int)
        df = pd.DataFrame(combinations, columns=['Id', 'SepalLengthCm', 'SepalWidthCm', 'PetalLengthCm', 'PetalWidthCm', 'Species'])   
        expl = explainer.compute((df, False))
        assert round(expl.importances[1]['Id'], 5) == 0.
        assert round(expl.importances[1]['SepalLengthCm'], 5) == 0.62684
        assert round(expl.importances[1]['SepalWidthCm'], 5) == 2.11577
        assert round(expl.importances[1]['PetalLengthCm'], 5) == 3.68744
        assert round(expl.importances[1]['PetalWidthCm'], 5) == 3.38405

ts = unittest.TestSuite()
addTests(ts, ShapConditionalTest)

