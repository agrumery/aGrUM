import sys
import os
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests
import pyAgrum.causal as csl


class TestSimpson(pyAgrumTestCase):
  def setUp(self):
    m1 = gum.fastBN("Gender->Drug->Patient;Gender->Patient")

    m1.cpt("Gender")[:] = [0.5, 0.5]
    m1.cpt("Drug")[:] = [[0.25, 0.75],  # Gender=0
                         [0.75, 0.25]]  # Gender=1

    m1.cpt("Patient")[{'Drug': 0, 'Gender': 0}] = [0.2, 0.8]  # No Drug, Male -> healed in 0.8 of cases
    m1.cpt("Patient")[{'Drug': 0, 'Gender': 1}] = [0.6, 0.4]  # No Drug, Female -> healed in 0.4 of cases
    m1.cpt("Patient")[{'Drug': 1, 'Gender': 0}] = [0.3, 0.7]  # Drug, Male -> healed 0.7 of cases
    m1.cpt("Patient")[{'Drug': 1, 'Gender': 1}] = [0.8, 0.2]  # Drug, Female -> healed in 0.2 of cases
    self.model = csl.CausalModel(m1)

  def test_CausalImpactWithoutObservation(self):
    evs = {}
    latex, impact, explain = csl.causalImpact(self.model, "Patient", doing="Drug", knowing=evs)

  def test_CausalImpactWithObservation(self):
    evs = {'Gender'}
    latex, impact, explain = csl.causalImpact(self.model, "Patient", doing="Drug", knowing=evs)


class TestFromR(pyAgrumTestCase):
  def setUp(self):
    m = gum.fastBN("z2->x->z1->y;z2->z1;z2->z3->y")

    m.cpt("z2")[:] = [0.5, 0.5]
    m.cpt("x")[:] = [[0.4, 0.6],  # z2=0
                     [0.4, 0.6]]  # z2=1
    m.cpt("z3")[:] = [[0.3, 0.7],  # z2=0
                      [0.3, 0.7]]  # z2=1
    m.cpt("z1")[{"z2": 0, "x": 0}] = [0.2, 0.8]
    m.cpt("z1")[{"z2": 0, "x": 1}] = [0.25, 0.75]
    m.cpt("z1")[{"z2": 1, "x": 0}] = [0.1, 0.9]
    m.cpt("z1")[{"z2": 1, "x": 1}] = [0.15, 0.85]

    m.cpt("y")[{"z1": 0, "z3": 0}] = [0.5, 0.5]
    m.cpt("y")[{"z1": 0, "z3": 1}] = [0.45, 0.55]
    m.cpt("y")[{"z1": 1, "z3": 0}] = [0.4, 0.6]
    m.cpt("y")[{"z1": 1, "z3": 1}] = [0.35, 0.65]

    self.d = csl.CausalModel(m, [("X-Z2", ["x", "z2"]),
                                 ("X-Z3", ["x", "z3"]),
                                 ("X-Y", ["x", "y"]),
                                 ("Y-Z2", ["y", "z2"])],
                             True)
    try:
      formula, result, msg = csl.causalImpact(self.d, on={"y", "z2", "z1", "z3"}, doing={"x"})
    except csl.HedgeException as h:
      self.fail("Should not raise")


ts = unittest.TestSuite()
addTests(ts, TestSimpson)
addTests(ts, TestFromR)
