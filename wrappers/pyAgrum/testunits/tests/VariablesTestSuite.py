# (c) Copyright 2015-2023 by Pierre-Henri Wuillemin(@LIP6)
# (pierre-henri.wuillemin@lip6.fr)

# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.

# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!
import unittest

import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class VariablesTestCase(pyAgrumTestCase):
  def setUp(self):
    self.varL1 = gum.LabelizedVariable("var1", "1")
    self.varL2 = gum.LabelizedVariable("var2", "2", 5)

  def tearDown(self):
    self.varL1 = self.varL2 = None


class DiscreteVariableTestCase(VariablesTestCase):
  def testLabelizedVarCopy(self):
    self.varL1.addLabel("coucou")
    self.varL1.addLabel("super")
    self.varD = self.varL1.clone()
    self.assertFalse(self.varD.empty())
    self.assertEqual(self.varD.label(3), "super")
    self.varD = None

  def testLabelsOfVars(self):
    v = gum.LabelizedVariable("a", "a")
    self.assertEqual(v.labels(), ("0", "1"))
    self.assertNotEqual(v.labels(), ("1", "0"))

    v = gum.LabelizedVariable("b", "b", 0).addLabel(
      "toto").addLabel("titi").addLabel("yes")
    self.assertEqual(v.labels(), ("toto", "titi", "yes"))

    v = gum.RangeVariable("c", "c", 0, 5)
    self.assertEqual(v.labels(), ("0", "1", "2", "3", "4", "5"))

    v = gum.RangeVariable("d", "d", 3, 5)
    self.assertEqual(v.labels(), ("3", "4", "5"))

    v = gum.DiscretizedVariable("e", "e").addTick(
      1).addTick(2).addTick(3).addTick(4)
    self.assertEqual(v.labels(), ("[1;2[", "[2;3[", "[3;4]"))

    v = gum.DiscretizedVariable("e", "e").addTick(
      1).addTick(2).addTick(3).addTick(4)
    v.setEmpirical(True)
    self.assertEqual(v.labels(), ("(1;2[", "[2;3[", "[3;4)"))

    v = gum.DiscretizedVariable("f", "f", [1, 5, 2, 4])
    self.assertEqual(v.labels(), ("[1;2[", "[2;4[", "[4;5]"))

    v = gum.DiscretizedVariable("f", "f", [1, 5, 2, 4])
    v.setEmpirical(True)
    self.assertEqual(v.labels(), ("(1;2[", "[2;4[", "[4;5)"))

    v = gum.DiscretizedVariable("f", "f", [1, 5, 2, 4], True)
    self.assertEqual(v.labels(), ("(1;2[", "[2;4[", "[4;5)"))

    v = gum.DiscretizedVariable("f", "f", [1, 5, 2, 4], True)
    v.setEmpirical(False)
    self.assertEqual(v.labels(), ("[1;2[", "[2;4[", "[4;5]"))

  def testHashableDiscreteVariable(self):
    va = gum.LabelizedVariable("a", "a")
    vb = gum.LabelizedVariable("b", "b", ["toto", "titi", "yes"])
    vc = gum.RangeVariable("c", "c", 0, 5)
    vd = gum.RangeVariable("d", "d", 3, 5)
    ve = gum.DiscretizedVariable("e", "e").addTick(1).addTick(2).addTick(3).addTick(4)

    s = set([va, vb, vc] + [vc, vd, ve] + [va, ve])
    self.assertEqual(len(s), 5)

  def testExportDerivedReadOnlyAPIforDiscreteVariable(self):
    bn = gum.fastBN("A{yes|no}->B{3.14|5|10|9.2}->C->D{1|3|9|5}->E[1,3.15,9.23,4.5]")
    self.assertEqual(bn.variable("A").isLabel("no"), True)
    self.assertEqual(bn.variable("A").posLabel("no"), 1)
    self.assertEqual(bn.variable("C").belongs(1), True)
    self.assertEqual(bn.variable("C").minVal(), 0)
    self.assertEqual(bn.variable("C").maxVal(), 1)
    self.assertEqual(bn.variable("B").numericalDomain(), [3.14, 5.0, 9.2, 10.])
    self.assertEqual(bn.variable("B").closestLabel(3), '3.14')
    self.assertEqual(bn.variable("B").closestIndex(3), 0)
    self.assertEqual(bn.variable("B").isValue(3), False)
    self.assertEqual(bn.variable("D").integerDomain(), [1, 3, 5, 9])
    self.assertEqual(bn.variable("D").isValue(3), True)
    self.assertEqual(bn.variable("E").isTick(3.14), False)
    self.assertEqual(bn.variable("E").ticks(), (1.0, 3.15, 4.5, 9.23))
    self.assertEqual(bn.variable("E").isEmpirical(), False)
    self.assertEqual(bn.variable("E").tick(1), 3.15)

  def testCheckDerivedReadOnlyAPIforDiscreteVariable(self):
    def not_imported_api(typ: str):
      bn = gum.fastBN("X")
      y = bn.variable("X")
      x = eval(f'gum.{typ}("X","X")')
      return set(x.__dir__()) - set(y.__dir__())

    # these methods are not down-exported in variables.i
    self.assertEqual(not_imported_api("LabelizedVariable"), {'addLabel', 'eraseLabels', 'changeLabel'})
    self.assertEqual(not_imported_api("RangeVariable"), {'setMinVal', 'setMaxVal'})
    self.assertEqual(not_imported_api("NumericalDiscreteVariable"),
                      {'eraseValue', 'eraseValues', 'addValue', 'changeValue'})
    self.assertEqual(not_imported_api("DiscretizedVariable"), {'eraseTicks', 'setEmpirical', 'addTick'})
    self.assertEqual(not_imported_api("IntegerVariable"), {'eraseValue', 'eraseValues', 'addValue', 'changeValue'})


