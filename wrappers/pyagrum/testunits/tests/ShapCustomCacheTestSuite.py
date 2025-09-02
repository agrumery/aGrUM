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

import pyagrum as gum
import pandas as pd
import unittest
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

from pyagrum.explain._CustomShapleyCache import CustomShapleyCache


class ShapCustomCacheTestCase(pyAgrumTestCase):
  def test_generate_keys(self):
    bn = gum.fastBN(
      "Id;SepalLengthCm;SepalWidthCm;PetalLengthCm;PetalWidthCm;Species;PetalLengthCm->SepalLengthCm;Species->PetalLengthCm;PetalWidthCm->Species;Species->SepalWidthCm;Id"
    )
    nodes = [0, 1, 2]
    target = 5
    feat = 2
    key1, key2, key3 = CustomShapleyCache.generate_keys(bn, target, feat, nodes)
    self.assertEqual(key1, (0, 1, 2))
    self.assertEqual(key2, (0, 1))
    self.assertEqual(key3, (1, 2))

    nodes = [0, 1, 2, 3, 4]
    key1, key2, key3 = CustomShapleyCache.generate_keys(bn, target, feat, nodes)
    self.assertEqual(key1, (0, 1, 2, 3, 4))
    self.assertEqual(key2, (0, 1, 3, 4))
    self.assertEqual(key3, (1, 2, 3, 4))

    nodes = [1, 2]
    key1, key2, key3 = CustomShapleyCache.generate_keys(bn, target, feat, nodes)
    self.assertEqual(key1, (1, 2))
    self.assertEqual(key2, (1,))
    self.assertEqual(key3, (1, 2))

  def test_cache_operations(self):
    cache = CustomShapleyCache(max_capacity=3)
    cache.set(1, (0, 1), 10)
    self.assertEqual(cache.get(1, (0, 1)), 10)
    cache.set(2, (0, 1), 20)
    self.assertEqual(cache.get(2, (0, 1)), 20)
    cache.set(2, (0, 1, 2), 30)
    self.assertEqual(cache.get(2, (0, 1, 2)), 30)
    cache.set(3, (0, 1, 2, 3), 40)
    self.assertEqual(cache.get(3, (0, 1, 2, 3)), 40)
    self.assertEqual(cache._current_size, 2)

    with self.assertRaises(KeyError):
      cache.get(1, (0, 1))


ts = unittest.TestSuite()
addTests(ts, ShapCustomCacheTestCase)
