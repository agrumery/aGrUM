/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/PRM/classBayesNet.h>
#include <agrum/PRM/inference/groundedInference.h>
#include <agrum/PRM/inference/structuredBayesBall.h>
#include <agrum/PRM/inference/structuredInference.h>
#include <agrum/PRM/inference/SVE.h>
#include <agrum/PRM/inference/SVED.h>
#include <agrum/PRM/instanceBayesNet.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

#define GET_RESSOURCES_PATH_O3PRM(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/o3prm/" #x

namespace gum_tests {

  class [[maybe_unused]] PRMTestSuite: public CxxTest::TestSuite {
    private:
    gum::prm::PRM< double >* prm;
    gum::prm::PRM< double >* small;

    public:
    void setUp() {
      {
        gum::prm::o3prm::O3prmReader< double > reader;
        reader.readFile(GET_RESSOURCES_PATH_O3PRM(inference.o3prm));
        prm = reader.prm();
      }
      {
        gum::prm::o3prm::O3prmReader< double > reader;
        reader.readFile(GET_RESSOURCES_PATH_O3PRM(printers_systems.o3prm));
        small = reader.prm();
      }
    }

    void tearDown() {
      delete prm;
      delete small;
    }

    GUM_ACTIVE_TEST(Creation) {
      gum::prm::ClassBayesNet< double >* c = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("SafeComputer"))
      TS_GUM_ASSERT_THROWS_NOTHING(
          c = new gum::prm::ClassBayesNet< double >(prm->getClass("SafeComputer")));
      TS_GUM_ASSERT_THROWS_NOTHING(delete c)
      gum::prm::InstanceBayesNet< double >* inst = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(
          inst = new gum::prm::InstanceBayesNet< double >(prm->getSystem("aSys").get("c1")));
      TS_GUM_ASSERT_THROWS_NOTHING(delete inst)
    }

    GUM_ACTIVE_TEST(ClassAccess) {
      gum::prm::PRMClass< double >&      c  = prm->getClass("SafeComputer");
      gum::prm::ClassBayesNet< double >* bn = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(
          bn = new gum::prm::ClassBayesNet< double >(prm->getClass("SafeComputer")));
      gum::Size elts = c.attributes().size() + c.aggregates().size();
      TS_ASSERT_EQUALS(bn->size(), elts)

      for (const auto attr: c.attributes()) {
        gum::NodeId id = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(attr->cpf())
        TS_GUM_ASSERT_THROWS_NOTHING(id = bn->idFromName(attr->safeName()))
        TS_GUM_ASSERT_THROWS_NOTHING(bn->cpt(id))
        TS_ASSERT_EQUALS(attr->cpf().nbrDim(), bn->cpt(id).nbrDim())
        TS_ASSERT_EQUALS(attr->cpf().domainSize(), bn->cpt(id).domainSize())
      }

      TS_ASSERT(bn->modalities().size() > 0)
      TS_GUM_ASSERT_THROWS_NOTHING(delete bn)
    }

    GUM_ACTIVE_TEST(InstanceAccess) {
      gum::prm::InstanceBayesNet< double >* bn = 0;
      gum::prm::PRMInstance< double >&      i  = prm->getSystem("aSys").get("c1");
      TS_GUM_ASSERT_THROWS_NOTHING(bn = new gum::prm::InstanceBayesNet< double >(i))
      TS_ASSERT_EQUALS(bn->size(), i.size())

      for (auto attr = i.begin(); attr != i.end(); ++attr) {
        gum::NodeId id = 0;
        TS_GUM_ASSERT_THROWS_NOTHING((*(attr.val())).cpf())
        TS_GUM_ASSERT_THROWS_NOTHING(id = bn->idFromName((*(attr.val())).safeName()))
        TS_GUM_ASSERT_THROWS_NOTHING(bn->cpt(id))
        TS_ASSERT_EQUALS((*(attr.val())).cpf().nbrDim(), bn->cpt(id).nbrDim())
        TS_ASSERT_EQUALS((*(attr.val())).cpf().domainSize(), bn->cpt(id).domainSize())
      }

      TS_ASSERT(bn->modalities().size() > 0)
      TS_GUM_ASSERT_THROWS_NOTHING(delete bn)
    }

    GUM_ACTIVE_TEST(GroundedBN) {
      gum::prm::PRMSystem< double >& sys = prm->getSystem("aSys");
      gum::BayesNet< double >        bn;
      gum::BayesNetFactory< double > bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(sys.groundedBN(bn_factory))
      int count = 0;

      for (auto iter = sys.begin(); iter != sys.end(); ++iter) {
        for (auto jter = (*(iter.val())).begin(); jter != (*(iter.val())).end(); ++jter) {
          ++count;
        }
      }

      int wount = 0;

      for (const auto node: bn.nodes()) {
        wount++;
        std::string                       var      = bn.variable(node).name();
        size_t                            pos      = var.find_first_of('.');
        gum::prm::PRMInstance< double >&  instance = sys.get(var.substr(0, pos));
        gum::prm::PRMAttribute< double >& attr     = instance.get(var.substr(pos + 1));
        TS_ASSERT_DIFFERS(bn.cpt(node).nbrDim(), (gum::Size)0)

        if (gum::prm::PRMClassElement< double >::isAggregate(instance.type().get(attr.id()))) {
          TS_ASSERT_DIFFERS(attr.cpf().nbrDim(), (gum::Size)1)
        }
      }

      TS_ASSERT_EQUALS(count, wount)

      for (const auto node: bn.nodes()) {
        const gum::DiscreteVariable* var = &(bn.variable(node));

        for (const auto node2: bn.nodes())
          if (node != node2) { TS_ASSERT_DIFFERS(var, &(bn.variable(node2))); }
      }
    }

    GUM_ACTIVE_TEST(CPF) {
      gum::prm::PRMSystem< double >& sys = prm->getSystem("aSys");

      for (gum::prm::PRMSystem< double >::iterator iter = sys.begin(); iter != sys.end(); ++iter) {
        for (gum::prm::PRMInstance< double >::iterator jter = (*(iter.val())).begin();
             jter != (*(iter.val())).end();
             ++jter) {
          gum::Instantiation i((*(jter.val())).cpf()), var;
          var.add((*(jter.val())).type().variable());

          for (i.setFirstOut(var); !i.end(); i.incOut(var)) {
            double f = 0.0;

            for (i.setFirstIn(var); !i.end(); i.incIn(var)) {
              f += (*(jter.val())).cpf().get(i);
            }

            TS_ASSERT_DELTA(f, 1.0, 1e-7)
            i.unsetOverflow();
          }
        }
      }
    }

    GUM_ACTIVE_TEST(NormalisedCPT) {
      gum::prm::PRMSystem< double >& sys = prm->getSystem("aSys");
      gum::BayesNet< double >        bn;
      gum::BayesNetFactory< double > bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(sys.groundedBN(bn_factory))

      for (const auto node: bn.nodes()) {
        const gum::Tensor< double >& cpt = bn.cpt(node);
        gum::Instantiation           i(cpt), j;
        j.add(bn.variable(node));

        for (i.setFirstOut(j); !i.end(); i.incOut(j)) {
          double sum = 0.0;

          for (i.setFirstIn(j); !i.end(); i.incIn(j))
            sum += cpt.get(i);

          TS_ASSERT_DELTA(sum, 1.0, 1e-7)
          i.unsetOverflow();
        }
      }
    }
  };

}   // namespace gum_tests
