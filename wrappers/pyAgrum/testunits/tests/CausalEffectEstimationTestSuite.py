############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import unittest
from typing import Set, List

import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests
import pyAgrum.causal as csl

from pyAgrum.causal.causalEffectEstimation._utils import (
  RCTError,
  BackdoorError,
  FrontdoorError,
  IVError
)

import logging

import pandas as pd
import numpy as np

from scipy.stats import norm, logistic

def _getDiscretizedVariable(name, tick_start, tick_end, num_intervals):
    variable = gum.DiscretizedVariable(name, name)
    ticks = np.linspace(tick_start, tick_end, num_intervals+1)
    for i in ticks:
        variable.addTick(i)
    variable.setEmpirical(True)
    return variable

_n = int(1e6)
_delta = 1e-1

class TestCausalEffectEstimation(pyAgrumTestCase):

  # Adjustment + Estimation testing

  def test_trivial_case(self):

    T = np.random.binomial(1, 0.5, _n)
    Y = np.random.normal(2*T, 1, _n)
    df = pd.DataFrame({"T": T, "Y": Y})

    bn = gum.BayesNet()
    bn.beginTopologyTransformation()
    bn.add(_getDiscretizedVariable("Y", -4,6, 20))
    bn.add(gum.IntegerVariable("T", "T", [0,1]))
    bn.addArc("T", "Y")
    bn.cpt("T").fillWith([0.5, 0.5])
    bn.cpt("Y").fillFromDistribution(norm, loc="2*T", scale=1)
    bn.endTopologyTransformation()
    cslbn = csl.CausalModel(bn)

    cee = csl.CausalEffectEstimation(df, cslbn)

    adjustment = cee.identifyAdjustmentSet("T", "Y", verbose=False)
    self.assertEqual(adjustment, cee._RCT)

    cee.fitCausalBNEstimator()
    CBN_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(CBN_estimation, 2.0, delta=_delta)

    cee.fitDM()
    DM_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(DM_estimation, 2.0, delta=_delta)

    self.assertRaises(RCTError, cee.fitSLearner)
    self.assertRaises(RCTError, cee.fitTLearner)
    self.assertRaises(RCTError, cee.fitXLearner)
    self.assertRaises(RCTError, cee.fitPStratification)
    self.assertRaises(RCTError, cee.fitIPW)

    self.assertRaises(RCTError, cee.fitSimplePlugIn)
    self.assertRaises(RCTError, cee.fitGeneralizedPlugIn)

    self.assertRaises(RCTError, cee.fitWald)
    self.assertRaises(RCTError, cee.fitWaldIPW)
    self.assertRaises(RCTError, cee.fitNormalizedWaldIPW)
    self.assertRaises(RCTError, cee.fitTSLS)

  def test_RCT_case(self):

    X = np.random.normal(1, 1, _n)
    T = np.random.binomial(1, 0.3, _n)
    Y = np.random.normal(X+2*T, 1, _n)
    df = pd.DataFrame({"X": X, "T": T, "Y":Y})

    bn = gum.BayesNet()
    bn.beginTopologyTransformation()
    bn.add(_getDiscretizedVariable("X", -4,6, 20))
    bn.add(_getDiscretizedVariable("Y", -4,6, 20))
    bn.add(gum.IntegerVariable("T", "T", [0,1]))
    bn.addArc("X", "Y")
    bn.addArc("T", "Y")
    bn.cpt("X").fillFromDistribution(norm, loc=1, scale=1)
    bn.cpt("T").fillWith([0.7, 0.3])
    bn.cpt("Y").fillFromDistribution(norm, loc="X + 2*T", scale=1)
    bn.endTopologyTransformation()
    cslbn = csl.CausalModel(bn)

    cee = csl.CausalEffectEstimation(df, cslbn)

    adjustment = cee.identifyAdjustmentSet("T", "Y", verbose=False)
    self.assertEqual(adjustment, cee._RCT)

    cee.fitCausalBNEstimator()
    CBN_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(CBN_estimation, 2.0, delta=_delta)

    cee.fitDM()
    DM_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(DM_estimation, 2.0, delta=_delta)

    cee.fitSLearner()
    SL_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(SL_estimation, 2.0, delta=_delta)

    cee.fitTLearner()
    TL_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(TL_estimation, 2.0, delta=_delta)

    cee.fitXLearner()
    XL_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(XL_estimation, 2.0, delta=_delta)

    cee.fitPStratification()
    PS_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(PS_estimation, 2.0, delta=_delta)

    cee.fitIPW()
    IPW_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(IPW_estimation, 2.0, delta=_delta)

    self.assertRaises(RCTError, cee.fitSimplePlugIn)
    self.assertRaises(RCTError, cee.fitGeneralizedPlugIn)

    self.assertRaises(RCTError, cee.fitWald)
    self.assertRaises(RCTError, cee.fitWaldIPW)
    self.assertRaises(RCTError, cee.fitNormalizedWaldIPW)
    self.assertRaises(RCTError, cee.fitTSLS)

  def test_backdoor_case(self):

    X = np.random.normal(1, 1, _n)
    T = np.random.binomial(1, np.power(1+np.exp(-X), -1))
    Y = np.random.normal(X+2*T, 1, _n)
    df = pd.DataFrame({"X": X, "T": T, "Y": Y}, index=np.arange(0, _n))

    bn = gum.BayesNet()
    bn.beginTopologyTransformation()
    bn.add(_getDiscretizedVariable("X", -4,6, 20))
    bn.add(_getDiscretizedVariable("Y", -4,6, 20))
    bn.add(gum.IntegerVariable("T", "T", [0,1]))
    bn.addArc("X", "T")
    bn.addArc("X", "Y")
    bn.addArc("T", "Y")
    bn.cpt("X").fillFromDistribution(norm, loc=1, scale=1)
    bn.cpt("T").fillFromDistribution(logistic, loc="X", scale=1)
    bn.cpt("Y").fillFromDistribution(norm, loc="X + 2*T", scale=1)
    bn.endTopologyTransformation()
    cslbn = csl.CausalModel(bn)

    cee = csl.CausalEffectEstimation(df, cslbn)

    adjustment = cee.identifyAdjustmentSet("T", "Y", verbose=False)
    self.assertEqual(adjustment, cee._BACKDOOR)

    cee.fitCausalBNEstimator()
    CBN_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(CBN_estimation, 2.0, delta=_delta)

    cee.fitSLearner()
    SL_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(SL_estimation, 2.0, delta=_delta)

    cee.fitTLearner()
    TL_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(TL_estimation, 2.0, delta=_delta)

    cee.fitXLearner()
    XL_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(XL_estimation, 2.0, delta=_delta)

    cee.fitPStratification()
    PS_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(PS_estimation, 2.0, delta=_delta)

    cee.fitIPW()
    IPW_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(IPW_estimation, 2.0, delta=_delta)

    self.assertRaises(BackdoorError, cee.fitDM)

    self.assertRaises(BackdoorError, cee.fitSimplePlugIn)
    self.assertRaises(BackdoorError, cee.fitGeneralizedPlugIn)

    self.assertRaises(BackdoorError, cee.fitWald)
    self.assertRaises(BackdoorError, cee.fitWaldIPW)
    self.assertRaises(BackdoorError, cee.fitNormalizedWaldIPW)
    self.assertRaises(BackdoorError, cee.fitTSLS)

  def test_simple_frontdoor_case(self):

    U = np.random.normal(1, 1, _n)
    T = np.random.binomial(1, np.power(1+np.exp(-U), -1))
    M = np.random.normal(T, 1, _n)
    Y = np.random.normal(U + 2*M, 1, _n)
    df = pd.DataFrame({"M": M, "T": T, "Y": Y}, index=np.arange(0, _n))

    bn = gum.BayesNet()
    bn.beginTopologyTransformation()
    bn.add(_getDiscretizedVariable("M", -4,6, 10))
    bn.add(_getDiscretizedVariable("Y", -4,6, 100))
    bn.add(gum.IntegerVariable("T", "T", [0,1]))
    bn.add(_getDiscretizedVariable("U", -4,6, 3))
    bn.addArc("U", "T")
    bn.addArc("U", "Y")
    bn.addArc("T", "M")
    bn.addArc("M", "Y")
    bn.cpt("U").fillFromDistribution(norm, loc=1, scale=1)
    bn.cpt("T").fillFromDistribution(logistic, loc="U", scale=1)
    bn.cpt("M").fillFromDistribution(norm, loc="T", scale=1)
    bn.cpt("Y").fillFromDistribution(norm, loc="U + 2*M", scale=1)
    ie = gum.LazyPropagation(bn)
    cpt_T = ie.posterior("T")
    cpt_Y = ie.evidenceImpact("Y", ["T", "M"])
    ie = None
    bn.eraseArc("U", "T")
    bn.eraseArc("U", "Y")
    bn.addArc("T", "Y")
    bn.erase("U")
    bn.cpt("T").fillWith(cpt_T)
    bn.cpt("Y").fillWith(cpt_Y)
    bn.endTopologyTransformation()
    cslbn = csl.CausalModel(bn)
    cslbn.addLatentVariable("u", ("T", "Y"))

    cee = csl.CausalEffectEstimation(df, cslbn)

    adjustment = cee.identifyAdjustmentSet("T", "Y", verbose=False)
    self.assertEqual(adjustment, cee._FRONTDOOR)

    cee.fitCausalBNEstimator()
    cee.estimateCausalEffect()

    cee.fitSimplePlugIn()
    SPI_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(SPI_estimation, 2.0, delta=_delta)

    self.assertRaises(FrontdoorError, cee.fitDM)

    self.assertRaises(FrontdoorError, cee.fitSLearner)
    self.assertRaises(FrontdoorError, cee.fitTLearner)
    self.assertRaises(FrontdoorError, cee.fitXLearner)
    self.assertRaises(FrontdoorError, cee.fitPStratification)
    self.assertRaises(FrontdoorError, cee.fitIPW)

    self.assertRaises(ValueError, cee.fitGeneralizedPlugIn)

    self.assertRaises(FrontdoorError, cee.fitWald)
    self.assertRaises(FrontdoorError, cee.fitWaldIPW)
    self.assertRaises(FrontdoorError, cee.fitNormalizedWaldIPW)
    self.assertRaises(FrontdoorError, cee.fitTSLS)

  def test_conditional_frontdoor_case(self):

    U = np.random.normal(1, 1, _n)
    X1 = np.random.normal(1, 1, _n)
    X2 = np.random.normal(1, 1, _n)
    X3 = np.random.normal(1, 1, _n)
    T = np.random.binomial(1, np.power(1+np.exp(-(U + X1 - 5*X3)), -1))
    M = np.random.normal(T + 2*X1 - X2, 1, _n)
    Y = np.random.normal(U + 2*M + 3*X2 + X3, 1, _n)
    df = pd.DataFrame({"U": U, "X1": X1, "X2": X2, "X3": X3, "M": M, "T": T, "Y":Y}, index=np.arange(0,_n))

    bn = gum.BayesNet()
    bn.beginTopologyTransformation()
    bn.add(_getDiscretizedVariable("X1", -4,6, 3))
    bn.add(_getDiscretizedVariable("X2", -4,6, 3))
    bn.add(_getDiscretizedVariable("X3", -4,6, 3))
    bn.add(_getDiscretizedVariable("M", -4,6, 10))
    bn.add(_getDiscretizedVariable("Y", -4,6, 100))
    bn.add(gum.IntegerVariable("T", "T", [0,1]))
    bn.add(_getDiscretizedVariable("U", -4,6, 3))
    bn.addArc("U", "T")
    bn.addArc("U", "Y")
    bn.addArc("X1", "T")
    bn.addArc("X1", "M")
    bn.addArc("X2", "M")
    bn.addArc("X2", "Y")
    bn.addArc("X3", "T")
    bn.addArc("X3", "Y")
    bn.addArc("T", "M")
    bn.addArc("M", "Y")
    bn.cpt("U").fillFromDistribution(norm, loc=1, scale=1)
    bn.cpt("X1").fillFromDistribution(norm, loc=1, scale=1)
    bn.cpt("X2").fillFromDistribution(norm, loc=1, scale=1)
    bn.cpt("X3").fillFromDistribution(norm, loc=1, scale=1)
    bn.cpt("T").fillFromDistribution(logistic, loc="U + X1 - 5*X3", scale=1)
    bn.cpt("M").fillFromDistribution(norm, loc="T + 2*X1 - X2", scale=1)
    bn.cpt("Y").fillFromDistribution(norm, loc="U + 2*M + 3*X2 + X3", scale=1)
    ie = gum.LazyPropagation(bn)
    cpt_T = ie.evidenceImpact("T",["X1", "X3"])
    cpt_Y = ie.evidenceImpact("Y", ["T", "M", "X2", "X3"])
    ie = None
    bn.eraseArc("U", "T")
    bn.eraseArc("U", "Y")
    bn.addArc("T", "Y")
    bn.erase("U")
    bn.cpt("T").fillWith(cpt_T)
    bn.cpt("Y").fillWith(cpt_Y)
    bn.endTopologyTransformation()
    cslbn = csl.CausalModel(bn)
    cslbn.addLatentVariable("u", ("T", "Y"))

    cee = csl.CausalEffectEstimation(df, cslbn)

    adjustment = cee.identifyAdjustmentSet("T", "Y", verbose=False)
    self.assertEqual(adjustment, cee._FRONTDOOR)

    cee.fitCausalBNEstimator()
    cee.estimateCausalEffect()

    cee.fitGeneralizedPlugIn()
    GPI_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(GPI_estimation, 2.0, delta=_delta)

    self.assertRaises(FrontdoorError, cee.fitDM)

    self.assertRaises(FrontdoorError, cee.fitSLearner)
    self.assertRaises(FrontdoorError, cee.fitTLearner)
    self.assertRaises(FrontdoorError, cee.fitXLearner)
    self.assertRaises(FrontdoorError, cee.fitPStratification)
    self.assertRaises(FrontdoorError, cee.fitIPW)

    self.assertRaises(FrontdoorError, cee.fitWald)
    self.assertRaises(FrontdoorError, cee.fitWaldIPW)
    self.assertRaises(FrontdoorError, cee.fitNormalizedWaldIPW)
    self.assertRaises(FrontdoorError, cee.fitTSLS)

  def test_simple_IV_case(self):

    U = np.random.normal(1, 1, _n)
    W = np.random.binomial(1, 0.3, _n)
    T = np.random.binomial(1, np.power(1+np.exp(- (U - 2*W)), -1))
    Y = np.random.normal(U + 2*T, 1, _n)
    df = pd.DataFrame({"W": W, "T": T, "Y":Y}, index=np.arange(0,_n))

    bn = gum.BayesNet()
    bn.beginTopologyTransformation()
    bn.add(gum.IntegerVariable("W", "W", [0,1]))
    bn.add(_getDiscretizedVariable("Y", -4,6, 100))
    bn.add(gum.IntegerVariable("T", "T", [0,1]))
    bn.add(_getDiscretizedVariable("U", -4,6, 3))
    bn.addArc("U", "T")
    bn.addArc("U", "Y")
    bn.addArc("W", "T")
    bn.addArc("T", "Y")
    bn.cpt("U").fillFromDistribution(norm, loc=1, scale=1)
    bn.cpt("W")[:] = [0.7, 0.3]
    bn.cpt("T").fillFromDistribution(logistic, loc="U - 2*W", scale=1)
    bn.cpt("Y").fillFromDistribution(norm, loc="U + 2*T", scale=1)
    ie = gum.LazyPropagation(bn)
    cpt_T = ie.evidenceImpact("T",["W"])
    cpt_Y = ie.evidenceImpact("Y", ["T"])
    ie = None
    bn.eraseArc("U", "T")
    bn.eraseArc("U", "Y")
    bn.erase("U")
    bn.cpt("T").fillWith(cpt_T)
    bn.cpt("Y").fillWith(cpt_Y)
    bn.endTopologyTransformation()
    cslbn = csl.CausalModel(bn)
    cslbn.addLatentVariable("u", ("T", "Y"))
    cslbn.addCausalArc("T", "Y")

    cee = csl.CausalEffectEstimation(df, cslbn)

    adjustment = cee.identifyAdjustmentSet("T", "Y", verbose=False)
    self.assertEqual(adjustment, cee._IV)

    cee.fitCausalBNEstimator()
    cee.estimateCausalEffect()

    cee.fitWald()
    Wald_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(Wald_estimation, 2.0, delta=_delta)

    cee.fitTSLS()
    TSLS_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(TSLS_estimation, 2.0, delta=_delta)

    self.assertRaises(IVError, cee.fitDM)

    self.assertRaises(IVError, cee.fitSLearner)
    self.assertRaises(IVError, cee.fitTLearner)
    self.assertRaises(IVError, cee.fitXLearner)
    self.assertRaises(IVError, cee.fitPStratification)
    self.assertRaises(IVError, cee.fitIPW)

    self.assertRaises(IVError, cee.fitSimplePlugIn)
    self.assertRaises(IVError, cee.fitGeneralizedPlugIn)

    self.assertRaises(ValueError, cee.fitWaldIPW)
    self.assertRaises(ValueError, cee.fitNormalizedWaldIPW)

  def test_conditional_IV_case(self):

    U = np.random.normal(1, 1, _n)
    X1 = np.random.normal(1, 1, _n)
    X2 = np.random.normal(1, 1, _n)
    X3 = np.random.normal(1, 1, _n)
    W = np.random.binomial(1, np.power(1+np.exp(-(5 - 4*X1 + X3)), -1))
    T = np.random.binomial(1, np.power(1+np.exp(-(U - 2*W + 3*X1 + X2)), -1))
    Y = np.random.normal(U + 2*T - 4*X3, 1, _n)
    df = pd.DataFrame({"W": W, "X1": X1, "X2": X2, "X3": X3, "T": T, "Y":Y}, index=np.arange(0,_n))

    bn = gum.BayesNet()
    bn.beginTopologyTransformation()
    bn.add(gum.IntegerVariable("W", "W", [0,1]))
    bn.add(_getDiscretizedVariable("Y", -4,6, 100))
    bn.add(gum.IntegerVariable("T", "T", [0,1]))
    bn.add(_getDiscretizedVariable("X1", -4,6, 3))
    bn.add(_getDiscretizedVariable("X2", -4,6, 3))
    bn.add(_getDiscretizedVariable("X3", -4,6, 3))
    bn.add(_getDiscretizedVariable("U", -4,6, 3))
    bn.addArc("U", "T")
    bn.addArc("U", "Y")
    bn.addArc("W", "T")
    bn.addArc("T", "Y")
    bn.addArc("X1", "W")
    bn.addArc("X1", "T")
    bn.addArc("X2", "T")
    bn.addArc("X2", "Y")
    bn.addArc("X3", "W")
    bn.addArc("X3", "Y")
    bn.cpt("U").fillFromDistribution(norm, loc=1, scale=1)
    bn.cpt("X1").fillFromDistribution(norm, loc=1, scale=1)
    bn.cpt("X2").fillFromDistribution(norm, loc=1, scale=1)
    bn.cpt("X3").fillFromDistribution(norm, loc=1, scale=1)
    bn.cpt("W").fillFromDistribution(logistic, loc="5 - 4*X1 + X3", scale=1)
    bn.cpt("T").fillFromDistribution(logistic, loc="U - 2*W + 3*X1 + X2", scale=1)
    bn.cpt("Y").fillFromDistribution(norm, loc="U + 2*T - 4*X3 + X2", scale=1)
    ie = gum.LazyPropagation(bn)
    cpt_T = ie.evidenceImpact("T", ["W", "X1", "X2"])
    cpt_Y = ie.evidenceImpact("Y", ["T", "X3", "X2"])
    ie = None
    bn.eraseArc("U", "T")
    bn.eraseArc("U", "Y")
    bn.erase("U")
    bn.cpt("T").fillWith(cpt_T)
    bn.cpt("Y").fillWith(cpt_Y)
    bn.endTopologyTransformation()
    cslbn = csl.CausalModel(bn)
    cslbn.addLatentVariable("u", ("T", "Y"))
    cslbn.addCausalArc("T", "Y")

    cee = csl.CausalEffectEstimation(df, cslbn)

    adjustment = cee.identifyAdjustmentSet("T", "Y", verbose=False)
    self.assertEqual(adjustment, cee._IV)

    cee.fitCausalBNEstimator()
    cee.estimateCausalEffect()

    cee.fitWaldIPW()
    WIPW_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(WIPW_estimation, 2.0, delta=_delta)

    cee.fitNormalizedWaldIPW()
    NWIPW_estimation = cee.estimateCausalEffect()
    self.assertAlmostEqual(NWIPW_estimation, 2.0, delta=_delta)

    self.assertRaises(IVError, cee.fitDM)

    self.assertRaises(IVError, cee.fitSLearner)
    self.assertRaises(IVError, cee.fitTLearner)
    self.assertRaises(IVError, cee.fitXLearner)
    self.assertRaises(IVError, cee.fitPStratification)
    self.assertRaises(IVError, cee.fitIPW)

    self.assertRaises(IVError, cee.fitSimplePlugIn)
    self.assertRaises(IVError, cee.fitGeneralizedPlugIn)

    self.assertRaises(ValueError, cee.fitWald)

ts = unittest.TestSuite()
addTests(ts, TestCausalEffectEstimation)