class LabelizedVariableTestCase(VariablesTestCase):
  def testCopyConstructor(self):
    var = gum.LabelizedVariable(self.varL1)

  def testPythonListComprehension(self):
    c, s, r, w = [gum.LabelizedVariable(name, name, 2)
                  for name in 'c s r w'.split()]
    for var, name in zip([c, s, r, w], 'c s r w'.split()):
      self.assertEqual(var.name(), name)
      self.assertEqual(var.description(), name)
      self.assertEqual(var.domainSize(), 2)
    self.assertEqual(var.varType(), gum.VarType_Labelized)

    c.setDescription("this is a test")
    self.assertEqual(c.description(), "this is a test")

  def testLabels(self):
    gum.LabelizedVariable('a', '', 0).addLabel('a1').addLabel('a2') \
      .addLabel('a3').addLabel('a4') \
      .addLabel('a5').addLabel('a6')
    self.assertEqual(self.varL2.domainSize(), 5)
    self.varL1.addLabel("coucou")
    self.varL1.addLabel("super")
    self.assertEqual(self.varL1.domainSize(), 4)
    self.varL1.isLabel("coucou")
    self.assertEqual(self.varL1["super"], 3)
    self.assertEqual(self.varL1.label(3), "super")

  def testEraseLabels(self):
    self.varL2.eraseLabels()
    self.assertEqual(self.varL2.domainSize(), 0)


class RangeVariableTestCase(VariablesTestCase):
  def testCopyConstructor(self):
    var1 = gum.RangeVariable("var 1", "this is var 1")
    self.assertEqual(var1.varType(), gum.VarType_Range)

    var2 = gum.RangeVariable("var 2", "this is var 2", 1, 4)

    var3 = gum.RangeVariable(var1)
    self.assertEqual(var3.minVal(), var1.minVal())
    self.assertEqual(var3.maxVal(), var1.maxVal())
    self.assertNotEqual(var1.maxVal(), var2.maxVal())

  def testLabels(self):
    var1 = gum.RangeVariable("var 1", "this is var 1")
    self.assertEqual(var1.domainSize(), 2)
    self.assertFalse(var1.empty())

    var1.setMinVal(1)
    var1.setMaxVal(0)
    self.assertTrue(var1.empty())

    var1.setMaxVal(9)
    self.assertFalse(var1.empty())
    self.assertEqual(var1.domainSize(), 9)
    self.assertTrue(var1.belongs(3))
    self.assertFalse(var1.belongs(0))
    self.assertFalse(var1.belongs(10))

    self.assertEqual(var1.label(1), "2")
    self.assertEqual(var1["2"], 1)

  def testEraseLabels(self):
    self.varL2.eraseLabels()
    self.assertEqual(self.varL2.domainSize(), 0)


