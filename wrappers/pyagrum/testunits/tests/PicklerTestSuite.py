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
import pickle

import pyagrum as gum
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
