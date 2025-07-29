# Imports
import pyagrum as gum
import pandas as pd
import unittest
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

from pyagrum.explain._CustomShapleyCache import CustomShapleyCache

# Load the data
data = pd.read_csv('tests/resources/iris.csv')
data['PetalLengthCm'] = pd.cut(data['PetalLengthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['PetalWidthCm'] = pd.cut(data['PetalWidthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['SepalLengthCm'] = pd.cut(data['SepalLengthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['SepalWidthCm'] = pd.cut(data['SepalWidthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)

# Create the Bayesian Network and the ConditionalShapValues instance
learner = gum.BNLearner(data)
bn = learner.learnBN()

class CustomShapleyCacheTest(pyAgrumTestCase) :
    def test_generate_keys(self):
        nodes = [0, 1, 2]
        target = 5
        feat = 2
        key1, key2, key3 = CustomShapleyCache.generate_keys(bn, target, feat, nodes)
        assert key1 == (0, 1, 2)
        assert key2 == (0, 1)
        assert key3 == (1, 2)

        nodes = [0, 1, 2, 3, 4]
        key1, key2, key3 = CustomShapleyCache.generate_keys(bn, target, feat, nodes)
        assert key1 == (0, 1, 2, 3, 4)
        assert key2 == (0, 1, 3, 4)
        assert key3 == (1, 2, 3, 4)

        nodes = [1, 2]
        key1, key2, key3 = CustomShapleyCache.generate_keys(bn, target, feat, nodes)
        assert key1 == (1, 2)
        assert key2 == (1,)
        assert key3 == (1, 2)

    def test_cache_operations(self) :
        cache = CustomShapleyCache(max_capacity=3)
        cache.set(1, (0, 1), 10)
        assert cache.get(1, (0, 1)) == 10
        cache.set(2, (0, 1), 20)
        assert cache.get(2, (0, 1)) == 20
        cache.set(2, (0, 1, 2), 30) == 30
        assert cache.get(2, (0, 1, 2)) == 30
        cache.set(3, (0, 1, 2, 3), 40)
        assert cache.get(3, (0, 1, 2, 3)) == 40
        assert (cache._current_size == 2)
        try :   # Should be purged due to capacity limit
            cache.get(1, (0, 1))
            assert False
        except KeyError :
            pass

ts = unittest.TestSuite()
addTests(ts, CustomShapleyCacheTest)