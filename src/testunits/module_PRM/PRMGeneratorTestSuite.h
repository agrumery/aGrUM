/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <fstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/PRM/generator/clusteredLayerGenerator.h>
#include <agrum/PRM/generator/layerGenerator.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  PRMGenerator
#define GUM_CURRENT_MODULE PRM

namespace gum_tests {

  struct PRMGeneratorTestSuite {
    private:

    public:
    static void testConstructors() {
      gum::prm::LayerGenerator< double >* gen = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(gen = new gum::prm::LayerGenerator< double >());
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen);
    }   // namespace gum_tests

    void generateLayerLayer(std::vector< gum::prm::LayerGenerator< double >::LayerData >& v,
                            size_t layer_count) {
      for (size_t lvl = 0; lvl < layer_count; ++lvl) {
        v.push_back(gum::prm::LayerGenerator< double >::LayerData());
        v[lvl].a              = 30;
        v[lvl].g              = 2;
        v[lvl].c              = 1;
        v[lvl].o              = 10;
        v[lvl].inner_density  = 0.2f;
        v[lvl].outter_density = 0.05f;
      }
    }

    PRMGeneratorTestSuite() {
      // std::cerr << std::endl;
    }

    static void testLayerGenerator() {
      gum::prm::LayerGenerator< double >* gen = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(gen = new gum::prm::LayerGenerator< double >());
      gen->setDomainSize(6);
      gen->setMaxParents(5);
      std::vector< gum::prm::LayerGenerator< double >::LayerData > v;
      generateLayerLayer(v, 10);
      gen->setLayers(v);
      gum::prm::PRM< double >* prm = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(prm = gen->generate());
      // testing interfaces
      const gum::Set< gum::prm::PRMInterface< double >* >& i_set = prm->interfaces();
      CHECK((i_set.size()) == (static_cast< gum::Size >(10)));

      for (auto iter = i_set.begin(); iter != i_set.end(); ++iter) {
        const gum::prm::PRMInterface< double >& i = **iter;

        if (i.referenceSlots().size()) {
          CHECK((i.referenceSlots().size()) == (static_cast< gum::Size >(1)));
          CHECK((i.attributes().size()) == (static_cast< gum::Size >(32)));
        } else {
          CHECK((i.referenceSlots().size()) == (static_cast< gum::Size >(0)));
          CHECK((i.attributes().size()) == (static_cast< gum::Size >(30)));
        }

        gum::Size                                            six  = 0;
        gum::Size                                            two  = 0;
        const gum::Set< gum::prm::PRMAttribute< double >* >& attr = i.attributes();

        for (gum::Set< gum::prm::PRMAttribute< double >* >::const_iterator a = attr.begin();
             a != attr.end();
             ++a) {
          if ((**a).type()->domainSize() == static_cast< gum::Size >(6)) {
            ++six;
          } else if ((**a).type()->domainSize() == 2) {
            ++two;
          } else {
            CHECK(false);
          }
        }

        CHECK((six) == (static_cast< gum::Size >(30)));

        if (i.referenceSlots().size()) { CHECK((two) == (static_cast< gum::Size >(2))); }
      }

      // testing classes
      const gum::Set< gum::prm::PRMClass< double >* >& c_set = prm->classes();

      for (gum::Set< gum::prm::PRMClass< double >* >::const_iterator c = c_set.begin();
           c != c_set.end();
           ++c) {
        CHECK(((**c).attributes().size()) == (static_cast< gum::Size >(30)));

        for (gum::Set< gum::prm::PRMAttribute< double >* >::const_iterator a
             = (**c).attributes().begin();
             a != (**c).attributes().end();
             ++a) {
          CHECK((**c).containerDag().parents((**a).id()).size() < 6);
        }
      }

      // testing instances
      const gum::prm::PRMSystem< double >& sys = **(prm->systems().begin());
      CHECK((sys.size()) == (static_cast< gum::Size >(100)));

      if (prm) delete prm;

      if (gen) { GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen); }
    }

    static void testClusterGenerator() {
      gum::prm::ClusteredLayerGenerator< double >* gen = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(gen = new gum::prm::ClusteredLayerGenerator< double >());
      gen->setDomainSize(6);
      gen->setMaxParents(5);
      gen->setClusterRatio(1.0);
      std::vector< gum::prm::LayerGenerator< double >::LayerData > v;
      generateLayerLayer(v, 10);
      gen->setLayers(v);
      gum::prm::PRM< double >* prm = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(prm = gen->generate());
      // testing interfaces
      const gum::Set< gum::prm::PRMInterface< double >* >& i_set = prm->interfaces();
      CHECK((i_set.size()) == (static_cast< gum::Size >(10)));

      for (gum::Set< gum::prm::PRMInterface< double >* >::const_iterator iter = i_set.begin();
           iter != i_set.end();
           ++iter) {
        const gum::prm::PRMInterface< double >& i = **iter;

        if (i.referenceSlots().size()) {
          CHECK((i.referenceSlots().size()) == (static_cast< gum::Size >(1)));
          CHECK((i.attributes().size()) == (static_cast< gum::Size >(32)));
        } else {
          CHECK((i.referenceSlots().size()) == (static_cast< gum::Size >(0)));
          CHECK((i.attributes().size()) == (static_cast< gum::Size >(30)));
        }

        gum::Size                                            six  = 0;
        gum::Size                                            two  = 0;
        const gum::Set< gum::prm::PRMAttribute< double >* >& attr = i.attributes();

        for (gum::Set< gum::prm::PRMAttribute< double >* >::const_iterator a = attr.begin();
             a != attr.end();
             ++a) {
          if ((**a).type()->domainSize() == static_cast< gum::Size >(6)) {
            ++six;
          } else if ((**a).type()->domainSize() == 2) {
            ++two;
          } else {
            CHECK(false);
          }
        }

        CHECK((six) == (static_cast< gum::Size >(30)));

        if (i.referenceSlots().size()) { CHECK((two) == (static_cast< gum::Size >(2))); }
      }

      // testing classes
      const gum::Set< gum::prm::PRMClass< double >* >& c_set = prm->classes();

      for (gum::Set< gum::prm::PRMClass< double >* >::const_iterator c = c_set.begin();
           c != c_set.end();
           ++c) {
        CHECK(((**c).attributes().size()) == (static_cast< gum::Size >(30)));

        for (gum::Set< gum::prm::PRMAttribute< double >* >::const_iterator a
             = (**c).attributes().begin();
             a != (**c).attributes().end();
             ++a) {
          CHECK((**c).containerDag().parents((**a).id()).size() < 6);
        }
      }

      // testing instances
      const gum::prm::PRMSystem< double >& sys = **(prm->systems().begin());
      CHECK(sys.size() > static_cast< gum::Size >(100));

      if (prm) delete prm;

      if (gen) { GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen); }
    }
  };

  GUM_TEST_ACTIF(Constructors)
  GUM_TEST_ACTIF(LayerGenerator)
  GUM_TEST_ACTIF(ClusterGenerator)

}   // namespace gum_tests
