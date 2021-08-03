# -*- encoding: UTF-8 -*-
import unittest
import sys
import numpy as np

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class BNLearnerCSVTestCase(pyAgrumTestCase):
  def setUp(self):
    pass

  def testHillClimbing(self):
    learner = gum.BNLearner(self.agrumSrcDir(
        'src/testunits/ressources/asia.csv'))
    learner.useGreedyHillClimbing()
    bn = learner.learnBN()
    self.assertEqual(bn.size(), 8)

    with self.assertRaises(gum.IOError):
      learner = gum.BNLearner("shouldNotExist.csv")

    try:
      learner = gum.BNLearner("shouldNotExist.csv")
    except gum.IOError:
      self.assertTrue(True)
      pass
    else:
      self.assertTrue(False)

  def testHillClimbingAccurate(self):
    learner = gum.BNLearner(self.agrumSrcDir(
        'src/testunits/ressources/asia.csv'))

    witness = ['smoking?',
               'lung_cancer?',
               'bronchitis?',
               'visit_to_Asia?',
               'tuberculosis?',
               'tuberculos_or_cancer?',
               'dyspnoea?',
               'positive_XraY?']
    for n in witness:
      self.assertTrue(n in learner.names())
    for n in learner.names():
      self.assertTrue(n in witness)

    learner.useGreedyHillClimbing()
    bn = learner.learnBN()

    ref = gum.loadBN(self.agrumSrcDir(
        'src/testunits/ressources/asia2.bif'), verbose=False)

    f = gum.ExactBNdistance(bn, ref)
    res = f.compute()
    self.assertAlmostEqual(res['klPQ'], 0.5, delta=0.5)

  def testLocalSearchWithTabu(self):
    with self.assertRaises(gum.IOError):
      learner = gum.BNLearner("shouldNotExist.csv")

    try:
      learner = gum.BNLearner("shouldNotExist.csv")
    except gum.IOError:
      self.assertTrue(True)
      pass
    else:
      self.assertTrue(False)

  def testLocalSearchWithTabuAccurate(self):
    learner = gum.BNLearner(self.agrumSrcDir(
        'src/testunits/ressources/asia.csv'))
    learner.useLocalSearchWithTabuList()

    bn = learner.learnBN()

    ref = gum.loadBN(self.agrumSrcDir(
        'src/testunits/ressources/asia2.bif'), verbose=False)

    f = gum.ExactBNdistance(bn, ref)
    res = f.compute()
    self.assertAlmostEqual(res['klPQ'], 0.5, delta=0.5)

  def testParameterLearning(self):
    bn = gum.loadBN(self.agrumSrcDir(
        'src/testunits/ressources/asia_bool.bif'), verbose=False)

    learner = gum.BNLearner(self.agrumSrcDir(
        'src/testunits/ressources/asia3.csv'), bn)
    learner.setInitialDAG(bn.dag())
    learner.useScoreLog2Likelihood()
    learner.useAprioriSmoothing(1.0)

    bn2 = learner.learnParameters()
    for i in range(bn.size()):
      # self.assertEqual(str(bn2.variable(i)), str(bn.variable(bn.idFromName(bn2.variable(i).name()))))
      self.assertEqual(set(bn2.variable(i).labels()), set(
          bn.variable(bn.idFromName(bn2.variable(i).name())).labels()))

    bn = gum.loadBN(self.agrumSrcDir(
        'src/testunits/ressources/asia_bool.bif'), verbose=False)
    # there is a beurk modality in asia3-faulty.csv
    with self.assertRaises(gum.UnknownLabelInDatabase):
      learner = gum.BNLearner(self.agrumSrcDir(
          'src/testunits/ressources/asia3-faulty.csv'), bn)

  def testDBNTonda(self):
    dbn = gum.BayesNet()
    l = [dbn.add(gum.LabelizedVariable(name, name, nbr)) for (name, nbr) in [
      ("bf_0", 4),
      ("bf_t", 4),
      ("c_0", 5),
      ("c_t", 5),
      ("h_0", 5),
      ("h_t", 5),
      ("tf_0", 5),
      ("tf_t", 5),
      ("wl_0", 4),
      ("wl_t", 4)
    ]]
    for node in ["c_t", "h_t", "wl_t"]:
      dbn.addArc(dbn.idFromName("tf_0"), dbn.idFromName(node))
      dbn.addArc(dbn.idFromName("bf_0"), dbn.idFromName(node))
    dbn.addArc(dbn.idFromName("c_0"), dbn.idFromName("c_t"))
    dbn.addArc(dbn.idFromName("h_0"), dbn.idFromName("h_t"))
    dbn.addArc(dbn.idFromName("wl_0"), dbn.idFromName("wl_t"))

    csvfile = self.agrumSrcDir('src/testunits/ressources/DBN_Tonda.csv')
    l1 = gum.BNLearner(csvfile)
    l1.setInitialDAG(dbn.dag())
    l1.useScoreLog2Likelihood()
    l1.useAprioriSmoothing()
    bn1 = l1.learnParameters()

    l2 = gum.BNLearner(csvfile, dbn)
    l2.setInitialDAG(dbn.dag())
    l2.useScoreLog2Likelihood()
    l2.useAprioriSmoothing()
    bn2 = l2.learnParameters()

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

  def test3off2(self):
    learner = gum.BNLearner(self.agrumSrcDir(
        'src/testunits/ressources/asia.csv'))
    learner.use3off2()
    learner.useNMLCorrection()
    learner.addForbiddenArc(4, 1)
    learner.addMandatoryArc(7, 5)

    d = gum.DAG()
    for i in range(8):
      d.addNodeWithId(i)
    learner.setInitialDAG(d)

    self.assertNotEqual(len(learner.names()), 0)

    try:
      bn = learner.learnBN()
    except:
      self.fail("Exception has been raised and should not")
    self.assertEqual(len(bn.arcs()), 9)
    self.assertFalse(bn.dag().existsArc(4, 1))
    self.assertTrue(bn.dag().existsArc(7, 5))

    try:
      mg = learner.learnMixedStructure()
    except:
      self.fail("Exception has been raised and should not")
    self.assertEqual(mg.sizeArcs(), 8)
    self.assertEqual(mg.sizeEdges(), 1)
    self.assertFalse(bn.dag().existsArc(4, 1))
    self.assertTrue(bn.dag().existsArc(7, 5))
    self.assertEqual(len(learner.latentVariables()), 2)

  def test_setSliceOrder_with_names(self):
    learner = gum.BNLearner(self.agrumSrcDir(
        'src/testunits/ressources/asia3.csv'))
    learner.setSliceOrder([["smoking?", "lung_cancer?"],
                           ["bronchitis?", "visit_to_Asia?"],
                           ["tuberculosis?"]])

    learner = gum.BNLearner(self.agrumSrcDir(
        'src/testunits/ressources/asia3.csv'))
    learner.setSliceOrder([[0, "lung_cancer?"],
                           [2, "visit_to_Asia?"],
                           ["tuberculosis?"]])

    learner = gum.BNLearner(self.agrumSrcDir(
        'src/testunits/ressources/asia3.csv'))

    with self.assertRaises(gum.DuplicateElement):
      learner.setSliceOrder([["smoking?", "lung_cancer?"],
                             [0, "visit_to_Asia?"],
                             ["tuberculosis?"]])

    with self.assertRaises(gum.MissingVariableInDatabase):
      learner.setSliceOrder([["smoking?", "lung_cancer?"],
                             ["bronchitis?", "CRUCRU?"],
                             ["tuberculosis?"]])

  def test_dirichlet(self):
    bn = gum.fastBN("A->B<-C->D->E<-B")
    gum.generateCSV(bn, "resources/dirichlet.csv", 2000, with_labels=True)

    bn2 = gum.fastBN("A->B->C->D->E")
    gum.generateCSV(bn2, "resources/database.csv", 2000, with_labels=True)

    learner = gum.BNLearner("resources/database.csv", bn)  # bn is used to give the variables and their domains
    learner.useAprioriDirichlet("resources/dirichlet.csv", 10)
    learner.useScoreAIC()  # or another score with no included prior such as BDeu

    bn3 = learner.learnBN()

    self.assertEqual(bn.size(), 5)

  def test_EM(self):
    learner = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/EM.csv'), True,["#"])
    self.assertFalse(learner.hasMissingValues())

    learner = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/EM.csv'), True,["?"])
    self.assertTrue(learner.hasMissingValues())

    dag = gum.DAG()
    for i in range(len(learner.names())):
      dag.addNodeWithId(i)

    dag.addArc(1, 0)
    dag.addArc(2, 1)
    dag.addArc(3, 2)

    with self.assertRaises(gum.MissingValueInDatabase):
      learner.learnParameters(dag)

    learner.useEM(1e-3)
    learner.useAprioriSmoothing()
    learner.learnParameters(dag, False)

  def test_chi2(self):
    learner = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/asia3.csv'))

    stat, pvalue = learner.chi2("smoking?", "lung_cancer?")
    self.assertAlmostEqual(stat, 36.2256, delta=1e-4)
    self.assertAlmostEqual(pvalue, 0, delta=1e-4)

    stat, pvalue = learner.chi2("smoking?", "visit_to_Asia?")
    self.assertAlmostEqual(stat, 1.1257, delta=1e-4)
    self.assertAlmostEqual(pvalue, 0.2886, delta=1e-4)

    stat, pvalue = learner.chi2("lung_cancer?", "tuberculosis?")
    self.assertAlmostEqual(stat, 0.6297, delta=1e-4)
    self.assertAlmostEqual(pvalue, 0.4274, delta=1e-4)

    stat, pvalue = learner.chi2("lung_cancer?", "tuberculosis?", ["tuberculos_or_cancer?"])
    self.assertAlmostEqual(stat, 58.0, delta=1e-4)
    self.assertAlmostEqual(pvalue, 0.0, delta=1e-4)

    learner2 = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/chi2.csv'))

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
    learner = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/chi2.csv'))
    self.assertEqual(learner.nbRows(), 500)
    self.assertEqual(learner.nbCols(), 4)

    siz = -1.0 * learner.nbRows()

    stat = learner.logLikelihood(["A"]) / siz  # LL=-N.H
    self.assertAlmostEqual(stat, 0.99943499, delta=1e-5)
    stat = learner.logLikelihood(["B"]) / siz  # LL=-N.H
    self.assertAlmostEqual(stat, 0.9986032, delta=1e-5)
    stat = learner.logLikelihood(["A", "B"]) / siz  # LL=-N.H
    self.assertAlmostEqual(stat, 1.9668973, delta=1e-5)
    stat = learner.logLikelihood(["A"], ["B"]) / siz  # LL=-N.H
    self.assertAlmostEqual(stat, 1.9668973 - 0.9986032, delta=1e-5)

    stat = learner.logLikelihood(["C"]) / siz  # LL=-N.H
    self.assertAlmostEqual(stat, 0.99860302, delta=1e-5)
    stat = learner.logLikelihood(["D"]) / siz  # LL=-N.H
    self.assertAlmostEqual(stat, 0.40217919, delta=1e-5)
    stat = learner.logLikelihood(["C", "D"]) / siz  # LL=-N.H
    self.assertAlmostEqual(stat, 1.40077995, delta=1e-5)
    stat = learner.logLikelihood(["C"], ["D"]) / siz  # LL=-N.H
    self.assertAlmostEqual(stat, 1.40077995 - 0.40217919, delta=1e-5)

  def testPossibleEdge(self):
    # possible edges are not relevant
    learner = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/asia3.csv'))
    learner.addPossibleEdge("visit_to_Asia?", "lung_cancer?")
    learner.addPossibleEdge("visit_to_Asia?", "smoking?")
    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 0)

    # possible edges are relevant
    learner = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/asia3.csv'))
    learner.addPossibleEdge("smoking?", "lung_cancer?")
    learner.addPossibleEdge("bronchitis?", "smoking?")
    possiblearcs = {(bn.idFromName("lung_cancer?"),bn.idFromName("smoking?")),
                    (bn.idFromName("smoking?"),bn.idFromName("lung_cancer?")),
                    (bn.idFromName("bronchitis?"),bn.idFromName("smoking?")),
                    (bn.idFromName("smoking?"),bn.idFromName("bronchitis?"))}

    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 2)
    self.assertTrue(possiblearcs.issuperset(bn.arcs()))

    # mixed with a forbidden arc
    learner = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/asia3.csv'))
    learner.addPossibleEdge("smoking?", "lung_cancer?")
    learner.addPossibleEdge("bronchitis?", "smoking?")
    learner.addForbiddenArc("smoking?", "bronchitis?")

    possiblearcs = {(bn.idFromName("lung_cancer?"),bn.idFromName("smoking?")),
                    (bn.idFromName("smoking?"),bn.idFromName("lung_cancer?")),
                    (bn.idFromName("bronchitis?"),bn.idFromName("smoking?")),
                    (bn.idFromName("smoking?"),bn.idFromName("bronchitis?"))}

    forbiddenarcs = {(bn.idFromName("smoking?"),bn.idFromName("bronchitis?"))}

    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 2)
    self.assertTrue(possiblearcs.issuperset(bn.arcs()))
    self.assertTrue(forbiddenarcs.isdisjoint(bn.arcs()));

    # mixed with a mandatory arc
    learner = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/asia3.csv'))
    learner.addPossibleEdge("smoking?", "lung_cancer?")
    learner.addPossibleEdge("bronchitis?", "smoking?")
    learner.addMandatoryArc("visit_to_Asia?", "bronchitis?")

    testedarcs = {(bn.idFromName("lung_cancer?"),bn.idFromName("smoking?")),
                  (bn.idFromName("smoking?"),bn.idFromName("lung_cancer?")),
                  (bn.idFromName("bronchitis?"),bn.idFromName("smoking?")),
                  (bn.idFromName("smoking?"),bn.idFromName("bronchitis?")),
                  (bn.idFromName("visit_to_Asia?"),bn.idFromName("bronchitis?"))}

    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 3)
    self.assertTrue(testedarcs.issuperset(bn.arcs()))

  def testPossibleEdgeWithTabuList(self):
    # possible edges are not relevant
    learner = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/asia3.csv'))
    learner.useLocalSearchWithTabuList()
    learner.addPossibleEdge("visit_to_Asia?", "lung_cancer?")
    learner.addPossibleEdge("visit_to_Asia?", "smoking?")

    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 0)

    # possible edges are relevant
    learner = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/asia3.csv'))
    learner.useLocalSearchWithTabuList()
    learner.addPossibleEdge("smoking?", "lung_cancer?")
    learner.addPossibleEdge("bronchitis?", "smoking?")
    possiblearcs = {(bn.idFromName("lung_cancer?"),bn.idFromName("smoking?")),
                    (bn.idFromName("smoking?"),bn.idFromName("lung_cancer?")),
                    (bn.idFromName("bronchitis?"),bn.idFromName("smoking?")),
                    (bn.idFromName("smoking?"),bn.idFromName("bronchitis?"))}

    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 2)
    self.assertTrue(possiblearcs.issuperset(bn.arcs()))

    # mixed with a forbidden arc
    learner = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/asia3.csv'))
    learner.useLocalSearchWithTabuList()
    learner.addPossibleEdge("smoking?", "lung_cancer?")
    learner.addPossibleEdge("bronchitis?", "smoking?")
    learner.addForbiddenArc("smoking?", "bronchitis?")

    possiblearcs = {(bn.idFromName("lung_cancer?"),bn.idFromName("smoking?")),
                    (bn.idFromName("smoking?"),bn.idFromName("lung_cancer?")),
                    (bn.idFromName("bronchitis?"),bn.idFromName("smoking?")),
                    (bn.idFromName("smoking?"),bn.idFromName("bronchitis?"))}

    forbiddenarcs = {(bn.idFromName("smoking?"),bn.idFromName("bronchitis?"))}
    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 2)
    self.assertTrue(possiblearcs.issuperset(bn.arcs()))
    self.assertTrue(forbiddenarcs.isdisjoint(bn.arcs()));

    # mixed with a mandatory arc
    learner = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/asia3.csv'))
    learner.useLocalSearchWithTabuList()
    learner.addPossibleEdge("smoking?", "lung_cancer?")
    learner.addPossibleEdge("bronchitis?", "smoking?")
    learner.addMandatoryArc("visit_to_Asia?", "bronchitis?")

    testedarcs = {(bn.idFromName("lung_cancer?"),bn.idFromName("smoking?")),
                    (bn.idFromName("smoking?"),bn.idFromName("lung_cancer?")),
                    (bn.idFromName("bronchitis?"),bn.idFromName("smoking?")),
                    (bn.idFromName("smoking?"),bn.idFromName("bronchitis?")),
                    (bn.idFromName("visit_to_Asia?"),bn.idFromName("bronchitis?"))}

    bn = learner.learnBN()
    self.assertEqual(bn.sizeArcs(), 3)
    self.assertTrue(testedarcs.issuperset(bn.arcs()))

  def testHybridLearning(self):
    learner = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/data1.csv'))
    learner.useMIIC()
    eg = learner.learnMixedStructure()
    skel = eg.skeleton()

    learner = gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/data1.csv'))
    learner.setPossibleSkeleton(skel)
    bn = learner.learnBN()

    self.assertEqual(bn.sizeArcs(), 4)
    self.assertEqual(bn.parents(bn.idFromName("V")), {bn.idFromName("A")})
    self.assertEqual(bn.parents(bn.idFromName("Y")), {bn.idFromName("X"), bn.idFromName("V")})
    self.assertEqual(bn.parents(bn.idFromName("Z")), {bn.idFromName("Y")})


  def test_RecordWeight(self):
    filename = self.agrumSrcDir('src/testunits/ressources/dataW.csv')
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
    filename = self.agrumSrcDir('src/testunits/ressources/dataW.csv')
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

    self.assertEqual(learner.rawPseudoCount(["X"]),(5,2))
    self.assertEqual(learner.rawPseudoCount(["X","Z"]),(1,0,2,1,2,1))
    self.assertEqual(learner.rawPseudoCount(["Y","Z"]),(0,1,0,3,2,1))

    learner.useAprioriSmoothing(0.1)

    self.assertEqual(learner.rawPseudoCount(["X"]),(5.1,2.1))
    self.assertEqual(learner.rawPseudoCount(["X","Z"]),(1.1,0.1,2.1,1.1,2.1,1.1))
    self.assertEqual(learner.rawPseudoCount(["Y","Z"]),(0.1,1.1,0.1,3.1,2.1,1.1))


    learner = gum.BNLearner(filename)
    self.assertEqual(learner.pseudoCount(["X"]).tolist(),[5,2])
    self.assertEqual(learner.pseudoCount(["X","Z"]).tolist(),[[1,0],[2,1],[2,1]])
    self.assertEqual(learner.pseudoCount(["Y","Z"]).tolist(),[[0,1],[0,3],[2,1]])
    self.assertEqual(learner.pseudoCount(["Z","Y"]).tolist(),[[0,0,2],[1,3,1]])

    learner.useAprioriSmoothing(0.1)

    self.assertEqual(learner.pseudoCount(["X"]).tolist(),[5.1,2.1])
    self.assertEqual(learner.pseudoCount(["X","Z"]).tolist(),[[1.1,0.1],[2.1,1.1],[2.1,1.1]])
    self.assertEqual(learner.pseudoCount(["Y","Z"]).tolist(),[[0.1,1.1],[0.1,3.1],[2.1,1.1]])
    self.assertEqual(learner.pseudoCount(["Z","Y"]).tolist(),[[0.1,0.1,2.1],[1.1,3.1,1.1]])

ts = unittest.TestSuite()
addTests(ts, BNLearnerCSVTestCase)
