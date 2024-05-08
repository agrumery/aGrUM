# (c) Copyright 2015-2024 by Pierre-Henri Wuillemin(@LIP6)
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
import pickle

import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestPickler(pyAgrumTestCase):
  def testPicklingBN(self):
    model = gum.randomBN(n=30, ratio_arc=1.3, domain_size=5)
    model2 = gum.randomBN(n=30, ratio_arc=1.3, domain_size=5)
    self.assertNotEqual(model, model2)

    model.setProperty("test", "test")
    model.setProperty("test2", "test2")
    self.assertIn("test", model.properties())
    self.assertEqual(model.property("test"), "test")
    self.assertIn("test2", model.properties())
    self.assertEqual(model.property("test2"), "test2")

    filename = self.agrumSrcDir("pickled.pkl")
    gum.saveBN(model, filename)
    model2 = gum.loadBN(filename)

    self.assertEqual(model, model2)
    self.assertIn("test", model.properties())
    self.assertEqual(model.property("test"), "test")
    self.assertIn("test2", model.properties())
    self.assertEqual(model.property("test2"), "test2")

  def testPicklingInfluenceDiagram(self):
    model = gum.fastID("C<-A->*B<-C->D->$U<-B;*E->B")
    model2 = gum.fastID("C<-A->*B<-C->D->$U<-B;*E->B")
    self.assertNotEqual(model, model2)

    model.setProperty("test", "test")
    model.setProperty("test2", "test2")
    self.assertIn("test", model.properties())
    self.assertEqual(model.property("test"), "test")
    self.assertIn("test2", model.properties())
    self.assertEqual(model.property("test2"), "test2")

    filename = self.agrumSrcDir("pickled.pkl")
    gum.saveBN(model, filename)
    model2 = gum.loadBN(filename)
    self.assertEqual(model, model2)
    self.assertIn("test", model.properties())
    self.assertEqual(model.property("test"), "test")
    self.assertIn("test2", model.properties())
    self.assertEqual(model.property("test2"), "test2")

  def testPicklingMarkovRandomField(self):
    model = gum.fastMRF("A--B--C;D--A--F;G--B--C;G--C")
    model2 = gum.fastMRF("A--B--C;D--A--F;G--B--C;G--C")
    self.assertNotEqual(model, model2)

    model.setProperty("test", "test")
    model.setProperty("test2", "test2")
    self.assertIn("test", model.properties())
    self.assertEqual(model.property("test"), "test")
    self.assertIn("test2", model.properties())
    self.assertEqual(model.property("test2"), "test2")

    filename = self.agrumSrcDir("pickled.pkl")
    gum.saveBN(model, filename)
    model2 = gum.loadBN(filename)
    self.assertEqual(model, model2)
    self.assertIn("test", model.properties())
    self.assertEqual(model.property("test"), "test")
    self.assertIn("test2", model.properties())
    self.assertEqual(model.property("test2"), "test2")

  def testProperties(self):
    model = gum.randomBN(n=30, ratio_arc=1.3, domain_size=5)
    gum.config.asBool["Pickle", "add_version"] = True

    filename = self.agrumSrcDir("pickled.pkl")
    gum.saveBN(model, filename)

    model2 = gum.loadBN(filename)
    self.assertEqual(model2.property("version"), f"pyAgrum {gum.__version__}")
    self.assertIn("creation", model2.properties())
    self.assertIn("lastModification", model2.properties())

  def testEmpiricalDistribution(self):
    s = "A+[1.5,2.5,5,10.5]"
    model = gum.fastBN(s)
    self.assertTrue(model["A"].isEmpirical())
    self.assertEqual(model["A"].toFast(), s)

    filename = self.agrumSrcDir("pickled.pkl")
    gum.saveBN(model, filename)

    model2 = gum.loadBN(filename)
    self.assertTrue(model2["A"].isEmpirical())
    self.assertEqual(model2["A"].toFast(), s)


ts = unittest.TestSuite()
addTests(ts, TestPickler)
