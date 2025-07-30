# Imports
import unittest
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum as gum
from pyagrum.explain._ComputationCausal import CausalComputation

import numpy as np
import pandas as pd

# Load the data
data = pd.read_csv('tests/resources/iris.csv')
data.drop(columns='Id', inplace=True)
data['PetalLengthCm'] = pd.cut(data['PetalLengthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['PetalWidthCm'] = pd.cut(data['PetalWidthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['SepalLengthCm'] = pd.cut(data['SepalLengthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['SepalWidthCm'] = pd.cut(data['SepalWidthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)

class ExplainCausalTest(pyAgrumTestCase) :
    def test_doCalculus(self) :
        bn1 = gum.fastBN("A->B")
        bn2 = gum.fastBN("B->A<-C")
        bn3 = gum.fastBN("B->A<-C; A->D->E<-F")
        doNet1 = CausalComputation._doCalculus(bn1, [bn1.idFromName('A')])
        doNet2 = CausalComputation._doCalculus(bn2, [bn2.idFromName('A')])
        doNet3 = CausalComputation._doCalculus(bn3, [bn3.idFromName('A'), bn3.idFromName('E')])

        assert doNet1.sizeArcs() == 1
        assert doNet2.sizeArcs() == 0
        assert doNet3.sizeArcs() == 1
    
    def test_chgCpt(self) :
        bn = gum.fastBN("X0->X1->X2<-X3")
        CausalComputation._chgCpt(bn, [0, 3], [0, 1])
        assert bn.cpt(0).sum() == 1.
        assert bn.cpt(0)[0] == 1.
        assert bn.cpt(3).sum() == 1.
        assert bn.cpt(3)[1] == 1.
    
    def test_extract(self) :
        assert np.all(CausalComputation._extract(data.head(10).to_numpy(), [0, 1], [1, 3]) == np.array([0, 5]))
        assert np.all(CausalComputation._extract(data.head(10).to_numpy(), [0, 1], [0, 2]) == np.array([1, 2, 3, 6, 7, 9]))
        assert len(CausalComputation._extract(data.head(10).to_numpy(), [2, 3], [1, 0])) == 0

ts = unittest.TestSuite()
addTests(ts, ExplainCausalTest)