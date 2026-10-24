############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
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
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
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
from unittest.mock import MagicMock, patch

import numpy as np
from qiskit import QuantumCircuit, QuantumRegister

import pyagrum as gum
from pyagrum import BayesNet, DiscreteVariable, LabelizedVariable
from pyagrum.qBNSampling import qBNMC, qBNRejection

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


class QBNMCTestCase(pyAgrumTestCase):
  def setUp(self):
    self.mock_bn = MagicMock(spec=BayesNet)
    self.mock_bn.nodes.return_value = {0, 1, 2}

    mock_var0 = MagicMock(spec=DiscreteVariable)
    mock_var0.domainSize.return_value = 4
    mock_var0.name.return_value = "Variable0"

    mock_var1 = MagicMock(spec=DiscreteVariable)
    mock_var1.domainSize.return_value = 2
    mock_var1.name.return_value = "Variable1"

    mock_var2 = MagicMock(spec=DiscreteVariable)
    mock_var2.domainSize.return_value = 2
    mock_var2.name.return_value = "Variable2"

    self.mock_bn.variable.side_effect = lambda n_id: {
      0: mock_var0,
      1: mock_var1,
      2: mock_var2,
    }[n_id]

    self.qb_net = qBNMC(self.mock_bn)
    # Fix map explicitly to avoid relying on set iteration order
    self.qb_net.n_qb_map = {0: [0, 1], 1: [2], 2: [3]}

    self.asia_bn = gum.loadBN(self.agrumSrcDir("asia.bgum"))
    self.qb_asia = qBNMC(self.asia_bn)

  def test_initialization(self):
    self.assertEqual(self.qb_net.bn, self.mock_bn)

  def test_mapNodeToQBit(self):
    with patch.object(qBNMC, "getWidth", side_effect=[2, 1, 1]):
      qb_map = self.qb_net.mapNodeToQBit({0, 1, 2})
    # 4 qubits total, no overlap, each node has correct width
    self.assertEqual(len(qb_map), 3)
    all_qbs = sorted(q for qbs in qb_map.values() for q in qbs)
    self.assertEqual(all_qbs, list(range(4)))

  def test_getWidth(self):
    # domainSize=4 → ceil(log2(4))=2
    self.assertEqual(self.qb_net.getWidth(0), 2)
    # domainSize=2 → ceil(log2(2))=1
    self.assertEqual(self.qb_net.getWidth(1), 1)

  def test_getBinarizedParameters(self):
    width_dict = {"A": 2, "B": 3}
    param_dict = {"A": 2, "B": 5}

    def var_side(key):
      m = MagicMock()
      m.name.return_value = key
      return m

    self.mock_bn.variable.side_effect = var_side
    self.mock_bn.nodeId.side_effect = lambda var: ord(var.name()) - ord("A")

    result = self.qb_net.getBinarizedParameters(width_dict, param_dict)
    self.assertEqual(result[0], [1, 0])  # binary(2, width=2)
    self.assertEqual(result[1], [1, 0, 1])  # binary(5, width=3)

  def test_getRootNodes(self):
    self.mock_bn.nodes.return_value = {0, 1, 2, 3}
    self.mock_bn.parents.side_effect = lambda n: [] if n in {0, 2} else [0]
    self.assertEqual(self.qb_net.getRootNodes(), {0, 2})

  def test_getAllParentSates(self):
    expected = [{"smoking?": 0}, {"smoking?": 1}]
    self.assertEqual(self.qb_asia.getAllParentSates("lung_cancer?"), expected)

    expected2 = [
      {"tuberculosis?": 0, "lung_cancer?": 0},
      {"tuberculosis?": 1, "lung_cancer?": 0},
      {"tuberculosis?": 0, "lung_cancer?": 1},
      {"tuberculosis?": 1, "lung_cancer?": 1},
    ]
    self.assertEqual(self.qb_asia.getAllParentSates("tuberculos_or_cancer?"), expected2)

  def test_getQuantumRegisters(self):
    def var_side(n_id):
      v = LabelizedVariable(str(n_id), "label", ["0", "1"])
      return v

    self.mock_bn.variable.side_effect = var_side
    regs = self.qb_net.getQuantumRegisters()
    self.assertEqual(len(regs), 3)
    for key, reg in regs.items():
      self.assertIsInstance(reg, QuantumRegister)
      self.assertEqual(reg.size, 1)  # domainSize=2 → width=1
      self.assertEqual(reg.name, str(key))

  def test_indicatorFunction(self):
    binary_list = [[0, 0, 1], [0, 1, 1], [1, 0, 1], [1, 1, 1]]
    self.assertEqual(
      self.qb_net.indicatorFunction(binary_list, {2: 1}),
      [True, True, True, True],
    )
    self.assertEqual(
      self.qb_net.indicatorFunction([[0, 0, 0], [0, 1, 0]], {2: 1}),
      [False, False],
    )
    self.assertEqual(
      self.qb_net.indicatorFunction([[0, 0, 1], [0, 1, 0], [1, 0, 1], [1, 1, 0]], {1: 1, 2: 0}),
      [False, True, False, True],
    )
    self.assertEqual(
      self.qb_net.indicatorFunction([[0, 1], [1, 0]], {}),
      [False, False],
    )

  @patch.object(qBNMC, "getWidth", return_value=2)
  @patch.object(qBNMC, "indicatorFunction", return_value=[True, False, True, False])
  def test_getProbability(self, mock_indicator, mock_width):
    mock_cpt = MagicMock()
    mock_cpt.__getitem__.return_value = np.array([0.1, 0.2, 0.3, 0.4])
    mock_cpt.shape = (4,)
    self.mock_bn.cpt.return_value = mock_cpt

    result = self.qb_net.getProbability(1, 0, 0, {1: 0}, {0: 1})
    self.assertAlmostEqual(result, 0.4)
    mock_width.assert_called_once_with(0)
    mock_indicator.assert_called_once()

  @patch("pyagrum.qBNSampling._qBNMC.transpile")
  @patch("pyagrum.qBNSampling._qBNMC.SamplerV2")
  @patch("pyagrum.qBNSampling._qBNMC.AerSimulator")
  def test_aerSimulation(self, MockAer, MockSampler, mock_transpile):
    mock_backend = MagicMock()
    MockAer.return_value = mock_backend

    mock_sampler = MagicMock()
    MockSampler.return_value = mock_sampler
    mock_transpile.return_value = MagicMock(spec=QuantumCircuit)

    mock_job = MagicMock()
    mock_sampler.run.return_value = mock_job
    mock_job.result.return_value.__getitem__.return_value.data.meas.get_counts.return_value = {
      "000000": 5000,
      "111111": 5000,
    }

    qc = QuantumCircuit(6)
    qc.h([0, 1, 2, 3, 4, 5])
    res = self.qb_net.aerSimulation(qc, shots=10000)

    self.assertIsInstance(res, dict)
    self.assertIn("Variable0", res)
    self.assertAlmostEqual(sum(res["Variable0"]), 1.0)

  @staticmethod
  def _assertMarginalsClose(testcase, bn, run, delta):
    exact = gum.LazyPropagation(bn)
    exact.makeInference()
    for n in bn.nodes():
      name = bn.variable(n).name()
      got = run[name].tolist()
      ref = exact.posterior(name).tolist()
      for g, r in zip(got, ref):
        testcase.assertAlmostEqual(g, r, delta=delta, msg=name)

  def test_getQuantumRegistersFollowsQubitMap(self):
    # issue #200: the circuit is built from the registers in dict order, while the qubit ids of
    # n_qb_map are assigned in sorted node order: both orders must agree
    bn = gum.BayesNet()
    for i in range(10):
      bn.add(gum.LabelizedVariable(f"V{i}", "", 2))
    for i in range(1, 9):
      bn.erase(f"V{i}")
    bn.add(gum.LabelizedVariable("X", "", 2))  # reuses a freed id: ids {0, 3, 9}
    bn.addArc("V0", "X")
    bn.addArc("X", "V9")
    bn.cpt("V0").fillWith([0.2, 0.8])
    bn.cpt("X").fillWith([0.9, 0.1, 0.3, 0.7])
    bn.cpt("V9").fillWith([0.6, 0.4, 0.1, 0.9])
    # precondition: the iteration order of the nodes is not the sorted one
    self.assertNotEqual(list(bn.nodes()), sorted(bn.nodes()))

    qbn = qBNMC(bn)
    self.assertEqual(list(qbn.getQuantumRegisters().keys()), sorted(qbn.n_qb_map.keys()))
    self._assertMarginalsClose(self, bn, qbn.runBN(shots=8000), 0.05)

  def test_singleStateVariableIsRejected(self):
    # issue #203: a variable with a single state is not a random variable
    bn = gum.BayesNet()
    bn.add(gum.LabelizedVariable("A", "", 2))
    bn.add(gum.LabelizedVariable("D", "", 1))
    bn.add(gum.LabelizedVariable("C", "", 2))
    bn.addArc("A", "C")
    bn.addArc("D", "C")
    with self.assertRaises(gum.OperationNotAllowed):
      qBNMC(bn)

  def test_xFlipMaskFollowsCPTOrder(self):
    # issue #204: the binarized parent states must be matched to the parent qubits in CPT order,
    # whatever the iteration order of the dict returned by getBinarizedParameters
    bn = gum.BayesNet()
    bn.add(gum.LabelizedVariable("P", "", 3))
    bn.add(gum.LabelizedVariable("Q", "", 2))
    bn.add(gum.LabelizedVariable("C", "", 2))
    bn.addArc("P", "C")
    bn.addArc("Q", "C")
    bn.cpt("P").fillWith([0.5, 0.3, 0.2])
    bn.cpt("Q").fillWith([0.4, 0.6])
    for p, q, v in [(0, 0, 0.9), (0, 1, 0.2), (1, 0, 0.7), (1, 1, 0.4), (2, 0, 0.5), (2, 1, 0.1)]:
      bn.cpt("C")[{"P": p, "Q": q}] = [v, 1 - v]

    qbn = qBNMC(bn)
    original = qbn.getBinarizedParameters

    def reversedOrder(width_dict, param_dict):
      return dict(reversed(list(original(width_dict, param_dict).items())))

    qbn.getBinarizedParameters = reversedOrder
    self._assertMarginalsClose(self, bn, qbn.runBN(shots=8000), 0.05)


