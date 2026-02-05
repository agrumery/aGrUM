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

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  PRMInference
#define GUM_CURRENT_MODULE PRM

namespace gum_tests {

  struct PRMInferenceTestSuite {
    private:
    gum::prm::PRM< double >* prm;
    gum::prm::PRM< double >* small;

    public:
    static void testEvidence() {
      gum::prm::GroundedInference< double >* g_ve = nullptr;
      gum::VariableElimination< double >*    ve   = nullptr;
      gum::BayesNet< double >                bn;
      gum::BayesNetFactory< double >         bn_factory(&bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getSystem("aSys").groundedBN(bn_factory));
      GUM_CHECK_ASSERT_THROWS_NOTHING(ve = new gum::VariableElimination< double >(&bn));
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          g_ve = new gum::prm::GroundedInference< double >(*prm, prm->getSystem("aSys")));
      GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
      const gum::prm::PRMInstance< double >&  instance  = prm->getSystem("aSys").get("c1");
      const gum::prm::PRMAttribute< double >& attribute = instance.get("can_print");
      gum::Tensor< double >                   e;
      e.add(attribute.type().variable());
      e.fill(static_cast< double >(0.0));
      gum::Instantiation inst(e);
      inst.setFirst();
      e.set(inst, static_cast< double >(1.0));
      gum::prm::PRMInference< double >::Chain chain = std::make_pair(&instance, &attribute);
      GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
      CHECK(g_ve->hasEvidence(chain));
      delete g_ve;
    }

    PRMInferenceTestSuite() {
      {
        gum::prm::o3prm::O3prmReader< double > reader;

        reader.readFile(GET_RESSOURCES_PATH("o3prm/inference.o3prm"));

        reader.showElegantErrorsAndWarnings();

        prm = reader.prm();

        if (reader.errors() > 0) { GUM_ERROR(gum::FatalError, "could not load ressource file") }
      }

      {
        gum::prm::o3prm::O3prmReader< double > reader;

        reader.readFile(GET_RESSOURCES_PATH("o3prm/printers_systems.o3prm"));

        reader.showElegantErrorsAndWarnings();

        small = reader.prm();

        if (reader.errors() > 0) { GUM_ERROR(gum::FatalError, "could not load ressource file") }
      }
    }

    ~PRMInferenceTestSuite() {
      delete prm;

      delete small;
    }