class DiscretizedVariableTestCase(VariablesTestCase):
  def testAddTicks(self):
    v = gum.DiscretizedVariable('a', '').addTick(0.5).addTick(
      5.9).addTick(5.99).addTick(0.1).addTick(0.23).addTick(12)
    self.assertEqual(v.varType(), gum.VarType_Discretized)

    var = gum.DiscretizedVariable("var", "test var")

    self.assertTrue(var.empty())
    self.assertEqual(var.domainSize(), 0)
    var.addTick(0.2)
    self.assertTrue(var.empty())
    self.assertEqual(var.domainSize(), 0)
    var.addTick(3)
    self.assertTrue(var.empty())
    self.assertEqual(var.domainSize(), 1)
    var.addTick(2)
    self.assertFalse(var.empty())
    self.assertEqual(var.domainSize(), 2)

    self.assertRaises(gum.DefaultInLabel, var.addTick, 2)
    self.assertEqual(str(var), "var:Discretized(<[0.2;2[,[2;3]>)")

  def testNormalLimits(self):
    var = gum.DiscretizedVariable("var", "test var")

    var.addTick(2.1).addTick(2.5).addTick(2.3).addTick(2.7)
    self.assertEqual(str(var), "var:Discretized(<[2.1;2.3[,[2.3;2.5[,[2.5;2.7]>)")

    with self.assertRaises(gum.OutOfBounds):
      x = var.index("0")
    self.assertEqual(var.index("2.1"), 0)
    self.assertEqual(var.index("2.2"), 0)
    self.assertEqual(var.index("2.3"), 1)
    self.assertEqual(var.index("2.4"), 1)
    self.assertEqual(var.index("2.5"), 2)
    self.assertEqual(var.index("2.6"), 2)
    self.assertEqual(var.index("2.7"), 2)
    with self.assertRaises(gum.OutOfBounds):
      x = var.index("8")

  def testOrderTicks(self):

    def _testOrderTicks(i, j, k, l, m, n):
      var = gum.DiscretizedVariable("var", "test var")
      var.addTick(i).addTick(j).addTick(k).addTick(l).addTick(m).addTick(n)
      self.assertEqual(var.domainSize(), 5)
      self.assertEqual(str(var), "var:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)")

    for i in range(1, 7):
      for j in range(1, 7):
        for k in range(1, 7):
          for l in range(1, 7):
            for m in range(1, 7):
              for n in range(1, 7):
                if (i + j + k + l + m + n == 21) & (i * j * k * l * m * n == 720):
                  _testOrderTicks(i, j, k, l, m, n)
  
  def testFastSyntax(self):
    var = gum.fastVariable("A[1,2,3,4,5,6]", 4)
    self.assertEqual(str(var), "A:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)")

    var = gum.fastVariable("A[1:6:5]", 4)
    self.assertEqual(str(var), "A:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)")

class NumericalDiscreteVariableTestCase(VariablesTestCase):
  def testAddValue(self):
    v = gum.NumericalDiscreteVariable('a', '').addValue(0.5).addValue(
      5.9).addValue(5.99).addValue(0.1).addValue(0.23).addValue(12)
    self.assertEqual(v.varType(), gum.VarType_Numerical)

    var = gum.NumericalDiscreteVariable("var", "test var")

    self.assertTrue(var.empty())
    self.assertEqual(var.domainSize(), 0)
    var.addValue(0.2)
    self.assertTrue(var.empty())
    self.assertEqual(var.domainSize(), 1)
    var.addValue(3.0)
    self.assertFalse(var.empty())
    self.assertEqual(var.domainSize(), 2)
    var.addValue(2.25)
    self.assertFalse(var.empty())
    self.assertEqual(var.domainSize(), 3)

    self.assertRaises(gum.DuplicateElement, var.addValue, 2.25)
    self.assertEqual(str(var), "var:NumericalDiscrete({0.2|2.25|3})")

  def testConstructor1(self):
    var = gum.NumericalDiscreteVariable("var", "test var", [1, 3.15, 2])
    self.assertFalse(var.empty())
    self.assertEqual(var.domainSize(), 3)
    self.assertEqual(str(var), "var:NumericalDiscrete({1|2|3.15})")

  def testConstructor2(self):
    var = gum.NumericalDiscreteVariable("var", "test var", 1.2, 5.2, 5)
    self.assertFalse(var.empty())
    self.assertEqual(var.domainSize(), 5)
    self.assertEqual(str(var), "var:NumericalDiscrete({1.2|2.2|3.2|4.2|5.2})")

  def testFastSyntax(self):
    var = gum.fastVariable("A{1|1.5|6|2.9|3.14}", 4)
    self.assertEqual(str(var), "A:NumericalDiscrete({1|1.5|2.9|3.14|6})")
    with self.assertRaises(gum.InvalidArgument):
      var = gum.fastVariable("A{3.14}", 4)

    var = gum.fastVariable("A{3.14}", 1)
    self.assertEqual(str(var), "A:NumericalDiscrete({3.14})")

    var = gum.fastVariable("A{1.2:5.2:5}", 5)
    self.assertEqual(str(var), "A:NumericalDiscrete({1.2|2.2|3.2|4.2|5.2})")

    var = gum.fastVariable("A{1.2:5.2:2}", 2)
    self.assertEqual(str(var), "A:NumericalDiscrete({1.2|5.2})")

    var = gum.fastVariable("A{1.2|5.2}", 2)
    self.assertEqual(str(var), "A:NumericalDiscrete({1.2|5.2})")

    var = gum.fastVariable("A{1.2:5.2:2.8}", 2)
    self.assertEqual(str(var), "A:NumericalDiscrete({1.2|5.2})")


ts = unittest.TestSuite()
addTests(ts, DiscreteVariableTestCase)
addTests(ts, LabelizedVariableTestCase)
addTests(ts, RangeVariableTestCase)
addTests(ts, DiscretizedVariableTestCase)
addTests(ts, NumericalDiscreteVariableTestCase)
