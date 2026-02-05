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


#include <iostream>
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphicalModels/inference/scheduler/scheduleMultiDim.h>
#include <agrum/BN/algorithms/BayesBall.h>
#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  BayesBall
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct BayesBallTestSuite {
    public:
    static void testRequisiteNodes() {
      gum::SimpleBayesNetGenerator< double > gen(50, 200, 2);
      gum::BayesNet< double >                bn;
      gen.generateBN(bn);
      gum::Set< gum::NodeId > requisite;

      gum::Set< gum::NodeId >      query, hardEvidence, softEvidence;
      gum::Sequence< gum::NodeId > nodes_seq;
      for (const auto node: bn.nodes())
        nodes_seq.insert(node);

      for (gum::Idx i = 0; i < 5; ++i)
        hardEvidence.insert(nodes_seq.atPos(i));

      for (gum::Idx j = 24; j > 19; --j)
        query.insert(nodes_seq.atPos(j));

      CHECK_NOTHROW(
          gum::BayesBall::requisiteNodes(bn.dag(), query, hardEvidence, softEvidence, requisite));

      CHECK(requisite.size() >= 5);
    }   // namespace gum_tests

    static void testRelevantTensors() {
      std::string file = GET_RESSOURCES_PATH("bif/asia.bif");

      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = reader.proceed();
      CHECK(nbrErr == gum::Size(0));

      gum::Sequence< gum::NodeId >             nodes_seq;
      gum::Set< const gum::Tensor< double >* > tensors;

      for (const auto node: bn.nodes()) {
        nodes_seq.insert(node);
        tensors.insert(&bn.cpt(node));
        // std::cout << node << " : " << bn.variable(node).name() << std::endl;
      }

      gum::Set< gum::NodeId > requisite;
      gum::Set< gum::NodeId > query, hardEvidence, softEvidence;

      query.insert(gum::NodeId(1));          // tuberculosis
      hardEvidence.insert(gum::NodeId(5));   // smoking

      CHECK_NOTHROW(
          gum::BayesBall::requisiteNodes(bn.dag(), query, hardEvidence, softEvidence, requisite));
      CHECK_NOTHROW(
          gum::BayesBall::relevantTensors(bn, query, hardEvidence, softEvidence, tensors));

      CHECK(tensors.size() == 5);
    }

    static void testRelevantScheduleMultiDims() {
      std::string file = GET_RESSOURCES_PATH("bif/asia.bif");

      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = reader.proceed();
      CHECK(nbrErr == gum::Size(0));

      gum::Sequence< gum::NodeId >                                      nodes_seq;
      gum::Set< const gum::ScheduleMultiDim< gum::Tensor< double > >* > tensors;

      for (const auto node: bn.nodes()) {
        nodes_seq.insert(node);
        auto sched = new gum::ScheduleMultiDim< gum::Tensor< double > >(bn.cpt(node), false, node);
        tensors.insert(sched);
        // std::cout << node << " : " << bn.variable(node).name() << std::endl;
      }
      gum::Set< const gum::ScheduleMultiDim< gum::Tensor< double > >* > pots = tensors;

      gum::Set< gum::NodeId > requisite;
      gum::Set< gum::NodeId > query, hardEvidence, softEvidence;

      query.insert(gum::NodeId(1));          // tuberculosis
      hardEvidence.insert(gum::NodeId(5));   // smoking

      CHECK_NOTHROW(
          gum::BayesBall::requisiteNodes(bn.dag(), query, hardEvidence, softEvidence, requisite));
      CHECK_NOTHROW(
          gum::BayesBall::relevantTensors(bn, query, hardEvidence, softEvidence, tensors));

      CHECK(tensors.size() == 5);

      for (const auto pot: pots)
        delete pot;
    }
  };

  GUM_TEST_ACTIF(RequisiteNodes)
  GUM_TEST_ACTIF(RelevantTensors)
  GUM_TEST_ACTIF(RelevantScheduleMultiDims)
}   // namespace gum_tests
