# -*- encoding: UTF-8 -*-

import inspect
import logging
import os
import unittest

from numpy import ndarray

FORMAT = '%(asctime)-15s %(message)s'
logging.basicConfig(format=FORMAT)


def addTests(ts, cl):
  """
  adding test methods (which names begin by 'test')of class cl in testsuite ts

  :param ts: test suite
  :param cl: class
  """
  for met, _ in inspect.getmembers(cl):
    if met[0:4] == 'test':
      ts.addTest(cl(met))


class pyAgrumTestCase(unittest.TestCase):
  log = None

  def __init__(self, *args, **kwargs):
    super(pyAgrumTestCase, self).__init__(*args, **kwargs)
    if pyAgrumTestCase.log is None:
      pyAgrumTestCase.log = logging.getLogger("pyAgrumTestSuite")
      pyAgrumTestCase.log.warning("Initializing logger")

  def warn(self, s):
    pyAgrumTestCase.log.warning(s)

  def agrumSrcDir(self, s):
    t = s.split("ressources/")
    return os.path.dirname(__file__) + "/resources/" + t[1]

  def assertListsAlmostEqual(self, seq1, seq2, places=7):
    sequence = (tuple, list, ndarray)
    if len(seq1) != len(seq2):
      raise AssertionError("%s != %s" % (str(seq1), str(seq2)))
    for i, j in zip(seq1, seq2):
      if isinstance(i, sequence) and isinstance(j, (list, sequence)):
        self.assertListsAlmostEqual(i, j, places)
      else:
        self.assertAlmostEqual(i, j, places)

  def assertDelta(self, x, y, delta=0.05):
    number = (int, float)
    sequence = (list, tuple)

    if isinstance(x, number) and isinstance(y, number):
      if not ((max(x, y) - min(x, y)) <= delta):
        raise AssertionError("%s != %s" % (str(x), str(y)))

    elif isinstance(x, sequence) and isinstance(y, sequence):
      if len(x) != len(y):
        raise AssertionError("%s != %s" % (str(x), str(y)))
      for i, j in zip(x, y):
        self.assertDelta(i, j, delta)
    else:
      raise TypeError("assertDelta parameters must have the same type")