    static void testSmallGrdInference() {
      // Creating the inference engine
      gum::prm::GroundedInference< double >* g_ve = 0;
      gum::VariableElimination< double >*    ve   = 0;
      gum::BayesNet< double >                bn;
      gum::BayesNetFactory< double >         bn_factory(&bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(small->getSystem("microSys").groundedBN(bn_factory));
      GUM_CHECK_ASSERT_THROWS_NOTHING(ve = new gum::VariableElimination< double >(&bn));
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          g_ve = new gum::prm::GroundedInference< double >(*small, small->getSystem("microSys")));
      GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
      // Building query
      {
        const gum::prm::PRMInstance< double >&  instance  = small->getSystem("microSys").get("c");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("can_print");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->posterior(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        CHECK((m.get(i)) == doctest::Approx(0.600832).epsilon(1e-6));
        i.inc();
        CHECK((m.get(i)) == doctest::Approx(0.399168).epsilon(1e-6));
      }
      {
        const gum::prm::PRMInstance< double >&  instance  = small->getSystem("microSys").get("p");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("equipState");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->posterior(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        CHECK((m.get(i)) == doctest::Approx(0.49896).epsilon(1e-6));
        i.inc();
        CHECK((m.get(i)) == doctest::Approx(0.50104).epsilon(1e-6));
      }
      {
        const gum::prm::PRMInstance< double >&  instance  = small->getSystem("microSys").get("pow");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("powState");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->posterior(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        CHECK((m.get(i)) == doctest::Approx(0.99).epsilon(1e-6));
        i.inc();
        CHECK((m.get(i)) == doctest::Approx(0.01).epsilon(1e-6));
      }
      delete g_ve;
    }

    static void testSmallSVEInference() {
      gum::prm::SVE< double >* sve = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          sve = new gum::prm::SVE< double >(*small, small->getSystem("microSys")));
      {
        gum::BayesNet< double >        bn;
        gum::BayesNetFactory< double > bn_factory(&bn);
        small->getSystem("microSys").groundedBN(bn_factory);
        auto                                  ve = new gum::VariableElimination< double >(&bn);
        gum::prm::GroundedInference< double > g_ve(*small, small->getSystem("microSys"));
        g_ve.setBNInference(ve);

        const gum::prm::PRMInstance< double >&  instance  = small->getSystem("microSys").get("c");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("can_print");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);

        gum::Tensor< double > m, n;
        GUM_CHECK_ASSERT_THROWS_NOTHING(sve->posterior(chain, m));
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve.posterior(chain, n));
        gum::Instantiation i(m), j(n);
        i.setFirst(), j.setFirst();
        CHECK((m.get(i)) == doctest::Approx(0.600832).epsilon(1e-6));
        CHECK((m.get(i)) == doctest::Approx(n.get(j)).epsilon(1e-6));
        i.inc(), j.inc();
        CHECK((m.get(i)) == doctest::Approx(0.399168).epsilon(1e-6));
        CHECK((m.get(i)) == doctest::Approx(n.get(j)).epsilon(1e-6));
      }
      {
        const gum::prm::PRMInstance< double >&  instance  = small->getSystem("microSys").get("p");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("equipState");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(sve->posterior(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        CHECK((m.get(i)) == doctest::Approx(0.49896).epsilon(1e-6));
        i.inc();
        CHECK((m.get(i)) == doctest::Approx(0.50104).epsilon(1e-6));
      }
      {
        const gum::prm::PRMInstance< double >&  instance  = small->getSystem("microSys").get("pow");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("powState");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(sve->posterior(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        CHECK((m.get(i)) == doctest::Approx(0.99).epsilon(1e-6));
        i.inc();
        CHECK((m.get(i)) == doctest::Approx(0.01).epsilon(1e-6));
      }
      delete sve;
    }

    static void testSmallSVEDInference() {
      gum::prm::SVED< double >* sved = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          sved = new gum::prm::SVED< double >(*small, small->getSystem("microSys")));
      {
        const gum::prm::PRMInstance< double >&  instance  = small->getSystem("microSys").get("c");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("can_print");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(sved->posterior(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        CHECK((m.get(i)) == doctest::Approx(0.600832).epsilon(1e-6));
        i.inc();
        CHECK((m.get(i)) == doctest::Approx(0.399168).epsilon(1e-6));
      }
      {
        const gum::prm::PRMInstance< double >&  instance  = small->getSystem("microSys").get("p");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("equipState");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(sved->posterior(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        CHECK((m.get(i)) == doctest::Approx(0.49896).epsilon(1e-6));
        i.inc();
        CHECK((m.get(i)) == doctest::Approx(0.50104).epsilon(1e-6));
      }
      {
        const gum::prm::PRMInstance< double >&  instance  = small->getSystem("microSys").get("pow");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("powState");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(sved->posterior(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        CHECK((m.get(i)) == doctest::Approx(0.99).epsilon(1e-6));
        i.inc();
        CHECK((m.get(i)) == doctest::Approx(0.01).epsilon(1e-6));
      }
      delete sved;
    }

    static void testSmallStructInference() {
      gum::prm::StructuredInference< double >* inf = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          inf = new gum::prm::StructuredInference< double >(*small, small->getSystem("microSys")));
      {
        const gum::prm::PRMInstance< double >&  instance  = small->getSystem("microSys").get("c");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("can_print");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(inf->posterior(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        CHECK((m.get(i)) == doctest::Approx(0.600832).epsilon(1e-6));
        i.inc();
        CHECK((m.get(i)) == doctest::Approx(0.399168).epsilon(1e-6));
      }
      {
        const gum::prm::PRMInstance< double >&  instance  = small->getSystem("microSys").get("p");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("equipState");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(inf->posterior(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        CHECK((m.get(i)) == doctest::Approx(0.49896).epsilon(1e-6));
        i.inc();
        CHECK((m.get(i)) == doctest::Approx(0.50104).epsilon(1e-6));
      }
      {
        const gum::prm::PRMInstance< double >&  instance  = small->getSystem("microSys").get("pow");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("powState");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(inf->posterior(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        CHECK((m.get(i)) == doctest::Approx(0.99).epsilon(1e-6));
        i.inc();
        CHECK((m.get(i)) == doctest::Approx(0.01).epsilon(1e-6));
      }
      delete inf;
    }

    static void testInference() {
      gum::prm::GroundedInference< double >* g_ve = 0;
      gum::prm::GroundedInference< double >* g_ss = 0;
      gum::VariableElimination< double >*    ve   = 0;
      gum::ShaferShenoyInference< double >*  ss   = 0;
      gum::BayesNet< double >                bn;
      gum::BayesNetFactory< double >         bn_factory(&bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getSystem("aSys").groundedBN(bn_factory));
      GUM_CHECK_ASSERT_THROWS_NOTHING(ve = new gum::VariableElimination< double >(&bn));
      GUM_CHECK_ASSERT_THROWS_NOTHING(ss = new gum::ShaferShenoyInference< double >(&bn));
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          g_ve = new gum::prm::GroundedInference< double >(*prm, prm->getSystem("aSys")));
      GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          g_ss = new gum::prm::GroundedInference< double >(*prm, prm->getSystem("aSys")));
      GUM_CHECK_ASSERT_THROWS_NOTHING(g_ss->setBNInference(ss));

      for (const auto node: bn.nodes()) {
        gum::Tensor< double > m_ve, m_ss, m_sve, m_sved, m_struct;

        try {
          size_t pos = bn.variableNodeMap().name(node).find_first_of('.');
          const gum::prm::PRMInstance< double >& instance
              = prm->getSystem("aSys").get(bn.variableNodeMap().name(node).substr(0, pos));
          const gum::prm::PRMAttribute< double >& attribute
              = instance.get(bn.variableNodeMap().name(node).substr(pos + 1));
          gum::prm::PRMInference< double >::Chain chain = std::make_pair(&instance, &attribute);
          std::string                             dot   = ".";
          g_ve->posterior(chain, m_ve);
          g_ss->posterior(chain, m_ss);
          gum::prm::SVE< double > sve(*prm, prm->getSystem("aSys"));
          sve.posterior(chain, m_sve);
          gum::prm::SVED< double > sved(*prm, prm->getSystem("aSys"));
          sved.posterior(chain, m_sved);
          gum::prm::StructuredInference< double > structinf(*prm, prm->getSystem("aSys"));
          structinf.posterior(chain, m_struct);
          // We need two instantiations, one for the grounded tensors and one
          // for the PRM<double>-level ones
          gum::Instantiation inst(m_ve), jnst(m_sve);
          std::string        foo = instance.name() + dot + attribute.safeName();

          for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end());
               inst.inc(), jnst.inc()) {
            CHECK((m_ve.nbrDim()) == (m_ss.nbrDim()));
            CHECK((m_ve.nbrDim()) == (m_sve.nbrDim()));
            CHECK((m_ve.nbrDim()) == (m_sved.nbrDim()));
            CHECK((m_ve.nbrDim()) == (m_struct.nbrDim()));
            CHECK((m_ve.domainSize()) == (m_ss.domainSize()));
            CHECK((m_ve.domainSize()) == (m_sve.domainSize()));
            CHECK((m_ve.domainSize()) == (m_sved.domainSize()));
            CHECK((m_ve.domainSize()) == (m_struct.domainSize()));
            CHECK((m_ve.get(inst)) == doctest::Approx(m_ss.get(inst)).epsilon(1.0e-3));
            CHECK((m_sve.get(jnst)) == doctest::Approx(m_ss.get(inst)).epsilon(1.0e-3));
            CHECK((m_sved.get(jnst)) == doctest::Approx(m_sve.get(jnst)).epsilon(1.0e-3));
            CHECK((m_sved.get(jnst)) == doctest::Approx(m_struct.get(jnst)).epsilon(1.0e-3));
          }

        } catch (gum::Exception& e) {
          GUM_CHECK_ASSERT_THROWS_NOTHING(throw e);
          break;
        }

#ifdef GUM_DEBUG_MODE
        // In debug mode we do it for just one node
        break;
#endif
      }

      delete g_ve;
      delete g_ss;
    }

    static void testStructuredBB() {
      gum::prm::SVE< double >                  sve(*prm, prm->getSystem("aSys"));
      gum::prm::StructuredBayesBall< double >* bb = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(bb = new gum::prm::StructuredBayesBall< double >(sve));
      gum::prm::PRMInstance< double >& i = prm->getSystem("aSys").get("pow");
      GUM_CHECK_ASSERT_THROWS_NOTHING(bb->compute(i, i.get("(t_state)state").id()));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete bb);
    }

    static void testEvidenceSioux() {
      gum::prm::GroundedInference< double >* g_ve = 0;
      gum::VariableElimination< double >*    ve   = 0;
      gum::BayesNet< double >                bn;
      gum::BayesNetFactory< double >         bn_factory(&bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getSystem("aSys").groundedBN(bn_factory));
      GUM_CHECK_ASSERT_THROWS_NOTHING(ve = new gum::VariableElimination< double >(&bn));
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          g_ve = new gum::prm::GroundedInference< double >(*prm, prm->getSystem("aSys")));
      GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
      // Adding evidence
      {
        const gum::prm::PRMInstance< double >& instance
            = prm->getSystem("aSys").get("bw_printers[0]");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("(t_degraded)equipState");
        gum::Tensor< double >                   e;
        e.add(attribute.type().variable());
        e.fill(static_cast< double >(0.0));
        gum::Instantiation inst(e);
        inst.setFirst();
        e.set(inst, static_cast< double >(1.0));
        gum::prm::PRMInference< double >::Chain chain = std::make_pair(&instance, &attribute);
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        CHECK(g_ve->hasEvidence(chain));
        CHECK(g_ve->hasEvidence(chain.first));
      }
      {
        const gum::prm::PRMInstance< double >&  instance  = prm->getSystem("aSys").get("c1");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("can_print");
        gum::Tensor< double >                   e;
        e.add(attribute.type().variable());
        e.fill(static_cast< double >(0.0));
        gum::Instantiation inst(e);
        inst.setFirst();
        e.set(inst, static_cast< double >(1.0));
        gum::prm::PRMInference< double >::Chain chain = std::make_pair(&instance, &attribute);
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        CHECK(g_ve->hasEvidence(chain));
        CHECK(g_ve->hasEvidence(chain.first));
      }
      // making a first inference with evidence
      {
        const gum::prm::PRMInstance< double >&  q_i     = prm->getSystem("aSys").get("pow");
        const gum::prm::PRMAttribute< double >& q_a     = q_i.get("state");
        gum::prm::PRMInference< double >::Chain q_chain = std::make_pair(&q_i, &q_a);
        gum::Tensor< double >                   m;
        g_ve->posterior(q_chain, m);
        gum::Instantiation inst(m);
        inst.setFirst();
        CHECK((m.get(inst)) == doctest::Approx(1.0).epsilon(1e-6));
        inst.inc();
        CHECK((m.get(inst)) == doctest::Approx(0.0).epsilon(1e-6));
      }
      // Removing evidences
      {
        const gum::prm::PRMInstance< double >&  instance  = prm->getSystem("aSys").get("c1");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("can_print");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      {
        const gum::prm::PRMInstance< double >& instance
            = prm->getSystem("aSys").get("bw_printers[0]");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("(t_degraded)equipState");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      // Inference without evidences
      {
        const gum::prm::PRMInstance< double >&  q_i     = prm->getSystem("aSys").get("pow");
        const gum::prm::PRMAttribute< double >& q_a     = q_i.get("state");
        gum::prm::PRMInference< double >::Chain q_chain = std::make_pair(&q_i, &q_a);
        gum::Tensor< double >                   m;
        g_ve->posterior(q_chain, m);
        gum::Instantiation inst(m);
        inst.setFirst();
        CHECK((m.get(inst)) == doctest::Approx(0.5).epsilon(1e-6));
        inst.inc();
        CHECK((m.get(inst)) == doctest::Approx(0.5).epsilon(1e-6));
      }
      delete g_ve;
    }

    static void testEvidenceSioux2() {
      gum::prm::SVE< double >* g_ve = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve
                                      = new gum::prm::SVE< double >(*prm, prm->getSystem("aSys")));
      // Adding evidence
      {
        const gum::prm::PRMInstance< double >& instance
            = prm->getSystem("aSys").get("bw_printers[0]");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("(t_degraded)equipState");
        gum::Tensor< double >                   e;
        e.add(attribute.type().variable());
        e.fill(static_cast< double >(0.0));
        gum::Instantiation inst(e);
        inst.setFirst();
        e.set(inst, static_cast< double >(1.0));
        gum::prm::PRMInference< double >::Chain chain = std::make_pair(&instance, &attribute);
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        CHECK(g_ve->hasEvidence(chain));
        CHECK(g_ve->hasEvidence(chain.first));
      }
      {
        const gum::prm::PRMInstance< double >&  instance  = prm->getSystem("aSys").get("c1");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("can_print");
        gum::Tensor< double >                   e;
        e.add(attribute.type().variable());
        e.fill(static_cast< double >(0.0));
        gum::Instantiation inst(e);
        inst.setFirst();
        e.set(inst, static_cast< double >(1.0));
        gum::prm::PRMInference< double >::Chain chain = std::make_pair(&instance, &attribute);
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        CHECK(g_ve->hasEvidence(chain));
        CHECK(g_ve->hasEvidence(chain.first));
      }
      // making a first inference with evidence
      {
        const gum::prm::PRMInstance< double >&  q_i     = prm->getSystem("aSys").get("pow");
        const gum::prm::PRMAttribute< double >& q_a     = q_i.get("state");
        gum::prm::PRMInference< double >::Chain q_chain = std::make_pair(&q_i, &q_a);
        gum::Tensor< double >                   m;
        g_ve->posterior(q_chain, m);
        CHECK((m.nbrDim()) == (static_cast< gum::Size >(1)));
        gum::Instantiation inst(m);
        inst.setFirst();
        CHECK((m.get(inst)) == doctest::Approx(1.0).epsilon(1e-6));
        inst.inc();
        CHECK((m.get(inst)) == doctest::Approx(0.0).epsilon(1e-6));
      }
      // Removing evidences
      {
        const gum::prm::PRMInstance< double >&  instance  = prm->getSystem("aSys").get("c1");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("can_print");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      {
        const gum::prm::PRMInstance< double >& instance
            = prm->getSystem("aSys").get("bw_printers[0]");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("(t_degraded)equipState");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      // Inference without evidences
      {
        const gum::prm::PRMInstance< double >&  q_i     = prm->getSystem("aSys").get("pow");
        const gum::prm::PRMAttribute< double >& q_a     = q_i.get("state");
        gum::prm::PRMInference< double >::Chain q_chain = std::make_pair(&q_i, &q_a);
        gum::Tensor< double >                   m;
        g_ve->posterior(q_chain, m);
        gum::Instantiation inst(m);
        inst.setFirst();
        CHECK((m.get(inst)) == doctest::Approx(0.5).epsilon(1e-6));
        inst.inc();
        CHECK((m.get(inst)) == doctest::Approx(0.5).epsilon(1e-6));
      }
      delete g_ve;
    }

    static void testEvidenceSioux3withSVE() {
      gum::prm::PRMInference< double >* g_ve = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve
                                      = new gum::prm::SVE< double >(*prm, prm->getSystem("aSys")));
      _testEvidencenSioux3_(g_ve);
      delete g_ve;
    }

    void /*test*/ EvidenceSioux3withSVED() {
      gum::prm::PRMInference< double >* g_ve = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve
                                      = new gum::prm::SVED< double >(*prm, prm->getSystem("aSys")));
      _testEvidencenSioux3_(g_ve);
      delete g_ve;
    }

    private:
    void _testEvidencenSioux3_(gum::prm::PRMInference< double >* g_ve) {
      // Adding evidence
      {
        const gum::prm::PRMInstance< double >& instance
            = prm->getSystem("aSys").get("bw_printers[0]");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("(t_degraded)equipState");
        gum::Tensor< double >                   e;
        e.add(attribute.type().variable());
        e.fill(static_cast< double >(0.0));
        gum::Instantiation inst(e);
        inst.setFirst();
        e.set(inst, static_cast< double >(1.0));
        gum::prm::PRMInference< double >::Chain chain = std::make_pair(&instance, &attribute);
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        CHECK(g_ve->hasEvidence(chain));
        CHECK(g_ve->hasEvidence(chain.first));
      }
      {
        const gum::prm::PRMInstance< double >&  instance  = prm->getSystem("aSys").get("c1");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("can_print");
        gum::Tensor< double >                   e;
        e.add(attribute.type().variable());
        e.fill(static_cast< double >(0.0));
        gum::Instantiation inst(e);
        inst.setFirst();
        e.set(inst, static_cast< double >(1.0));
        gum::prm::PRMInference< double >::Chain chain = std::make_pair(&instance, &attribute);
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        CHECK(g_ve->hasEvidence(chain));
        CHECK(g_ve->hasEvidence(chain.first));
      }
      // making a first inference with evidence
      {
        const gum::prm::PRMInstance< double >&  q_i     = prm->getSystem("aSys").get("pow");
        const gum::prm::PRMAttribute< double >& q_a     = q_i.get("state");
        gum::prm::PRMInference< double >::Chain q_chain = std::make_pair(&q_i, &q_a);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->posterior(q_chain, m));
        gum::Instantiation inst(m);
        inst.setFirst();
        CHECK((m.get(inst)) == doctest::Approx(1.0).epsilon(1e-6));
        inst.inc();
        CHECK((m.get(inst)) == doctest::Approx(0.0).epsilon(1e-6));
      }
      // Removing evidences
      {
        const gum::prm::PRMInstance< double >&  instance  = prm->getSystem("aSys").get("c1");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("can_print");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      {
        const gum::prm::PRMInstance< double >& instance
            = prm->getSystem("aSys").get("bw_printers[0]");
        const gum::prm::PRMAttribute< double >& attribute = instance.get("(t_degraded)equipState");
        gum::prm::PRMInference< double >::Chain chain     = std::make_pair(&instance, &attribute);
        GUM_CHECK_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      // Inference without evidences
      {
        const gum::prm::PRMInstance< double >&  q_i     = prm->getSystem("aSys").get("pow");
        const gum::prm::PRMAttribute< double >& q_a     = q_i.get("state");
        gum::prm::PRMInference< double >::Chain q_chain = std::make_pair(&q_i, &q_a);
        gum::Tensor< double >                   m;
        g_ve->posterior(q_chain, m);
        gum::Instantiation inst(m);
        inst.setFirst();
        CHECK((m.get(inst)) == doctest::Approx(0.5).epsilon(1e-6));
        inst.inc();
        CHECK((m.get(inst)) == doctest::Approx(0.5).epsilon(1e-6));
      }
    }
  };

  GUM_TEST_ACTIF(Evidence)
  GUM_TEST_ACTIF(SmallGrdInference)
  GUM_TEST_ACTIF(SmallSVEInference)
  GUM_TEST_ACTIF(SmallSVEDInference)
  GUM_TEST_ACTIF(SmallStructInference)
  GUM_TEST_ACTIF(Inference)
  GUM_TEST_ACTIF(StructuredBB)
  GUM_TEST_ACTIF(EvidenceSioux)
  GUM_TEST_ACTIF(EvidenceSioux2)
  GUM_TEST_ACTIF(EvidenceSioux3withSVE)

}   // namespace gum_tests
