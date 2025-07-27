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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2025                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import unittest
import numpy as np

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


SMOKING = "smoking?"
LUNG = "lung_cancer?"
BRONCHITIS = "bronchitis?"
VISIT_ASIA = "visit_to_Asia?"
TUBERCULOSIS = "tuberculosis?"
T_OR_C = "tuberculos_or_cancer?"
DYSPNOEA = "dyspnoea?"
POSITIVE_XRAY = "positive_XraY?"


class BNLearnerCSVTestCase(pyAgrumTestCase):
  def testHillClimbing(self):
    learner = gum.BNLearner(self.agrumSrcDir("asia.csv"))
    learner.useGreedyHillClimbing()
    bn = learner.learnBN()
    self.assertEqual(bn.size(), 8)

    with self.assertRaises(gum.IOError):
      gum.BNLearner("shouldNotExist.csv")

  def testHillClimbingAccurate(self):
    reference = gum.loadBN(self.agrumSrcDir("asia2.bif"), verbose=False)

    learner = gum.BNLearner(self.agrumSrcDir("asia.csv"), reference)

    witness = [
      SMOKING,
      LUNG,
      BRONCHITIS,
      VISIT_ASIA,
      TUBERCULOSIS,
      T_OR_C,
      DYSPNOEA,
      POSITIVE_XRAY,
    ]
    for n in witness:
      self.assertIn(n, learner.names())
    for n in learner.names():
      self.assertIn(n, witness)

    learner.useGreedyHillClimbing()
    bn = learner.learnBN()

    f = gum.ExactBNdistance(bn, reference)
    res = f.compute()
    self.assertAlmostEqual(res["klPQ"], 0.5, delta=0.5)

  def testLocalSearchWithTabu(self):
    with self.assertRaises(gum.IOError):
      gum.BNLearner("shouldNotExist.csv")

  def testLocalSearchWithTabuAccurate(self):
    reference = gum.loadBN(self.agrumSrcDir("asia2.bif"), verbose=False)
    learner = gum.BNLearner(self.agrumSrcDir("asia.csv"), reference)
    learner.useLocalSearchWithTabuList()
    bn = learner.learnBN()
    f = gum.ExactBNdistance(bn, reference)
    res = f.compute()
    self.assertAlmostEqual(res["klPQ"], 0.5, delta=0.5)

  def testParameterLearning(self):
    bn = gum.loadBN(self.agrumSrcDir("asia_bool.bif"), verbose=False)

    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"), bn)
    learner.useScoreLog2Likelihood()
    learner.useSmoothingPrior(1.0)

    bn2 = learner.learnParameters(bn)
    for i in range(bn.size()):
      # self.assertEqual(str(bn2.variable(i)), str(bn.variable(bn.idFromName(bn2.variable(i).name()))))
      self.assertEqual(set(bn2.variable(i).labels()), set(bn.variable(bn.idFromName(bn2.variable(i).name())).labels()))

    bn3 = learner.learnParameters(bn.dag())
    for i in range(bn.size()):
      self.assertListsAlmostEqual(bn3.cpt(i).tolist(), bn2.cpt(i).tolist(), delta=0.01)

    bn = gum.loadBN(self.agrumSrcDir("asia_bool.bif"), verbose=False)
    # there is a beurk modality in asia3-faulty.csv
    with self.assertRaises(gum.UnknownLabelInDatabase):
      gum.BNLearner(self.agrumSrcDir("asia3-faulty.csv"), bn)

  def testDBNTonda(self):
    dbn = gum.BayesNet()
    for name, nbr in [
      ("bf_0", 4),
      ("bf_t", 4),
      ("c_0", 5),
      ("c_t", 5),
      ("h_0", 5),
      ("h_t", 5),
      ("tf_0", 5),
      ("tf_t", 5),
      ("wl_0", 4),
      ("wl_t", 4),
    ]:
      dbn.add(gum.LabelizedVariable(name, name, nbr))

    for node in ["c_t", "h_t", "wl_t"]:
      dbn.addArc(dbn.idFromName("tf_0"), dbn.idFromName(node))
      dbn.addArc(dbn.idFromName("bf_0"), dbn.idFromName(node))
    dbn.addArc(dbn.idFromName("c_0"), dbn.idFromName("c_t"))
    dbn.addArc(dbn.idFromName("h_0"), dbn.idFromName("h_t"))
    dbn.addArc(dbn.idFromName("wl_0"), dbn.idFromName("wl_t"))

    csvfile = self.agrumSrcDir("DBN_Tonda.csv")
    l1 = gum.BNLearner(csvfile, dbn)
    l1.useScoreLog2Likelihood()
    l1.useSmoothingPrior()
    bn1 = l1.learnParameters(dbn)

    l2 = gum.BNLearner(csvfile, dbn)
    l2.setInitialDAG(dbn.dag())
    l2.useScoreLog2Likelihood()
    l2.useSmoothingPrior()
    bn2 = l2.learnParameters(dbn)

    p1 = bn1.cpt(bn1.idFromName("c_0"))
    I1 = gum.Instantiation(p1)
    p2 = bn2.cpt(bn2.idFromName("c_0"))
    I2 = gum.Instantiation(p2)
    I1.setFirst()
    I2.setFirst()
    while not I1.end():
      self.assertEqual(p1.get(I1), p2.get(I2))
      I1.inc()
      I2.inc()

  def testMiic(self):
    learner = gum.BNLearner(self.agrumSrcDir("asia.csv"))
    learner.useMIIC()
    learner.useNMLCorrection()
    learner.addForbiddenArc(4, 1)
    learner.addMandatoryArc(7, 5)

    d = gum.DAG()
    for i in range(8):
      d.addNodeWithId(i)
    learner.setInitialDAG(d)

    self.assertNotEqual(len(learner.names()), 0)

    learner.learnPDAG()

    bn = learner.learnBN()
    self.assertFalse(bn.dag().existsArc(4, 1))
    self.assertTrue(bn.dag().existsArc(7, 5))

    self.assertFalse(bn.dag().existsArc(4, 1))
    self.assertTrue(bn.dag().existsArc(7, 5))
    self.assertEqual(len(learner.latentVariables()), 2)

  def test_setSliceOrder_with_names(self):
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))
    learner.setSliceOrder([[SMOKING, LUNG], [BRONCHITIS, VISIT_ASIA], [TUBERCULOSIS]])

    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))
    learner.setSliceOrder([[0, LUNG], [2, VISIT_ASIA], [TUBERCULOSIS]])

    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))

    with self.assertRaises(gum.DuplicateElement):
      learner.setSliceOrder([[SMOKING, LUNG], [0, VISIT_ASIA], [TUBERCULOSIS]])

    with self.assertRaises(gum.MissingVariableInDatabase):
      learner.setSliceOrder([[SMOKING, LUNG], [BRONCHITIS, "CRUCRU?"], [TUBERCULOSIS]])

  def test_dirichlet(self):
    bn = gum.fastBN("A->B<-C->D->E<-B")
    gum.generateSample(bn, 2000, self.agrumSrcDir("dirichlet.csv"), with_labels=True)

    bn2 = gum.fastBN("A->B->C->D->E")
    gum.generateSample(bn2, 2000, self.agrumSrcDir("database.csv"), with_labels=True)

    # bn is used to give the variables and their domains
    learner = gum.BNLearner(self.agrumSrcDir("database.csv"), bn)
    learner.useDirichletPrior(self.agrumSrcDir("dirichlet.csv"), 10)
    learner.useScoreAIC()  # or another score with no included prior such as BDeu

    bn3 = learner.learnBN()

    self.assertEqual(bn3.size(), 5)

  # todo: reactivate as soon as a correct database is available
  def skipped_EM(self):
    learner = gum.BNLearner(self.agrumSrcDir("EM.csv"), ["#"])
    self.assertFalse(learner.hasMissingValues())

    learner = gum.BNLearner(self.agrumSrcDir("EM.csv"), ["?"])
    self.assertTrue(learner.hasMissingValues())

    dag = gum.DAG()
    for i in range(len(learner.names())):
      dag.addNodeWithId(i)

    dag.addArc(1, 0)
    dag.addArc(2, 1)
    dag.addArc(3, 2)

    with self.assertRaises(gum.MissingValueInDatabase):
      learner.learnParameters(dag)

    # EM with log-likelihood's evolution rate stopping criterion
    learner.useEM(1e-3, 0.15).EMsetMaxIter(10).EMsetMaxTime(200).EMdisableMaxIter().EMdisableMaxTime()
    learner.useSmoothingPrior()

    self.assertEqual(learner.EMMinEpsilonRate(), 1e-3)
    self.assertTrue(learner.EMisEnabledMinEpsilonRate())
    self.assertFalse(learner.EMisEnabledEpsilon())
    self.assertFalse(learner.EMisEnabledMaxIter())
    self.assertFalse(learner.EMisEnabledMaxTime())
    self.assertEqual(learner.EMMaxIter(), 10)
    self.assertEqual(learner.EMMaxTime(), 200)

    bn1 = learner.learnParameters(dag, False)

    # relaunch EM with the output of the previous EM to initialize the CPTs
    learner.useEM(1e-3, 0.002)
    bn2 = learner.learnParameters(bn1, False)

    # check that bn1 and bn2 are more or less the same
    for i in range(bn1.size()):
      if bn1.cpt(i).nbrDim() > 1:
        cpt1 = np.array(bn1.cpt(i).tolist())
        cpt1.shape = (bn1.cpt(i).domainSize(),)
        cpt2 = np.array(bn2.cpt(i).tolist())
        cpt2.shape = (bn2.cpt(i).domainSize(),)
      else:
        cpt1 = bn1.cpt(i)
        cpt2 = bn2.cpt(i)

      self.assertListsAlmostEqual(cpt1.tolist(), cpt2.tolist(), delta=0.1)

    # relaunch EM with diff stopping criterion
    learner.useEMWithDiffCriterion(0.05, 0.0)
    bn3 = learner.learnParameters(bn2, False)

    learner.fitParameters(bn1, False)
    for i in range(bn1.size()):
      if bn1.cpt(i).nbrDim() > 1:
        cpt1 = np.array(bn1.cpt(i).tolist())
        cpt1.shape = (bn1.cpt(i).domainSize(),)
        cpt2 = np.array(bn2.cpt(i).tolist())
        cpt2.shape = (bn2.cpt(i).domainSize(),)
      else:
        cpt1 = bn1.cpt(i)
        cpt2 = bn2.cpt(i)

      self.assertListsAlmostEqual(cpt1.tolist(), cpt2.tolist(), delta=0.1)

    learner.useEMWithRateCriterion(1e-4, 0.35).EMsetMaxIter(2)
    learner.learnParameters(dag, False)
    self.assertEqual(learner.EMStateMessage(), "stopped with max iteration=2")

    learner.forbidEM()
    self.assertEqual(learner.EMStateMessage(), "EM is currently forbidden. Please enable it with useEM()")

  # todo: reactivate as soon as a correct database is available
  def skipped_EM2(self):
    learner = gum.BNLearner(self.agrumSrcDir("EM2.csv"), ["?"])
    self.assertTrue(learner.hasMissingValues())
    learner.useSmoothingPrior()

    dag = gum.DAG()
    dag.addNodeWithId(0)
    dag.addNodeWithId(1)
    dag.addArc(0, 1)

    for i in range(10):
      learner.useEM(1e-3, 0.9).EMsetVerbosity(True)
      learner.EMsetMinEpsilonRate(1e-2)
      bn1 = learner.learnParameters(dag)
      delta = learner.EMHistory()[-1]

      learner.useEM(1e-3, 0)
      bn2 = learner.learnParameters(bn1)
      for i in range(bn1.size()):
        if bn1.cpt(i).nbrDim() > 1:
          cpt1 = np.array(bn1.cpt(i).tolist())
          cpt1.shape = (bn1.cpt(i).domainSize(),)
          cpt2 = np.array(bn2.cpt(i).tolist())
          cpt2.shape = (bn2.cpt(i).domainSize(),)
        else:
          cpt1 = bn1.cpt(i)
          cpt2 = bn2.cpt(i)

        self.assertListsAlmostEqual(cpt1.tolist(), cpt2.tolist(), delta=delta * 1.3)

  def test_chi2(self):
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))

    stat, pvalue = learner.chi2(SMOKING, LUNG)
    self.assertAlmostEqual(stat, 36.2256, delta=1e-4)
    self.assertAlmostEqual(pvalue, 0, delta=1e-4)

    stat, pvalue = learner.chi2(SMOKING, VISIT_ASIA)
    self.assertAlmostEqual(stat, 1.1257, delta=1e-4)
    self.assertAlmostEqual(pvalue, 0.2886, delta=1e-4)

    stat, pvalue = learner.chi2(LUNG, TUBERCULOSIS)
    self.assertAlmostEqual(stat, 0.6297, delta=1e-4)
    self.assertAlmostEqual(pvalue, 0.4274, delta=1e-4)

    stat, pvalue = learner.chi2(LUNG, TUBERCULOSIS, [T_OR_C])
    self.assertAlmostEqual(stat, 58.0, delta=1e-4)
    self.assertAlmostEqual(pvalue, 0.0, delta=1e-4)

    learner2 = gum.BNLearner(self.agrumSrcDir("chi2.csv"))

    stat, pvalue = learner2.chi2("A", "C")
    self.assertAlmostEqual(stat, 0.0007, delta=1e-3)
    self.assertAlmostEqual(pvalue, 0.978, delta=1e-3)

    stat, pvalue = learner2.chi2("A", "B")
    self.assertAlmostEqual(stat, 21.4348, delta=1e-3)
    self.assertAlmostEqual(pvalue, 3.6e-6, delta=1e-5)

    stat, pvalue = learner2.chi2("B", "A")
    self.assertAlmostEqual(stat, 21.4348, delta=1e-3)
    self.assertAlmostEqual(pvalue, 3.6e-6, delta=1e-5)

    stat, pvalue = learner2.chi2("B", "D")
    self.assertAlmostEqual(stat, 0.903, delta=1e-3)
    self.assertAlmostEqual(pvalue, 0.341, delta=1e-3)

    stat, pvalue = learner2.chi2("A", "C", ["B"])
    self.assertAlmostEqual(stat, 15.2205, delta=1e-3)
    self.assertAlmostEqual(pvalue, 0.0005, delta=1e-4)

  def test_loglikelihood(self):
    learner = gum.BNLearner(self.agrumSrcDir("chi2.csv"))
    self.assertEqual(learner.nbRows(), 500)
    self.assertEqual(learner.nbCols(), 4)

    siz = -1.0 * learner.nbRows()

    stat = learner.logLikelihood(["A"]) / siz
    self.assertAlmostEqual(stat, 0.99943499, delta=1e-5)
    stat = learner.logLikelihood(["B"]) / siz
    self.assertAlmostEqual(stat, 0.9986032, delta=1e-5)
    stat = learner.logLikelihood(["A", "B"]) / siz
    self.assertAlmostEqual(stat, 1.9668973, delta=1e-5)
    stat = learner.logLikelihood(["A"], ["B"]) / siz
    self.assertAlmostEqual(stat, 1.9668973 - 0.9986032, delta=1e-5)

    stat = learner.logLikelihood(["C"]) / siz
    self.assertAlmostEqual(stat, 0.99860302, delta=1e-5)
    stat = learner.logLikelihood(["D"]) / siz
    self.assertAlmostEqual(stat, 0.40217919, delta=1e-5)
    stat = learner.logLikelihood(["C", "D"]) / siz
    self.assertAlmostEqual(stat, 1.40077995, delta=1e-5)
    stat = learner.logLikelihood(["C"], ["D"]) / siz
    self.assertAlmostEqual(stat, 1.40077995 - 0.40217919, delta=1e-5)

  def testPossibleEdge(self):
    # possible edges are not relevant
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))
    learner.addPossibleEdge(VISIT_ASIA, LUNG)
    learner.addPossibleEdge(VISIT_ASIA, SMOKING)
    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 0)

    # possible edges are relevant
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))
    learner.addPossibleEdge(SMOKING, BRONCHITIS)
    learner.addPossibleEdge(T_OR_C, POSITIVE_XRAY)
    possiblearcs = {
      (bn.idFromName(BRONCHITIS), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(BRONCHITIS)),
      (bn.idFromName(T_OR_C), bn.idFromName(POSITIVE_XRAY)),
      (bn.idFromName(POSITIVE_XRAY), bn.idFromName(T_OR_C)),
    }
    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 2)
    self.assertTrue(possiblearcs.issuperset(bn.arcs()))

    # mixed with a forbidden arc
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))
    learner.addPossibleEdge(SMOKING, LUNG)
    learner.addPossibleEdge(T_OR_C, POSITIVE_XRAY)
    learner.addForbiddenArc(SMOKING, BRONCHITIS)

    possiblearcs = {
      (bn.idFromName(LUNG), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(LUNG)),
      (bn.idFromName(T_OR_C), bn.idFromName(POSITIVE_XRAY)),
      (bn.idFromName(POSITIVE_XRAY), bn.idFromName(T_OR_C)),
      (bn.idFromName(BRONCHITIS), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(BRONCHITIS)),
    }

    forbiddenarcs = {(bn.idFromName(SMOKING), bn.idFromName(BRONCHITIS))}

    bn = learner.learnBN()
    self.assertLessEqual(bn.sizeArcs(), 4)
    self.assertTrue(possiblearcs.issuperset(bn.arcs()))
    self.assertTrue(forbiddenarcs.isdisjoint(bn.arcs()))

    # mixed with a mandatory arc
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))
    learner.addPossibleEdge(SMOKING, LUNG)
    learner.addPossibleEdge(BRONCHITIS, SMOKING)
    learner.addMandatoryArc(VISIT_ASIA, BRONCHITIS)

    testedarcs = {
      (bn.idFromName(LUNG), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(LUNG)),
      (bn.idFromName(BRONCHITIS), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(BRONCHITIS)),
      (bn.idFromName(VISIT_ASIA), bn.idFromName(BRONCHITIS)),
    }

    forcedarcs = {(bn.idFromName(VISIT_ASIA), bn.idFromName(BRONCHITIS))}

    bn = learner.learnBN()
    self.assertLessEqual(bn.sizeArcs(), 2)
    self.assertGreaterEqual(bn.sizeArcs(), 1)
    self.assertTrue(testedarcs.issuperset(bn.arcs()))
    self.assertTrue(set(bn.arcs()).issuperset(forcedarcs))

  def testPossibleEdgeWithTabuList(self):
    # possible edges are not relevant
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))
    learner.useLocalSearchWithTabuList()
    learner.addPossibleEdge(VISIT_ASIA, LUNG)
    learner.addPossibleEdge(VISIT_ASIA, SMOKING)

    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 0)

    # possible edges are relevant
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))
    learner.useLocalSearchWithTabuList()
    learner.addPossibleEdge(SMOKING, LUNG)
    learner.addPossibleEdge(BRONCHITIS, SMOKING)
    possiblearcs = {
      (bn.idFromName(LUNG), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(LUNG)),
      (bn.idFromName(BRONCHITIS), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(BRONCHITIS)),
    }

    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 2)
    self.assertTrue(possiblearcs.issuperset(bn.arcs()))

    # mixed with a forbidden arc
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))
    learner.useLocalSearchWithTabuList()
    learner.addPossibleEdge(SMOKING, LUNG)
    learner.addPossibleEdge(BRONCHITIS, SMOKING)
    learner.addForbiddenArc(SMOKING, BRONCHITIS)

    possiblearcs = {
      (bn.idFromName(LUNG), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(LUNG)),
      (bn.idFromName(BRONCHITIS), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(BRONCHITIS)),
    }

    forbiddenarcs = {(bn.idFromName(SMOKING), bn.idFromName(BRONCHITIS))}
    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 2)
    self.assertTrue(possiblearcs.issuperset(bn.arcs()))
    self.assertTrue(forbiddenarcs.isdisjoint(bn.arcs()))

    # mixed with a mandatory arc
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))
    learner.useLocalSearchWithTabuList()
    learner.addPossibleEdge(SMOKING, LUNG)
    learner.addPossibleEdge(BRONCHITIS, SMOKING)
    learner.addMandatoryArc(VISIT_ASIA, BRONCHITIS)

    testedarcs = {
      (bn.idFromName(LUNG), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(LUNG)),
      (bn.idFromName(BRONCHITIS), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(BRONCHITIS)),
      (bn.idFromName(VISIT_ASIA), bn.idFromName(BRONCHITIS)),
    }

    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 3)
    self.assertTrue(testedarcs.issuperset(bn.arcs()))

  def testPossibleEdgeWithGHC(self):
    # possible edges are not relevant
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))
    learner.useGreedyHillClimbing()
    learner.addPossibleEdge(VISIT_ASIA, LUNG)
    learner.addPossibleEdge(VISIT_ASIA, SMOKING)

    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 0)

    # possible edges are relevant
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))
    learner.useGreedyHillClimbing()
    learner.addPossibleEdge(SMOKING, LUNG)
    learner.addPossibleEdge(BRONCHITIS, SMOKING)
    possiblearcs = {
      (bn.idFromName(LUNG), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(LUNG)),
      (bn.idFromName(BRONCHITIS), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(BRONCHITIS)),
    }

    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 2)
    self.assertTrue(possiblearcs.issuperset(bn.arcs()))

    # mixed with a forbidden arc
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))
    learner.useGreedyHillClimbing()
    learner.addPossibleEdge(SMOKING, LUNG)
    learner.addPossibleEdge(BRONCHITIS, SMOKING)
    learner.addForbiddenArc(SMOKING, BRONCHITIS)

    possiblearcs = {
      (bn.idFromName(LUNG), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(LUNG)),
      (bn.idFromName(BRONCHITIS), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(BRONCHITIS)),
    }

    forbiddenarcs = {(bn.idFromName(SMOKING), bn.idFromName(BRONCHITIS))}
    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 2)
    self.assertTrue(possiblearcs.issuperset(bn.arcs()))
    self.assertTrue(forbiddenarcs.isdisjoint(bn.arcs()))

    # mixed with a mandatory arc
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))
    learner.useGreedyHillClimbing()
    learner.addPossibleEdge(SMOKING, LUNG)
    learner.addPossibleEdge(BRONCHITIS, SMOKING)
    learner.addMandatoryArc(VISIT_ASIA, BRONCHITIS)

    testedarcs = {
      (bn.idFromName(LUNG), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(LUNG)),
      (bn.idFromName(BRONCHITIS), bn.idFromName(SMOKING)),
      (bn.idFromName(SMOKING), bn.idFromName(BRONCHITIS)),
      (bn.idFromName(VISIT_ASIA), bn.idFromName(BRONCHITIS)),
    }

    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 3)
    self.assertTrue(testedarcs.issuperset(bn.arcs()))

  def testHybridLearning(self):
    learner = gum.BNLearner(self.agrumSrcDir("data1.csv"))
    learner.useMIIC()
    eg = learner.learnEssentialGraph()
    skel = eg.skeleton()

    learner = gum.BNLearner(self.agrumSrcDir("data1.csv"))
    learner.setPossibleSkeleton(skel)
    bn = learner.learnBN()

    self.assertEqual(bn.sizeArcs(), 4)
    self.assertEqual(bn.parents(bn.idFromName("V")), {bn.idFromName("A")})
    self.assertEqual(bn.parents(bn.idFromName("Y")), {bn.idFromName("X"), bn.idFromName("V")})
    self.assertEqual(bn.parents(bn.idFromName("Z")), {bn.idFromName("Y")})

  def testHybridLearning2(self):
    learner = gum.BNLearner(self.agrumSrcDir("data1.csv"))
    learner.useGreedyHillClimbing()
    eg = learner.learnEssentialGraph()
    skel = eg.skeleton()

    learner = gum.BNLearner(self.agrumSrcDir("data1.csv"))
    learner.setPossibleSkeleton(skel)
    bn = learner.learnBN()

    self.assertEqual(bn.sizeArcs(), 4)
    self.assertEqual(bn.parents(bn.idFromName("V")), {bn.idFromName("A")})
    self.assertEqual(bn.parents(bn.idFromName("Y")), {bn.idFromName("X"), bn.idFromName("V")})
    self.assertEqual(bn.parents(bn.idFromName("Z")), {bn.idFromName("Y")})

    learner = gum.BNLearner(self.agrumSrcDir("data1.csv"))
    learner.setPossibleEdges(skel.edges())
    bn = learner.learnBN()

    self.assertEqual(bn.sizeArcs(), 4)
    self.assertEqual(bn.parents(bn.idFromName("V")), {bn.idFromName("A")})
    self.assertEqual(bn.parents(bn.idFromName("Y")), {bn.idFromName("X"), bn.idFromName("V")})
    self.assertEqual(bn.parents(bn.idFromName("Z")), {bn.idFromName("Y")})

  def test_RecordWeight(self):
    filename = self.agrumSrcDir("dataW.csv")
    bn = gum.fastBN("X->Y")
    with open(filename, "w") as src:
      src.write("""X,Y
  1,0
  0,1
  0,1
  0,0
  1,0
  0,1
  1,1
  0,1
  """)
    learner = gum.BNLearner(filename)
    bn1 = learner.learnParameters(bn.dag())

    with open(filename, "w") as src:
      src.write("""X,Y
  0,0
  1,0
  0,1
  1,1
  """)
    learner = gum.BNLearner(filename)

    learner.setRecordWeight(1, 2.0)
    learner.setRecordWeight(2, 4.0)

    self.assertEqual(learner.recordWeight(0), 1.0)
    self.assertEqual(learner.recordWeight(1), 2.0)
    self.assertEqual(learner.recordWeight(2), 4.0)
    self.assertEqual(learner.recordWeight(3), 1.0)

    self.assertEqual(learner.databaseWeight(), 8)

    learner.setDatabaseWeight(3.0 * learner.nbRows())

    self.assertEqual(learner.recordWeight(0), 3.0)
    self.assertEqual(learner.recordWeight(1), 3.0)
    self.assertEqual(learner.recordWeight(2), 3.0)
    self.assertEqual(learner.recordWeight(3), 3.0)

    self.assertEqual(learner.databaseWeight(), 3.0 * learner.nbRows())

    learner.setRecordWeight(1, 1.0)
    learner.setRecordWeight(2, 1.0)

    self.assertEqual(learner.recordWeight(0), 3.0)
    self.assertEqual(learner.recordWeight(1), 1.0)
    self.assertEqual(learner.recordWeight(2), 1.0)
    self.assertEqual(learner.recordWeight(3), 3.0)

    self.assertEqual(learner.databaseWeight(), 8)

    learner = gum.BNLearner(filename)

    learner.setRecordWeight(1, 2.0)
    learner.setRecordWeight(2, 4.0)

    bn2 = learner.learnParameters(bn.dag())
    self.assertTrue(np.array_equal(bn1.cpt("X").toarray(), bn2.cpt("X").toarray()))
    self.assertTrue(np.array_equal(bn1.cpt("Y").toarray(), bn2.cpt("Y").toarray()))

  def testPseudoCount(self):
    filename = self.agrumSrcDir("dataW.csv")
    with open(filename, "w") as src:
      src.write("""X,Y,Z
0,1,2
0,1,0
0,0,2
1,0,2
0,1,1
1,1,1
0,1,1
""")
    learner = gum.BNLearner(filename)
    self.assertEqual(learner.nbRows(), 7)
    self.assertEqual(learner.nbCols(), 3)

    self.assertEqual(learner.rawPseudoCount(["X"]), (5, 2))
    self.assertEqual(learner.rawPseudoCount(["X", "Z"]), (1, 0, 2, 1, 2, 1))
    self.assertEqual(learner.rawPseudoCount(["Y", "Z"]), (0, 1, 0, 3, 2, 1))

    learner.useSmoothingPrior(0.1)

    self.assertEqual(learner.rawPseudoCount(["X"]), (5.1, 2.1))
    self.assertEqual(learner.rawPseudoCount(["X", "Z"]), (1.1, 0.1, 2.1, 1.1, 2.1, 1.1))
    self.assertEqual(learner.rawPseudoCount(["Y", "Z"]), (0.1, 1.1, 0.1, 3.1, 2.1, 1.1))

    learner = gum.BNLearner(filename)
    self.assertEqual(learner.pseudoCount(["X"]).tolist(), [5, 2])
    self.assertEqual(learner.pseudoCount(["X", "Z"]).tolist(), [[1, 0], [2, 1], [2, 1]])
    self.assertEqual(learner.pseudoCount(["Y", "Z"]).tolist(), [[0, 1], [0, 3], [2, 1]])
    self.assertEqual(learner.pseudoCount(["Z", "Y"]).tolist(), [[0, 0, 2], [1, 3, 1]])

    learner.useSmoothingPrior(0.1)

    self.assertEqual(learner.pseudoCount(["X"]).tolist(), [5.1, 2.1])
    self.assertEqual(learner.pseudoCount(["X", "Z"]).tolist(), [[1.1, 0.1], [2.1, 1.1], [2.1, 1.1]])
    self.assertEqual(learner.pseudoCount(["Y", "Z"]).tolist(), [[0.1, 1.1], [0.1, 3.1], [2.1, 1.1]])
    self.assertEqual(learner.pseudoCount(["Z", "Y"]).tolist(), [[0.1, 0.1, 2.1], [1.1, 3.1, 1.1]])

  def test_dbWithGuil(self):
    filename = self.agrumSrcDir("csv_quoted.csv")
    with open(filename, "w") as src:
      src.write("""X,Y,Z
0,1,2
0,1",0
0,0,2
1,"0,2
0,"1",1
1,1,1
0,1,1
""")

    with self.assertRaises(SyntaxError):
      learner = gum.BNLearner(filename)
      learner.useScoreBIC()
      learner.useGreedyHillClimbing()
      learner.learnBN()

  def test_mutualInfo(self):
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))

    self.assertAlmostEqual(learner.mutualInformation(LUNG, SMOKING), 0.0332346154587978, delta=1e-6)

    learner.useNoCorrection()
    self.assertAlmostEqual(learner.mutualInformation(LUNG, SMOKING), 0.0332346154587978, delta=1e-6)

    learner.useMDLCorrection()
    self.assertAlmostEqual(learner.mutualInformation(LUNG, SMOKING), 0.0332346154587978, delta=1e-6)

    learner.useNMLCorrection()
    self.assertAlmostEqual(learner.mutualInformation(LUNG, SMOKING), 0.0332346154587978, delta=1e-6)

    learner.useNoCorrection()
    self.assertAlmostEqual(learner.correctedMutualInformation(LUNG, SMOKING), 0.0332346154587978, delta=1e-6)

    learner.useMDLCorrection()
    self.assertAlmostEqual(learner.correctedMutualInformation(LUNG, SMOKING), 0.0279523, delta=1e-6)

    learner.useNMLCorrection()
    self.assertAlmostEqual(learner.correctedMutualInformation(LUNG, SMOKING), 0.0292368, delta=1e-6)

  def test_conditionalMutualInfo(self):
    learner = gum.BNLearner(self.agrumSrcDir("asia3.csv"))

    self.assertAlmostEqual(learner.mutualInformation(BRONCHITIS, LUNG, [SMOKING]), 0.00159046091867418, delta=1e-6)

    learner.useNoCorrection()
    self.assertAlmostEqual(learner.mutualInformation(BRONCHITIS, LUNG, [SMOKING]), 0.00159046091867418, delta=1e-6)

    learner.useMDLCorrection()
    self.assertAlmostEqual(learner.mutualInformation(BRONCHITIS, LUNG, [SMOKING]), 0.00159046091867418, delta=1e-6)

    learner.useNMLCorrection()
    self.assertAlmostEqual(learner.mutualInformation(BRONCHITIS, LUNG, [SMOKING]), 0.00159046091867418, delta=1e-6)

    learner.useNoCorrection()
    self.assertAlmostEqual(
      learner.correctedMutualInformation(BRONCHITIS, LUNG, [SMOKING]), 0.00159046091867418, delta=1e-6
    )

    learner.useMDLCorrection()
    self.assertAlmostEqual(learner.correctedMutualInformation(BRONCHITIS, LUNG, [SMOKING]), -0.00897408, delta=1e-6)

    learner.useNMLCorrection()
    self.assertAlmostEqual(learner.correctedMutualInformation(BRONCHITIS, LUNG, [SMOKING]), -0.00486096, delta=1e-6)


ts = unittest.TestSuite()
addTests(ts, BNLearnerCSVTestCase)
