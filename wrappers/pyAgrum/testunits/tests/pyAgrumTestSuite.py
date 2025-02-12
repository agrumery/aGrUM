# -*- encoding: UTF-8 -*-

import inspect
import logging
import os
import unittest

from numpy import ndarray


def addTests(ts, cl):
  """
  adding test methods (which names begin by 'test')of class cl in testsuite ts

  Parameters
  ----------
  ts
    the test suite
  cl
    the test class t odd
  """
  for met, _ in inspect.getmembers(cl):
    if met[0:4] == 'test':
      ts.addTest(cl(met))


class pyAgrumTestCase(unittest.TestCase):
  def __init__(self, *args, **kwargs):
    super(pyAgrumTestCase, self).__init__(*args, **kwargs)
    self.log = logging.getLogger('gumTestLog')
    self.nbLoopForApproximatedTest = 10

  @staticmethod
  def agrumSrcDir(s: str) -> str:
    return f"{os.path.dirname(__file__)}/resources/{s}"

  def assertListsAlmostEqual(self, seq1, seq2, places=7, delta=None):
    sequence = (tuple, list, ndarray)
    if len(seq1) != len(seq2):
      raise AssertionError("%s != %s" % (str(seq1), str(seq2)))
    for i, j in zip(seq1, seq2):
      if isinstance(i, sequence) and isinstance(j, sequence):
        self.assertListsAlmostEqual(i, j, delta)
      else:
        if delta is None:
          self.assertAlmostEqual(i, j, places=places)
        else:
          self.assertAlmostEqual(i, j, delta=delta)
