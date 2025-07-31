# Les imports
import unittest
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum as gum
from pyagrum.explain import CausalShallValues

import pandas as pd

# Load the data
data = pd.read_csv('tests/resources/iris.csv', index_col="Id").drop(columns=['Species'])

data['PetalLengthCm'] = pd.cut(data['PetalLengthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['PetalWidthCm'] = pd.cut(data['PetalWidthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['SepalLengthCm'] = pd.cut(data['SepalLengthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['SepalWidthCm'] = pd.cut(data['SepalWidthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)

learner = gum.BNLearner(data)
bn = learner.learnBN()

explainer = CausalShallValues(bn, background=(data, True), log=True)

inst = gum.Instantiation()
for var in bn.ids(data.columns):
    inst.add(bn.variable(var))

class ShallMarginalTest(pyAgrumTestCase) :
    def test__shap_1dim(self) :
        explainer = CausalShallValues(bn, background=(data, True), log=True)

        instance_0 = {'SepalLengthCm': 1, 'SepalWidthCm': 3, 'PetalLengthCm': 0, 'PetalWidthCm': 0}
        instance_1 = {'SepalLengthCm': 0, 'SepalWidthCm': 2, 'PetalLengthCm': 0, 'PetalWidthCm': 0}
        instance_2 = {'SepalLengthCm': 1, 'SepalWidthCm': 3, 'PetalLengthCm': 0, 'PetalWidthCm': 0}
        instance_3 = {'SepalLengthCm': 1, 'SepalWidthCm': 3, 'PetalLengthCm': 0, 'PetalWidthCm': 0}
        instance_4 = {'SepalLengthCm': 4, 'SepalWidthCm': 3, 'PetalLengthCm': 4, 'PetalWidthCm': 3}

        joint0 = sum(list(explainer.compute((instance_0, False)).values())) + explainer.baseline
        joint1 = sum(list(explainer.compute((instance_1, False)).values())) + explainer.baseline
        joint2 = sum(list(explainer.compute((instance_2, False)).values())) + explainer.baseline
        joint3 = sum(list(explainer.compute((instance_3, False)).values())) + explainer.baseline
        joint4 = sum(list(explainer.compute((instance_4, False)).values())) + explainer.baseline
        
        inst.fromdict(instance_0)
        x = round(explainer.func( bn.jointProbability(inst) ), 5)
        assert x == round(joint0, 5), f'{x} ?= {round(joint0, 5)}, {joint0}'

        inst.fromdict(instance_1)
        x = round(explainer.func( bn.jointProbability(inst) ), 5)
        assert x == round(joint1, 5), f'{x} ?= {round(joint1, 5)}, {joint1}'

        inst.fromdict(instance_2)
        x = round(explainer.func( bn.jointProbability(inst) ), 5)
        assert x == round(joint2, 5), f'{x} ?= {round(joint2, 5)}, {joint2}'

        inst.fromdict(instance_3)
        x = round(explainer.func( bn.jointProbability(inst) ), 5)
        assert x == round(joint3, 5), f'{x} ?= {round(joint3, 5)}, {joint3}'

        inst.fromdict(instance_4)
        x = round(explainer.func( bn.jointProbability(inst) ), 5)
        assert x == round(joint4, 5), f'{x} ?= {round(joint4, 5)}, {joint4}'

ts = unittest.TestSuite()
addTests(ts, ShallMarginalTest)

