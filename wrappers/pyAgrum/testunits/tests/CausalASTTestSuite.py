import sys
import os
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests
import pyAgrum.causal
import pyAgrum.causal._doAST as ast


class TestASTmethod(pyAgrumTestCase):
  def test__str__(self):
    bn = gum.fastBN("A->B->C<-D")

    nod = ast.ASTposteriorProba(bn, {"A", "C"}, {"B"})
    self.assertEqual(nod.__str__(), "P(A,C|B)")

    nod2 = ast.ASTposteriorProba(bn, {"A"}, {"C"})
    self.assertEqual(nod2.__str__(), "P(A|C)")

    su = ast.ASTsum(['A'], nod)
    self.assertEqual(su.__str__(), "sum on A for \n| P(A,C|B)")

    su2 = ast.ASTsum(['A', 'C'], nod)
    self.assertEqual(su2.__str__(), "sum on A,C for \n| P(A,C|B)")

    op = ast.ASTdiv(nod, nod2)
    self.assertEqual(op.__str__(), "/\n| P(A,C|B)\n| P(A|C)")

    op2 = ast.ASTplus(nod, nod2)
    self.assertEqual(op2.__str__(), "+\n| P(A,C|B)\n| P(A|C)")

    self.assertEqual(ast.ASTmult(op, op2).__str__(), """*
| /
| | P(A,C|B)
| | P(A|C)
| +
| | P(A,C|B)
| | P(A|C)""")

    self.assertEqual(ast.ASTminus(op2, op).__str__(), """-
| +
| | P(A,C|B)
| | P(A|C)
| /
| | P(A,C|B)
| | P(A|C)""")

    self.assertEqual(ast.ASTjointProba(["A"]).__str__(), "joint P(A)")
    self.assertEqual(ast.ASTjointProba(["A", "C"]).__str__(), "joint P(A,C)")


class TestASTFromNotebooks(pyAgrumTestCase):
  def testConditionalSimplification(self):
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

    nod = ast.ASTposteriorProba(m, {"y"}, {"x", "z1", "z2", "z3"})
    self.assertEqual(nod.__str__(), "P(y|z1,z3)")


ts = unittest.TestSuite()
addTests(ts, TestASTFromNotebooks)
addTests(ts, TestASTmethod)