class QBNRejectionTestCase(pyAgrumTestCase):
  def _make_simple(self):
    bn = gum.fastBN("A->B<-C", 2)
    return qBNMC(bn), bn

  def test_addA(self):
    qbn, _ = self._make_simple()
    circ = qbn.buildCircuit(add_measure=False)
    size_before = circ.size()
    inf = qBNRejection(qbn)
    inf.addA(circ)
    self.assertGreater(circ.size(), size_before)

  def test_addB(self):
    qbn, _ = self._make_simple()
    circ = qbn.buildCircuit(add_measure=False)
    size_before = circ.size()
    inf = qBNRejection(qbn)
    inf.addB(circ, {0: 0})  # qubit 0 forced to 0 → one X gate added
    self.assertEqual(circ.size(), size_before + 1)

  def test_addZ(self):
    qbn, _ = self._make_simple()
    circ = qbn.buildCircuit(add_measure=False)
    size_before = circ.size()
    inf = qBNRejection(qbn)
    inf.addZ(circ, {0: 0})  # single qubit → one Z gate
    self.assertEqual(circ.size(), size_before + 1)

  def test_addS(self):
    qbn, _ = self._make_simple()
    circ = qbn.buildCircuit(add_measure=False)
    size_before = circ.size()
    inf = qBNRejection(qbn)
    inf.addS(circ, {0: 0})  # B + Z + B = 3 gates
    self.assertEqual(circ.size(), size_before + 3)

  def test_addInverse(self):
    qbn, _ = self._make_simple()
    circ = qbn.buildCircuit(add_measure=False)
    size_before = circ.size()
    inf = qBNRejection(qbn)
    inf.A = QuantumCircuit(*list(inf.q_registers.values()))
    inf.addA(inf.A)
    inf.addInverse(circ, inf.A)
    self.assertGreater(circ.size(), size_before)

  def test_makeInference(self):
    bn = gum.fastBN("A->B<-C", 2)
    qbn = qBNMC(bn)
    evidence = {"B": 0}

    ie = gum.LazyPropagation(bn)
    ie.setEvidence(evidence)
    ie.makeInference()
    exact_A = ie.posterior("A")

    qinf = qBNRejection(qbn)
    qinf.setEvidence(evidence)
    qinf.setMaxIter(200)
    qinf.makeInference()
    quantum_A = qinf.posterior("A")

    epsilon = 0.1
    self.assertLess(abs(exact_A[0] - quantum_A[0]), epsilon)
    self.assertLess(abs(exact_A[1] - quantum_A[1]), epsilon)

  @staticmethod
  def _xorBN():
    bn = gum.BayesNet()
    for name in "ABC":
      bn.add(gum.LabelizedVariable(name, "", 2))
    bn.addArc("A", "C")
    bn.addArc("B", "C")
    bn.cpt("A").fillWith([0.5, 0.5])
    bn.cpt("B").fillWith([0.5, 0.5])
    for a, b, v in [(0, 0, 0.95), (0, 1, 0.05), (1, 0, 0.95), (1, 1, 0.05)]:
      bn.cpt("C")[{"A": a, "B": b}] = [v, 1 - v]
    return bn

  def test_setEvidenceInvalidatesPreviousInference(self):
    # issue #197: after setEvidence, the posterior must answer the new evidence
    bn = self._xorBN()
    qr = qBNRejection(qBNMC(bn))
    qr.setMaxIter(50)
    qr.setEvidence({"C": 0})
    qr.makeInference()
    qr.setEvidence({"C": 1})
    post = qr.posterior("B").tolist()

    ie = gum.LazyPropagation(bn)
    ie.setEvidence({"C": 1})
    ie.makeInference()
    for g, r in zip(post, ie.posterior("B").tolist()):
      self.assertAlmostEqual(g, r, delta=0.2)

  def test_useFragmentBNInvalidatesGates(self):
    # issue #197: after useFragmentBN, the gates must be rebuilt for the new circuit layout
    bn = gum.fastBN("Y<-X0->X1->X2")
    qr = qBNRejection(qBNMC(bn))
    qr.setMaxIter(20)
    qr.setEvidence({"X2": 1})
    qr.getGates()
    qr.useFragmentBN(target={"X1"})
    qr.makeInference()
    self.assertEqual(qr.A.num_qubits, qr.qbn.getTotNumQBits())

  def test_useFragmentBNKeepsIntKeyedEvidence(self):
    # issue #202: useFragmentBN rebuilds the network; an int-keyed evidence must still refer to the
    # same variable afterwards
    bn = gum.BayesNet()
    bn.add(gum.LabelizedVariable("Y", "", 2))
    for i in range(4):
      bn.add(gum.LabelizedVariable(f"X{i}", "", 2))
    for i in range(3):
      bn.addArc(f"X{i}", f"X{i + 1}")
    bn.addArc("X0", "Y")
    x3 = bn.idFromName("X3")

    qr = qBNRejection(qBNMC(bn))
    qr.setEvidence({x3: 1})
    qr.useFragmentBN(target={"X2"})
    self.assertEqual({qr.qbn.bn.variable(k).name(): v for k, v in qr.evidence.items()}, {"X3": 1})


ts = unittest.TestSuite()
addTests(ts, QBNMCTestCase)
addTests(ts, QBNRejectionTestCase)
